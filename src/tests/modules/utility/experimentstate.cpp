
#include <sstream>

#include "gtest/gtest.h"
#include "plugins/simulator/visualizations/webviz/utility/EExperimentState.h"

using argos::Webviz::EExperimentState;

TEST(UtilityExperimentState, CheckAllStatesToStr) {
  ASSERT_EQ(
    "EXPERIMENT_INITIALIZED",
    EExperimentStateToStr(EExperimentState::EXPERIMENT_INITIALIZED));

  /************************************************/

  ASSERT_EQ(
    "EXPERIMENT_PLAYING",
    EExperimentStateToStr(EExperimentState::EXPERIMENT_PLAYING));

  /************************************************/

  ASSERT_EQ(
    "EXPERIMENT_PAUSED",
    EExperimentStateToStr(EExperimentState::EXPERIMENT_PAUSED));

  /************************************************/

  ASSERT_EQ(
    "EXPERIMENT_FAST_FORWARDING",
    EExperimentStateToStr(EExperimentState::EXPERIMENT_FAST_FORWARDING));

  /************************************************/

  ASSERT_EQ(
    "EXPERIMENT_DONE",
    EExperimentStateToStr(EExperimentState::EXPERIMENT_DONE));
};

/****************************************/
/****************************************/

TEST(UtilityExperimentState, InvalidValue) {
  auto temp = new EExperimentState();

  delete temp;

  /* Forcefully test for invalid State */

  ASSERT_EQ("unknown", EExperimentStateToStr(*temp));
};
