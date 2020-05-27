#include "plugins/simulator/visualizations/webviz/utility/LogStream.h"

#include <sstream>  // std::stringstream

#include "gtest/gtest.h"

TEST(UtilityLogStream, BasicStreamTest) {
  std::stringstream ss;

  auto log = new argos::Webviz::CLogStream(ss, [](std::string str_logData) {
    /*  */
    EXPECT_EQ("LOG:1", str_logData);
  });

  ss << "LOG";
  ss << ':';
  ss << 1;

  delete log;
};

/****************************************/
/****************************************/

TEST(UtilityLogStream, EscapingCharacters) {
  std::stringstream ss;

  new argos::Webviz::CLogStream(ss, [](std::string str_logData) {
    /*  */
    EXPECT_EQ("&lt;Test&gt;", str_logData);
  });

  ss << "<Test>\n"; /* program looks for newline to process escaping */
};

/****************************************/
/****************************************/

TEST(UtilityLogStream, OverflowFunction) {
  /* Test Overflow function */
  std::stringstream ss;

  auto log = new argos::Webviz::CLogStream(ss, [](std::string str_logData) {
    /*  */
    EXPECT_EQ("a", str_logData);
  });

  /* add temp character */
  log->overflow('a');

  /* Send newline to start processing */
  log->overflow('\n');

  delete log;
};