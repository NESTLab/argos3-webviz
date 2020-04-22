add_library(user_loop_functions MODULE user_loop_functions.h user_loop_functions.cpp)

target_link_libraries(user_loop_functions
  argos3core_simulator
  nlohmann_json::nlohmann_json)
