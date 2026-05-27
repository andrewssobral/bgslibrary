# Self-contained CMake project for the C++ golden-master emitter (Phase 0 safety net).
# Modeled on examples/CMakeLists.txt: it re-globs the core sources and builds its own
# bgslibrary_core, so it does NOT depend on (or modify) the main production build.
#
#   cmake -S tests/golden/cpp -B tests/golden/cpp/build -G Ninja
#   cmake --build tests/golden/cpp/build
#   # then drive it with: python tests/golden/golden_cpp.py --check --opencv <ver>
cmake_minimum_required(VERSION 3.10)

project(bgslibrary-golden-cpp)

set(CMAKE_CXX_STANDARD 14)

if(UNIX)
  set(CMAKE_MACOSX_RPATH 1)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  set(BUILD_SHARED_LIBS TRUE)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif()

# Build outputs in the build dir root.
set(bgs_out_dir ".")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${bgs_out_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${bgs_out_dir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${bgs_out_dir})

find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
  message(STATUS "OpenCV version: ${OpenCV_VERSION}")
endif()
if(${OpenCV_VERSION} VERSION_LESS 2.3.1)
  message(FATAL_ERROR "OpenCV version is not compatible: ${OpenCV_VERSION}")
endif()

set(CORE ../../../bgslibrary)
file(GLOB_RECURSE tools_src ${CORE}/tools/*.cpp ${CORE}/tools/*.c)
file(GLOB_RECURSE tools_inc ${CORE}/tools/*.h ${CORE}/tools/*.hpp)
file(GLOB_RECURSE utils_src ${CORE}/utils/*.cpp ${CORE}/utils/*.c)
file(GLOB_RECURSE utils_inc ${CORE}/utils/*.h ${CORE}/utils/*.hpp)
file(GLOB_RECURSE bgs_src   ${CORE}/algorithms/*.cpp ${CORE}/algorithms/*.c)
file(GLOB_RECURSE bgs_inc   ${CORE}/algorithms/*.h   ${CORE}/algorithms/*.hpp)

# GMG is not available in older OpenCV versions (matches the main build / examples).
if(${OpenCV_VERSION} VERSION_LESS 2.4.3)
  file(GLOB gmg ${CORE}/algorithms/GMG.cpp)
  list(REMOVE_ITEM bgs_src ${gmg})
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})  # for the generated export header
include_directories(${OpenCV_INCLUDE_DIRS})

add_library(bgslibrary_core SHARED ${bgs_src} ${tools_src} ${utils_src} ${bgs_inc} ${tools_inc} ${utils_inc})
include(GenerateExportHeader)
GENERATE_EXPORT_HEADER(bgslibrary_core
    BASE_NAME bgslibrary_core
    EXPORT_MACRO_NAME bgslibrary_core_EXPORTS
    EXPORT_FILE_NAME bgslibrary_core_EXPORTS.h
    STATIC_DEFINE BGSLIBRARY_CORE_EXPORTS_BUILT_AS_STATIC)
target_link_libraries(bgslibrary_core ${OpenCV_LIBS})

add_executable(golden_cpp golden_cpp.cpp)
target_link_libraries(golden_cpp ${OpenCV_LIBS} bgslibrary_core)
# Find libbgslibrary_core next to the executable regardless of the (temp) working directory the
# driver launches us from: `@loader_path` on macOS, `$ORIGIN` on Linux (set per-platform to avoid
# junk RPATH entries). On Windows the DLL sits beside the .exe, which the loader finds.
if(APPLE)
  set(golden_rpath "@loader_path")
else()
  set(golden_rpath "$ORIGIN")
endif()
set_target_properties(golden_cpp PROPERTIES
    BUILD_RPATH "${golden_rpath}"
    INSTALL_RPATH "${golden_rpath}")
