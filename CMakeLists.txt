#
# Set minimum required version
#
cmake_minimum_required(VERSION 3.0.2)
project(network-api)
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
# Make sure we are under Unix
#
if(NOT UNIX)
    message(FATAL_ERROR "Network-API compiles only under UNIX, sorry!")
endif(NOT UNIX)

#
# Add all the CMake modules we need
#
include(DownloadProject)
# Set build options
include(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildOptions.cmake)
# Set compiler flags
include(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildFlags.cmake)
# Check for libraries
include(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildChecks.cmake)
# Set up CPack
include(${CMAKE_SOURCE_DIR}/cmake/ARGoSPackaging.cmake)


#
# Download uWebSockets library from github
#
download_project(PROJ     uWebSockets
    GIT_REPOSITORY      https://github.com/uNetworking/uWebSockets.git
    GIT_TAG             v0.17.1
    UPDATE_DISCONNECTED 1
)


#
# Setup Include paths
#
include_directories(
    ${CMAKE_SOURCE_DIR} 
    ${CMAKE_BINARY_DIR}
    ${uWebSockets_SOURCE_DIR}/src/
    ${uWebSockets_SOURCE_DIR}/uSockets/src/
)


#
# Headers
# 
set(ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI 
    ${CMAKE_SOURCE_DIR}/include/networkapi_server.h)

#
# Source files
#
set(ARGOS3_SOURCES_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI
    ${ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI}
    ${CMAKE_SOURCE_DIR}/src/networkapi_server.cpp)

#
# Build Network API
#
add_library(argos3plugin_${ARGOS_BUILD_FOR}_networkapi SHARED
    ${ARGOS3_SOURCES_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI})

# Run with -std=c++17 
set_target_properties(argos3plugin_${ARGOS_BUILD_FOR}_networkapi PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
)

target_link_libraries(argos3plugin_${ARGOS_BUILD_FOR}_networkapi 
    argos3plugin_${ARGOS_BUILD_FOR}_entities
    ${ARGOS_NETWORKAPI_LIBRARIES}
)

#
# Install in system
#

# install(DIRECTORY icons textures models DESTINATION include/argos3/plugins/simulator/visualizations/network-api)
install(FILES ${ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI} 
    DESTINATION include/argos3/plugins/simulator/visualizations/network-api)
install(TARGETS argos3plugin_${ARGOS_BUILD_FOR}_networkapi
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib/argos3
    ARCHIVE DESTINATION lib/argos3
)


# #
# # Add Uninstall Target
# #
# CONFIGURE_FILE(
#   "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake"
#   IMMEDIATE @ONLY)
# ADD_CUSTOM_TARGET(uninstall
#   "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake")


# # ########### Add uninstall target ###############

# add_custom_command(
#     WORKING_DIR ${CMAKE_BINARY_DIR}
#     COMMAND "xargs rm < install_manifest.txt"
# )
add_custom_target(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/cmake/uninstall.cmake"
)

# CONFIGURE_FILE(
#   "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake"
#   IMMEDIATE @ONLY)
# ADD_CUSTOM_TARGET(uninstall
#   "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake") 
