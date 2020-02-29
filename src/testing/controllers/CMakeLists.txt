add_library(diffusion MODULE diffusion.h diffusion.cpp)
target_link_libraries(diffusion
  argos3core_simulator
  argos3plugin_simulator_kheperaiv
  argos3plugin_simulator_genericrobot)
