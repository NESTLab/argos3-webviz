#include "networkapi_server.h"
#include "helpers/utils.h"

#define LOGURU_WITH_STREAMS 1
#include <loguru.cpp>

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/space/space.h>

namespace argos {

  /****************************************/
  /****************************************/

  CNetworkAPI::CNetworkAPI()
      : m_vecWebThreads(std::thread::hardware_concurrency()),
        m_bFastForwarding(false),
        m_cTimer() {}

  /****************************************/
  /****************************************/

  void CNetworkAPI::Init(TConfigurationNode &t_tree) {
    /* Setting up Logging */
    LOG_SCOPE_FUNCTION(INFO);

    /* Parse options from the XML */
    GetNodeAttributeOrDefault(
      t_tree, "port", this->m_unPort, argos::UInt16(3000));
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::Execute() {
    try {
      LOG_S(INFO) << "Starting " << m_vecWebThreads.size()
                  << " threads for WebServer";

      /* Start Webserver */
      std::transform(
        m_vecWebThreads.begin(),
        m_vecWebThreads.end(),
        m_vecWebThreads.begin(),
        [&](std::thread *t) {
          return new std::thread([&]() {
            auto cMyApp = uWS::App();

            this->SetupWebApp(cMyApp);
            /* Start listening to Port */
            cMyApp
              .listen(
                m_unPort,
                [&](auto *pc_token) {
                  if (pc_token) {
                    LOG_S(INFO) << "Thread listening on port " << m_unPort;

                    /* Set experiment state */
                    m_eExperimentState = EXPERIMENT_INITIALIZED;
                  } else {
                    ABORT_F(
                      "CNetworkAPI::Execute() failed to listen on port "
                      "%d",
                      m_unPort);
                    return;
                  }
                })
              .run();
          });
        });

      /* Main cycle */
      // while (!m_cSimulator.IsExperimentFinished()) {
      //   RealTimeStep();
      // }
      // /* The experiment is finished */
      // m_cSimulator.GetLoopFunctions().PostExperiment();

      std::for_each(
        m_vecWebThreads.begin(), m_vecWebThreads.end(), [](std::thread *t) {
          t->join();
        });
    } catch (CARGoSException &ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error while executing the experiment.", ex);
    }
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::SetupWebApp(uWS::App &c_MyApp) {
    /* Setup WebSockets */
    c_MyApp.ws<m_sPerSocketData>(
      "/*",
      {/* Settings */
       .compression = uWS::SHARED_COMPRESSOR,
       .maxPayloadLength = 16 * 1024 * 1024,
       .idleTimeout = 10,
       .maxBackpressure = 1 * 1024 * 1204,
       /* Handlers */
       .open =
         [&](auto *pc_ws, uWS::HttpRequest *pc_req) {
           /* Selectivly subscribe to different channels */
           if (pc_req->getQuery().size() > 0) {
             std::vector<std::string_view> vecQueries =
               SplitSV(pc_req->getQuery(), ",");

             std::for_each(
               vecQueries.begin(),
               vecQueries.end(),
               [pc_ws](std::string_view strv_channel) {
                 pc_ws->subscribe(strv_channel);
               });
           } else {
             /*
              * making every connection subscribe to the
              * "broadcast" and "events" topics
              */
             pc_ws->subscribe("broadcast");
             pc_ws->subscribe("events");
           }

           /*
            * Add to list of clients connected
            */
           m_vecWebSocketClients.push_back(pc_ws);
         },
       .message =
         [](auto *pc_ws, std::string_view strv_message, uWS::OpCode e_opCode) {
           /* broadcast every single message it got */
           pc_ws->publish("broadcast", strv_message, e_opCode);
         },
       .close =
         [&](auto *pc_ws, int n_code, std::string_view strv_message) {
           /* it automatically unsubscribe from any topic here */

           /*
            * Remove from the list of all clients connected
            */
           EraseFromVector(m_vecWebSocketClients, pc_ws);
         }});

    /* Setup routes */
    c_MyApp.get("/start", [&](auto *pc_res, auto *pc_req) {
      PlayExperiment();
      nlohmann::json cMyJson;
      cMyJson["status"] = "Started Playing";
      this->SendJSON(pc_res, cMyJson);
    });
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::SendJSON(
    uWS::HttpResponse<false> *pc_res, nlohmann::json c_json_data) {
    pc_res->cork([&]() {
      pc_res->writeHeader("Access-Control-Allow-Origin", "*");
      pc_res->writeHeader("Content-Type", "application/json");
      pc_res->end(c_json_data.dump());
    });
  }

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
      EmitEvent("Experiment started");
    }
    /* Change state and emit signals */
    m_eExperimentState = EXPERIMENT_PLAYING;
    EmitEvent("Experiment playing");

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
      EmitEvent("Experiment started");
    }
    /* Change state and emit signals */
    m_eExperimentState = EXPERIMENT_FAST_FORWARDING;
    EmitEvent("Experiment fast-forwarding");
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
    EmitEvent("Experiment paused");
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
      EmitEvent("Experiment step done");
    } else {
      PauseExperiment();
      EmitEvent("Experiment done");
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

  void CNetworkAPI::EmitEvent(std::string_view event_name) {
    std::stringstream strJson;

    // TODO : Build a JSON to string marshal function
    strJson << "{";
    strJson << "\"event\":\"";
    strJson << event_name;
    strJson << "\",\"state\":\"";
    strJson << EExperimentStateToString(this->m_eExperimentState);
    strJson << "\",\"isFastForward\":";
    strJson << (this->m_bFastForwarding ? "true" : "false");
    strJson << "}";

    std::string strJs = strJson.str();

    std::for_each(
      m_vecWebSocketClients.begin(),
      m_vecWebSocketClients.end(),
      [strJs](auto *ws) {
        //                                            Compress = true
        ws->publish("events", strJs, uWS::OpCode::TEXT, true);
      });
  }

  /****************************************/
  /****************************************/

  // jrd::time::Timer timer(true);
  //     // Kill some time
  //     for (int i = 0; i < 1000000000; i++)
  //         ;
  //     std::cout << "Elapsed time: " << std::fixed << timer << "ms\n";
  //     timer.Reset();
  //     // Kill some more time
  //     for (int i = 0; i < 10000000; i++)
  //         ;
  //     auto elapsed = timer.Elapsed();
  //     std::cout << "Elapsed time: " << std::fixed << elapsed.count() <<
  //     "ms\n";

  void CNetworkAPI::BroadcastState() {
    std::stringstream strJson;

    // TODO : Build a JSON to string marshal function
    strJson << "{";
    strJson << "\"state\":\"";
    strJson << EExperimentStateToString(this->m_eExperimentState);
    strJson << "\",\"isFastForward\":";
    strJson << (this->m_bFastForwarding ? "true" : "false");
    strJson << "}";

    std::string strJs = strJson.str();

    std::for_each(
      m_vecWebSocketClients.begin(),
      m_vecWebSocketClients.end(),
      [strJs](auto *ws) {
        try {
          //                                            Compress = true
          ws->publish("broadcast", strJs, uWS::OpCode::TEXT, true);
        } catch (const std::exception &e) {
          LOG_S(ERROR) << e.what() << '\n';
        }
      });
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::BroadcastMessage(std::string_view message) {
    std::for_each(
      m_vecWebSocketClients.begin(),
      m_vecWebSocketClients.end(),
      [message](auto *ws) {
        //                                            Compress = true
        ws->publish("broadcast", message, uWS::OpCode::TEXT, true);
      });
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