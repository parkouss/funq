cmake_minimum_required(VERSION 3.14)

# Set the project name and target
project(funq-test-qml-app)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt packages
find_package(Qt5 REQUIRED COMPONENTS Widgets Quick Qml)

# Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)

# Add the main source file
add_executable(${PROJECT_NAME} main.cpp)

# Add the resource file
qt5_add_resources(QT_RESOURCES resources.qrc)
target_sources(${PROJECT_NAME} PRIVATE ${QT_RESOURCES})

# Link against Qt libraries
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Quick Qt5::Qml)
