#
# Set minimum required version
#
CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)
PROJECT(webviz)

#
# Set C++ Version 17
#
SET(CMAKE_CXX_STANDARD 17)

#
# Deactivate RPATH for MacOSX
#
SET(CMAKE_MACOSX_RPATH 0)

#
# Set CMake policies to select wanted behaviors
#
# Use new policies introduced up to this version
CMAKE_POLICY(VERSION 2.8.12)
IF(POLICY CMP0042)
  CMAKE_POLICY(SET CMP0042 NEW)
ENDIF(POLICY CMP0042)

#
# add path where cmake files must be searched for
#
LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)


#
# Include path points to the base source dir
#
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

#
# Make sure we are under Unix
#
IF(NOT UNIX)
    MESSAGE(FATAL_ERROR "WebViz compiles only under UNIX, sorry!")
ENDIF(NOT UNIX)

#
# Add all the CMake modules we need
#
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/DownloadProject.cmake)
# Set build options
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildOptions.cmake)
# Set compiler flags
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildFlags.cmake)
# Check for libraries
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildChecks.cmake)
# Set up CPack
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/ARGoSPackaging.cmake)

#
# Compile stuff
#
ADD_SUBDIRECTORY(plugins)
ADD_SUBDIRECTORY(testing)

# Add Uninstall target
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/cmake/uninstall.cmake"
)