#
# List of all the entities files to compile
#
SET(ENTITIES_SOURCE_FILES
  entity/webviz_box.cpp
  entity/webviz_light.cpp
  entity/webviz_floor.cpp
  entity/webviz_cylinder.cpp
  entity/webviz_footbot.cpp
)

## External entities (added as plugins)
FindARGoSPlugin(KheperaIV kheperaiv_entity)
IF(ARGOS_KheperaIV_FOUND)
  LIST(APPEND ENTITIES_SOURCE_FILES entity/webviz_kheperaiv.cpp)
ENDIF(ARGOS_KheperaIV_FOUND)

############################## PROJECT SETUP ##############################
#
# Download uWebSockets library from github
#
download_project(PROJ     uWebSockets
  GIT_REPOSITORY      https://github.com/uNetworking/uWebSockets.git
  GIT_TAG             v0.17.3
  UPDATE_DISCONNECTED 1
)

# Build uSockets(inside uWebSockets directory)

## Optionally enable Open SSL for SSL support
if(OpenSSL_FOUND)
  message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
  set(OPENSSL_DEP WITH_OPENSSL=1)
endif(OpenSSL_FOUND)

EXECUTE_PROCESS(
  WORKING_DIRECTORY ${uWebSockets_SOURCE_DIR}/uSockets
  COMMAND make default ${OPENSSL_DEP})

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

#
# Setup Include paths
#
INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${uWebSockets_SOURCE_DIR}/src/
  ${uWebSockets_SOURCE_DIR}/uSockets/src/
)


#
# Headers
#
# argos3/plugins/simulator/visualizations/webviz
SET(ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_WEBVIZ
  webviz.h
  webviz_webserver.h)

#
# Source files
#
# Common for all builds
SET(ARGOS3_SOURCES_PLUGINS_SIMULATOR_VISUALIZATIONS_WEBVIZ
  ${ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_WEBVIZ}
  webviz.cpp
  webviz_webserver.cpp
  ${ENTITIES_SOURCE_FILES})


SET(TARGET_NAME
  argos3plugin_${ARGOS_BUILD_FOR}_webviz
)

#
# Build WebViz Plugin
#
ADD_LIBRARY(${TARGET_NAME} SHARED ${ARGOS3_SOURCES_PLUGINS_SIMULATOR_VISUALIZATIONS_WEBVIZ})

TARGET_LINK_LIBRARIES(${TARGET_NAME} 
  argos3plugin_${ARGOS_BUILD_FOR}_entities
  ${ARGOS_WEBVIZ_LIBRARIES}
  ${uWebSockets_SOURCE_DIR}/uSockets/uSockets.a
  nlohmann_json::nlohmann_json
)

SET_TARGET_PROPERTIES( 
  ${TARGET_NAME}  
PROPERTIES 
  VERSION ${VERSION_STRING}
  PUBLIC_HEADER "${ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_WEBVIZ}"
  ARCHIVE_OUTPUT_DIRECTORY "lib/argos3"
  LIBRARY_OUTPUT_DIRECTORY "lib/argos3"
  OUTPUT_NAME ${TARGET_NAME}
)

# Install main project libs and headers
INSTALL(TARGETS ${TARGET_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/argos3
  ARCHIVE DESTINATION lib/argos3
  # Use if we want to publish header files
  PUBLIC_HEADER DESTINATION include/argos3/plugins/simulator/visualizations/webviz
)

# Stop compiling on first error
TARGET_COMPILE_OPTIONS(${TARGET_NAME} PRIVATE -Wfatal-errors)