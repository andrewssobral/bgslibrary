cmake_minimum_required(VERSION 2.8)

project(bgslibrary)

# cmake -D BGS_PYTHON_SUPPORT=ON ..
if(NOT DEFINED BGS_PYTHON_SUPPORT)
  set(BGS_PYTHON_SUPPORT OFF)
elseif()
  # add_definitions(-DBGS_PYTHON_SUPPORT)
endif()
message(STATUS "BGSLIBRARY WITH PYTHON SUPPORT: ${BGS_PYTHON_SUPPORT}")

# cmake -D BGS_PYTHON_SUPPORT=ON -D BGS_PYTHON_VERSION=2 ..
if(NOT DEFINED BGS_PYTHON_VERSION)
  set(BGS_PYTHON_VERSION 2)
endif()
message(STATUS "PYTHON VERSION: ${BGS_PYTHON_VERSION}")

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
  set(CMAKE_MACOSX_RPATH 1)
endif(UNIX)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

# compilation mode setup
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

if(WIN32)
  if(BGS_PYTHON_SUPPORT)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
  else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  endif()
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
  message(STATUS "OpenCV library status:")
  message(STATUS "    version: ${OpenCV_VERSION}")
  message(STATUS "    libraries: ${OpenCV_LIBS}")
  message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
endif()

# if(${OpenCV_VERSION} VERSION_EQUAL 3 OR ${OpenCV_VERSION} VERSION_GREATER 3)
#   message(FATAL_ERROR "OpenCV version is not compatible: ${OpenCV_VERSION}")
# endif()

if(${OpenCV_VERSION} VERSION_LESS 2.3.1)
  message(FATAL_ERROR "OpenCV version is not compatible: ${OpenCV_VERSION}")
endif()

if(BGS_PYTHON_SUPPORT)
  if(WIN32)
    set(Boost_USE_STATIC_LIBS ON)
  else()
    set(Boost_USE_STATIC_LIBS OFF)
  endif()
  
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
  
  message(STATUS "SEARCHING FOR BOOST COMPONENT FOR PYTHON ${BGS_PYTHON_VERSION}")
  if(BGS_PYTHON_VERSION EQUAL 2)
    find_package(Boost REQUIRED COMPONENTS python)
  else()
    find_package(Boost REQUIRED COMPONENTS python3)
  endif()
  find_package(PythonInterp REQUIRED)
  find_package(PythonLibs REQUIRED)

  message(STATUS "Boost library status:")
  message(STATUS "    version: ${Boost_VERSION}")
  message(STATUS "    libraries: ${Boost_LIBRARIES}")
  message(STATUS "    include path: ${Boost_INCLUDE_DIRS}")

  message(STATUS "Python library status:")
  message(STATUS "    executable: ${PYTHON_EXECUTABLE}")
  message(STATUS "    version: ${PYTHON_VERSION_STRING}")
  #message(STATUS "    libraries: ${PYTHON_LIBRARIES}")
  message(STATUS "    library: ${PYTHON_LIBRARY}")
  message(STATUS "    include path: ${PYTHON_INCLUDE_DIRS}")
  
  if(NOT NUMPY_INCLUDE_DIR)
    # message(FATAL_ERROR "You must define NUMPY_INCLUDE_DIR by 'cmake -D NUMPY_INCLUDE_DIR=/python/lib/site-packages/numpy/core/include ..'")
    exec_program ("${PYTHON_EXECUTABLE}"
      ARGS "-c \"import numpy; print(numpy.get_include())\""
      OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
      RETURN_VALUE NUMPY_NOT_FOUND)
  endif()
  message(STATUS "NUMPY_INCLUDE_DIR: ${NUMPY_INCLUDE_DIR}")
endif()

#file(GLOB sources FrameProcessor.cpp PreProcessor.cpp VideoAnalysis.cpp VideoCapture.cpp)
file(GLOB main Main.cpp FrameProcessor.cpp PreProcessor.cpp VideoAnalysis.cpp VideoCapture.cpp)
file(GLOB demo Demo.cpp)
file(GLOB demo2 Demo2.cpp)

# list(REMOVE_ITEM sources ${demo} ${demo2})

file(GLOB_RECURSE analysis_src package_analysis/*.cpp)
if(BGS_PYTHON_SUPPORT)
  file(GLOB_RECURSE bgs_src package_bgs/*.cpp package_bgs/*.c wrapper_python/*.cpp)
  file(GLOB_RECURSE bgs_include package_bgs/*.h wrapper_python/*.h)
  include_directories(${CMAKE_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${NUMPY_INCLUDE_DIR})
else()
  file(GLOB_RECURSE bgs_src package_bgs/*.cpp package_bgs/*.c)
  file(GLOB_RECURSE bgs_include package_bgs/*.h)
  include_directories(${CMAKE_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})
endif()

# GMG is not available in older OpenCV versions
if(${OpenCV_VERSION} VERSION_LESS 2.4.3)
  file(GLOB gmg package_bgs/GMG.cpp)
  list(REMOVE_ITEM bgs_src ${gmg})
endif()

if(BGS_PYTHON_SUPPORT)
  #add_library(libbgs SHARED ${sources} ${bgs_src} ${analysis_src})
  add_library(libbgs SHARED ${bgs_src} ${analysis_src})
  target_link_libraries(libbgs ${OpenCV_LIBS} ${Boost_LIBRARIES} ${PYTHON_LIBRARY})
  target_compile_definitions(libbgs PRIVATE BGS_PYTHON_SUPPORT=1)
else()
  #add_library(libbgs STATIC ${sources} ${bgs_src} ${analysis_src})
  add_library(libbgs STATIC ${bgs_src} ${analysis_src})
  target_link_libraries(libbgs ${OpenCV_LIBS})
endif()
set_property(TARGET libbgs PROPERTY PUBLIC_HEADER ${bgs_include})

if(WIN32)
  # set_property(TARGET libbgs PROPERTY SUFFIX ".lib")
  if(BGS_PYTHON_SUPPORT)
    set_property(TARGET libbgs PROPERTY SUFFIX ".pyd")
  endif()
else()
  set_property(TARGET libbgs PROPERTY OUTPUT_NAME "bgs")
endif()

if(NOT BGS_PYTHON_SUPPORT)
add_executable(bgslibrary ${main})
target_link_libraries(bgslibrary ${OpenCV_LIBS} libbgs)
# set_target_properties(bgslibrary PROPERTIES OUTPUT_NAME bgs)

add_executable(bgs_demo ${demo})
target_link_libraries(bgs_demo ${OpenCV_LIBS} libbgs)

add_executable(bgs_demo2 ${demo2})
target_link_libraries(bgs_demo2 ${OpenCV_LIBS} libbgs)

install(TARGETS libbgs
  bgslibrary
  RUNTIME DESTINATION bin COMPONENT app
  LIBRARY DESTINATION lib COMPONENT runtime
  ARCHIVE DESTINATION lib COMPONENT runtime
  PUBLIC_HEADER DESTINATION include/package_bgs COMPONENT dev
  FRAMEWORK DESTINATION "/Library/Frameworks"
)
endif()