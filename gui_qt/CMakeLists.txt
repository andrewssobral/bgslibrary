cmake_minimum_required(VERSION 2.8.11)

project(bgslibrary_gui)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Handle the Qt uic code generator automatically
set(CMAKE_AUTOUIC ON)

# Find the Qt5Widgets library
find_package(Qt5Widgets)

SET(app_RESOURCES application.qrc)
QT5_ADD_RESOURCES(app_RESOURCES_RCC ${app_RESOURCES})

# Find the OpenCV library
set(OpenCV_STATIC OFF)
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

file(GLOB main bgslibrary_gui.cpp mainwindow.cpp qt_utils.cpp texteditor.cpp)

file(GLOB_RECURSE analysis_src ../package_analysis/*.cpp)
file(GLOB_RECURSE analysis_inc ../package_analysis/*.h)
file(GLOB_RECURSE bgs_src ../package_bgs/*.cpp ../package_bgs/*.c)
file(GLOB_RECURSE bgs_inc ../package_bgs/*.h)

include_directories(${CMAKE_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})

add_library(libbgs STATIC ${bgs_src} ${analysis_src})
target_link_libraries(libbgs ${OpenCV_LIBS})
set_property(TARGET libbgs PROPERTY PUBLIC_HEADER ${bgs_inc} ${analysis_inc})

if(WIN32)
	# set_property(TARGET libbgs PROPERTY SUFFIX ".lib")
else()
	set_property(TARGET libbgs PROPERTY OUTPUT_NAME "bgs")
endif()

# Tell CMake to create the bgslibrary_gui executable
add_executable(bgslibrary_gui ${main} ${app_RESOURCES_RCC})

# Use the Widgets module from Qt 5.
target_link_libraries(bgslibrary_gui Qt5::Widgets ${OpenCV_LIBS} libbgs)
