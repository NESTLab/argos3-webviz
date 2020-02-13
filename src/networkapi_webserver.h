#ifndef ARGOS_NETWORKAPI_WEBSERVER_H
#define ARGOS_NETWORKAPI_WEBSERVER_H

#include "networkapi_webserver.fwd.h"

#include "networkapi.fwd.h"

#include "networkapi.h"

#include <loguru.hpp>
#include <nlohmann/json.hpp>
#include <string_view>
#include "App.h"

namespace argos {
  namespace NetworkAPI {
    class CWebServer {
     private:
      /** HTTP Port to Listen to */
      unsigned short m_unPort;

      /* Reference to CNetworkAPI object to call function over it */
      argos::CNetworkAPI* m_pcMyNetworkAPI;

      /** Threads serving web requests */
      std::vector<uWS::WebSocket<false, true>*> m_vecWebSocketClients;
      /** List of all WebSocket clients connected */
      std::vector<std::thread*> m_vecWebThreads;

      /* Data attached to each socket,
       * ws->getUserData returns one of these */
      struct m_sPerSocketData {};

      /** Broadcasts a message to all the connected clients */
      void BroadcastMessage(std::string_view message);

      /** Broadcasts on event channel to all the connected clients */
      void EmitEvent(std::string_view event_name);

      /** Broadcasts game state to all the connected clients */
      void BroadcastState();

      /** Function to setup all routes and webhooks */
      void SetupWebApp(uWS::App& c_MyApp);

      /** Function to send JSON over HttpResponse */
      void SendJSON(
        uWS::HttpResponse<false>* pc_res, nlohmann::json c_json_data);

      //   inline std::string EExperimentStateToString(EExperimentState type) {
      //     switch (type) {
      //       case EXPERIMENT_INITIALIZED:
      //         return "EXPERIMENT_INITIALIZED";
      //       case EXPERIMENT_PLAYING:
      //         return "EXPERIMENT_PLAYING";
      //       case EXPERIMENT_FAST_FORWARDING:
      //         return "EXPERIMENT_FAST_FORWARDING";
      //       case EXPERIMENT_PAUSED:
      //         return "EXPERIMENT_PAUSED";
      //       case EXPERIMENT_SUSPENDED:
      //         return "EXPERIMENT_SUSPENDED";
      //       case EXPERIMENT_DONE:
      //         return "EXPERIMENT_DONE";
      //       default:
      //         return "unknown";
      //     }
      //   }

     public:
      CWebServer(argos::CNetworkAPI*, short unsigned);
      ~CWebServer();

      void Start();
    };
  }  // namespace NetworkAPI
}  // namespace argos
#endif