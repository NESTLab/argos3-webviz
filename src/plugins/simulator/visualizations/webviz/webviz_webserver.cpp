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
          /* Initialize broadcast Timer */
          m_cBroadcastTimer(argos::Webviz::CTimer()) {
      /* We dont want to divide by zero or negative frequency */
      if (un_freq <= 0) {
        un_freq = 10;  // Defaults to 10 Hz
      }

      /* max allowed time for one broadcast cycle */
      m_cBroadcastDuration = std::chrono::milliseconds(1000 / un_freq);

      m_strBroadcastString = "";

      /* SSL parameters */
      m_strKeyFile = str_key_file;
      m_strCertFile = str_cert_file;
      m_strDHparamsFile = str_dh_params_file;
      m_strCAFile = str_ca_file;
      m_strPassphrase = str_cert_passphrase;
    }

    /****************************************/
    /****************************************/

    CWebServer::~CWebServer() {}

    /****************************************/
    /****************************************/

    void CWebServer::Start(std::atomic<bool> &b_IsServerRunning) {
      /* Check if ssl paramters are empty */
      if (
        m_strKeyFile.empty() && m_strCertFile.empty() &&
        m_strDHparamsFile.empty() && m_strCAFile.empty() &&
        m_strPassphrase.empty()) {
        /* Start with out SSL */
        this->RunServer<false>(b_IsServerRunning);
      } else {
        /* Start with SSL */
        this->RunServer<true>(b_IsServerRunning);
      }
    }

    /****************************************/
    /****************************************/

    template <bool SSL>
    void CWebServer::RunServer(std::atomic<bool> &b_IsServerRunning) {
      /* Create a vector for list of all connected clients */
      std::vector<SWebSocketClient<SSL>> vecWebSocketClients;

      /* Mutex to protect access to vecWebSocketClients */
      std::mutex mutex4VecWebClients;

      std::thread *tWebServerThread = new std::thread([&]() {
        try {
          /* Set up thread-safe buffers for this new thread */
          LOG.AddThreadSafeBuffer();
          LOGERR.AddThreadSafeBuffer();

          struct us_socket_context_options_t sSSLOptions;

          /* Build the SSL options object from parameters */
          if (!m_strKeyFile.empty()) {
            /* NOTE: string.c_str lifecycle is till end of this scope */
            sSSLOptions.key_file_name = m_strKeyFile.c_str();
          } else {
            sSSLOptions.key_file_name = nullptr;
          }

          if (!m_strCertFile.empty()) {
            sSSLOptions.cert_file_name = m_strCertFile.c_str();
          } else {
            sSSLOptions.cert_file_name = nullptr;
          }

          if (!m_strDHparamsFile.empty()) {
            sSSLOptions.dh_params_file_name = m_strDHparamsFile.c_str();
          } else {
            sSSLOptions.dh_params_file_name = nullptr;
          }

          if (!m_strCAFile.empty()) {
            sSSLOptions.ca_file_name = m_strCAFile.c_str();
          } else {
            sSSLOptions.ca_file_name = nullptr;
          }

          if (!m_strPassphrase.empty()) {
            sSSLOptions.passphrase = m_strPassphrase.c_str();
          } else {
            sSSLOptions.passphrase = nullptr;
          }

          auto cMyApp = uWS::TemplatedApp<SSL>(sSSLOptions);

          /* Setup WebSockets from the templated app */
          cMyApp
            .template ws<m_sPerSocketData>(
              "/*",
              {/* Settings */
               .compression = uWS::DEDICATED_COMPRESSOR_8KB,
               .maxPayloadLength = 16 * 1024,
               .idleTimeout = 10,
               .maxBackpressure = 16 * 1024,
               /* Handlers */
               /* new client is connected */
               .open =
                 [&](
                   uWS::WebSocket<SSL, true> *pc_ws, uWS::HttpRequest *pc_req) {
                   /* Selectivly subscribe to different channels */
                   if (pc_req->getQuery().size() > 0) {
                     std::stringstream str_stream(
                       std::string(pc_req->getQuery()));
                     std::string str_token;
                     while (std::getline(str_stream, str_token, ',')) {
                       pc_ws->subscribe(str_token);
                     }
                   } else {
                     /* making every connection subscribe to the "broadcast",
                      * "events" and "logs" topics */
                     pc_ws->subscribe("broadcasts");
                     pc_ws->subscribe("events");
                     pc_ws->subscribe("logs");
                   }

                   /* Guard the mutex which locks vecWebSocketClients */
                   std::lock_guard<std::mutex> guard(mutex4VecWebClients);

                   /* Add to list of clients connected */
                   vecWebSocketClients.push_back({pc_ws, uWS::Loop::get()});

                   LOG << "1 client connected (Total: "
                       << vecWebSocketClients.size() << ")" << '\n';
                 },
               /* Incoming message from client */
               .message =
                 [&](
                   uWS::WebSocket<SSL, true> *pc_ws,
                   std::string_view strv_message,
                   uWS::OpCode e_opCode) {
                   if (strv_message.compare("play") == 0) {
                     m_pcMyWebviz->PlayExperiment();
                   } else if (strv_message.compare("pause") == 0) {
                     m_pcMyWebviz->PauseExperiment();
                   } else if (strv_message.compare("step") == 0) {
                     m_pcMyWebviz->StepExperiment();
                   } else if (strv_message.compare("reset") == 0) {
                     m_pcMyWebviz->ResetExperiment();
                   } else if (strv_message.compare("fastforward") == 0) {
                     m_pcMyWebviz->FastForwardExperiment();
                   }
                 },
               .drain =
                 [](uWS::WebSocket<SSL, true> *ws) {
                   LOG << "Drainage: " << ws->getBufferedAmount() << '\n';
                 },
               .ping =
                 [](uWS::WebSocket<SSL, true> *ws) { LOG << "Ping" << '\n'; },
               .pong =
                 [](uWS::WebSocket<SSL, true> *ws) { LOG << "Pong" << '\n'; },
               .close =
                 [&](
                   uWS::WebSocket<SSL, true> *pc_ws,
                   int n_code,
                   std::string_view strv_message) {
                   /* client automatically unsubscribe from any topic here */

                   /* Guard the mutex which locks vecWebSocketClients */
                   std::lock_guard<std::mutex> guard(mutex4VecWebClients);

                   /* Remove from the list of all clients connected */
                   for (size_t i = 0; i < vecWebSocketClients.size(); i++) {
                     if (vecWebSocketClients[i].m_cWS == pc_ws) {
                       vecWebSocketClients.erase(
                         vecWebSocketClients.begin() + i);
                     }
                   }
                   LOG << "1 client disconnected (Total: "
                       << vecWebSocketClients.size() << ")" << '\n';
                 }})
            /* HTML banner */
            .get(
              "/", /* Start with SSL */
              [](auto *res, auto *req) {
                res->cork([res]() {
                  std::stringstream strs_stream;
                  strs_stream << "Reached ARGoS-Webviz server\n\n";
                  strs_stream << "Webviz version: ";
                  strs_stream << ARGOS_WEBVIZ_VERSION;
                  strs_stream << '\n';
                  strs_stream << "ARGoS3 version: ";
                  strs_stream << ARGOS_VERSION;
                  strs_stream << '\n';
                  strs_stream << "ARGoS3 release: ";
                  strs_stream << ARGOS_RELEASE;
                  strs_stream << '\n';

                  res->end(strs_stream.str());
                });
              })
            /* Start listening to Port */
            .listen(
              m_unPort,
              [&](auto *pc_token) {
                if (pc_token) {
                  LOG << "[INFO] Thread listening on port " << m_unPort << '\n';
                } else {
                  throw CARGoSException(
                    "[Error] CWebServer::Start() failed to listen on "
                    "port " +
                    std::to_string(m_unPort));
                  return;
                }
              })
            .run();  // Blocking the thread
        } catch (CARGoSException &ex) {
          THROW_ARGOSEXCEPTION_NESTED("[ERROR] Error in the webserver:", ex);
        }
      });

      std::thread *tBroadcasterThread = new std::thread([&]() {
        /* Start broadcast timer */
        m_cBroadcastTimer.Start();

        /* copy strings to free up the locks */
        /* Declaring local static here to help with lambda catching inside */
        static std::string strBroadcastString;
        static std::string strEventString;
        static std::string strLogString;

        while (b_IsServerRunning) {
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
          /* Mutex block for m_mutex4BroadcastString */
          {
            std::lock_guard<std::mutex> guard(m_mutex4BroadcastString);
            strBroadcastString = m_strBroadcastString;
          }  // End of mutex block: m_mutex4BroadcastString

          /* Mutex block for m_mutex4EventQueue */
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

          /* Mutex block for m_mutex4LogQueue */
          {
            std::lock_guard<std::mutex> guard(m_mutex4LogQueue);

            /* Get all log messages in one string with \n */
            std::stringstream str_stream;
            while (!m_cLogQueue.empty()) {
              str_stream << m_cLogQueue.front() << "\n";
              m_cLogQueue.pop();
            }
            strLogString = str_stream.str();
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
                    "broadcasts",
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
      });

      /* Join all the threads */
      tWebServerThread->join();
      tBroadcasterThread->join();
    }

    /****************************************/
    /****************************************/

    void CWebServer::EmitEvent(
      std::string str_event_name, argos::Webviz::EExperimentState e_state) {
      nlohmann::json cMyJson;
      cMyJson["event"] = str_event_name;
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