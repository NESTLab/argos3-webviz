
#
# Download uWebSockets library from github
#
download_project(PROJ     uWebSockets
  GIT_REPOSITORY      https://github.com/uNetworking/uWebSockets.git
  GIT_TAG             v0.17.1
  UPDATE_DISCONNECTED 1
)

# Build uSockets(inside uWebSockets directory)
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
SET(JSON_BuildTests OFF CACHE INTERNAL "")
SET(JSON_Install OFF CACHE INTERNAL "")
ADD_SUBDIRECTORY(${nlohmann_json_SOURCE_DIR} ${CMAKE_BINARY_DIR}/nlohmann-build)


# Logging Library
ADD_DEFINITIONS(-DLOGURU_WITH_STREAMS)
ADD_SUBDIRECTORY(libs/loguru)
SET_PROPERTY(TARGET loguru PROPERTY POSITION_INDEPENDENT_CODE ON)

#
# Setup Include paths
#
INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${uWebSockets_SOURCE_DIR}/src/
  ${uWebSockets_SOURCE_DIR}/uSockets/src/
  libs/loguru
)


#
# create variables
#


#
# Headers
#
# argos3/plugins/simulator/visualizations/network-api
SET(ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI
  networkapi.h
  networkapi_webserver.h)

#
# Source files
#
# Common for all builds
SET(ARGOS3_SOURCES_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI
  ${ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI}
  networkapi.cpp
  networkapi_webserver.cpp)


SET(TARGET_NAME
  argos3plugin_${ARGOS_BUILD_FOR}_networkapi
)

#
# Build Network-API Plugin
#
ADD_LIBRARY(${TARGET_NAME} SHARED  ${ARGOS3_SOURCES_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI})

TARGET_LINK_LIBRARIES(${TARGET_NAME} 
  argos3plugin_${ARGOS_BUILD_FOR}_entities
  ${ARGOS_NETWORKAPI_LIBRARIES}
  ${uWebSockets_SOURCE_DIR}/uSockets/uSockets.a
  nlohmann_json::nlohmann_json
  loguru
)

SET_TARGET_PROPERTIES( 
  ${TARGET_NAME}  
PROPERTIES 
  VERSION 0.1.0
  PUBLIC_HEADER "${ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_NETWORKAPI}"
  ARCHIVE_OUTPUT_DIRECTORY "lib/argos3"
  LIBRARY_OUTPUT_DIRECTORY "lib/argos3"
  OUTPUT_NAME ${TARGET_NAME}
)


TARGET_COMPILE_OPTIONS(${TARGET_NAME} PRIVATE -Wfatal-errors)

INSTALL(TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib/argos3
    ARCHIVE DESTINATION lib/argos3
    # Use if we want to publish header files
    PUBLIC_HEADER DESTINATION include/argos3/plugins/simulator/visualizations/network-api
)


# Already added in root CMake
# ## Add Uninstall target
# ADD_CUSTOM_TARGET(uninstall
#   "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/cmake/uninstall.cmake"
# )
