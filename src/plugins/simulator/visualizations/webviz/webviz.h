/**
 * @file <argos3/plugins/simulator/visualizations/webviz/webviz.h>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#ifndef ARGOS_WEBVIZ_H
#define ARGOS_WEBVIZ_H

namespace argos {
  class CWebviz;

  namespace Webviz {
    class CWebServer;
    class CTimer;
    class CLogStream;
    enum class EExperimentState;
  }  // namespace Webviz
}  // namespace argos

#include <argos3/core/simulator/entity/entity.h>
#include <nlohmann/json.hpp>

namespace argos {
  typedef nlohmann::json json;

  /****************************************/
  /****************************************/

  class CWebvizOperationGenerateJSON
      : public CEntityOperation<CWebvizOperationGenerateJSON, CWebviz, json> {
   public:
    virtual ~CWebvizOperationGenerateJSON() {}
  };

#define REGISTER_WEBVIZ_ENTITY_OPERATION(ACTION, OPERATION, ENTITY) \
  REGISTER_ENTITY_OPERATION(ACTION, CWebviz, OPERATION, json, ENTITY);

}  // namespace argos

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/visualization/visualization.h>
#include <argos3/core/utility/configuration/argos_exception.h>
#include <argos3/core/utility/math/vector3.h>
#include <atomic>
#include <thread>
#include "utility/CTimer.h"
#include "utility/EExperimentState.h"
#include "utility/LogStream.h"
#include "webviz_webserver.h"

namespace argos {
  /****************************************/
  /****************************************/

  class CWebviz : public CVisualization {
   public:
    CWebviz();
    ~CWebviz();

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
    std::atomic<Webviz::EExperimentState> m_eExperimentState;

    /** Timer used for the loop */
    Webviz::CTimer m_cTimer;

    /** Thread to run simulation steps */
    std::thread m_cSimulationThread;

    /** Reference to the space state */
    CSpace& m_cSpace;

    /** Boolean for fastForwarding */
    std::atomic<bool> m_bFastForwarding;

    /** Milliseconds required for one tick of simulator */
    std::chrono::milliseconds m_cSimulatorTickMillis;

    /** Webserver */
    Webviz::CWebServer* m_cWebServer;

    /** number of frames to drop in Fast-forwarding */
    unsigned short m_unDrawFrameEvery;

    /** Log stream objects, to catch logs from Argos */
    Webviz::CLogStream* m_pcLogStream;
    Webviz::CLogStream* m_pcLogErrStream;

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