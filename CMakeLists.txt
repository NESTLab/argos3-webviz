#
# Set minimum required version
#
CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)
PROJECT(network-api)

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
# Make sure we are under Unix
#
IF(NOT UNIX)
    MESSAGE(FATAL_ERROR "Network-API compiles only under UNIX, sorry!")
ENDIF(NOT UNIX)

#
# Add all the CMake modules we need
#
INCLUDE(DownloadProject)
# Set build options
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildOptions.cmake)
# Set compiler flags
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildFlags.cmake)
# Check for libraries
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildChecks.cmake)
# Set up CPack
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/ARGoSPackaging.cmake)

#
# Download uWebSockets library from github
#
download_project(PROJ     uWebSockets
    GIT_REPOSITORY      https://github.com/uNetworking/uWebSockets.git
    GIT_TAG             v0.17.1
    UPDATE_DISCONNECTED 1
)

EXECUTE_PROCESS(
    WORKING_DIRECTORY ${uWebSockets_SOURCE_DIR}/uSockets
    COMMAND make)

#
# Download nlohmann/json library from github
#
download_project(PROJ     nlohmann_json
GIT_REPOSITORY      https://github.com/nlohmann/json.git
GIT_TAG             v3.7.3
UPDATE_DISCONNECTED 1
)
## To disable tests inside JSON
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(${nlohmann_json_SOURCE_DIR})


# Logging Library
add_definitions(-DLOGURU_WITH_STREAMS)
add_subdirectory(${CMAKE_SOURCE_DIR}/libs/loguru)
set_property(TARGET loguru PROPERTY POSITION_INDEPENDENT_CODE ON)

#
# Setup Include paths
#
INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${uWebSockets_SOURCE_DIR}/src/
    ${uWebSockets_SOURCE_DIR}/uSockets/src/
    ${CMAKE_SOURCE_DIR}/libs/loguru
)

#
# create variables
#
SET(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/networkapi.cpp
    ${CMAKE_SOURCE_DIR}/src/networkapi_webserver.cpp
)

SET(HEADER_FILES 
    ${CMAKE_SOURCE_DIR}/src/networkapi.h
)

SET(REQUIRED_LIBRARIES
    argos3plugin_${ARGOS_BUILD_FOR}_entities
    ${ARGOS_NETWORKAPI_LIBRARIES}
    ${uWebSockets_SOURCE_DIR}/uSockets/uSockets.a
    nlohmann_json::nlohmann_json
    loguru
)

SET(TARGET_NAME
    argos3plugin_${ARGOS_BUILD_FOR}_networkapi
)

#
# Build Network-API Plugin
#
ADD_LIBRARY(${TARGET_NAME} SHARED ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(${TARGET_NAME} ${REQUIRED_LIBRARIES})

SET_TARGET_PROPERTIES( 
  ${TARGET_NAME}  
PROPERTIES 
  VERSION 0.1.0
  PUBLIC_HEADER "${HEADER_FILES}"
  ARCHIVE_OUTPUT_DIRECTORY "lib/argos3"
  LIBRARY_OUTPUT_DIRECTORY "lib/argos3"
  OUTPUT_NAME ${TARGET_NAME}
)

INSTALL(TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib/argos3
    ARCHIVE DESTINATION lib/argos3
    # Use if we want to publish header files
    # PUBLIC_HEADER DESTINATION include/argos3/plugins/simulator/visualizations/network-api
)

ADD_CUSTOM_TARGET(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/cmake/uninstall.cmake"
)


#
# Add examples / Tests to build
#
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/tests/controllers)