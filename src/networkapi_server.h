#ifndef ARGOS_NETWORKAPI_SERVER_H
#define ARGOS_NETWORKAPI_SERVER_H

namespace argos {
  class CNetworkAPI;
}

#include <argos3/core/simulator/visualization/visualization.h>
#include <sys/time.h>
#include <nlohmann/json.hpp>
#include <thread>
#include "App.h"

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
    /** Broadcasts a message to all the connected clients */
    void BroadcastMessage(std::string_view message);

    /** Broadcasts on event channel to all the connected clients */
    void EmitEvent(std::string_view event_name);

    /** Broadcasts game state to all the connected clients */
    void BroadcastState();

    /** Function to setup all routes and webhooks */
    void SetupWebApp(uWS::App& c_MyApp);

    /** Function to send JSON over HttpResponse */
    void SendJSON(uWS::HttpResponse<false>* pc_res, nlohmann::json c_json_data);

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
    /** HTTP Port to Listen to */
    UInt16 m_unPort;

    /* Data attached to each socket,
     * ws->getUserData returns one of these */
    struct m_sPerSocketData {};

    /* Experiment State */
    EExperimentState m_eExperimentState;

    /** Threads serving web requests */
    std::vector<uWS::WebSocket<false, true>*> m_vecWebSocketClients;
    /** List of all WebSocket clients connected */
    std::vector<std::thread*> m_vecWebThreads;

    /** True when fast forwarding */
    bool m_bFastForwarding;
  };

}  // namespace argos

#endif