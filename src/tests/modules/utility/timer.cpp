#include <chrono>
#include <sstream>
#include <thread>

#include "gtest/gtest.h"
#include "plugins/simulator/visualizations/webviz/utility/CTimer.h"

TEST(UtilityTimer, FunctionIsRunning) {
  auto timer = new argos::Webviz::CTimer();

  EXPECT_FALSE(timer->IsRunning());

  timer->Start();

  EXPECT_TRUE(timer->IsRunning());

  timer->Stop();

  EXPECT_FALSE(timer->IsRunning());

  delete timer;
};

/****************************************/
/****************************************/

TEST(UtilityTimer, FunctionReset) {
  auto timer = new argos::Webviz::CTimer();

  EXPECT_FALSE(timer->IsRunning());

  timer->Start();

  EXPECT_TRUE(timer->IsRunning());

  /* test if Reset stops the timer */
  timer->Reset();

  EXPECT_FALSE(timer->IsRunning());
  EXPECT_EQ(0, timer->Elapsed().count());

  delete timer;
};

/****************************************/
/****************************************/

TEST(UtilityTimer, FunctionStop) {
  auto timer = new argos::Webviz::CTimer();

  EXPECT_FALSE(timer->IsRunning());

  timer->Stop();

  EXPECT_FALSE(timer->IsRunning());

  timer->Start();
  timer->Stop();

  EXPECT_FALSE(timer->IsRunning());

  delete timer;
};

/****************************************/
/****************************************/

TEST(UtilityTimer, FunctionElapsed) {
  auto timer = new argos::Webviz::CTimer();

  // Expecting 0 milliseconds before timer starts
  EXPECT_EQ(0, timer->Elapsed().count());

  timer->Start();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Expecting 100 milliseconds after sleep of 100 ms
  EXPECT_EQ(100, timer->Elapsed().count());

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  timer->Stop();

  // Expecting 200 milliseconds after combined sleep of 200 ms
  EXPECT_EQ(200, timer->Elapsed().count());

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Still expecting 200 milliseconds even long after timer has stopped
  EXPECT_EQ(200, timer->Elapsed().count());

  delete timer;
};

/****************************************/
/****************************************/

TEST(UtilityTimer, StreamOperator) {
  std::stringstream ss;
  auto timer = argos::Webviz::CTimer();

  ss << timer;

  EXPECT_EQ("0", ss.str());

  /* Empty the stringstream */
  ss.str(std::string());

  /* Run the timer for 100 millis */
  timer.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  timer.Stop();

  ss << timer;

  EXPECT_EQ("100", ss.str());
};
