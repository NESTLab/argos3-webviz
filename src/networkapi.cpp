#include "networkapi.h"
#include "helpers/utils.h"

// #define LOGURU_WITH_STREAMS 1
// #include <loguru.cpp>

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/space/space.h>

namespace argos {

  /****************************************/
  /****************************************/

  CNetworkAPI::CNetworkAPI() : m_cTimer() {
    m_cSimulationThread =
      std::thread(&CNetworkAPI::SimulationThreadFunction, this);
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::Init(TConfigurationNode &t_tree) {
    /* Setting up Logging */
    LOG_SCOPE_FUNCTION(INFO);

    unsigned short unPort;
    /* Parse options from the XML */
    GetNodeAttributeOrDefault(t_tree, "port", unPort, argos::UInt16(3000));

    /* Initialize Webserver */
    m_cWebServer = new argos::NetworkAPI::CWebServer(this, unPort);
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::Execute() {
    std::thread t2([&]() { m_cWebServer->Start(); });
    t2.join();
    m_cSimulationThread.join();

    // Finish all..
  }

  void CNetworkAPI::SimulationThreadFunction() {
    while (true) {
      if (
        m_eExperimentState ==
        NetworkAPI::EExperimentState::EXPERIMENT_PLAYING) {
        if (!m_cSimulator.IsExperimentFinished()) {
          /* Run one step */
          m_cSimulator.UpdateSpace();

          /* Broadcast current experiment state */
          m_cWebServer->Broadcast();

          /* Take the time now */
          m_cTimer.Stop();

          /* If the elapsed time is lower than the tick length, wait */
          if (m_cTimer.Elapsed() < m_cSimulatorTickMillis) {
            /* Sleep for the difference duration */
            std::this_thread::sleep_for(
              m_cSimulatorTickMillis - m_cTimer.Elapsed());
            /* Restart Timer */
            m_cTimer.Start();
          } else {
            LOG_S(WARNING) << "Clock tick took " << m_cTimer
                           << " sec, more than the expected "
                           << m_cSimulatorTickMillis.count() << " sec. "
                           << "Recovering in next cycle." << std::endl;
            m_cTimer.Start();
          }
        } else {
          LOG_S(INFO) << "Experiment finished\n";
          // EmitEvent("Experiment done");

          /* The experiment is finished */
          m_cSimulator.GetLoopFunctions().PostExperiment();
          ResetExperiment();
        }
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
      }
    }
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::PlayExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState !=
        NetworkAPI::EExperimentState::EXPERIMENT_INITIALIZED &&
      m_eExperimentState != NetworkAPI::EExperimentState::EXPERIMENT_PAUSED) {
      LOG_S(WARNING) << "CNetworkAPI::PlayExperiment() called in wrong state: "
                     << m_eExperimentState << std::endl;

      // silently return;
      return;
    }
    /* Change state and emit signals */
    m_eExperimentState = NetworkAPI::EExperimentState::EXPERIMENT_PLAYING;

    m_cSimulatorTickMillis = std::chrono::milliseconds(
      (long int)(CPhysicsEngine::GetSimulationClockTick() * 1000.0f));
    m_cTimer.Start();

    /* Change state and emit signals */
    m_eExperimentState = NetworkAPI::EExperimentState::EXPERIMENT_PLAYING;
    m_cWebServer->EmitEvent("Experiment playing", m_eExperimentState);
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::PauseExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState != NetworkAPI::EExperimentState::EXPERIMENT_PLAYING) {
      LOG_S(ERROR) << "CNetworkAPI::PauseExperiment() called in wrong "
                      "state: "
                   << argos::NetworkAPI::EExperimentStateToStr(
                        m_eExperimentState);
      throw std::runtime_error(
        "Cannot pause the experiment, current state : " +
        argos::NetworkAPI::EExperimentStateToStr(m_eExperimentState));
      return;
    }

    /* Change state and emit signals */
    m_eExperimentState = NetworkAPI::EExperimentState::EXPERIMENT_PAUSED;
    m_cWebServer->EmitEvent("Experiment paused", m_eExperimentState);
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::StepExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState !=
        NetworkAPI::EExperimentState::EXPERIMENT_INITIALIZED &&
      m_eExperimentState != NetworkAPI::EExperimentState::EXPERIMENT_PAUSED) {
      LOG_S(ERROR) << "CNetworkAPI::StepExperiment() called in wrong "
                      "state: "
                   << argos::NetworkAPI::EExperimentStateToStr(
                        m_eExperimentState);
      throw std::runtime_error(
        "Cannot Step the experiment, current state : " +
        argos::NetworkAPI::EExperimentStateToStr(m_eExperimentState));

      return;
    }

    if (!m_cSimulator.IsExperimentFinished()) {
      m_cSimulator.UpdateSpace();
      /* Broadcast current experiment state */
      m_cWebServer->Broadcast();

      /* Change state and emit signals */
      m_cWebServer->EmitEvent("Experiment step done", m_eExperimentState);
    } else {
      PauseExperiment();
      m_cWebServer->EmitEvent("Experiment done", m_eExperimentState);
    }
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::ResetExperiment() {
    m_cSimulator.Reset();
    m_eExperimentState = NetworkAPI::EExperimentState::EXPERIMENT_INITIALIZED;
    // delete m_pcGroundTexture;
    // if (m_bUsingFloorTexture) delete m_pcFloorTexture;
    // initializeGL();
    // update();
  }

  /****************************************/
  /****************************************/

  CNetworkAPI::~CNetworkAPI() { delete m_cWebServer; }

  /****************************************/
  /****************************************/

  void CNetworkAPI::Reset() {}

  /****************************************/
  /****************************************/

  void CNetworkAPI::Destroy() {}
  /****************************************/
  /****************************************/

  REGISTER_VISUALIZATION(
    CNetworkAPI,
    "network-api",
    "Prajankya [contact@prajankya.me]",
    "1.0",
    "Network API to render over network in clientside.",
    " -- .\n",
    "It allows the user to watch and modify the "
    "simulation as it's running in an\n"
    "intuitive way.\n\n"
    "REQUIRED XML CONFIGURATION\n\n"
    "  <visualization>\n"
    "    <network-api />\n"
    "  </visualization>\n\n"
    "OPTIONAL XML CONFIGURATION\n\n");
}  // namespace argos