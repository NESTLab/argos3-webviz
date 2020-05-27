/**
 * @file
 * <argos3/plugins/simulator/visualizations/webviz/utility/EExperimentState.h>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#ifndef ARGOS_WEBVIZ_EEXPERIMENTSTATE_H
#define ARGOS_WEBVIZ_EEXPERIMENTSTATE_H

namespace argos {
  namespace Webviz {
    enum class EExperimentState {
      EXPERIMENT_INITIALIZED = 0,
      EXPERIMENT_PLAYING,
      EXPERIMENT_PAUSED,
      EXPERIMENT_FAST_FORWARDING,
      EXPERIMENT_DONE
    };

    inline static std::string EExperimentStateToStr(
      const EExperimentState& obj) {
      switch (obj) {
        case EExperimentState::EXPERIMENT_INITIALIZED:
          return "EXPERIMENT_INITIALIZED";
        case EExperimentState::EXPERIMENT_PLAYING:
          return "EXPERIMENT_PLAYING";
        case EExperimentState::EXPERIMENT_PAUSED:
          return "EXPERIMENT_PAUSED";
        case EExperimentState::EXPERIMENT_FAST_FORWARDING:
          return "EXPERIMENT_FAST_FORWARDING";
        case EExperimentState::EXPERIMENT_DONE:
          return "EXPERIMENT_DONE";
      }
    };
  }  // namespace Webviz
}  // namespace argos

#endif