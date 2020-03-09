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

#include <mutex>
#include <nlohmann/json.hpp>
#include <queue>
#include <string_view>
#include "App.h"  // uWebSockets
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

      /** Threads serving web requests */
      std::vector<std::thread*> m_vecWebThreads;

      /** broadcast cycle timer */
      CTimer m_cBroadcastTimer;

      /** max time for one broadcast cycle */
      std::chrono::milliseconds m_cBroadcastDuration;

      /** mutexed string using m_mutex4BroadcastString to broadcast */
      std::string m_strBroadcastString;

      /** A Queue to push events to client */
      std::queue<std::string> m_cEventQueue;

      /** A Queue to push logs to client */
      std::queue<std::string> m_cLogQueue;

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

      /** SSL settings */
      struct us_socket_context_options_t m_sSSLOptions;

      /** Data attached to each socket, ws->getUserData returns one of these */
      struct m_sPerSocketData {};

      /** Function to intialize server depending on SSL */
      template <bool SSL>
      void InitServer(struct us_socket_context_options_t);

      /** Function to setup the webapp with all routes and webhooks */
      template <bool SSL>
      void SetupWebApp(uWS::TemplatedApp<SSL>&);

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

      void Start();

      /** Broadcasts on event channel to all the connected clients */
      void EmitEvent(std::string_view, EExperimentState);

      /** Broadcasts on log channels to all the connected clients */
      void EmitLog(std::string, std::string);

      /** Broadcasts JSON to all the connected clients */
      void Broadcast(nlohmann::json);
    };
  }  // namespace Webviz
}  // namespace argos
#endif