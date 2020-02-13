#include "networkapi.h"
#include "helpers/utils.h"

// #define LOGURU_WITH_STREAMS 1
// #include <loguru.cpp>

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/space/space.h>

namespace argos {

  /****************************************/
  /****************************************/

  CNetworkAPI::CNetworkAPI()
      : m_bFastForwarding(false), m_cTimer(), m_cWebServer() {}

  /****************************************/
  /****************************************/

  void CNetworkAPI::Init(TConfigurationNode &t_tree) {
    /* Setting up Logging */
    LOG_SCOPE_FUNCTION(INFO);

    /* Parse options from the XML */
    // GetNodeAttributeOrDefault(
    //   t_tree, "port", this->m_unPort, argos::UInt16(3000));
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::Execute() { m_cWebServer.Start(); }

  /****************************************/
  /****************************************/

  void CNetworkAPI::RealTimeStep() {
    /* Run one step */
    m_cSimulator.UpdateSpace();

    /* Take the time now */
    m_cTimer.Stop();

    /* If the elapsed time is lower than the tick length, wait */
    if (m_cTimer.Elapsed() < m_cSimulatorTickMillis) {
      /* Sleep for the difference duration */
      std::this_thread::sleep_for(m_cSimulatorTickMillis - m_cTimer.Elapsed());
      /* Restart Timer */
      m_cTimer.Start();
    } else {
      LOG_S(WARNING) << "Clock tick took " << m_cTimer
                     << " sec, more than the expected ";
      //  << m_cSimulatorTickMillis << " sec." << std::endl;
    }
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::PlayExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState != EXPERIMENT_INITIALIZED &&
      m_eExperimentState != EXPERIMENT_PAUSED) {
      LOG_S(ERROR) << "CNetworkAPI::PlayExperiment() called in wrong state: "
                   << m_eExperimentState << std::endl;

      return;
    }
    /* Change state and emit signals */
    m_eExperimentState = EXPERIMENT_PLAYING;

    m_bFastForwarding = false;

    m_cSimulatorTickMillis = std::chrono::milliseconds(
      (long int)(CPhysicsEngine::GetSimulationClockTick() * 1000.0f));
    m_cTimer.Start();

    if (m_eExperimentState == EXPERIMENT_INITIALIZED) {
      /* The experiment has just been started */
      // EmitEvent("Experiment started");
    }
    /* Change state and emit signals */
    m_eExperimentState = EXPERIMENT_PLAYING;
    // EmitEvent("Experiment playing");

    while (!m_cSimulator.IsExperimentFinished()) {
      RealTimeStep();
    }
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::FastForwardExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState != EXPERIMENT_INITIALIZED &&
      m_eExperimentState != EXPERIMENT_PAUSED) {
      LOG_S(ERROR) << "CNetworkAPI::FastForwardExperiment() called in "
                      "wrong state: "
                   << m_eExperimentState << std::endl;
      return;
    }

    m_bFastForwarding = true;
    // if (nTimerId != -1) killTimer(nTimerId);
    // nTimerId = startTimer(1);

    if (m_eExperimentState == EXPERIMENT_INITIALIZED) {
      /* The experiment has just been started */
      // EmitEvent("Experiment started");
    }
    /* Change state and emit signals */
    m_eExperimentState = EXPERIMENT_FAST_FORWARDING;
    // EmitEvent("Experiment fast-forwarding");
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::PauseExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState != EXPERIMENT_PLAYING &&
      m_eExperimentState != EXPERIMENT_FAST_FORWARDING) {
      LOG_S(ERROR) << "CNetworkAPI::PauseExperiment() called in wrong "
                      "state: "
                   << m_eExperimentState << std::endl;
      return;
    }

    m_bFastForwarding = false;
    // if (nTimerId != -1) killTimer(nTimerId);
    // nTimerId = -1;

    /* Change state and emit signals */
    m_eExperimentState = EXPERIMENT_PAUSED;
    // EmitEvent("Experiment paused");
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::StepExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState != EXPERIMENT_INITIALIZED &&
      m_eExperimentState != EXPERIMENT_PAUSED) {
      LOG_S(ERROR) << "CNetworkAPI::StepExperiment() called in wrong "
                      "state: "
                   << m_eExperimentState << std::endl;
      return;
    }

    // TODO: Implement Fast forwarding step
    // if (!m_cSimulator.IsExperimentFinished()) {
    //   m_cSimulator.UpdateSpace();
    //   if (m_bFastForwarding) {
    //     /* Frame dropping happens only in fast-forward */
    //     m_nFrameCounter = m_nFrameCounter % m_nDrawFrameEvery;
    //     if (m_nFrameCounter == 0) {
    //       // update();
    //     }
    //     ++m_nFrameCounter;
    //   } else {
    //     // update();
    //   }
    //   emit StepDone(m_cSpace.GetSimulationClock());
    // } else {
    //   PauseExperiment();
    //   emit ExperimentDone();
    // }

    if (!m_cSimulator.IsExperimentFinished()) {
      m_cSimulator.UpdateSpace();

      /* Change state and emit signals */
      // EmitEvent("Experiment step done");
    } else {
      PauseExperiment();
      // EmitEvent("Experiment done");
    }
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::ResetExperiment() {
    m_cSimulator.Reset();
    // delete m_pcGroundTexture;
    // if (m_bUsingFloorTexture) delete m_pcFloorTexture;
    // initializeGL();
    // update();
  }

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