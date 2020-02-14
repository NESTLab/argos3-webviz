#ifndef ARGOS_NETWORKAPI_WEBSERVER_H
#define ARGOS_NETWORKAPI_WEBSERVER_H

#include "networkapi_webserver.fwd.h"

#include "networkapi.fwd.h"

#include "networkapi.h"

#include <loguru.hpp>
#include <nlohmann/json.hpp>
#include <string_view>
#include "App.h"
#include "helpers/EExperimentState.h"

namespace argos {
  namespace NetworkAPI {
    class CWebServer {
     private:
      /** HTTP Port to Listen to */
      unsigned short m_unPort;

      /* Reference to CNetworkAPI object to call function over it */
      argos::CNetworkAPI* m_pcMyNetworkAPI;

      /** Threads serving web requests */
      std::vector<std::thread*> m_vecWebThreads;

      /** List of all WebSocket clients connected */
      std::vector<uWS::WebSocket<false, true>*> m_vecWebSocketClients;

      /* Mutex to protect access to m_vecWebSocketClients */
      std::mutex m_mutex_web_clients;

      /* Data attached to each socket,
       * ws->getUserData returns one of these */
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
      CWebServer(argos::CNetworkAPI*, short unsigned);
      ~CWebServer();

      void Start();

      /** Broadcasts on event channel to all the connected clients */
      void EmitEvent(std::string_view, NetworkAPI::EExperimentState);

      /** Broadcasts experiment state to all the connected clients */
      void Broadcast();
    };
  }  // namespace NetworkAPI
}  // namespace argos
#endif