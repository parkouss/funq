# CMake version configuration
cmake_minimum_required(VERSION 3.5...3.19)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Define project
project(funq-test-app LANGUAGES CXX)

# Global options
option(BUILD_DISALLOW_WARNINGS
       "Disallow compiler warnings during build (build with -Werror)." OFF
)

# Create a release build by default
# (Code based on https://blog.kitware.com/cmake-and-the-default-build-type/)
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  # Set the build type
  message(
    STATUS
      "Setting build type to '${default_build_type}' as none was specified."
  )
  set(CMAKE_BUILD_TYPE
      "${default_build_type}"
      CACHE STRING "Choose the type of build." FORCE
  )

  # Set the possible values of build type for cmake-gui
  set_property(
    CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel"
                                    "RelWithDebInfo"
  )
endif()

# Auto-include current directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set common compile options
add_compile_options(-Wall -Wextra -Wpedantic)
if(BUILD_DISALLOW_WARNINGS)
  message(STATUS "Building with -Werror")
  add_compile_options(-Werror)
endif()
if(WIN32)
  add_definitions(-DUNICODE -D_UNICODE)
endif()

# Disable Qt functions which are deprecated in Qt <= 5.0 to enforce us
# migrating away from them. Always set this to the lowest Qt version we want
# to support.
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050000)

# Not sure what we should do with deprecation warnings. Better keeping
# backwards compatibility with already existing & supported environments
# instead of forward compatibility with not yet existing environments, no?
add_definitions(-DQT_NO_DEPRECATED_WARNINGS)

# Find Qt
find_package(Qt5 REQUIRED COMPONENTS Core Gui Network Quick Widgets Test)
message(STATUS "Building with Qt ${Qt5_VERSION}")

# Build application
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)
add_executable(
  funq-test-app
  main.cpp
  widgets.h
)
target_link_libraries(
  funq-test-app PUBLIC
  Qt5::Core Qt5::Gui Qt5::Widgets
)
