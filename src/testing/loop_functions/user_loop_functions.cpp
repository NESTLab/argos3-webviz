#include "user_loop_functions.h"

/****************************************/
/****************************************/

CTestUserFunctions::CTestUserFunctions() {
  RegisterWebvizUserFunction<CTestUserFunctions, CFootBotEntity>(
    &CTestUserFunctions::sendRobotData);
}

/****************************************/
/****************************************/

CTestUserFunctions::~CTestUserFunctions() {}

/****************************************/
/****************************************/

nlohmann::json CTestUserFunctions::sendRobotData(CFootBotEntity &robot) {
  nlohmann::json outJson;

  outJson["distance_travelled"] = 1.46;

  outJson["bump_count"] = 48128;

  return outJson;
}

nlohmann::json CTestUserFunctions::sendExtraData() {
  nlohmann::json outJson;

  outJson["status"] = "running";

  std::vector<float> weights;
  weights.push_back(0.14525);
  weights.push_back(0.8);
  weights.push_back(0.342);
  weights.push_back(-0.456);
  weights.push_back(0.12342);

  outJson["weights"] = weights;

  return outJson;
}

/****************************************/
/****************************************/

REGISTER_WEBVIZ_USER_FUNCTIONS(CTestUserFunctions, "test_user_functions")
