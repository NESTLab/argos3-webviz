/**
 * @file
 * <argos3/plugins/simulator/visualizations/webviz/entity/webviz_floor.cpp>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#include <argos3/core/simulator/entity/floor_entity.h>
#include <argos3/plugins/simulator/visualizations/webviz/webviz.h>
#include <iomanip>
#include <nlohmann/json.hpp>

namespace argos {
  namespace Webviz {

    /****************************************/
    /****************************************/

    class CWebvizOperationGenerateFloorJSON
        : public CWebvizOperationGenerateJSON {
     public:
      nlohmann::json ApplyTo(CWebviz& c_webviz, CFloorEntity& c_entity) {
        nlohmann::json cJson;

        cJson["type"] = c_entity.GetTypeDescription();
        cJson["id"] = c_entity.GetId();

        // cArenaSize.GetX()
        // c_entity.GetColorAtPoint();
        // TODO implement Floor image
        return cJson;
      }
    };

    REGISTER_WEBVIZ_ENTITY_OPERATION(
      CWebvizOperationGenerateJSON,
      CWebvizOperationGenerateFloorJSON,
      CFloorEntity);

  }  // namespace Webviz
}  // namespace argos