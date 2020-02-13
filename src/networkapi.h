#ifndef ARGOS_NETWORKAPI_SERVER_H
#define ARGOS_NETWORKAPI_SERVER_H

namespace argos {
  class CNetworkAPI;
}
#include <argos3/core/simulator/visualization/visualization.h>
#include <sys/time.h>
#include <loguru.hpp>
#include "helpers/Timer.h"

#include "networkapi_webserver.h"

namespace argos {

  class CNetworkAPI : public CVisualization {
   public:
    virtual ~CNetworkAPI() {}

    virtual void Reset() {}

    virtual void Destroy() {}

    CNetworkAPI();

    virtual void Execute();

    virtual void Init(TConfigurationNode& t_tree);

    /**
     * Plays the experiment.
     * Internally sets a timer whose period corresponds to the
     * XML attribute 'ticks_per_second' in the .argos file.
     */
    void PlayExperiment();

    /**
     * Fast forwards the experiment.
     * Internally sets a timer whose period is 1ms.
     */
    void FastForwardExperiment();

    /**
     * Pauses the experiment.
     * The experiment can be resumed with PlayExperiment() or
     * FastForwardExperiment().
     */
    void PauseExperiment();

    /**
     * Executes one experiment time step.
     */
    void StepExperiment();

    /**
     * Resets the state of the experiment to its state right after
     * initialization.
     */
    void ResetExperiment();

   private:
    /** Function to run simulation step in realtime */
    void RealTimeStep();

   private:
    enum EExperimentState {
      EXPERIMENT_INITIALIZED = 0,
      EXPERIMENT_PLAYING,
      EXPERIMENT_FAST_FORWARDING,
      EXPERIMENT_PAUSED,
      EXPERIMENT_SUSPENDED,
      EXPERIMENT_DONE
    };

   private:
    /* Experiment State */
    EExperimentState m_eExperimentState;

    /** True when fast forwarding */
    bool m_bFastForwarding;

    /** Timer used for the loop */
    NetworkAPI::Timer m_cTimer;

    /** Milliseconds required for one tick of simulator */
    std::chrono::milliseconds m_cSimulatorTickMillis;

    /** Webserver */
    NetworkAPI::CWebServer m_cWebServer;
  };

}  // namespace argos

#endif