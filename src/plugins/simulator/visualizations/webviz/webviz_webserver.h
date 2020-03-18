/**
 * @file <argos3/plugins/simulator/visualizations/webviz/webviz_webserver.h>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#ifndef ARGOS_WEBVIZ_WEBSERVER_H
#define ARGOS_WEBVIZ_WEBSERVER_H

namespace argos {
  class CWebviz;

  namespace Webviz {
    class CWebServer;
    class CTimer;
    enum class EExperimentState;
  }  // namespace Webviz
}  // namespace argos

#include <future>
#include <mutex>
#include <nlohmann/json.hpp>
#include <queue>
#include <string_view>
#include "App.h"  // uWebSockets
#include "config.h"
#include "utility/CTimer.h"
#include "utility/EExperimentState.h"
#include "webviz.h"

namespace argos {
  namespace Webviz {
    class CWebServer {
     private:
      /** Reference to CWebviz object to call function over it */
      CWebviz* m_pcMyWebviz;

      /** HTTP Port to Listen to */
      unsigned short m_unPort;

      /** broadcast cycle timer */
      CTimer m_cBroadcastTimer;

      /** max time for one broadcast cycle */
      std::chrono::milliseconds m_cBroadcastDuration;

      /** mutexed string using m_mutex4BroadcastString to broadcast */
      std::string m_strBroadcastString;

      /** A Queue to push events to client */
      std::queue<std::string> m_cEventQueue;

      /** A Queue to push logs to client */
      std::queue<nlohmann::json> m_cLogQueue;

      /** Struct to hold websocket with its loop thread */
      template <bool SSL>
      struct SWebSocketClient {
        uWS::WebSocket<SSL, true>* m_cWS;
        struct uWS::Loop* m_cLoop;
      };

      /** Mutex to protect access to m_mutex4BroadcastString */
      std::mutex m_mutex4BroadcastString;

      /** Mutex to protect access to m_cEventQueue */
      std::mutex m_mutex4EventQueue;

      /** Mutex to protect access to m_cLogQueue */
      std::mutex m_mutex4LogQueue;

      /** SSL options */
      std::string m_strKeyFile;
      std::string m_strCertFile;
      std::string m_strDHparamsFile;
      std::string m_strCAFile;
      std::string m_strPassphrase;

      /** Data attached to each socket, ws->getUserData returns one of these */
      struct m_sPerSocketData {};

      /**
       * @brief Function to run server depending on SSL
       *
       * @tparam SSL bool: to start with SSL
       * @param b_IsServerRunning bool: used to stop the threads gracefully
       */
      template <bool SSL>
      void RunServer(std::atomic<bool>& b_IsServerRunning);

      /** Function to send JSON over HttpResponse */
      template <bool SSL>
      void SendJSON(uWS::HttpResponse<SSL>*, nlohmann::json);

      /** Function to send JSON with Error over HttpResponse */
      template <bool SSL>
      void SendJSONError(
        uWS::HttpResponse<SSL>*,
        nlohmann::json,
        std::string = "400 Bad Request");

     public:
      CWebServer(
        CWebviz*,
        unsigned short,
        unsigned short,
        std::string&,
        std::string&,
        std::string&,
        std::string&,
        std::string&);

      ~CWebServer();

      /**
       * @brief Start the Webserver threads (one for server, one for
       * broadcasting)
       *
       * @param b_IsServerRunning used to stop the threads gracefully
       */
      void Start(std::atomic<bool>& b_IsServerRunning);

      /**
       * @brief Broadcasts on event channel to all the connected clients
       *
       * @param str_event_name string: event name
       * @param e_state EExperimentState: state of the experiment
       */
      void EmitEvent(std::string str_event_name, EExperimentState e_state);

      /**
       * @brief Broadcasts on log channels to all the connected clients
       *
       * @param log_type either LOG or LOGERR
       * @param timestep simulation clock/timestep
       * @param message log message
       */
      void EmitLog(
        std::string log_type, std::string timestep, std::string message);

      /** Broadcasts JSON to all the connected clients */
      void Broadcast(nlohmann::json);
    };
  }  // namespace Webviz
}  // namespace argos
#endif