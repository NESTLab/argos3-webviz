/**
 * @file <argos3/plugins/simulator/visualizations/webviz/utility/CTimer.h>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#ifndef ARGOS_WEBVIZ_TIMER_H
#define ARGOS_WEBVIZ_TIMER_H

#include <chrono>
#include <ostream>
#include <type_traits>

namespace argos {
  namespace Webviz {
    class CTimer {
      typedef typename std::conditional<
        std::chrono::high_resolution_clock::is_steady,
        std::chrono::high_resolution_clock,
        std::chrono::steady_clock>::type TClockType;

      typedef std::chrono::milliseconds TMilliseconds;

     public:
      CTimer() { Reset(); }

      /****************************************/
      /****************************************/

      void Start() {
        m_StartTime = TClockType::now();
        m_EndTime = m_StartTime;
        m_bRunning = true;
      }

      /****************************************/
      /****************************************/

      void Stop() {
        m_EndTime = TClockType::now();
        m_bRunning = false;
      }

      /****************************************/
      /****************************************/

      void Reset() {
        m_StartTime = TClockType::now();
        m_EndTime = m_StartTime;
        m_bRunning = false;
      }

      /****************************************/
      /****************************************/

      TMilliseconds Elapsed() const {
        TClockType::time_point endTime;

        if (m_bRunning) {
          endTime = TClockType::now();
        } else {
          endTime = m_EndTime;
        }

        return std::chrono::duration_cast<TMilliseconds>(endTime - m_StartTime);
      }

      /****************************************/
      /****************************************/

      bool IsRunning() { return m_bRunning; }

      /****************************************/
      /****************************************/

      template <typename T, typename Traits>
      friend std::basic_ostream<T, Traits>& operator<<(
        std::basic_ostream<T, Traits>& out, const CTimer& timer) {
        return out << timer.Elapsed().count();
      }

     private:
      TClockType::time_point m_StartTime;
      TClockType::time_point m_EndTime;
      bool m_bRunning;
    };
  }  // namespace Webviz
}  // namespace argos

#endif