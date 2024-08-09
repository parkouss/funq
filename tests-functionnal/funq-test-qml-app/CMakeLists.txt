cmake_minimum_required(VERSION 3.14)

# Set the project name and target
project(funq-test-qml-app)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set a default build type if none is specified (important for CI)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Find the appropriate version of Qt (either Qt5 or Qt6)
find_package(Qt6 QUIET COMPONENTS Widgets Quick Qml)
if (NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Widgets Quick Qml)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

# Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)

# Add the main source file
add_executable(${PROJECT_NAME} main.cpp)

# Add the resource file and link against the appropriate Qt libraries
if (QT_VERSION_MAJOR EQUAL 5)
    qt5_add_resources(QT_RESOURCES resources.qrc)
    target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Quick Qt5::Qml)
elseif (QT_VERSION_MAJOR EQUAL 6)
    qt_add_resources(QT_RESOURCES resources.qrc)
    target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Quick Qt6::Qml)
endif()

# Include the generated resource files in the target
target_sources(${PROJECT_NAME} PRIVATE ${QT_RESOURCES})

# Platform-specific settings
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Windows-specific settings
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # macOS-specific settings (e.g., bundle into an .app)
    set(MACOSX_BUNDLE TRUE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.yourdomain.funq-test-qml-app
        MACOSX_BUNDLE_BUNDLE_NAME "FunqTestQMLApp"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Linux-specific settings
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()

# Installation rules (optional, for installation packaging)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
