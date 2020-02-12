#include "networkapi_server.h"
#include "helpers/utils.h"

#define LOGURU_WITH_STREAMS 1
#include <loguru.cpp>

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/space/space.h>
#include <chrono>

namespace argos {

  /****************************************/
  /****************************************/

  static Real TVTimeToHumanReadable(::timeval &t_time) {
    return static_cast<Real>(t_time.tv_sec) +
           static_cast<Real>(t_time.tv_usec * 10e-6);
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::Init(TConfigurationNode &t_tree) {
    /* Setting up Logging */
    LOG_SCOPE_FUNCTION(INFO);

    /* Parse options from the XML */
    GetNodeAttributeOrDefault(
      t_tree, "port", this->m_unPort, argos::UInt16(3000));

    /* Set the pointer to the step function
     */
    if (m_cSimulator.IsRealTimeClock()) {
      /* Use real-time
       * clock and set
       * time structures
       */
      m_tStepFunction = &CNetworkAPI::RealTimeStep;
      timerclear(&m_tStepClockTime);
      m_tStepClockTime.tv_usec = 1e6 * CPhysicsEngine::GetSimulationClockTick();
      ::gettimeofday(&m_tStepStartTime, NULL);

      m_tGameState.bFastForward = true;
    } else {
      /* Use normal clock
       */
      m_tStepFunction = &CNetworkAPI::NormalStep;
      m_tGameState.bFastForward = false;
    }
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
                    m_tGameState.eState = EXPERIMENT_INITIALIZED;
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
      //   (this->*m_tStepFunction)();
      //   this->BroadcastState();
      // }

      /* The experiment is finished */
      m_cSimulator.GetLoopFunctions().PostExperiment();

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

  void CNetworkAPI::PlayExperiment() {
    /* Make sure we are in the right state */
    if (
      m_tGameState.eState != EXPERIMENT_INITIALIZED &&
      m_tGameState.eState != EXPERIMENT_PAUSED) {
      LOG_S(ERROR) << "CNetworkAPI::PlayExperiment() called in wrong state: "
                   << m_tGameState.eState << std::endl;

      return;
    }

    /* Call OpenGL widget */
    // m_pcOpenGLWidget->PlayExperiment();

    /* Change state and emit signals */
    m_tGameState.eState = EXPERIMENT_PLAYING;

    EmitEvent("Experiment started");
    // m_sGameState BroadcastMessage("{\"status\":\"playing\"}");
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
    strJson << EExperimentStateToString(this->m_tGameState.eState);
    strJson << "\",\"isFastForward\":";
    strJson << (this->m_tGameState.bFastForward ? "true" : "false");
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

  void CNetworkAPI::BroadcastState() {
    std::stringstream strJson;

    // TODO : Build a JSON to string marshal function
    strJson << "{";
    strJson << "\"state\":\"";
    strJson << EExperimentStateToString(this->m_tGameState.eState);
    strJson << "\",\"isFastForward\":";
    strJson << (this->m_tGameState.bFastForward ? "true" : "false");
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

  void CNetworkAPI::NormalStep() { m_cSimulator.UpdateSpace(); }

  /****************************************/
  /****************************************/

  void CNetworkAPI::RealTimeStep() {
    /* m_tStepStartTime has already been set
     */
    m_cSimulator.UpdateSpace();
    /* Take the time now */
    ::gettimeofday(&m_tStepEndTime, NULL);
    /* Calculate the elapsed time */
    timersub(&m_tStepEndTime, &m_tStepStartTime, &m_tStepElapsedTime);
    /* If the elapsed time is lower than the
     * tick length, wait */
    if (!timercmp(&m_tStepElapsedTime, &m_tStepClockTime, >)) {
      /* Calculate the
       * waiting time */
      timersub(&m_tStepClockTime, &m_tStepElapsedTime, &m_tStepWaitTime);
      /* Wait */
      ::usleep(m_tStepWaitTime.tv_sec * 1e6 + m_tStepWaitTime.tv_usec);
      /* Get the new step
       * end */
      ::gettimeofday(&m_tStepEndTime, NULL);
    } else {
      LOG_S(WARNING) << "Clock tick "
                        "took "
                     << TVTimeToHumanReadable(m_tStepElapsedTime)
                     << " sec, more "
                        "than the "
                        "expected "
                     << TVTimeToHumanReadable(m_tStepClockTime) << " sec."
                     << std::endl;
    }
    /* Set the step start time to whatever
     * the step end time is */
    m_tStepStartTime.tv_sec = m_tStepEndTime.tv_sec;
    m_tStepStartTime.tv_usec = m_tStepEndTime.tv_usec;
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