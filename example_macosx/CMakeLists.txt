cmake_minimum_required(VERSION 2.8)

project(FrameDifferenceTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

IF(UNIX)
  # add some standard warnings
  ADD_DEFINITIONS(-Wno-variadic-macros -Wno-long-long -Wall -Wextra -Winit-self -Woverloaded-virtual -Wsign-promo -Wno-unused-parameter -pedantic -Woverloaded-virtual -Wno-unknown-pragmas)
  
  # -ansi does not compile with sjn module
  #ADD_DEFINITIONS(-ansi)

  # if you like to have warinings about conversions, e.g. double->int or double->float etc., or float compare
  #ADD_DEFINITIONS(-Wconversion -Wfloat-equal)
endif(UNIX)

find_package(OpenCV REQUIRED)

file(GLOB source FrameDifferenceTest.cpp)

file(GLOB_RECURSE bgs_src ../package_bgs/*.cpp ../package_bgs/*.c)
file(GLOB_RECURSE bgs_include ../package_bgs/*.h)

include_directories(${CMAKE_SOURCE_DIR})

add_library(bgs SHARED ${bgs_src})
target_link_libraries(bgs ${OpenCV_LIBS})
set_property(TARGET bgs PROPERTY PUBLIC_HEADER ${bgs_include})

add_executable(FrameDifferenceTest ${source})
target_link_libraries(FrameDifferenceTest ${OpenCV_LIBS} bgs)

