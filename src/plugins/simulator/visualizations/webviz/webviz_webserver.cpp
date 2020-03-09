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
      unsigned short un_freq,
      std::string &str_key_file,
      std::string &str_cert_file,
      std::string &str_dh_params_file,
      std::string &str_ca_file,
      std::string &str_cert_passphrase)
        : m_pcMyWebviz(pc_my_webviz),
          /* Port to host the application on */
          m_unPort(un_port),
          /* Initialize Threads to handle web traffic */
          /* std::thread::hardware_concurrency() */
          m_vecWebThreads(std::thread::hardware_concurrency()),
          /* Initialize broadcast Timer */
          m_cBroadcastTimer(argos::Webviz::CTimer()),
          m_sSSLOptions({}) {
      /* We dont want to divide by zero or negative frequency */
      if (un_freq <= 0) {
        un_freq = 10;  // Defaults to 10 Hz
      }

      /* max allowed time for one broadcast cycle */
      m_cBroadcastDuration = std::chrono::milliseconds(1000 / un_freq);

      m_strBroadcastString = "";

      /* Build the SSL options object from parameters */
      if (!str_key_file.empty()) {
        m_sSSLOptions.key_file_name = str_cert_file.c_str();
      }
      if (!str_cert_file.empty()) {
        m_sSSLOptions.cert_file_name = str_cert_file.c_str();
      }
      if (!str_dh_params_file.empty()) {
        m_sSSLOptions.dh_params_file_name = str_dh_params_file.c_str();
      }
      if (!str_ca_file.empty()) {
        m_sSSLOptions.ca_file_name = str_ca_file.c_str();
      }
      if (!str_cert_passphrase.empty()) {
        m_sSSLOptions.passphrase = str_cert_passphrase.c_str();
      }
    }

    /****************************************/
    /****************************************/

    CWebServer::~CWebServer() {}

    /****************************************/
    /****************************************/

    void CWebServer::Start() {
      LOG << "Starting " << m_vecWebThreads.size() << " threads for WebServer "
          << std::endl;

      struct us_socket_context_options_t sEmptySSLOptions = {};

      const bool bIsSSLCopied =
        memcmp(&m_sSSLOptions, &sEmptySSLOptions, sizeof(sEmptySSLOptions));

      /* As templates are handled at compile time */
      if (bIsSSLCopied) {
        this->InitServer<true>(sEmptySSLOptions);
      } else {
        this->InitServer<false>(sEmptySSLOptions);
      }
    }

    /****************************************/
    /****************************************/

    template <bool SSL>
    void CWebServer::InitServer(us_socket_context_options_t s_ssl_options) {
      /* Create a vector for list of all connected clients */
      std::vector<SWebSocketClient<SSL>> vecWebSocketClients;

      /** Mutex to protect access to vecWebSocketClients */
      std::mutex mutex4VecWebClients;

      /* File Server to host all HTTP requests */
      /* Initialize File server */
      CFileServer m_cFileServer;

      try {
        /* Loop through all threads */
        std::transform(
          m_vecWebThreads.begin(),
          m_vecWebThreads.end(),
          m_vecWebThreads.begin(),
          [&](std::thread *t) {
            return new std::thread([&]() {
              /* Set up thread-safe buffers for this new thread */
              LOG.AddThreadSafeBuffer();
              LOGERR.AddThreadSafeBuffer();

              auto cMyApp = uWS::TemplatedApp<SSL>(s_ssl_options);

              /* Set all routes of the web-server on this thread */
              /* Setup WebSockets from the templated app */
              cMyApp.template ws<m_sPerSocketData>(
                "/*",
                {/* Settings */
                 .compression = uWS::SHARED_COMPRESSOR,
                 .maxPayloadLength = 256 * 1024 * 1024,
                 .idleTimeout = 10,
                 .maxBackpressure = 256 * 1024 * 1204,
                 /* Handlers */
                 .open =
                   [&](
                     uWS::WebSocket<SSL, true> *pc_ws,
                     uWS::HttpRequest *pc_req) {
                     /* Selectivly subscribe to different channels */
                     if (pc_req->getQuery().size() > 0) {
                       std::stringstream str_stream(
                         std::string(pc_req->getQuery()));
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

                     // Guard the mutex which locks vecWebSocketClients
                     std::lock_guard<std::mutex> guard(mutex4VecWebClients);

                     /*
                      * Add to list of clients connected
                      */
                     vecWebSocketClients.push_back({pc_ws, uWS::Loop::get()});

                     LOG << "1 client connected (Total: "
                         << vecWebSocketClients.size() << ")";
                   },
                 .message =
                   [](
                     uWS::WebSocket<SSL, true> *pc_ws,
                     std::string_view strv_message,
                     uWS::OpCode e_opCode) {
                     /* broadcast every single message it got */
                     /* pc_ws->publish("broadcast", strv_message, e_opCode); */
                   },
                 .drain =
                   [](uWS::WebSocket<SSL, true> *ws) {
                     std::cout << "Drainage: " << ws->getBufferedAmount()
                               << std::endl;
                   },
                 .ping =
                   [](uWS::WebSocket<SSL, true> *ws) {
                     std::cout << "Ping" << std::endl;
                   },
                 .pong =
                   [](uWS::WebSocket<SSL, true> *ws) {
                     std::cout << "Pong" << std::endl;
                   },
                 .close =
                   [&](
                     uWS::WebSocket<SSL, true> *pc_ws,
                     int n_code,
                     std::string_view strv_message) {
                     /* it automatically unsubscribe from any topic here */

                     // Guard the mutex which locks vecWebSocketClients
                     std::lock_guard<std::mutex> guard(mutex4VecWebClients);

                     /*
                      * Remove from the list of all clients connected
                      */
                     for (size_t i = 0; i < vecWebSocketClients.size(); i++) {
                       if (vecWebSocketClients[i].m_cWS == pc_ws) {
                         vecWebSocketClients.erase(
                           vecWebSocketClients.begin() + i);
                       }
                     }
                     LOG << "1 client disconnected (Total: "
                         << vecWebSocketClients.size() << ")";
                   }});

              /****************************************/

              /* Setup routes */
              cMyApp.get("/start", [&](auto *pc_res, auto *pc_req) {
                m_pcMyWebviz->PlayExperiment();
                nlohmann::json cMyJson;
                cMyJson["status"] = "Started Playing";
                this->SendJSON(pc_res, cMyJson);
              });

              /****************************************/

              cMyApp.get("/pause", [&](auto *pc_res, auto *pc_req) {
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

              cMyApp.get("/step", [&](auto *pc_res, auto *pc_req) {
                nlohmann::json cMyJson;
                m_pcMyWebviz->StepExperiment();
                cMyJson["status"] = "Experiment step done";
                this->SendJSON(pc_res, cMyJson);
              });

              /****************************************/

              cMyApp.get("/fastforward", [&](auto *pc_res, auto *pc_req) {
                nlohmann::json cMyJson;
                m_pcMyWebviz->FastForwardExperiment();
                cMyJson["status"] = "Experiment fast-forwarding";
                this->SendJSON(pc_res, cMyJson);
              });

              /****************************************/

              cMyApp.get("/reset", [&](auto *pc_res, auto *pc_req) {
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

              /****************************************/

              cMyApp.get("/*", [&](auto *pc_res, auto *pc_req) {
                pc_res->cork([&]() {
                  m_cFileServer.handle_file_request(*pc_res, *pc_req);
                });
              });

              /****************************************/

              /* Start listening to Port */
              cMyApp
                .listen(
                  m_unPort,
                  [&](auto *pc_token) {
                    if (pc_token) {
                      LOG << "Thread listening on port " << m_unPort;
                    } else {
                      throw CARGoSException(
                        "[Error] CWebServer::Start() failed to listen on "
                        "port " +
                        std::to_string(m_unPort));
                      return;
                    }
                  })
                .run();
            });
          });

        /* Start broadcast timer */
        m_cBroadcastTimer.Start();

        /* copy strings to free up the locks */
        /* Declaring local static here to help with lambda catching inside */
        static std::string strBroadcastString;
        static std::string strEventString;
        static std::string strLogString;

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
            strBroadcastString = m_strBroadcastString;
          }  // End of mutex block: m_mutex4BroadcastString

          /*
           * Mutex block for m_mutex4EventQueue
           */
          {
            std::lock_guard<std::mutex> guard(m_mutex4EventQueue);

            if (!m_cEventQueue.empty()) {
              strEventString = m_cEventQueue.front();
              m_cEventQueue.pop();
            } else {
              strEventString = "";
            }
          }  // End of mutex block: m_mutex4EventQueue

          /* Initialize Log string */
          strLogString = "";

          /*
           * Mutex block for m_mutex4LogQueue
           */
          {
            std::lock_guard<std::mutex> guard(m_mutex4LogQueue);

            /* Get all log messages in one string with \n */
            /* TODO: change string to stringstream, better concatinations */
            while (!m_cLogQueue.empty()) {
              strLogString += m_cLogQueue.front() + "\n";
              m_cLogQueue.pop();
            }

          }  // End of mutex block: m_mutex4LogQueue

          std::lock_guard<std::mutex> guard(mutex4VecWebClients);

          /* Send the string to each client */
          std::for_each(
            vecWebSocketClients.begin(),
            vecWebSocketClients.end(),
            [](SWebSocketClient<SSL> wsStruct) {
              wsStruct.m_cLoop->defer([wsStruct]() {
                if (!strBroadcastString.empty()) {
                  wsStruct.m_cWS->publish(
                    "broadcast",
                    strBroadcastString,
                    uWS::OpCode::TEXT,
                    true);  // Compress = true
                }

                if (!strEventString.empty()) {
                  wsStruct.m_cWS->publish(
                    "events",
                    strEventString,
                    uWS::OpCode::TEXT,
                    true);  // Compress = true
                }

                if (!strLogString.empty()) {
                  wsStruct.m_cWS->publish(
                    "logs",
                    strLogString,
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

    template <bool SSL>
    void CWebServer::SendJSON(
      uWS::HttpResponse<SSL> *pc_res, nlohmann::json c_json_data) {
      pc_res->cork([&]() {
        pc_res->writeHeader("Access-Control-Allow-Origin", "*")
          ->writeHeader("Content-Type", "application/json")
          ->end(c_json_data.dump());
      });
    }

    /****************************************/
    /****************************************/
    template <bool SSL>
    void CWebServer::SendJSONError(
      uWS::HttpResponse<SSL> *pc_res,
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