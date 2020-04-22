#ifndef TEST_USER_LOOP_FUNCTIONS_H
#define TEST_USER_LOOP_FUNCTIONS_H

#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include <argos3/plugins/simulator/visualizations/webviz/webviz_user_functions.h>

#include <iostream>

using namespace argos;

class CTestUserFunctions : public CWebvizUserFunctions {
 public:
  CTestUserFunctions();

  virtual ~CTestUserFunctions();

  virtual nlohmann::json sendExtraData();
};

#endif