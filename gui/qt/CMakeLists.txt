cmake_minimum_required(VERSION 3.1)

project(bgslibrary_gui)

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
  set(CMAKE_MACOSX_RPATH 1)

  # add some standard warnings
  ADD_DEFINITIONS(-Wno-variadic-macros -Wno-long-long -Wall -Wextra -Winit-self -Woverloaded-virtual -Wsign-promo -Wno-unused-parameter -pedantic -Woverloaded-virtual -Wno-unknown-pragmas)

  # -ansi does not compile with sjn module
  #ADD_DEFINITIONS(-ansi)

  # if you like to have warinings about conversions, e.g. double->int or double->float etc., or float compare
  #ADD_DEFINITIONS(-Wconversion -Wfloat-equal)
endif(UNIX)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Handle the Qt uic code generator automatically
set(CMAKE_AUTOUIC ON)

# Find the Qt5Widgets library
find_package(Qt5Widgets)
# To fix the issue: https://stackoverflow.com/questions/38557755/cmake-cant-seem-to-find-needed-qt-cmake-files-even-though-ive-set-cmake-prefix
#find_package(Qt5Widgets CONFIG PATHS C:/Qt/5.11.1/msvc2017_64/lib/cmake NO_DEFAULT_PATH)
if(Qt5Widgets_FOUND)
  message(STATUS "Qt5Widgets status:")
  message(STATUS "    version: ${Qt5Widgets_VERSION}")
  message(STATUS "    libraries: ${Qt5Widgets_LIBRARIES}")
  message(STATUS "    include path: ${Qt5Widgets_INCLUDE_DIRS}")
endif()

SET(app_RESOURCES application.qrc)
QT5_ADD_RESOURCES(app_RESOURCES_RCC ${app_RESOURCES})

# Find the OpenCV library
set(OpenCV_STATIC OFF)
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
  message(STATUS "OpenCV library status:")
  message(STATUS "    version: ${OpenCV_VERSION}")
  message(STATUS "    libraries: ${OpenCV_LIBS}")
  message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
endif()

file(GLOB main_src *.cpp *.c)
file(GLOB main_inc *.h *.hpp)

file(GLOB_RECURSE tools_src ../../bgslibrary/tools/*.cpp ../../bgslibrary/tools/*.c)
file(GLOB_RECURSE tools_inc ../../bgslibrary/tools/*.h ../../bgslibrary/tools/*.hpp)

file(GLOB_RECURSE utils_src ../../bgslibrary/utils/*.cpp ../../bgslibrary/utils/*.c)
file(GLOB_RECURSE utils_inc ../../bgslibrary/utils/*.h ../../bgslibrary/utils/*.hpp)

file(GLOB_RECURSE bgs_src ../../bgslibrary/algorithms/*.cpp ../../bgslibrary/algorithms/*.c)
file(GLOB_RECURSE bgs_inc ../../bgslibrary/algorithms/*.h)

include_directories(${CMAKE_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})

add_library(libbgs STATIC ${bgs_src} ${tools_src} ${utils_src} ${bgs_inc} ${tools_inc} ${utils_inc})
target_link_libraries(libbgs ${OpenCV_LIBS})
set_property(TARGET libbgs PROPERTY PUBLIC_HEADER ${bgs_inc} ${tools_inc} ${utils_inc})

if(WIN32)
  # set_property(TARGET libbgs PROPERTY SUFFIX ".lib")
else()
  set_property(TARGET libbgs PROPERTY OUTPUT_NAME "bgs")
endif()

# Tell CMake to create the bgslibrary_gui executable
add_executable(bgslibrary_gui ${main_src} ${main_inc} ${app_RESOURCES_RCC})

# Use the Widgets module from Qt 5.
target_link_libraries(bgslibrary_gui Qt5::Widgets ${OpenCV_LIBS} libbgs)
