#include "networkapi.h"
#include <typeinfo>  // operator typeid
#include "helpers/utils.h"

namespace argos {

  /****************************************/
  /****************************************/

  CNetworkAPI::CNetworkAPI() : m_cTimer(), m_cSpace(m_cSimulator.GetSpace()) {
    m_cSimulationThread =
      std::thread(&CNetworkAPI::SimulationThreadFunction, this);
    m_bFastForwarding = false;

    /* Disable Colors in LOG, as its going to be shown in web and not in CLI */
    argos::LOG.DisableColoredOutput();
    argos::LOGERR.DisableColoredOutput();

    /* Initialize the LOG streams from Execute thread */
    m_pcLogStream = new argos::NetworkAPI::CLogStream(
      argos::LOG.GetStream(), [this](std::string str_logData) {
        // LOG_S(INFO) << "ARGOS_LOG:" << str_logData;
        m_cWebServer->EmitLog("LOG", str_logData);
      });

    m_pcLogErrStream = new argos::NetworkAPI::CLogStream(
      argos::LOGERR.GetStream(), [this](std::string str_logData) {
        // LOG_S(INFO) << "ARGOS_LOGERR:" << str_logData;
        m_cWebServer->EmitLog("LOGERR", str_logData);
      });
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::Init(TConfigurationNode& t_tree) {
    /* Setting up Logging */
    LOG_SCOPE_FUNCTION(INFO);

    unsigned short unPort;
    unsigned short unBroadcastFrequency;

    /* Parse options from the XML */
    GetNodeAttributeOrDefault(t_tree, "port", unPort, argos::UInt16(3000));
    GetNodeAttributeOrDefault(
      t_tree, "broadcast_frequency", unBroadcastFrequency, argos::UInt16(10));

    GetNodeAttributeOrDefault(
      t_tree, "ff_draw_frames_every", m_unDrawFrameEvery, argos::UInt16(2));

    /* Initialize Webserver */
    m_cWebServer =
      new argos::NetworkAPI::CWebServer(this, unPort, unBroadcastFrequency);
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::Execute() {
    /* Start the frame counter from 0 */
    m_unStepCounter = 0;

    std::thread t2([&]() { m_cWebServer->Start(); });
    t2.join();
    m_cSimulationThread.join();

    // Finish all..
  }

  void CNetworkAPI::SimulationThreadFunction() {
    while (true) {
      if (
        m_eExperimentState ==
          argos::NetworkAPI::EExperimentState::EXPERIMENT_PLAYING ||
        m_eExperimentState ==
          argos::NetworkAPI::EExperimentState::EXPERIMENT_FAST_FORWARDING) {
        if (!m_cSimulator.IsExperimentFinished()) {
          /* Run user's pre step function */
          m_cSimulator.GetLoopFunctions().PreStep();

          /* For non-fastforwarding mode, steps is 1 */
          int unFFStepCounter = 1;

          if (m_bFastForwarding) {
            /* Number of frames to drop in fast-forward */
            unFFStepCounter = m_unDrawFrameEvery;
          }

          /* Loop for steps (multiple for fast-forward) */
          while (unFFStepCounter > 0 && !m_cSimulator.IsExperimentFinished()) {
            /* Run one step */
            m_cSimulator.UpdateSpace();

            /* Steps counter in this while loop */
            --unFFStepCounter;

            /* Increment global Counter */
            m_unStepCounter++;
          }

          /* Broadcast current experiment state */
          BroadcastExperimentState();

          /* Run user's post step function */
          m_cSimulator.GetLoopFunctions().PostStep();

          /* Experiment done while in while loop */
          if (m_cSimulator.IsExperimentFinished()) {
            /* The experiment is done */
            m_cSimulator.GetLoopFunctions().PostExperiment();

            ResetExperiment();

            /* Change state and emit signals */
            m_cWebServer->EmitEvent("Experiment done", m_eExperimentState);
            LOG_S(INFO) << "Experiment done\n";
            return; /* Go back once done */
          }

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
          /* The experiment is already done */
          m_cSimulator.GetLoopFunctions().PostExperiment();

          ResetExperiment();

          /* Change state and emit signals */
          m_cWebServer->EmitEvent("Experiment done", m_eExperimentState);
          LOG_S(INFO) << "Experiment done\n";
        }
      } else {
        /* Broadcast stopped state of experiment at 4 Hz */
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        BroadcastExperimentState();
      }
    }
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::PlayExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState !=
        argos::NetworkAPI::EExperimentState::EXPERIMENT_INITIALIZED &&
      m_eExperimentState !=
        argos::NetworkAPI::EExperimentState::EXPERIMENT_PAUSED) {
      LOG_S(WARNING) << "CNetworkAPI::PlayExperiment() called in wrong state: "
                     << argos::NetworkAPI::EExperimentStateToStr(
                          m_eExperimentState)
                     << std::endl;

      // silently return;
      return;
    }
    /* Disable fast-forward */
    m_bFastForwarding = false;

    m_cSimulatorTickMillis = std::chrono::milliseconds(
      (long int)(CPhysicsEngine::GetSimulationClockTick() * 1000.0f));

    /* Change state and emit signals */
    m_eExperimentState =
      argos::NetworkAPI::EExperimentState::EXPERIMENT_PLAYING;
    m_cWebServer->EmitEvent("Experiment playing", m_eExperimentState);

    LOG_S(INFO) << "Experiment playing";

    m_cTimer.Start();
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::FastForwardExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState !=
        argos::NetworkAPI::EExperimentState::EXPERIMENT_INITIALIZED &&
      m_eExperimentState !=
        argos::NetworkAPI::EExperimentState::EXPERIMENT_PAUSED) {
      LOG_S(WARNING)
        << "CNetworkAPI::FastForwardExperiment() called in wrong state: "
        << argos::NetworkAPI::EExperimentStateToStr(m_eExperimentState)
        << "\nRunning the experiment in FastForward mode" << std::endl;
    }
    m_bFastForwarding = true;

    m_cSimulatorTickMillis = std::chrono::milliseconds(
      (long int)(CPhysicsEngine::GetSimulationClockTick() * 1000.0f));

    /* Change state and emit signals */
    m_eExperimentState =
      argos::NetworkAPI::EExperimentState::EXPERIMENT_FAST_FORWARDING;
    m_cWebServer->EmitEvent("Experiment fast-forwarding", m_eExperimentState);

    LOG_S(INFO) << "Experiment fast-forwarding";

    m_cTimer.Start();
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::PauseExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState !=
        argos::NetworkAPI::EExperimentState::EXPERIMENT_PLAYING &&
      m_eExperimentState !=
        argos::NetworkAPI::EExperimentState::EXPERIMENT_FAST_FORWARDING) {
      LOG_S(WARNING) << "CNetworkAPI::PauseExperiment() called in wrong "
                        "state: "
                     << argos::NetworkAPI::EExperimentStateToStr(
                          m_eExperimentState);
      throw std::runtime_error(
        "Cannot pause the experiment, current state : " +
        argos::NetworkAPI::EExperimentStateToStr(m_eExperimentState));
      return;
    }
    /* Disable fast-forward */
    m_bFastForwarding = false;

    /* Change state and emit signals */
    m_eExperimentState = argos::NetworkAPI::EExperimentState::EXPERIMENT_PAUSED;
    m_cWebServer->EmitEvent("Experiment paused", m_eExperimentState);

    LOG_S(INFO) << "Experiment paused";
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::StepExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState ==
        argos::NetworkAPI::EExperimentState::EXPERIMENT_PLAYING ||
      m_eExperimentState ==
        argos::NetworkAPI::EExperimentState::EXPERIMENT_FAST_FORWARDING) {
      LOG_S(WARNING) << "CNetworkAPI::StepExperiment() called in wrong "
                        "state: "
                     << argos::NetworkAPI::EExperimentStateToStr(
                          m_eExperimentState)
                     << " pausing the experiment to run a step";

      /* Make experiment pause */
      m_eExperimentState =
        argos::NetworkAPI::EExperimentState::EXPERIMENT_PAUSED;
    }

    /* Disable fast-forward */
    m_bFastForwarding = false;

    if (!m_cSimulator.IsExperimentFinished()) {
      /* Run user's pre step function */
      m_cSimulator.GetLoopFunctions().PreStep();

      /* Run one step */
      m_cSimulator.UpdateSpace();

      /* Increment Counter */
      m_unStepCounter++;

      /* Run user's post step function */
      m_cSimulator.GetLoopFunctions().PostStep();

      /* Change state and emit signals */
      m_cWebServer->EmitEvent("Experiment step done", m_eExperimentState);
    } else {
      /* The experiment is done */
      m_cSimulator.GetLoopFunctions().PostExperiment();

      ResetExperiment();

      /* Change state and emit signals */
      m_cWebServer->EmitEvent("Experiment done", m_eExperimentState);
      LOG_S(INFO) << "Experiment done\n";
    }

    /* Broadcast current experiment state */
    BroadcastExperimentState();
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::ResetExperiment() {
    /* Reset Simulator */
    m_cSimulator.Reset();

    /* Disable fast-forward */
    m_bFastForwarding = false;

    /* Reset Counter */
    m_unStepCounter = 0;

    m_eExperimentState =
      argos::NetworkAPI::EExperimentState::EXPERIMENT_INITIALIZED;

    /* Change state and emit signals */
    m_cWebServer->EmitEvent("Experiment reset", m_eExperimentState);

    /* Broadcast current experiment state */
    BroadcastExperimentState();

    LOG_S(INFO) << "Experiment reset";
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::BroadcastExperimentState() {
    nlohmann::json cStateJson;

    cStateJson["entities"] = GetEntitiesAsJSON(m_cSpace);

    cStateJson["timestamp"] = m_cSpace.GetSimulationClock();

    /* Added Unix Epoch in milliseconds */
    // std::chrono::duration_cast<std::chrono::milliseconds>(
    //   std::chrono::system_clock::now().time_since_epoch())
    //   .count();

    /* Current state of the experiment */
    cStateJson["state"] =
      argos::NetworkAPI::EExperimentStateToStr(m_eExperimentState);

    /* Current Step from the counter */
    cStateJson["steps"] = m_unStepCounter;

    /* Send to webserver to broadcast */
    m_cWebServer->Broadcast(cStateJson);
  }

  /****************************************/
  /****************************************/

  CNetworkAPI::~CNetworkAPI() {
    delete m_cWebServer;
    delete m_pcLogStream;
    delete m_pcLogErrStream;
  }

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