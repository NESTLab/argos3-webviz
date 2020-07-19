/**
 * @file <argos3/plugins/simulator/visualizations/webviz/webviz.cpp>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#include "webviz.h"

namespace argos {

  /****************************************/
  /****************************************/

  CWebviz::CWebviz()
      : m_eExperimentState(Webviz::EExperimentState::EXPERIMENT_INITIALIZED),
        m_cTimer(),
        m_cSpace(m_cSimulator.GetSpace()),
        m_bFastForwarding(false) {}

  /****************************************/
  /****************************************/

  // cppcheck-suppress unusedFunction
  void CWebviz::Init(TConfigurationNode& t_tree) {
    unsigned short unPort;
    unsigned short unBroadcastFrequency;

    std::string strKeyFilePath;
    std::string strCertFilePath;
    std::string strDHParamsFilePath;
    std::string strCAFilePath;
    std::string strCertPassphrase;

    /* Parse options from the XML */
    GetNodeAttributeOrDefault(t_tree, "port", unPort, UInt16(3000));
    GetNodeAttributeOrDefault(
      t_tree, "broadcast_frequency", unBroadcastFrequency, UInt16(10));
    GetNodeAttributeOrDefault(
      t_tree, "ff_draw_frames_every", m_unDrawFrameEvery, UInt16(2));

    /* Get options for ssl certificate from XML */
    GetNodeAttributeOrDefault(
      t_tree, "ssl_key_file", strKeyFilePath, std::string(""));
    GetNodeAttributeOrDefault(
      t_tree, "ssl_cert_file", strCertFilePath, std::string(""));
    GetNodeAttributeOrDefault(
      t_tree, "ssl_dh_params_file", strDHParamsFilePath, std::string(""));
    GetNodeAttributeOrDefault(
      t_tree, "ssl_ca_file", strCAFilePath, std::string(""));
    GetNodeAttributeOrDefault(
      t_tree, "ssl_cert_passphrase", strCertPassphrase, std::string(""));

    /* check parameters  */
    if (unPort < 1 || 65535 < unPort) {
      throw CARGoSException(
        "\"Port number\" set in configuration is out of range [1,65535]");
    }

    if (unBroadcastFrequency < 1 || 1000 < unBroadcastFrequency) {
      throw CARGoSException(
        "Broadcast frequency set in configuration is out of range [1,1000]");
    }

    if (m_unDrawFrameEvery < 1 || 1000 < m_unDrawFrameEvery) {
      throw CARGoSException(
        "Broadcast frequency set in configuration is invalid ( < 1 )");
    }

    /* Parse XML for user functions */
    if (NodeExists(t_tree, "user_functions")) {
      /* Use the passed user functions */
      /* Get data from XML */
      TConfigurationNode tNode = GetNode(t_tree, "user_functions");
      std::string strLabel, strLibrary;
      GetNodeAttribute(tNode, "label", strLabel);
      GetNodeAttributeOrDefault(tNode, "library", strLibrary, strLibrary);
      try {
        /* Load the library */
        if (strLibrary != "") {
          CDynamicLoading::LoadLibrary(strLibrary);
        }
        /* Create the user functions */
        m_pcUserFunctions = CFactory<CWebvizUserFunctions>::New(strLabel);

        /* Initialize user functions */
        m_pcUserFunctions->Init(tNode);

      } catch (CARGoSException& ex) {
        THROW_ARGOSEXCEPTION_NESTED(
          "Failed opening Webviz user function library", ex);
      }
    } else {
      /* Use standard (empty) user functions */
      m_pcUserFunctions = new CWebvizUserFunctions;
    }

    /* Check if port is available to bind */
    if (!PortChecker::CheckPortTCPisAvailable(unPort)) {
      THROW_ARGOSEXCEPTION("Port " + std::to_string(unPort) + " already in use")
      return;
    }

    /* Initialize Webserver */
    m_cWebServer = new Webviz::CWebServer(
      this,
      unPort,
      unBroadcastFrequency,
      strKeyFilePath,
      strCertFilePath,
      strDHParamsFilePath,
      strCAFilePath,
      strCertPassphrase);

    /* Should we play instantly? */
    bool bAutoPlay = false;
    GetNodeAttributeOrDefault(t_tree, "autoplay", bAutoPlay, bAutoPlay);
    if (bAutoPlay) {
      PlayExperiment();
    }
  }

  /****************************************/
  /****************************************/

  // cppcheck-suppress unusedFunction
  void CWebviz::Execute() {
    /* To manage all threads to exit gracefully */
    std::atomic<bool> bIsServerRunning{true};

    /* Start this->Simulation Thread */
    std::thread tSimulationTread(
      [&]() { this->SimulationThreadFunction(std::ref(bIsServerRunning)); });

    /* Start WebServer */
    m_cWebServer->Start(std::ref(bIsServerRunning));  // blocking the thread

    /* Join the simulation thread */
    tSimulationTread.join();

    /* Cleanup */
    LOG.Flush();
    LOGERR.Flush();
  }

  /* main simulation thread fuction */
  void CWebviz::SimulationThreadFunction(
    const std::atomic<bool>& b_IsServerRunning) {
    /* Set up thread-safe buffers for this new thread */
    LOG.AddThreadSafeBuffer();
    LOGERR.AddThreadSafeBuffer();

    while (b_IsServerRunning) {
      if (
        m_eExperimentState == Webviz::EExperimentState::EXPERIMENT_PLAYING ||
        m_eExperimentState ==
          Webviz::EExperimentState::EXPERIMENT_FAST_FORWARDING) {
        /* Fast forward steps counter used inside */
        int unFFStepCounter;

        if (m_bFastForwarding) {
          /* Number of frames to drop in fast-forward */
          unFFStepCounter = m_unDrawFrameEvery;
        } else {
          /* For non-fastforwarding mode, steps is 1 */
          unFFStepCounter = 1;
        }

        /* Loop for steps (multiple for fast-forward) */
        while (unFFStepCounter > 0 &&  // FF counter
               !m_cSimulator
                  .IsExperimentFinished() &&  // experiment was already finished
               b_IsServerRunning &&    // to stop if whole server is stopped
               (m_eExperimentState ==  // Check if we are in right state
                  Webviz::EExperimentState::EXPERIMENT_PLAYING ||
                m_eExperimentState ==
                  Webviz::EExperimentState::EXPERIMENT_FAST_FORWARDING)) {
          /* Run one step */
          m_cSimulator.UpdateSpace();

          /* Steps counter in this while loop */
          --unFFStepCounter;
        }

        /* Broadcast current experiment state */
        BroadcastExperimentState();

        /* Experiment done while in while loop */
        if (m_cSimulator.IsExperimentFinished()) {
          LOG << "[INFO] Experiment done" << '\n';

          /* The experiment is done */
          m_cSimulator.GetLoopFunctions().PostExperiment();

          /* Disable fast-forward */
          m_bFastForwarding = false;

          /* Set Experiment state to Done */
          m_eExperimentState = Webviz::EExperimentState::EXPERIMENT_DONE;

          /* Change state and emit signals */
          m_cWebServer->EmitEvent("Experiment done", m_eExperimentState);
        }

        /* Take the time now */
        m_cTimer.Stop();

        /* If the elapsed time is lower than the tick length, wait */
        if (m_cTimer.Elapsed() < m_cSimulatorTickMillis) {
          /* Sleep for the difference duration */
          std::this_thread::sleep_for(
            m_cSimulatorTickMillis - m_cTimer.Elapsed());
        } else {
          LOGERR << "[WARNING] Clock tick took " << m_cTimer
                 << " milli-secs, more than the expected "
                 << m_cSimulatorTickMillis.count() << " milli-secs. "
                 << "Recovering in next cycle." << '\n';
        }

        /* Restart Timer */
        m_cTimer.Start();
      } else {
        /*
         * Update the experiment state variable and sleep for some time,
         * we sleep to reduce the number of updates done in
         * "PAUSED"/"INITIALIZED"/"DONE" state
         */
        BroadcastExperimentState();
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
      }
    }
    /* do any cleanups */
  }

  /****************************************/
  /****************************************/

  void CWebviz::HandleCommandFromClient(
    const std::string& str_ip, nlohmann::json c_json_command) {
    if (c_json_command.contains("command")) {
      /* Try to get Command key from the JSON */
      std::string strCmd = c_json_command["command"].get<std::string>();

      /* Dispatch commands */
      if (strCmd.compare("play") == 0) {
        PlayExperiment();

      } else if (strCmd.compare("pause") == 0) {
        PauseExperiment();

      } else if (strCmd.compare("step") == 0) {
        StepExperiment();

      } else if (strCmd.compare("reset") == 0) {
        ResetExperiment();

      } else if (strCmd.compare("terminate") == 0) {
        TerminateExperiment();

      } else if (strCmd.compare("fastforward") == 0) {
        try {
          /* number of Steps defined */
          int16_t unSteps = c_json_command["steps"].get<int16_t>();

          /* Validate steps */
          if (1 <= unSteps && unSteps <= 1000) {
            FastForwardExperiment(unSteps);
          } else {
            /* Fastforward without steps defined */
            FastForwardExperiment();
          }

        } catch (const std::exception& _ignored) {
          /* No steps defined */
          FastForwardExperiment();
        }

      } else if (strCmd.compare("moveEntity") == 0) {
        try {
          CVector3 cNewPos;
          CQuaternion cNewOrientation;

          /* Parse Position */
          cNewPos.SetX(c_json_command["position"]["x"].get<float_t>());
          cNewPos.SetY(c_json_command["position"]["y"].get<float_t>());
          cNewPos.SetZ(c_json_command["position"]["z"].get<float_t>());

          /* Parse Orientation */
          cNewOrientation.SetX(
            c_json_command["orientation"]["x"].get<float_t>());
          cNewOrientation.SetY(
            c_json_command["orientation"]["y"].get<float_t>());
          cNewOrientation.SetZ(
            c_json_command["orientation"]["z"].get<float_t>());
          cNewOrientation.SetW(
            c_json_command["orientation"]["w"].get<float_t>());

          MoveEntity(
            c_json_command["entity_id"].get<std::string>(),
            cNewPos,
            cNewOrientation);

        } catch (const std::exception& e) {
          LOGERR << "[ERROR] In function MoveEntity: " << e.what() << '\n';
        }

      } else {
        /* "command" key has unknown value */
        try {
          m_pcUserFunctions->HandleCommandFromClient(str_ip, c_json_command);
        } catch (const std::exception& e) {
          LOGERR
            << "[ERROR] Error in overridden function HandleCommandFromClient "
               "in UserFunction subclass implementation by user\n\t"
            << e.what() << '\n';
        }
      }

    } else {
      /* "command" key in the JSON doesn't exists */
      try {
        m_pcUserFunctions->HandleCommandFromClient(str_ip, c_json_command);
      } catch (const std::exception& e) {
        LOGERR
          << "[ERROR] Error in overridden function HandleCommandFromClient "
             "in UserFunction subclass implementation by user\n\t"
          << e.what() << '\n';
      }
    }
  }

  /****************************************/
  /****************************************/

  void CWebviz::PlayExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState != Webviz::EExperimentState::EXPERIMENT_INITIALIZED &&
      m_eExperimentState != Webviz::EExperimentState::EXPERIMENT_PAUSED) {
      LOGERR << "[WARNING] PlayExperiment() called in wrong state: "
             << Webviz::EExperimentStateToStr(m_eExperimentState) << '\n';

      // silently return;
      return;
    }
    /* Disable fast-forward */
    m_bFastForwarding = false;

    m_cSimulatorTickMillis = std::chrono::milliseconds(
      (long int)(CPhysicsEngine::GetSimulationClockTick() * 1000.0f));

    /* Change state and emit signals */
    m_eExperimentState = Webviz::EExperimentState::EXPERIMENT_PLAYING;
    m_cWebServer->EmitEvent("Experiment playing", m_eExperimentState);

    LOG << "[INFO] Experiment playing" << '\n';

    m_cTimer.Start();
  }

  /****************************************/
  /****************************************/

  void CWebviz::FastForwardExperiment(unsigned short un_steps) {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState != Webviz::EExperimentState::EXPERIMENT_INITIALIZED &&
      m_eExperimentState != Webviz::EExperimentState::EXPERIMENT_PAUSED) {
      LOGERR << "[WARNING] FastForwardExperiment() called in wrong state: "
             << Webviz::EExperimentStateToStr(m_eExperimentState)
             << ", Running the experiment in FastForward mode" << '\n';

      /* Do not fast forward if experiment is done */
      if (m_eExperimentState == Webviz::EExperimentState::EXPERIMENT_DONE) {
        return;
      }
    }

    /* If Steps are passed, and valid else to use existing steps */
    if (1 <= un_steps && un_steps <= 1000) {
      /* Update FF steps variable */
      m_unDrawFrameEvery = un_steps;
    }

    m_bFastForwarding = true;

    m_cSimulatorTickMillis = std::chrono::milliseconds(
      (long int)(CPhysicsEngine::GetSimulationClockTick() * 1000.0f));

    /* Change state and emit signals */
    m_eExperimentState = Webviz::EExperimentState::EXPERIMENT_FAST_FORWARDING;
    m_cWebServer->EmitEvent("Experiment fast-forwarding", m_eExperimentState);

    LOG << "[INFO] Experiment fast-forwarding" << '\n';

    m_cTimer.Start();
  }

  /****************************************/
  /****************************************/

  void CWebviz::PauseExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState != Webviz::EExperimentState::EXPERIMENT_PLAYING &&
      m_eExperimentState !=
        Webviz::EExperimentState::EXPERIMENT_FAST_FORWARDING) {
      LOGERR << "[WARNING] PauseExperiment() called in wrong state: "
             << Webviz::EExperimentStateToStr(m_eExperimentState) << '\n';

      return;
    }
    /* Disable fast-forward */
    m_bFastForwarding = false;

    /* Change state and emit signals */
    m_eExperimentState = Webviz::EExperimentState::EXPERIMENT_PAUSED;
    m_cWebServer->EmitEvent("Experiment paused", m_eExperimentState);

    LOG << "[INFO] Experiment paused" << '\n';
  }

  /****************************************/
  /****************************************/

  void CWebviz::StepExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState == Webviz::EExperimentState::EXPERIMENT_PLAYING ||
      m_eExperimentState ==
        Webviz::EExperimentState::EXPERIMENT_FAST_FORWARDING) {
      LOGERR << "[WARNING] StepExperiment() called in wrong state: "
             << Webviz::EExperimentStateToStr(m_eExperimentState)
             << " pausing the experiment to run a step" << '\n';

      /* Make experiment pause */
      m_eExperimentState = Webviz::EExperimentState::EXPERIMENT_PAUSED;

      /* Do not go further, as the while loop in SimulationThreadFunction might
       * be halfway into execution */
      return;
    }

    /* Disable fast-forward */
    m_bFastForwarding = false;

    if (!m_cSimulator.IsExperimentFinished()) {
      /* Run one step */
      m_cSimulator.UpdateSpace();

      /* Make experiment pause */
      m_eExperimentState = Webviz::EExperimentState::EXPERIMENT_PAUSED;

      /* Change state and emit signals */
      m_cWebServer->EmitEvent("Experiment step done", m_eExperimentState);
    } else {
      LOG << "[INFO] Experiment done" << '\n';

      /* The experiment is done */
      m_cSimulator.GetLoopFunctions().PostExperiment();

      /* Set Experiment state to Done */
      m_eExperimentState = Webviz::EExperimentState::EXPERIMENT_DONE;

      /* Change state and emit signals */
      m_cWebServer->EmitEvent("Experiment done", m_eExperimentState);
    }

    /* Broadcast current experiment state */
    BroadcastExperimentState();
  }

  /****************************************/
  /****************************************/

  void CWebviz::ResetExperiment() {
    /* Reset Simulator */
    m_cSimulator.Reset();

    /* Disable fast-forward */
    m_bFastForwarding = false;

    /* Reset the simulator if Reset was called after experiment was done */
    if (m_eExperimentState == Webviz::EExperimentState::EXPERIMENT_DONE) {
      /* Reset simulator */
      m_cSimulator.Reset();
    }

    m_eExperimentState = Webviz::EExperimentState::EXPERIMENT_INITIALIZED;

    /* Change state and emit signals */
    m_cWebServer->EmitEvent("Experiment reset", m_eExperimentState);

    /* Broadcast current experiment state */
    BroadcastExperimentState();

    LOG << "[INFO] Experiment reset" << '\n';
  }

  /****************************************/
  /****************************************/

  void CWebviz::TerminateExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState != Webviz::EExperimentState::EXPERIMENT_PLAYING &&
      m_eExperimentState != Webviz::EExperimentState::EXPERIMENT_PAUSED &&
      m_eExperimentState !=
        Webviz::EExperimentState::EXPERIMENT_FAST_FORWARDING) {
      LOGERR << "[WARNING] TerminateExperiment() called in wrong state: "
             << Webviz::EExperimentStateToStr(m_eExperimentState) << '\n';

      return;
    }
    /* Disable fast-forward */
    m_bFastForwarding = false;

    /* Call ARGoS to terminate the experiment */
    CSimulator::GetInstance().Terminate();
    CSimulator::GetInstance().GetLoopFunctions().PostExperiment();

    /* Set Experiment state to Done */
    m_eExperimentState = Webviz::EExperimentState::EXPERIMENT_DONE;

    /* Change state and emit signals */
    m_cWebServer->EmitEvent("Experiment done", m_eExperimentState);

    LOG << "[INFO] Experiment done" << '\n';
  }

  /****************************************/
  /****************************************/

  void CWebviz::BroadcastExperimentState() {
    /************* Build a JSON object to be sent to all clients *************/
    nlohmann::json cStateJson;

    /************* Convert Entities info to JSON *************/

    /* Get all entities in the experiment */
    CEntity::TVector& vecEntities = m_cSpace.GetRootEntityVector();

    for (auto itEntities = vecEntities.begin();  //
         itEntities != vecEntities.end();        //
         ++itEntities) {
      /************* Generate JSON from Entities *************/

      auto cEntityJSON = CallEntityOperation<
        CWebvizOperationGenerateJSON,
        CWebviz,
        nlohmann::json>(*this, **itEntities);

      if (cEntityJSON != nullptr) {
        /************* get data from User functions for entity *************/
        const nlohmann::json& user_data = m_pcUserFunctions->Call(**itEntities);

        if (!user_data.is_null()) {
          cEntityJSON["user_data"] = user_data;
        }

        cStateJson["entities"].push_back(cEntityJSON);
      } else {
        LOGERR << "[ERROR] Unknown Entity:"
               << (**itEntities).GetTypeDescription() << "\n"
               << "Please register a class to convert Entity to JSON, "
               << "Check documentation for how to implement custom entity";
      }
    }

    /************* get data from User functions for experiment *************/

    const nlohmann::json& user_data = m_pcUserFunctions->sendUserData();

    if (!user_data.is_null()) {
      cStateJson["user_data"] = user_data;
    }

    /************* Add other information about experiment *************/

    /* Get Arena details */

    const CVector3& cArenaSize = m_cSpace.GetArenaSize();
    cStateJson["arena"]["size"]["x"] = cArenaSize.GetX();
    cStateJson["arena"]["size"]["y"] = cArenaSize.GetY();
    cStateJson["arena"]["size"]["z"] = cArenaSize.GetZ();

    const CVector3& cArenaCenter = m_cSpace.GetArenaCenter();
    cStateJson["arena"]["center"]["x"] = cArenaCenter.GetX();
    cStateJson["arena"]["center"]["y"] = cArenaCenter.GetY();
    cStateJson["arena"]["center"]["z"] = cArenaCenter.GetZ();

    // TODO: m_cSpace.GetArenaLimits();

    /* Added Unix Epoch in milliseconds */
    cStateJson["timestamp"] =
      std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch())
        .count();

    /* Current state of the experiment */
    cStateJson["state"] = Webviz::EExperimentStateToStr(m_eExperimentState);

    /* Number of step from the simulator */
    cStateJson["steps"] = m_cSpace.GetSimulationClock();

    /* Type of message */
    cStateJson["type"] = "broadcast";

    /* Send to webserver to broadcast */
    m_cWebServer->Broadcast(cStateJson);
  }

  /****************************************/
  /****************************************/

  void CWebviz::MoveEntity(
    std::string str_entity_id, CVector3 c_pos, CQuaternion c_orientation) {
    /* throws CARGoSException if entity doesn't exist */
    try {
      CEntity* cEntity = &m_cSpace.GetEntity(str_entity_id);
      CEmbodiedEntity* pcEntity = dynamic_cast<CEmbodiedEntity*>(cEntity);

      if (pcEntity == NULL) {
        /* Treat selected entity as a composable entity with an embodied
         * component */
        CComposableEntity* pcCompEntity =
          dynamic_cast<CComposableEntity*>(cEntity);
        if (pcCompEntity != NULL && pcCompEntity->HasComponent("body")) {
          pcEntity = &pcCompEntity->GetComponent<CEmbodiedEntity>("body");
        } else {
          /* All conversions failed, get out */
          // s
          THROW_ARGOSEXCEPTION(
            "[ERROR] No entity found with id:" + str_entity_id);
          return;
        }
      }

      if (pcEntity->MoveTo(c_pos, c_orientation)) {
        LOG << "[INFO] Entity Moved (" + str_entity_id + ")" << '\n';
      } else {
        LOGERR << "[WARNING] Entity cannot be moved, collision detected. (" +
                    str_entity_id + ")"
               << '\n';
      }
    } catch (CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED(
        "[ERROR] No entity found with id:" + str_entity_id, ex);
    }
  }

  /****************************************/
  /****************************************/

  void CWebviz::Destroy() {
    /* Get rid of the factory */

    CFactory<CWebvizUserFunctions>::Destroy();
  }
  /****************************************/
  /****************************************/

  REGISTER_VISUALIZATION(
    CWebviz,
    "webviz",
    "Prajankya [prajankya@gmail.com]",
    ARGOS_WEBVIZ_VERSION,
    "An interactive web interface to manage argos simulation over network\n",
    "It allows the user to watch and modify the simulation as it's running \n"
    "in an intuitive way.\n\n"
    "REQUIRED XML CONFIGURATION\n\n"
    "  <visualization>\n"
    "    <webviz />\n"
    "  </visualization>\n\n"
    "OPTIONAL XML CONFIGURATION with all the defaults:\n\n"
    "  <visualization>\n"
    "    <webviz port=3000\n"
    "         broadcast_frequency=10\n"
    "         ff_draw_frames_every=2\n"
    "         autoplay=\"true\"\n"
    "         ssl_key_file=\"NULL\"\n"
    "         ssl_cert_file=\"NULL\"\n"
    "         ssl_ca_file=\"NULL\"\n"
    "         ssl_dh_params_file=\"NULL\"\n"
    "         ssl_cert_passphrase=\"NULL\"\n"
    "    />\n"
    "  </visualization>\n\n"
    "\n"
    "Where:\n"
    "port(unsigned short): is the network port to listen incoming \n"
    "\ttraffic on (Websockets and HTTP both share the same port)\n"
    "    Default: 3000\n"
    "    Range: [1,65535]\n"
    "        Note: Ports < 1024 need root privileges.\n\n"

    "broadcast_frequency(unsigned short): Frequency (in Hertz) at which\n"
    "\tto broadcast the updates(through websockets)\n"
    "    Default: 10\n"
    "    Range: [1,1000]\n\n"

    "ff_draw_frames_every(unsigned short): Number of steps to skip\n"
    "\twhen in fast forward mode\n"
    "    Default: 2\n\n"

    "autoplay(bool): Allows user to auto-play the simulation at startup\n"
    "    Default: false\n\n"
    "--\n\n"
    "SSL CONFIGURATION\n"
    "SSL can be used to host the server over \"wss\"(analogous to \n"
    "\t\"https\" for websockets).\n"
    "NOTE: You need Webviz to be compiled with OpenSSL support to use SSL.\n"
    "\n"
    "You might have to use any combination of the following to enable\n"
    "\t SSL, depending upon your implementation.\n"
    "\n"
    "\t* ssl_key_file\n"
    "\t* ssl_cert_file\n"
    "\t* ssl_ca_file\n"
    "\t* ssl_dh_params_file\n"
    "\t* ssl_cert_passphrase\n"
    "Where file parameters supports relative and absolute paths.\n"
    "\tNOTE:(It needs read access\n"
    "\t to the files)\n"
    "\n",
    "Usable");
}  // namespace argos