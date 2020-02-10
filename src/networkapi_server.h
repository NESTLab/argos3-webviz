#ifndef NETWORKAPI_SERVER_H
#define NETWORKAPI_SERVER_H

namespace argos {
  class CNetworkAPI;
}

#include <argos3/core/simulator/visualization/visualization.h>
#include <sys/time.h>
#include <thread>
#include "App.h"

namespace argos {

  class CNetworkAPI : public CVisualization {
   public:
    CNetworkAPI() : m_vecWebThreads(std::thread::hardware_concurrency()) {}

    virtual ~CNetworkAPI() {}

    virtual void Reset() {}

    virtual void Destroy() {}

    virtual void Execute();

    virtual void Init(TConfigurationNode& t_tree);

   private:
    /** Performs a simulation step the normal way */
    void NormalStep();

    /** Performs a simulation step respecting the real-time constraint */
    void RealTimeStep();

    /** Broadcasts to all the connected clients */
    void BroadcastMessage(std::string_view message);

   private:
    typedef void (CNetworkAPI::*TStepFunction)();

   private:
    /** HTTP Port to Listen to */
    UInt16 m_unPort;

    /* Data attached to each socket,
     * ws->getUserData returns one of these */
    struct m_sPerSocketData {};

    /** Threads serving web requests */
    std::vector<uWS::WebSocket<false, true>*> m_vecWebSocketClients;

    /** List of all WebSocket clients connected */
    std::vector<std::thread*> m_vecWebThreads;

    /** Pointer to step function */
    TStepFunction m_tStepFunction;

    /** The length of a clock tick */
    ::timeval m_tStepClockTime;

    /** The starting time of a clock tick */
    ::timeval m_tStepStartTime;

    /** The end time of a clock tick*/
    ::timeval m_tStepEndTime;

    /** The elapsed time between start and end of a clock tick */
    ::timeval m_tStepElapsedTime;

    /** The time to wait to complete a clock tick */
    ::timeval m_tStepWaitTime;
  };

}  // namespace argos

#endif