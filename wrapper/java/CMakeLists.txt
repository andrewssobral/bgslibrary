cmake_minimum_required(VERSION 2.8)
project(libbgs_java_module)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=gnu++0x")
	set(CMAKE_MACOSX_RPATH 1)
endif(UNIX)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

# compilation mode setup
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

if(WIN32)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
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

find_package(JNI REQUIRED)
if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
endif()

set(OpenCV_STATIC OFF)
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
  message(STATUS "OpenCV library status:")
  message(STATUS "    version: ${OpenCV_VERSION}")
  message(STATUS "    libraries: ${OpenCV_LIBS}")
  message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
endif()

file(GLOB_RECURSE tools_src ../../src/tools/*.cpp ../../src/tools/*.c)
file(GLOB_RECURSE tools_inc ../../src/tools/*.h ../../src/tools/*.hpp)
file(GLOB_RECURSE utils_src ../../src/utils/*.cpp ../../src/utils/*.c)
file(GLOB_RECURSE utils_inc ../../src/utils/*.h ../../src/utils/*.hpp)
file(GLOB_RECURSE bgs_src ../../src/algorithms/*.cpp ../../src/algorithms/*.c)
file(GLOB_RECURSE bgs_inc ../../src/algorithms/*.h ../../src/algorithms/*.hpp)

include_directories(${CMAKE_SOURCE_DIR} ${JNI_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

add_library(libbgs STATIC ${bgs_src} ${tools_src} ${utils_src})
target_link_libraries(libbgs ${OpenCV_LIBS})
set_property(TARGET libbgs PROPERTY PUBLIC_HEADER ${bgs_inc} ${tools_inc} ${utils_inc})
if(WIN32)
	# set_property(TARGET libbgs PROPERTY SUFFIX ".lib")
else()
	set_property(TARGET libbgs PROPERTY OUTPUT_NAME "bgs")
endif()

set(SOURCE_FILES bgslibrary_java_module.cpp)
add_library(libbgs_java_module SHARED ${SOURCE_FILES})
set_target_properties(libbgs_java_module PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(libbgs_java_module libbgs ${OpenCV_LIBS})
if(WIN32)
	# set_property(TARGET libbgs_java_module PROPERTY SUFFIX ".lib")
else()
	set_property(TARGET libbgs_java_module PROPERTY OUTPUT_NAME "bgs_java_module")
endif()
