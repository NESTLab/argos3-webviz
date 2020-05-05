#
# Set minimum required version
#
cmake_minimum_required(VERSION 3.5)


#
# Project version
#
set(MAJOR_VERSION 0)
set(MINOR_VERSION 4)
set(PATCH_VERSION 74)

set(VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})

project(webviz LANGUAGES CXX)

message(STATUS "Building ARGoS3 Webviz :${VERSION_STRING}")

#
# Set C++ Version 17
#
set(CMAKE_CXX_STANDARD 17)

#
# Deactivate RPATH for MacOSX
#
set(CMAKE_MACOSX_RPATH 0)

#
# Set CMake policies to select wanted behaviors
#
# Use new policies introduced up to this version
cmake_policy(VERSION 2.8.12)
if(POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif(POLICY CMP0042)

#
# add path where cmake files must be searched for
#
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)


#
# Include path points to the base source dir
#
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

#
# Make sure we are under Unix
#
if(NOT UNIX)
    message(FATAL_ERROR "WebViz compiles only under UNIX, sorry!")
endif(NOT UNIX)

#
# Add all the CMake modules we need
#
include(DownloadProject)
# Set build options
include(ARGoSBuildOptions)
# Set compiler flags
include(ARGoSBuildFlags)
# Check for libraries
include(ARGoSBuildChecks)
# Set up CPack
include(ARGoSPackaging)
# Set FindArgosPlugin
include(FindARGoSPlugin)


#
# Testing
#
option(PACKAGE_TESTS "Build the tests" OFF)

string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)

if (uppercase_CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  set(IS_DEBUG_MODE True)
endif()

if (IS_DEBUG_MODE)
  message("Debug mode: Building all tests")
  set(PACKAGE_TESTS ON)

   # Enable CPPcheck (static cpp code check)
   include(CppcheckTargets)
endif()


if(PACKAGE_TESTS)
  enable_testing()
  # Add Google test suite
  include(GoogleTest)
  
  # Add Code coverage from CMake
  include(CodeCoverage)
  append_coverage_compiler_flags()
  
  add_subdirectory(tests)
endif()

#
# Compile stuff
#
add_subdirectory(plugins)

add_subdirectory(testing)

# Add Uninstall target
add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/cmake/uninstall.cmake"
)