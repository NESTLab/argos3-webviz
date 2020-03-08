/**
 * @file
 * <argos3/plugins/simulator/visualizations/webviz/webviz_webserver.h>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#ifndef ARGOS_WEBVIZ_WEBSERVER_H
#define ARGOS_WEBVIZ_WEBSERVER_H

/* Loguru with streams interface */
#define LOGURU_WITH_STREAMS 1

namespace argos {
  class CWebviz;

  namespace Webviz {
    class CWebServer;
    class CTimer;
    enum class EExperimentState;
  }  // namespace Webviz
}  // namespace argos

#include <loguru.hpp>
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
      /** HTTP Port to Listen to */
      unsigned short m_unPort;

      /** max time for one broadcast cycle */
      std::chrono::milliseconds m_cBroadcastDuration;

      /** broadcast cycle timer */
      CTimer m_cBroadcastTimer;

      /** mutexed string using m_mutex4BroadcastString to broadcast */
      std::string m_strBroadcastString;

      /** Reference to CWebviz object to call function over it */
      CWebviz* m_pcMyWebviz;

      /** Threads serving web requests */
      std::vector<std::thread*> m_vecWebThreads;

      /** A Queue to push events to client */
      std::queue<std::string> m_cEventQueue;

      /** A Queue to push logs to client */
      std::queue<std::string> m_cLogQueue;

      /** Struct to hold websocket with its loop thread */
      struct SWebSocketClient {
        uWS::WebSocket<false, true>* m_cWS;
        struct uWS::Loop* m_cLoop;
      };

      /** List of all WebSocket clients connected */
      std::vector<SWebSocketClient> m_vecWebSocketClients;

      /** Mutex to protect access to m_vecWebSocketClients */
      std::mutex m_mutex4VecWebClients;

      /** Mutex to protect access to m_vecWebSocketClients */
      std::mutex m_mutex4BroadcastString;

      /** Mutex to protect access to m_cEventQueue */
      std::mutex m_mutex4EventQueue;

      /** Mutex to protect access to m_cLogQueue */
      std::mutex m_mutex4LogQueue;

      /** Data attached to each socket, ws->getUserData returns one of these */
      struct m_sPerSocketData {};

      /** Function to setup all routes and webhooks */
      void SetupWebApp(uWS::App&);

      /** Function to send JSON over HttpResponse */
      void SendJSON(uWS::HttpResponse<false>*, nlohmann::json);

      /** Function to send JSON with Error over HttpResponse */
      void SendJSONError(
        uWS::HttpResponse<false>*,
        nlohmann::json,
        std::string = "400 Bad Request");

     public:
      CWebServer(CWebviz*, unsigned short, unsigned short);
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