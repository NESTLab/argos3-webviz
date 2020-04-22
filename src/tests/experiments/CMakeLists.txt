
#
# Using timeout to stop argos experiments after some time
#
macro(add_experiment_test TEST_NAME EXP_FILE TIMEOUT)
  add_test(NAME experiments.${TEST_NAME} 
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND bash -c "timeout ${TIMEOUT} argos3 -c ${EXP_FILE} || ( [[ $? -eq 124 ]])")
endmacro()

# add experiments like,
#
# add_experiment_test(empty_experiment empty.argos 2s)

# List all the experiments to test
add_subdirectory(empty_experiment)