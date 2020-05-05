
include(AddGoogleTests)

mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

# for keeping IDEs that support folders clean
set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)


add_subdirectory(experiments)

add_subdirectory(modules)


# Check Code coverage
setup_target_for_coverage_lcov(
    NAME coverage                               # New target name

    EXECUTABLE GTEST_COLOR=1 ctest -V --parallel# Executable in PROJECT_BINARY_DIR

    # BASE_DIRECTORY "."                        # Base directory for report
                                                #  (defaults to PROJECT_SOURCE_DIR)
    
    EXCLUDE "../build/*"                        # Patterns to exclude (can be relative
            "../src/argos3/*"                   #  to BASE_DIRECTORY, with CMake 3.4+)
            "../src/testing/*"
            "../src/tests/*"
            "/usr/*"
  
    # NO_DEMANGLE                                 # Don't demangle C++ symbols
                                                #  even if c++filt is found
)