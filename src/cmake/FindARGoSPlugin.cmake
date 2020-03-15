# TODO: Add license 
#
#[=======================================================================[.rst:
FindARGoSPlugin
-------

Cmake function to locate Argos plugins.

Call this function like

::

  FindARGoSPlugin(qtopengl)

You can pass additional parameters as alternative names for different 
folder/headerfile


This module defines::

::

  ARGOS_PLUGIN_<plugin_name>_FOUND  - if false, do not try to link to the plugin
  ARGOS_<plugin_name>_LIBRARY       - the lib file of the plugin
  ARGOS_<plugin_name>_INCLUDE_DIR   - folders for all the header files

#]=======================================================================]

#
# Find the ARGoS package and load all ARGOS_* defines
find_package(PkgConfig)
pkg_check_modules(ARGOS REQUIRED argos3_simulator)

#
#
function(FindARGoSPlugin plugin_name)
    message("-- Checking for ARGoS plugin '" ${plugin_name} "'")

    foreach(_PLUGIN_NAME ${ARGV})
        # plugin name variations
        string(TOLOWER ${_PLUGIN_NAME} _PLUGIN_NAME_LOWER) # lower case
        string(TOUPPER ${_PLUGIN_NAME} _PLUGIN_NAME_UPPER) # upper case

        # All to the list of possible names
        list(APPEND PLUGIN_NAMES ${_PLUGIN_NAME})
        list(APPEND PLUGIN_NAMES ${_PLUGIN_NAME_LOWER})
        list(APPEND PLUGIN_NAMES ${_PLUGIN_NAME_UPPER})
    endforeach(_PLUGIN_NAME)

    # Generate all possible file names for lib and headers
    foreach(_PREFIX ${PLUGIN_NAMES})
        # headers
        foreach(_SUFFIX ".h" ".hpp")
            list(APPEND _POSSIBLE_PLUGIN_INCLUDE_NAMES "${_PREFIX}${_SUFFIX}")
        endforeach(_SUFFIX)

        # library
        foreach(_SUFFIX ".so" ".dylib")
            # Currently expecting only libargos3plugin_simulator_* library names
            list(APPEND _POSSIBLE_PLUGIN_LIBRARY_NAMES "libargos3plugin_simulator_${_PREFIX}${_SUFFIX}")
        endforeach(_SUFFIX)
    endforeach(_PREFIX)

    ####################### Find libraries ######################

    find_library(_PLUGIN_LIB
        NAMES
            ${_POSSIBLE_PLUGIN_LIBRARY_NAMES}
        PATHS
            ${ARGOS_LIBRARY_DIRS}
    )
    if(_PLUGIN_LIB)
        message("--   Library file found")
    else(_PLUGIN_LIB)
        message("--   Library NOT found")
    endif(_PLUGIN_LIB)

    ####################### Find headers ######################

    # Create list of possible paths for ROBOT plugin files
    # for path: robots/<plugin_name>/*[control_interface,real_robot,simulator]
    # ex: robots/kheperaiv/simulator
    foreach(_PREFIX ${PLUGIN_NAMES})
        foreach(_SUFFIX "control_interface" "real_robot" "simulator")
            list(APPEND _POSSIBLE_PATHS_4ROBOT "robots/${_PREFIX}/${_SUFFIX}")
        endforeach(_SUFFIX)
    endforeach(_PREFIX)

    # Find the headers for robot plugin
    find_path(_PLUGIN_INCLUDES_DIR_4ROBOT 
        NAMES
            ${_POSSIBLE_PLUGIN_INCLUDE_NAMES}
        PATHS
            ${ARGOS_INCLUDE_DIRS}/argos3/plugins
        PATH_SUFFIXES
            ${_POSSIBLE_PATHS_4ROBOT}
    )

    # Create list of possible paths for Simulator plugin files
    # for path: simulator/*[entities,media,physics_engines,visualizations]/<plugin_name>
    #ex simulator/physics_engines/dynamics3d
    foreach(_PREFIX "entities" "media" "physics_engines" "visualizations")
        foreach(_SUFFIX ${PLUGIN_NAMES})
            list(APPEND _POSSIBLE_PATHS_4SIM "simulator/${_PREFIX}/${_SUFFIX}")
        endforeach(_SUFFIX)
    endforeach(_PREFIX)

    # Find the headers for simulator plugin
    find_path(_PLUGIN_INCLUDES_DIR_4SIM 
        NAMES
            ${_POSSIBLE_PLUGIN_INCLUDE_NAMES}
        PATHS
            ${ARGOS_INCLUDE_DIRS}/argos3/plugins
        PATH_SUFFIXES
            ${_POSSIBLE_PATHS_4SIM}
    )

    # Check if it was a robot plugin
    if(_PLUGIN_INCLUDES_DIR_4ROBOT)
        message("--   Headers found")
        message("--   It is a Robot plugin")
        
        # Get parent folder
        get_filename_component(_PARENT_DIR ${_PLUGIN_INCLUDES_DIR_4ROBOT} DIRECTORY)

        # add all three paths 
        # robots/<plugin_name>/*[control_interface,real_robot,simulator] paths
        list(APPEND _INCLUDE_DIR "${_PARENT_DIR}/control_interface")
        list(APPEND _INCLUDE_DIR "${_PARENT_DIR}/real_robot")
        list(APPEND _INCLUDE_DIR "${_PARENT_DIR}/simulator")
    elseif(_PLUGIN_INCLUDES_DIR_4SIM)
        message("--   Headers found")
        message("--   It is a Simulator plugin")

    else(_PLUGIN_INCLUDES_DIR_4ROBOT)
        message("--   Headers NOT found")
    endif(_PLUGIN_INCLUDES_DIR_4ROBOT)

    # Success
    if(_INCLUDE_DIR AND _PLUGIN_LIB)
        ## Define parent scoped (export) variables
        set(ARGOS_${plugin_name}_FOUND 1 PARENT_SCOPE)
        set(ARGOS_${plugin_name}_LIBRARY ${_PLUGIN_LIB} PARENT_SCOPE)
        set(ARGOS_${plugin_name}_INCLUDE_DIR ${_INCLUDE_DIR} PARENT_SCOPE)
    endif(_INCLUDE_DIR AND _PLUGIN_LIB)

endfunction(FindARGoSPlugin)