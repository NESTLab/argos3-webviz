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
# Set the path additional cmake files must be searched for
#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

#
# Set CMake policies to select wanted behaviors
#
# Use new policies introduced up to this version
cmake_policy(VERSION 2.8.12)
if(POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif(POLICY CMP0042)

#
# Include path points to the base source dir
#
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

#
# Make sure we are under Unix
#
if(NOT UNIX)
  message(FATAL_ERROR "ARGoS compiles only under UNIX, sorry!")
endif(NOT UNIX)

#
# Set build options
#
include(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildOptions.cmake)

#
# Set compiler flags
#
include(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildFlags.cmake)

#
# Check for libraries
#
include(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildChecks.cmake)

#
# Set up CPack for later use
#
include(${CMAKE_SOURCE_DIR}/cmake/ARGoSPackaging.cmake)





#
# Headers
# NETWORKAPI
set(ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI
include/networkapi_server.h)

#
# Source files
#
# Common for all builds
set(ARGOS3_SOURCES_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI
  ${ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI}
  src/networkapi_server.cpp)

#
# Create Network API
#
add_library(argos3plugin_${ARGOS_BUILD_FOR}_networkapi SHARED
  ${ARGOS3_SOURCES_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI})
target_link_libraries(argos3plugin_${ARGOS_BUILD_FOR}_networkapi argos3plugin_${ARGOS_BUILD_FOR}_entities ${ARGOS_NETWORKAPI_LIBRARIES})

# install(DIRECTORY icons textures models DESTINATION include/argos3/plugins/simulator/visualizations/network-api)
install(FILES ${ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI} DESTINATION include/argos3/plugins/simulator/visualizations/network-api)
install(TARGETS argos3plugin_${ARGOS_BUILD_FOR}_networkapi
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/argos3
  ARCHIVE DESTINATION lib/argos3
)


########### Add uninstall target ###############
CONFIGURE_FILE(
  "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake") 