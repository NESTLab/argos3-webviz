#ifndef NETWORKAPI_SERVER_H
#define NETWORKAPI_SERVER_H

namespace argos {
  class CNetworkAPI;
}

#include <argos3/core/simulator/visualization/visualization.h>
#include <sys/time.h>
#include <thread>
#include "App.h"

namespace argos {

  class CNetworkAPI : public CVisualization {
   public:
    CNetworkAPI() : m_vecWebThreads(std::thread::hardware_concurrency()) {}

    virtual ~CNetworkAPI() {}

    virtual void Reset() {}

    virtual void Destroy() {}

    virtual void Execute();

    virtual void Init(TConfigurationNode& t_tree);

    /**
     * Plays the experiment.
     * Internally sets a timer whose period corresponds to the
     * XML attribute 'ticks_per_second' in the .argos file.
     */
    void PlayExperiment();

    // /**
    //  * Fast forwards the experiment.
    //  * Internally sets a timer whose period is 1ms.
    //  */
    // void FastForwardExperiment();

    // /**
    //  * Executes one experiment time step.
    //  */
    // void StepExperiment();

    // /**
    //  * Pauses the experiment.
    //  * The experiment can be resumed with PlayExperiment() or
    //  * FastForwardExperiment().
    //  */
    // void PauseExperiment();

    // /**
    //  * Terminates the execution of the experiment.
    //  */
    // void TerminateExperiment();

    // /**
    //  * Resets the state of the experiment to its state right after
    //  * initialization.
    //  */
    // void ResetExperiment();

    // /**
    //  * Suspends an experiment due to an error.
    //  */
    // void SuspendExperiment();

    // /**
    //  * Resumes a suspended experiment.
    //  */
    // void ResumeExperiment();

   private:
    /** Broadcasts a message to all the connected clients */
    void BroadcastMessage(std::string_view message);

    /** Broadcasts on event channel to all the connected clients */
    void EmitEvent(std::string_view event_name);

    /** Broadcasts game state to all the connected clients */
    void BroadcastState();

    /** Performs a simulation step the normal way */
    void NormalStep();

    /** Performs a simulation step respecting the real-time constraint */
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
    inline std::string EExperimentStateToString(EExperimentState type) {
      switch (type) {
        case EXPERIMENT_INITIALIZED:
          return "EXPERIMENT_INITIALIZED";
        case EXPERIMENT_PLAYING:
          return "EXPERIMENT_PLAYING";
        case EXPERIMENT_FAST_FORWARDING:
          return "EXPERIMENT_FAST_FORWARDING";
        case EXPERIMENT_PAUSED:
          return "EXPERIMENT_PAUSED";
        case EXPERIMENT_SUSPENDED:
          return "EXPERIMENT_SUSPENDED";
        case EXPERIMENT_DONE:
          return "EXPERIMENT_DONE";
        default:
          return "unknown";
      }
    }

   private:
    typedef void (CNetworkAPI::*TStepFunction)();

   private:
    /** HTTP Port to Listen to */
    UInt16 m_unPort;

    /* Data attached to each socket,
     * ws->getUserData returns one of these */
    struct m_sPerSocketData {};

    /* Data to be sent to each client over websockets */
    struct m_sGameState {
      EExperimentState eState;
      bool bFastForward;
    };
    m_sGameState m_tGameState;

    /** Threads serving web requests */
    std::vector<uWS::WebSocket<false, true>*> m_vecWebSocketClients;

    /** List of all WebSocket clients connected */
    std::vector<std::thread*> m_vecWebThreads;

    /** Pointer to step function */
    TStepFunction m_tStepFunction;

    /** The length of a clock tick */
    ::timeval m_tStepClockTime;

    /** The starting time of a clock tick */
    ::timeval m_tStepStartTime;

    /** The end time of a clock tick*/
    ::timeval m_tStepEndTime;

    /** The elapsed time between start and end of a clock tick */
    ::timeval m_tStepElapsedTime;

    /** The time to wait to complete a clock tick */
    ::timeval m_tStepWaitTime;
  };

}  // namespace argos

#endif