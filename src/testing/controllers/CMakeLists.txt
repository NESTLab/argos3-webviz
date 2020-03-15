add_library(footbot_diffusion MODULE footbot_diffusion.h footbot_diffusion.cpp)
target_link_libraries(footbot_diffusion
  argos3core_simulator
  argos3plugin_${ARGOS_BUILD_FOR}_footbot
  argos3plugin_${ARGOS_BUILD_FOR}_genericrobot)
