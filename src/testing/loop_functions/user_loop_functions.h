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

  virtual const nlohmann::json sendUserData();

  virtual void HandleCommandFromClient(
    const std::string& str_ip, nlohmann::json c_json_command);

  const nlohmann::json sendRobotData(CFootBotEntity&);
};

#endif