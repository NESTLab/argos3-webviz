#ifndef ARGOS_NETWORKAPI_WEBSERVER_H
#define ARGOS_NETWORKAPI_WEBSERVER_H

#include "networkapi_webserver.fwd.h"

#include "networkapi.fwd.h"

#include "networkapi.h"

#include <loguru.hpp>
#include <mutex>
#include <nlohmann/json.hpp>
#include <queue>
#include <string_view>
#include "App.h"
#include "helpers/EExperimentState.h"

namespace argos {
  namespace NetworkAPI {
    class CWebServer {
     private:
      /** HTTP Port to Listen to */
      unsigned short m_unPort;

      /** max time for one broadcast cycle */
      std::chrono::milliseconds m_cBroadcastDuration;

      /** broadcast cycle timer */
      NetworkAPI::Timer m_cBroadcastTimer;

      /** mutexed string using m_mutex4BroadcastString to broadcast */
      std::string m_strBroadcastString;

      /** Reference to CNetworkAPI object to call function over it */
      argos::CNetworkAPI* m_pcMyNetworkAPI;

      /** Threads serving web requests */
      std::vector<std::thread*> m_vecWebThreads;

      /** A Queue to push events to client */
      std::queue<std::string> m_cEventQueue;

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
      CWebServer(argos::CNetworkAPI*, unsigned short, unsigned short);
      ~CWebServer();

      void Start();

      /** Broadcasts on event channel to all the connected clients */
      void EmitEvent(std::string_view, NetworkAPI::EExperimentState);

      /** Broadcasts JSON to all the connected clients */
      void Broadcast(nlohmann::json);
    };
  }  // namespace NetworkAPI
}  // namespace argos
#endif