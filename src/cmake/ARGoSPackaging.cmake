#
# Project name
#
project(argos3_webviz_packager)

#
# General CPack configuration
#


set(CPACK_PACKAGE_NAME "argos3plugin_simulator_webviz")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ARGoS-WebViz (Web interface plugin for ARGoS3)
  A Web interface plugin for  ARGoS multi-robot simulator (http://www.argos-sim.info/)")
set(CPACK_PACKAGE_HOMEPAGE "https://github.com/NESTLab/argos3-webviz/")
set(CPACK_PACKAGE_MAINTAINER "Prajankya Sonar <prajankya@gmail.com>")
set(CPACK_PACKAGE_VENDOR "NEST Lab, Worcester Polytechnic Institute")
set(CPACK_PACKAGE_CONTACT ${CPACK_PACKAGE_VENDOR})

# Version information
set(CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${PATCH_VERSION})
set(CPACK_PACKAGE_VERSION "${VERSION_STRING}")

# other common setup
set(CPACK_STRIP_FILES ON)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/../LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/../README.md")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${ARGOS_PROCESSOR_ARCH}")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})


#
# Configuration for the Debian generator
#
set(CPACK_DEBIAN_PACKAGE_DEPENDS "argos3_${ARGOS_BUILD_FOR} (>= 3.0.0)")
# autogenerate dependency information
set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${CPACK_PACKAGE_HOMEPAGE})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_MAINTAINER})
set(CPACK_DEBIAN_PACKAGE_SECTION "contrib/science")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${CPACK_PACKAGE_HOMEPAGE})

#
# Configuration for the RPM generator
#
set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
set(CPACK_RPM_PACKAGE_URL ${CPACK_PACKAGE_HOMEPAGE})
set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_REQUIRES "argos3_${ARGOS_BUILD_FOR} >= 3.0.0")

#
# Writing the output package name to file,
# no other way to give file name to TRAVIS
#
file(WRITE "${PROJECT_BINARY_DIR}/package_name.txt" "${CPACK_PACKAGE_FILE_NAME}")

#
# Time to call CPack
#
include(CPack)