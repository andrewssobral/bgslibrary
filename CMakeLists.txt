cmake_minimum_required(VERSION 2.8)

project(bgs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

IF(UNIX)
  # add some standard warnings
  ADD_DEFINITIONS(-Wno-variadic-macros -Wno-long-long -Wall -Wextra -Winit-self -Woverloaded-virtual -Wsign-promo -Wno-unused-parameter -pedantic -Woverloaded-virtual -Wno-unknown-pragmas)
  
  # -ansi does not compile with sjn module
  #ADD_DEFINITIONS(-ansi)

  # if you like to have warinings about conversions, e.g. double->int or double->float etc., or float compare
  #ADD_DEFINITIONS(-Wconversion -Wfloat-equal)
endif(UNIX)

find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

if(${OpenCV_VERSION} VERSION_EQUAL 3 OR ${OpenCV_VERSION} VERSION_GREATER 3)
  message (FATAL_ERROR "OpenCV version is not compatible: ${OpenCV_VERSION}")
endif()

if(${OpenCV_VERSION} VERSION_LESS 2.3.1)
  message (FATAL_ERROR "OpenCV version is not compatible: ${OpenCV_VERSION}")
endif()

file(GLOB sources FrameProcessor.cpp PreProcessor.cpp VideoAnalysis.cpp  VideoCapture.cpp)
file(GLOB main Main.cpp)
file(GLOB demo Demo.cpp)
file(GLOB demo2 Demo2.cpp)

list(REMOVE_ITEM sources ${demo} ${demo2})

file(GLOB_RECURSE analysis package_analysis/*.cpp)
file(GLOB_RECURSE bgs package_bgs/*.cpp package_bgs/*.c)
file(GLOB_RECURSE bgs_include package_bgs/*.h)

# GMG is not available in older OpenCV versions
if(${OpenCV_VERSION} VERSION_LESS 2.4.3)
  file(GLOB gmg package_bgs/GMG.cpp)
  list(REMOVE_ITEM bgs ${gmg})
endif()

include_directories(${CMAKE_SOURCE_DIR})

add_library(bgs SHARED ${sources} ${bgs} ${analysis})
target_link_libraries(bgs ${OpenCV_LIBS})
set_property(TARGET bgs PROPERTY PUBLIC_HEADER ${bgs_include})

add_executable(bgs_bin ${main})
target_link_libraries(bgs_bin ${OpenCV_LIBS} bgs)
set_target_properties(bgs_bin
  PROPERTIES OUTPUT_NAME bgs)

add_executable(bgs_demo ${demo})
target_link_libraries(bgs_demo ${OpenCV_LIBS} bgs)

add_executable(bgs_demo2 ${demo2})
target_link_libraries(bgs_demo2 ${OpenCV_LIBS} bgs)

INSTALL(TARGETS bgs
	bgs_bin
  RUNTIME DESTINATION bin COMPONENT app
  LIBRARY DESTINATION lib COMPONENT runtime
  ARCHIVE DESTINATION lib COMPONENT runtime
  PUBLIC_HEADER DESTINATION include/package_bgs COMPONENT dev
  FRAMEWORK DESTINATION "/Library/Frameworks"
)
