#ifndef ID_QTUSER_FUNCTIONS_H
#define ID_QTUSER_FUNCTIONS_H

#include <argos3/plugins/simulator/visualizations/webviz/webviz_user_functions.h>

#include <iostream>

using namespace argos;

class CTestUserFunctions : public CWebvizUserFunctions {
 public:
  CTestUserFunctions();

  virtual ~CTestUserFunctions() {}
};

#endif