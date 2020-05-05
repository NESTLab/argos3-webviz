#include <argos3/plugins/simulator/visualizations/webviz/utility/PortCheck.h>

#include "gtest/gtest.h"

TEST(modules_utility_portcheck, CheckEmptyPort) {
  EXPECT_EQ(true, PortChecker::CheckPortTCPisAvailable(3000));
};
