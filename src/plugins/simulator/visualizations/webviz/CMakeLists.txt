#
# List of all the entities files to compile
#
set(ENTITIES_SOURCE_FILES
  entity/webviz_box.cpp
  entity/webviz_light.cpp
  entity/webviz_floor.cpp
  entity/webviz_cylinder.cpp
  entity/webviz_footbot.cpp
)

## External entities (added as plugins)
FindARGoSPlugin(KheperaIV kheperaiv_entity)
if(ARGOS_KheperaIV_FOUND)
  list(APPEND ENTITIES_SOURCE_FILES entity/webviz_kheperaiv.cpp)
endif(ARGOS_KheperaIV_FOUND)

############################## PROJECT SETUP ##############################
#
# Download uWebSockets library from github
#
download_project(PROJ     uWebSockets
  GIT_REPOSITORY      https://github.com/uNetworking/uWebSockets.git
  GIT_TAG             v0.17.3
  UPDATE_DISCONNECTED 1
)

## Optionally enable Open SSL for SSL support
find_package(OpenSSL)
if(OpenSSL_FOUND)
  message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
  set(OPENSSL_DEP WITH_OPENSSL=1)
endif(OpenSSL_FOUND)

# Build uSockets(inside uWebSockets directory)
execute_process(
  WORKING_DIRECTORY ${uWebSockets_SOURCE_DIR}/uSockets
  COMMAND make ${OPENSSL_DEP})

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
add_subdirectory(${nlohmann_json_SOURCE_DIR} ${CMAKE_BINARY_DIR}/nlohmann-build)

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
# argos3/plugins/simulator/visualizations/webviz
set(ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_WEBVIZ
  webviz.h
  webviz_webserver.h)

#
# Source files
#
# Common for all builds
set(ARGOS3_SOURCES_PLUGINS_SIMULATOR_VISUALIZATIONS_WEBVIZ
  ${ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_WEBVIZ}
  webviz.cpp
  webviz_webserver.cpp
  ${ENTITIES_SOURCE_FILES})

set(TARGET_NAME
  argos3plugin_${ARGOS_BUILD_FOR}_webviz
)

#
# Setup config file
#
set(PLUGIN_FOLDER plugins/simulator/visualizations/webviz)
#
# Create config file with settings that must be accessed from C++
#
configure_file(
  config.h.in
  config.h
  @ONLY)
#
# make generated config.h file's folder visible to compiler
#
include_directories(${CMAKE_BINARY_DIR}/${PLUGIN_FOLDER})

#
# Install config file in output location
#
install(
FILES 
  ${CMAKE_BINARY_DIR}/${PLUGIN_FOLDER}/config.h 
DESTINATION 
  include/argos3/${PLUGIN_FOLDER}
)

#
# Build WebViz Plugin
#
add_library(${TARGET_NAME} SHARED ${ARGOS3_SOURCES_PLUGINS_SIMULATOR_VISUALIZATIONS_WEBVIZ})

target_link_libraries(${TARGET_NAME} 
  argos3plugin_${ARGOS_BUILD_FOR}_entities
  ${ARGOS_WEBVIZ_LIBRARIES}
  ${uWebSockets_SOURCE_DIR}/uSockets/uSockets.a
  nlohmann_json::nlohmann_json
  ${OPENSSL_LIBRARIES}
)

set_target_properties( 
  ${TARGET_NAME}  
PROPERTIES 
  VERSION ${VERSION_STRING}
  PUBLIC_HEADER "${ARGOS3_HEADERS_PLUGINS_SIMULATOR_VISUALIZATIONS_WEBVIZ}"
  ARCHIVE_OUTPUT_DIRECTORY "lib/argos3"
  LIBRARY_OUTPUT_DIRECTORY "lib/argos3"
  OUTPUT_NAME ${TARGET_NAME}
)

# Install main project libs and headers
install(TARGETS ${TARGET_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/argos3
  ARCHIVE DESTINATION lib/argos3
  # Use if we want to publish header files
  PUBLIC_HEADER DESTINATION include/argos3/${PLUGIN_FOLDER}
)

# Stop compiling on first error
target_compile_options(${TARGET_NAME} PRIVATE -Wfatal-errors)