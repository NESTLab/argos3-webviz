add_library(diffusion MODULE diffusion.h diffusion.cpp)
target_link_libraries(diffusion
  argos3core_simulator
  argos3plugin_${ARGOS_BUILD_FOR}_kheperaiv
  argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)
