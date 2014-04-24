cmake_minimum_required(VERSION 2.8)

project(bgs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

find_package(OpenCV REQUIRED)

if(${OpenCV_VERSION} VERSION_LESS 2.3.1)
  message (FATAL_ERROR "OpenCV version is not compatible: ${OpenCV_VERSION}")
endif()

file(GLOB sources FrameProcessor.cpp PreProcessor.cpp VideoAnalysis.cpp  VideoCapture.cpp)
file(GLOB main Main.cpp)
file(GLOB demo Demo.cpp)

list(REMOVE_ITEM sources ${demo})

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

INSTALL(TARGETS bgs
	bgs_demo
	bgs_bin
  RUNTIME DESTINATION bin COMPONENT app
  LIBRARY DESTINATION lib COMPONENT runtime
  ARCHIVE DESTINATION lib COMPONENT runtime
  PUBLIC_HEADER DESTINATION include/package_bgs COMPONENT dev
  FRAMEWORK DESTINATION "/Library/Frameworks"
)
