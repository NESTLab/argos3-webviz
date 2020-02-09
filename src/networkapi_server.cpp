#include "networkapi_server.h"
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/space/space.h>

#include <unistd.h>

namespace argos {

   /****************************************/
   /****************************************/

   static Real TVTimeToHumanReadable(::timeval& t_time) {
      return
         static_cast<Real>(t_time.tv_sec) +
         static_cast<Real>(t_time.tv_usec * 10e-6);
   }

   /****************************************/
   /****************************************/

   CNetworkAPI::CNetworkAPI() {
      /* Set the pointer to the step function */
      if(m_cSimulator.IsRealTimeClock()) {
         /* Use real-time clock and set time structures */
         m_tStepFunction = &CNetworkAPI::RealTimeStep;
         timerclear(&m_tStepClockTime);
         m_tStepClockTime.tv_usec = 1e6 * CPhysicsEngine::GetSimulationClockTick();
         ::gettimeofday(&m_tStepStartTime, NULL);
      }
      else {
         /* Use normal clock */
         m_tStepFunction = &CNetworkAPI::NormalStep;
      }
   }

   /****************************************/
   /****************************************/

   void CNetworkAPI::Execute() {
      /* Main cycle */
      while(!m_cSimulator.IsExperimentFinished()) {
         (this->*m_tStepFunction)();
      }
      /* The experiment is finished */
      m_cSimulator.GetLoopFunctions().PostExperiment();
      LOG.Flush();
      LOGERR.Flush();
   }

   /****************************************/
   /****************************************/

   void CNetworkAPI::NormalStep() {
      m_cSimulator.UpdateSpace();
   }

   /****************************************/
   /****************************************/

   void CNetworkAPI::RealTimeStep() {
      /* m_tStepStartTime has already been set */
      m_cSimulator.UpdateSpace();
      /* Take the time now */
      ::gettimeofday(&m_tStepEndTime, NULL);
      /* Calculate the elapsed time */
      timersub(&m_tStepEndTime, &m_tStepStartTime, &m_tStepElapsedTime);
      /* If the elapsed time is lower than the tick length, wait */
      if(!timercmp(&m_tStepElapsedTime, &m_tStepClockTime, >)) {
         /* Calculate the waiting time */
         timersub(&m_tStepClockTime, &m_tStepElapsedTime, &m_tStepWaitTime);
         /* Wait */
         ::usleep(m_tStepWaitTime.tv_sec * 1e6 + m_tStepWaitTime.tv_usec);
         /* Get the new step end */
         ::gettimeofday(&m_tStepEndTime, NULL);
      }
      else {
         LOGERR << "[WARNING] Clock tick took "
                << TVTimeToHumanReadable(m_tStepElapsedTime)
                << " sec, more than the expected "
                << TVTimeToHumanReadable(m_tStepClockTime)
                << " sec."
                << std::endl;
      }
      /* Set the step start time to whatever the step end time is */
      m_tStepStartTime.tv_sec = m_tStepEndTime.tv_sec;
      m_tStepStartTime.tv_usec = m_tStepEndTime.tv_usec;
   }

   /****************************************/
   /****************************************/

   REGISTER_VISUALIZATION(CNetworkAPI,
                          "network-api",
                          "Prajankya [contact@prajankya.me]",
                          "1.0",
                          "Network API to render over network in clientside.",
                          " -- .\n",
                          "It allows the user to watch and modify the simulation as it's running in an\n"
                          "intuitive way.\n\n"
                          "REQUIRED XML CONFIGURATION\n\n"
                          "  <visualization>\n"
                          "    <network-api />\n"
                          "  </visualization>\n\n"
                          "OPTIONAL XML CONFIGURATION\n\n"
      );
}