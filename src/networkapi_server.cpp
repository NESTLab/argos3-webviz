#include "networkapi_server.h"
#include "helpers/utils.h"

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/space/space.h>

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
    } else {
      /* Use normal clock
       */
      m_tStepFunction = &CNetworkAPI::NormalStep;
    }
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::Execute() {
    try {
      LOG << "Starting " << m_vecWebThreads.size() << " threads for WebServer"
          << std::endl;

      /* Start Webserver */
      std::transform(
        m_vecWebThreads.begin(),
        m_vecWebThreads.end(),
        m_vecWebThreads.begin(),
        [this](std::thread *t) {
          return new std::thread([this]() {
            auto app = uWS::App();

            /* Setup WebSockets */
            app.ws<m_sPerSocketData>(
              "/*",
              {/* Settings */
               .compression = uWS::SHARED_COMPRESSOR,
               .maxPayloadLength = 16 * 1024 * 1024,
               .idleTimeout = 10,
               .maxBackpressure = 1 * 1024 * 1204,
               /* Handlers */
               .open =
                 [this](auto *ws, auto *req) {
                   /*
                    * making every connection subscribe to the
                    * "broadcast" topic
                    */
                   ws->subscribe("broadcast");
                   /*
                    * Add to list of clients connected
                    */
                   m_vecWebSocketClients.push_back(ws);
                 },
               .message =
                 [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                   std::cout << "broadcast message" << std::endl;
                   /* broadcast every single message it got */
                   ws->publish("broadcast", message, opCode);
                 },
               .close =
                 [this](auto *ws, int code, std::string_view message) {
                   /* it automatically unsubscribe from any topic here */

                   /*
                    * Remove from the list of all clients connected
                    */
                   EraseFromVector(m_vecWebSocketClients, ws);
                 }});

            /* Setup routes */
            app.get(
              "/*", [](auto *res, auto *req) { res->end("Hello world!"); });

            /* Start listening to Port */
            app
              .listen(
                m_unPort,
                [this](auto *token) {
                  if (token) {
                    std::cout << "Thread " << std::this_thread::get_id()
                              << " listening on port " << m_unPort << "\n";
                  } else {
                    std::cout << "Thread " << std::this_thread::get_id()
                              << " failed to listen on port" << m_unPort
                              << "\n";
                  }
                })
              .run();
          });
        });

      /* Main cycle */
      while (!m_cSimulator.IsExperimentFinished()) {
        (this->*m_tStepFunction)();
        BroadcastMessage("Step");
      }

      /* The experiment is finished */
      m_cSimulator.GetLoopFunctions().PostExperiment();

      std::for_each(
        m_vecWebThreads.begin(), m_vecWebThreads.end(), [](std::thread *t) {
          t->join();
        });
    } catch (CARGoSException &ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error while executing the experiment.", ex);
    }
    LOG.Flush();
    LOGERR.Flush();
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
      LOGERR << "[WARNING] "
                "Clock tick "
                "took "
             << TVTimeToHumanReadable(m_tStepElapsedTime)
             << " sec, more "
                "than the "
                "expected "
             << TVTimeToHumanReadable(m_tStepClockTime) << " sec." << std::endl;
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