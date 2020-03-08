/**
 * @file <argos3/plugins/simulator/visualizations/webviz/webviz_webserver.cpp>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#include "webviz_webserver.h"

namespace argos {
  namespace Webviz {

    /****************************************/
    /****************************************/

    CWebServer::CWebServer(
      argos::CWebviz *pc_my_webviz,
      unsigned short un_port,
      unsigned short un_freq)
        : m_pcMyWebviz(pc_my_webviz),
          /* Port to host the application on */
          m_unPort(un_port),
          /* Initialize Threads to handle web traffic */
          /* std::thread::hardware_concurrency() */
          m_vecWebThreads(std::thread::hardware_concurrency()),
          /* Initialize broadcast Timer */
          m_cBroadcastTimer(argos::Webviz::CTimer()) {
      /* We dont want divide by zero or negative frequency */
      if (un_freq <= 0) {
        un_freq = 10;  // Defaults to 10 Hz
      }
      /* max allowed time for one broadcast cycle */
      m_cBroadcastDuration = std::chrono::milliseconds(1000 / un_freq);

      m_strBroadcastString = "";
    }

    /****************************************/
    /****************************************/

    void CWebServer::Start() {
      try {
        LOG << "Starting " << m_vecWebThreads.size()
            << " threads for WebServer " << std::endl;

        /* Start Webserver */
        std::transform(
          m_vecWebThreads.begin(),
          m_vecWebThreads.end(),
          m_vecWebThreads.begin(),
          [&](std::thread *t) {
            return new std::thread([&]() {
              auto cMyApp = uWS::App();

              /* Set up thread-safe buffers for this new thread */
              LOG.AddThreadSafeBuffer();
              LOGERR.AddThreadSafeBuffer();

              this->SetupWebApp(cMyApp);
              /* Start listening to Port */
              cMyApp
                .listen(
                  m_unPort,
                  [&](auto *pc_token) {
                    if (pc_token) {
                      LOG << "Thread listening on port " << m_unPort;
                    } else {
                      // TODO ASSERT
                      // ABORT_F(
                      //   "WebServer::Execute() failed to listen on port "
                      //   "%d",
                      //   m_unPort);
                      return;
                    }
                  })
                .run();
            });
          });

        /* Start broadcast timer */
        m_cBroadcastTimer.Start();

        /* Declaring local static here to help with lambda catching inside */
        static std::string str_broadcastString;
        static std::string str_EventString;
        static std::string str_LogString;

        // TODO : use thread notifying
        while (true) {
          /* stop the timer now to get total time spent */
          m_cBroadcastTimer.Stop();

          /* If the elapsed time is lower than the tick length, wait */
          if (m_cBroadcastTimer.Elapsed() < m_cBroadcastDuration) {
            /* Sleep for the difference duration */
            std::this_thread::sleep_for(
              m_cBroadcastDuration - m_cBroadcastTimer.Elapsed());
          } else {
            LOGERR << "[WARNING] Broadcast tick took " << m_cBroadcastTimer
                   << " milli-secs, more than the expected "
                   << m_cBroadcastDuration.count() << " milli-secs. "
                   << "Not able to reach all clients, Please reduce "
                      "the \'broadcast_frequency\' in "
                      "configuration file.\n";
            break;
          }

          /* Restart Timer */
          m_cBroadcastTimer.Start();

          /* Decouple the strings so a new broadcast message can be accepted
           * while old are sending */
          /*
           * Mutex block for m_mutex4BroadcastString
           */
          {
            std::lock_guard<std::mutex> guard(m_mutex4BroadcastString);
            str_broadcastString = m_strBroadcastString;
          }  // End of mutex block: m_mutex4BroadcastString

          /*
           * Mutex block for m_mutex4EventQueue
           */
          {
            std::lock_guard<std::mutex> guard(m_mutex4EventQueue);

            if (!m_cEventQueue.empty()) {
              str_EventString = m_cEventQueue.front();
              m_cEventQueue.pop();
            } else {
              str_EventString = "";
            }
          }  // End of mutex block: m_mutex4EventQueue

          /* Initialize Log string */
          str_LogString = "";

          /*
           * Mutex block for m_mutex4LogQueue
           */
          {
            std::lock_guard<std::mutex> guard(m_mutex4LogQueue);

            /* Get all log messages in one string with \n */
            /* TODO: change string to stringstream, better concatinations */
            while (!m_cLogQueue.empty()) {
              str_LogString += m_cLogQueue.front() + "\n";
              m_cLogQueue.pop();
            }

          }  // End of mutex block: m_mutex4LogQueue

          std::lock_guard<std::mutex> guard(m_mutex4VecWebClients);

          /* Send the string to each client */
          std::for_each(
            m_vecWebSocketClients.begin(),
            m_vecWebSocketClients.end(),
            [](auto wsStruct) {
              wsStruct.m_cLoop->defer([wsStruct]() {
                if (!str_broadcastString.empty()) {
                  wsStruct.m_cWS->publish(
                    "broadcast",
                    str_broadcastString,
                    uWS::OpCode::TEXT,
                    true);  // Compress = true
                }

                if (!str_EventString.empty()) {
                  wsStruct.m_cWS->publish(
                    "events",
                    str_EventString,
                    uWS::OpCode::TEXT,
                    true);  // Compress = true
                }

                if (!str_LogString.empty()) {
                  wsStruct.m_cWS->publish(
                    "logs",
                    str_LogString,
                    uWS::OpCode::TEXT,
                    true);  // Compress = true
                }
              });
            });
        }

        std::for_each(
          m_vecWebThreads.begin(), m_vecWebThreads.end(), [](std::thread *t) {
            t->join();
          });
      } catch (CARGoSException &ex) {
        THROW_ARGOSEXCEPTION_NESTED(
          "Error while executing the experiment.", ex);
      }
    }

    /****************************************/
    /****************************************/

    void CWebServer::SetupWebApp(uWS::App &c_MyApp) {
      /* Setup WebSockets */
      c_MyApp.ws<m_sPerSocketData>(
        "/*",
        {/* Settings */
         .compression = uWS::SHARED_COMPRESSOR,
         .maxPayloadLength = 256 * 1024 * 1024,
         .idleTimeout = 10,
         .maxBackpressure = 256 * 1024 * 1204,
         /* Handlers */
         .open =
           [&](auto *pc_ws, uWS::HttpRequest *pc_req) {
             /* Selectivly subscribe to different channels */
             if (pc_req->getQuery().size() > 0) {
               std::stringstream str_stream(std::string(pc_req->getQuery()));
               std::string str_token;
               while (std::getline(str_stream, str_token, ',')) {
                 pc_ws->subscribe(str_token);
               }
             } else {
               /*
                * making every connection subscribe to the
                * "broadcast", "events" and "logs" topics
                */
               pc_ws->subscribe("broadcast");
               pc_ws->subscribe("events");
               pc_ws->subscribe("logs");
             }

             // Guard the mutex which locks m_vecWebSocketClients
             std::lock_guard<std::mutex> guard(m_mutex4VecWebClients);

             /*
              * Add to list of clients connected
              */
             m_vecWebSocketClients.push_back({pc_ws, uWS::Loop::get()});
             LOG << "1 client connected (Total: "
                 << m_vecWebSocketClients.size() << ")";
           },
         //  .message =
         //    [](
         //      auto *pc_ws, std::string_view strv_message, uWS::OpCode
         //      e_opCode) {
         //      /* broadcast every single message it got */
         //      pc_ws->publish("broadcast", strv_message, e_opCode);
         //    },
         .close =
           [&](auto *pc_ws, int n_code, std::string_view strv_message) {
             /* it automatically unsubscribe from any topic here */

             // Guard the mutex which locks m_vecWebSocketClients
             std::lock_guard<std::mutex> guard(m_mutex4VecWebClients);

             /*
              * Remove from the list of all clients connected
              */
             for (size_t i = 0; i < m_vecWebSocketClients.size(); i++) {
               if (m_vecWebSocketClients[i].m_cWS == pc_ws) {
                 m_vecWebSocketClients.erase(m_vecWebSocketClients.begin() + i);
               }
             }
             LOG << "1 client disconnected (Total: "
                 << m_vecWebSocketClients.size() << ")";
           }});

      /****************************************/

      /* Setup routes */
      c_MyApp.get("/start", [&](auto *pc_res, auto *pc_req) {
        m_pcMyWebviz->PlayExperiment();
        nlohmann::json cMyJson;
        cMyJson["status"] = "Started Playing";
        this->SendJSON(pc_res, cMyJson);
      });

      /****************************************/

      c_MyApp.get("/pause", [&](auto *pc_res, auto *pc_req) {
        nlohmann::json cMyJson;
        try {
          m_pcMyWebviz->PauseExperiment();
          cMyJson["status"] = "Experiment Paused";
          this->SendJSON(pc_res, cMyJson);
        } catch (const std::exception &e) {
          cMyJson["status"] = "Error";
          cMyJson["message"] = e.what();

          this->SendJSONError(pc_res, cMyJson);
        }
      });

      /****************************************/

      c_MyApp.get("/step", [&](auto *pc_res, auto *pc_req) {
        nlohmann::json cMyJson;
        m_pcMyWebviz->StepExperiment();
        cMyJson["status"] = "Experiment step done";
        this->SendJSON(pc_res, cMyJson);
      });

      /****************************************/

      c_MyApp.get("/fastforward", [&](auto *pc_res, auto *pc_req) {
        nlohmann::json cMyJson;
        m_pcMyWebviz->FastForwardExperiment();
        cMyJson["status"] = "Experiment fast-forwarding";
        this->SendJSON(pc_res, cMyJson);
      });

      /****************************************/

      c_MyApp.get("/reset", [&](auto *pc_res, auto *pc_req) {
        nlohmann::json cMyJson;
        try {
          m_pcMyWebviz->ResetExperiment();
          cMyJson["status"] = "Experiment Reset";
          this->SendJSON(pc_res, cMyJson);
        } catch (const std::exception &e) {
          cMyJson["status"] = "Error";
          cMyJson["message"] = e.what();

          this->SendJSONError(pc_res, cMyJson);
        }
      });
    }

    /****************************************/
    /****************************************/

    void CWebServer::SendJSON(
      uWS::HttpResponse<false> *pc_res, nlohmann::json c_json_data) {
      pc_res->cork([&]() {
        pc_res->writeHeader("Access-Control-Allow-Origin", "*")
          ->writeHeader("Content-Type", "application/json")
          ->end(c_json_data.dump());
      });
    }

    /****************************************/
    /****************************************/

    void CWebServer::SendJSONError(
      uWS::HttpResponse<false> *pc_res,
      nlohmann::json c_json_data,
      std::string http_status) {
      pc_res->cork([&]() {
        pc_res->writeStatus(http_status)
          ->writeHeader("Access-Control-Allow-Origin", "*")
          ->writeHeader("Content-Type", "application/json")
          ->end(c_json_data.dump());
      });
    }

    /****************************************/
    /****************************************/

    void CWebServer::EmitEvent(
      std::string_view strv_event_name,
      argos::Webviz::EExperimentState e_state) {
      nlohmann::json cMyJson;
      cMyJson["event"] = strv_event_name;
      cMyJson["state"] = argos::Webviz::EExperimentStateToStr(e_state);

      // Guard the mutex which locks m_mutex4EventQueue
      std::lock_guard<std::mutex> guard(m_mutex4EventQueue);

      /* Add to the event queue */
      m_cEventQueue.push(cMyJson.dump());
    }

    /****************************************/
    /****************************************/

    void CWebServer::EmitLog(
      std::string str_log_name, std::string str_log_data) {
      nlohmann::json cMyJson;
      cMyJson["log_name"] = str_log_name;
      cMyJson["log_data"] = str_log_data;

      // Guard the mutex which locks m_mutex4LogQueue
      std::lock_guard<std::mutex> guard(m_mutex4LogQueue);

      /* Add to the Log queue */
      m_cLogQueue.push(cMyJson.dump());
    }

    /****************************************/
    /****************************************/

    void CWebServer::Broadcast(nlohmann::json cMyJson) {
      /* Guard the mutex which locks m_mutex4BroadcastString */
      std::lock_guard<std::mutex> guard(m_mutex4BroadcastString);
      /* Replaces the existing state, even if it was not sent
       * This enables us to discard stale experiment state
       */
      m_strBroadcastString = cMyJson.dump();
    }
  }  // namespace Webviz
}  // namespace argos