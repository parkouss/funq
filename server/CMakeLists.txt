# CMake version configuration
cmake_minimum_required(VERSION 3.5...3.19)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Define project
project(funq LANGUAGES CXX)

# Global options
option(BUILD_TESTS "Build tests." OFF)
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

# If Qt version is not explicitly specified, automatically determine it
if(NOT QT_MAJOR_VERSION)
  find_package(Qt NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
  set(QT_MAJOR_VERSION ${Qt_VERSION_MAJOR})
endif()
set(QT "Qt${QT_MAJOR_VERSION}")

# Use C++11 (Qt5) or C++17 (Qt6)
if(QT_MAJOR_VERSION EQUAL 6)
  set(CMAKE_CXX_STANDARD 17)
else()
  set(CMAKE_CXX_STANDARD 11)
endif()
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
find_package(
  ${QT}
  REQUIRED COMPONENTS Core Gui Network Widgets Test
  OPTIONAL_COMPONENTS Quick
)
set(QT_VERSION "${${QT}_VERSION}")
set(WITH_QTQUICK "${${QT}Quick_FOUND}")
message(STATUS "Building with Qt ${QT_VERSION}")
message(STATUS "QtQuick found: ${WITH_QTQUICK}")

# Add subprojects
add_subdirectory(libFunq)
if(BUILD_TESTS)
  add_subdirectory(player_tester)
  add_subdirectory(tests/libFunq)
  add_subdirectory(tests/protocole)
endif()
