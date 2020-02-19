#ifndef ARGOS_NETWORKAPI_H
#define ARGOS_NETWORKAPI_H

#include <argos3/core/simulator/entity/composable_entity.h>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/visualization/visualization.h>
#include <sys/time.h>
#include <atomic>
#include <loguru.hpp>
#include <thread>
#include "helpers/CTimer.h"
#include "helpers/EExperimentState.h"
#include "helpers/LogStream.h"
#include "networkapi_webserver.h"

namespace argos {
  class CNetworkAPI : public CVisualization {
   public:
    CNetworkAPI();
    ~CNetworkAPI();

    void Reset();
    void Destroy();
    void Execute();
    void Init(TConfigurationNode& t_tree);

    /**
     * Plays the experiment.
     * Internally sets a timer whose period corresponds to the
     * XML attribute 'ticks_per_second' in the .argos file.
     */
    void PlayExperiment();

    /**
     * Pauses the experiment.
     * The experiment can be resumed with PlayExperiment()
     */
    void PauseExperiment();

    /** Executes one experiment time step. */
    void StepExperiment();

    /**
     * Fast forwards the experiment.
     */
    void FastForwardExperiment();

    /** Resets the state of the experiment to its state right after
     * initialization.*/
    void ResetExperiment();

   private:
    /* Experiment State, is used by many threads, so atomic */
    std::atomic<NetworkAPI::EExperimentState> m_eExperimentState;

    /** Timer used for the loop */
    argos::NetworkAPI::CTimer m_cTimer;

    /** Steps Counter */
    unsigned long long m_unStepCounter;

    /** Milliseconds required for one tick of simulator */
    std::chrono::milliseconds m_cSimulatorTickMillis;

    /** Webserver */
    argos::NetworkAPI::CWebServer* m_cWebServer;

    /** THread to run simulation steps */
    std::thread m_cSimulationThread;

    /** Reference to the space state */
    CSpace& m_cSpace;

    /** Boolean for fastForwarding */
    bool m_bFastForwarding;

    /** number of frames to drop in Fast-forwarding */
    unsigned short m_unDrawFrameEvery;

    /** Log stream objects, to catch logs from Argos */
    argos::NetworkAPI::CLogStream* m_pcLogStream;
    argos::NetworkAPI::CLogStream* m_pcLogErrStream;

    /** Function to run simulation step in realtime */
    void RealTimeStep();

    /** Thread to run in Simulation thread */
    void SimulationThreadFunction();

    /** Function to broadcast experiment state */
    void BroadcastExperimentState();
  };

}  // namespace argos

#endif