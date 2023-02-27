cmake_minimum_required(VERSION 3.10)

project(bgslibrary-examples)

set(CMAKE_CXX_STANDARD 14)

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
  set(CMAKE_MACOSX_RPATH 1)
endif(UNIX)

# Avoid cmake warnings about changes in behavior of some Mac OS X path
# variable we don't care about.
if (POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

# compilation mode setup
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  set(BUILD_SHARED_LIBS TRUE)
  #if(BGS_PYTHON_SUPPORT)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
  #else()
  #  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  #  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  #endif()
endif(WIN32)

set(bgs_out_dir ".")
# First for the generic no-config case (e.g. with mingw)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${bgs_out_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${bgs_out_dir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${bgs_out_dir})
# Second, for multi-config builds (e.g. msvc)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${bgs_out_dir})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${bgs_out_dir})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${bgs_out_dir})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

if(UNIX)
  # add some standard warnings
  ADD_DEFINITIONS(-Wno-variadic-macros -Wno-long-long -Wall -Wextra -Winit-self -Woverloaded-virtual -Wsign-promo -Wno-unused-parameter -pedantic -Woverloaded-virtual -Wno-unknown-pragmas)

  # -ansi does not compile with sjn module
  #ADD_DEFINITIONS(-ansi)

  # if you like to have warinings about conversions, e.g. double->int or double->float etc., or float compare
  #ADD_DEFINITIONS(-Wconversion -Wfloat-equal)
endif(UNIX)

set(OpenCV_STATIC OFF)
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
  message(STATUS "")
  message(STATUS "OpenCV library status:")
  message(STATUS "    version: ${OpenCV_VERSION}")
  message(STATUS "    libraries: ${OpenCV_LIBS}")
  message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}\n")
endif()

# if(${OpenCV_VERSION} VERSION_EQUAL 3 OR ${OpenCV_VERSION} VERSION_GREATER 3)
#   message(FATAL_ERROR "OpenCV version is not compatible: ${OpenCV_VERSION}")
# endif()

if(${OpenCV_VERSION} VERSION_LESS 2.3.1)
  message(FATAL_ERROR "OpenCV version is not compatible: ${OpenCV_VERSION}")
endif()

file(GLOB demo Demo.cpp)
file(GLOB demo2 Demo2.cpp)

file(GLOB_RECURSE tools_src ../bgslibrary/tools/*.cpp ../bgslibrary/tools/*.c)
file(GLOB_RECURSE tools_inc ../bgslibrary/tools/*.h ../bgslibrary/tools/*.hpp)

file(GLOB_RECURSE utils_src ../bgslibrary/utils/*.cpp ../bgslibrary/utils/*.c)
file(GLOB_RECURSE utils_inc ../bgslibrary/utils/*.h ../bgslibrary/utils/*.hpp)

file(GLOB_RECURSE bgs_src ../bgslibrary/algorithms/*.cpp ../bgslibrary/algorithms/*.c)
file(GLOB_RECURSE bgs_inc ../bgslibrary/algorithms/*.h ../bgslibrary/algorithms/*.hpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})

# GMG is not available in older OpenCV versions
if(${OpenCV_VERSION} VERSION_LESS 2.4.3)
  file(GLOB gmg ../bgslibrary/algorithms/GMG.cpp)
  list(REMOVE_ITEM bgs_src ${gmg})
endif()

add_library(bgslibrary_core SHARED ${bgs_src} ${tools_src} ${utils_src} ${bgs_inc} ${tools_inc} ${utils_inc})
# generates the export header bgslibrary_core_EXPORTS.h automatically
include(GenerateExportHeader)
GENERATE_EXPORT_HEADER(bgslibrary_core
    BASE_NAME bgslibrary_core
    EXPORT_MACRO_NAME bgslibrary_core_EXPORTS
    EXPORT_FILE_NAME bgslibrary_core_EXPORTS.h
    STATIC_DEFINE BGSLIBRARY_CORE_EXPORTS_BUILT_AS_STATIC)
target_link_libraries(bgslibrary_core ${OpenCV_LIBS})
set_property(TARGET bgslibrary_core PROPERTY PUBLIC_HEADER ${bgs_inc} ${tools_inc} ${utils_inc})

#if(WIN32)
#  # set_property(TARGET bgslibrary_core PROPERTY SUFFIX ".lib")
#  #if(BGS_PYTHON_SUPPORT)
#  #  set_property(TARGET bgslibrary_core PROPERTY SUFFIX ".pyd")
#  #endif()
#else()
#  set_property(TARGET bgslibrary_core PROPERTY OUTPUT_NAME "bgs")
#endif()

#if(APPLE)
#  if(BGS_PYTHON_SUPPORT)
#    set_property(TARGET bgslibrary_core PROPERTY SUFFIX ".so")
#    set_target_properties(bgslibrary_core PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
#  endif()
#endif()

add_executable(bgs_demo ${demo})
target_link_libraries(bgs_demo ${OpenCV_LIBS} bgslibrary_core)

add_executable(bgs_demo2 ${demo2})
target_link_libraries(bgs_demo2 ${OpenCV_LIBS} bgslibrary_core)

if(UNIX)
  # to avoid: error while loading shared libraries: libbgslibrary_core.so
  message(STATUS "You might need to run:")
  message(STATUS "$ LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib")
  message(STATUS "$ export LD_LIBRARY_PATH")
  message(STATUS "after 'make install' to avoid error while loading libbgslibrary_core\n")
endif()

if(WIN32)
  message(STATUS "You might need to add ${CMAKE_CURRENT_BINARY_DIR} to your PATH to be able to run your applications.")
  message(STATUS "> set PATH=%PATH%;${CMAKE_CURRENT_BINARY_DIR}\n")
endif()
