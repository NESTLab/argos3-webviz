#ifndef ARGOS_NETWORKAPI_EEXPERIMENTSTATE_H
#define ARGOS_NETWORKAPI_EEXPERIMENTSTATE_H

namespace argos {
  namespace NetworkAPI {
    enum class EExperimentState {
      EXPERIMENT_INITIALIZED = 0,
      EXPERIMENT_PLAYING,
      EXPERIMENT_PAUSED,
      EXPERIMENT_FAST_FORWARDING
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
        default:
          return "unknown";
      }
      return "";
    };

    inline std::ostream& operator<<(
      std::ostream& os, const EExperimentState& obj) {
      switch (obj) {
        case EExperimentState::EXPERIMENT_INITIALIZED:
          os << "EXPERIMENT_INITIALIZED";
        case EExperimentState::EXPERIMENT_PLAYING:
          os << "EXPERIMENT_PLAYING";
        case EExperimentState::EXPERIMENT_PAUSED:
          os << "EXPERIMENT_PAUSED";
        case EExperimentState::EXPERIMENT_FAST_FORWARDING:
          os << "EXPERIMENT_FAST_FORWARDING";
        default:
          os << "unknown";
      }
      return os;
    };
  }  // namespace NetworkAPI
}  // namespace argos

#endif