/**
 * @file
 * <argos3/plugins/simulator/visualizations/webviz/webviz_user_functions.cpp>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#include "webviz_user_functions.h"

namespace argos {

  CWebvizUserFunctions::CWebvizUserFunctions() : m_vecFunctionHolders(1) {
    m_cThunks.Add<CEntity>((TThunk)NULL);
  }

  /****************************************/
  /****************************************/

  CWebvizUserFunctions::~CWebvizUserFunctions() {
    while (!m_vecFunctionHolders.empty()) {
      delete m_vecFunctionHolders.back();
      m_vecFunctionHolders.pop_back();
    }
  }

  /****************************************/
  /****************************************/

  // cppcheck-suppress unusedFunction
  const nlohmann::json CWebvizUserFunctions::Call(CEntity& c_entity) {
    TThunk t_thunk = m_cThunks[c_entity.GetTag()];
    if (t_thunk) {
      return (this->*t_thunk)(c_entity);
    } else {
      return nullptr;
    }
  }

}  // namespace argos