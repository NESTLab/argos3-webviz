/**
 * @file <argos3/plugins/simulator/visualizations/network-api/networkapi.h>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#ifndef ARGOS_NETWORKAPI_H
#define ARGOS_NETWORKAPI_H

/* Loguru with streams interface */
#define LOGURU_WITH_STREAMS 1

namespace argos {
  class CNetworkAPI;

  namespace NetworkAPI {
    class CWebServer;
    class CTimer;
    class CLogStream;
    enum class EExperimentState;
  }  // namespace NetworkAPI
}  // namespace argos

#include <argos3/core/simulator/entity/entity.h>
#include <nlohmann/json.hpp>

namespace argos {
  typedef nlohmann::json json;

  /****************************************/
  /****************************************/

  class CNetworkAPIOperationGenerateJSON : public CEntityOperation<
                                             CNetworkAPIOperationGenerateJSON,
                                             CNetworkAPI,
                                             json> {
   public:
    virtual ~CNetworkAPIOperationGenerateJSON() {}
  };

#define REGISTER_NETWORKAPI_ENTITY_OPERATION(ACTION, OPERATION, ENTITY) \
  REGISTER_ENTITY_OPERATION(ACTION, CNetworkAPI, OPERATION, json, ENTITY);

}  // namespace argos

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/visualization/visualization.h>
#include <argos3/core/utility/logging/argos_log.h>
#include <atomic>
#include <loguru.hpp>
#include <thread>
#include "networkapi_webserver.h"
#include "utility/CTimer.h"
#include "utility/EExperimentState.h"
#include "utility/LogStream.h"

namespace argos {
  /****************************************/
  /****************************************/

  class CNetworkAPI : public CVisualization {
   public:
    CNetworkAPI();
    ~CNetworkAPI();

    void Reset();
    void Destroy();
    void Execute();
    void Init(TConfigurationNode& t_tree);

    /**
     * @brief Plays the experiment.
     *
     * Internally sets a timer whose period corresponds to the
     * XML attribute 'ticks_per_second' in the .argos file.
     */
    void PlayExperiment();

    /**
     * @brief Pauses the experiment.
     *
     * The experiment can be resumed with PlayExperiment()
     */
    void PauseExperiment();

    /**
     * @brief Executes one experiment time step.
     *
     */
    void StepExperiment();

    /**
     * @brief Fast forwards the experiment.
     *
     */
    void FastForwardExperiment();

    /**
     * @brief Resets the state of the experiment to its state right after
     * initialization
     *
     */
    void ResetExperiment();

   private:
    /** Experiment State, declared atomic as it is used by many threads */
    std::atomic<NetworkAPI::EExperimentState> m_eExperimentState;

    /** Timer used for the loop */
    NetworkAPI::CTimer m_cTimer;

    /** Milliseconds required for one tick of simulator */
    std::chrono::milliseconds m_cSimulatorTickMillis;

    /** Webserver */
    NetworkAPI::CWebServer* m_cWebServer;

    /** THread to run simulation steps */
    std::thread m_cSimulationThread;

    /** Reference to the space state */
    CSpace& m_cSpace;

    /** Boolean for fastForwarding */
    bool m_bFastForwarding;

    /** number of frames to drop in Fast-forwarding */
    unsigned short m_unDrawFrameEvery;

    /** Log stream objects, to catch logs from Argos */
    NetworkAPI::CLogStream* m_pcLogStream;
    NetworkAPI::CLogStream* m_pcLogErrStream;

    /**
     * @brief Function which run in Simulation thread
     *
     */
    void SimulationThreadFunction();

    /**
     * @brief Function which broadcast experiment state
     *
     */
    void BroadcastExperimentState();
  };

};  // namespace argos

#endif