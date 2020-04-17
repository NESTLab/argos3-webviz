/**
 * @file
 * <argos3/plugins/simulator/visualizations/webviz/entity/webviz_light.cpp>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#include <argos3/plugins/simulator/entities/light_entity.h>
#include <argos3/plugins/simulator/visualizations/webviz/webviz.h>

#include <nlohmann/json.hpp>

namespace argos {
  namespace Webviz {

    /****************************************/
    /****************************************/

    class CWebvizOperationGenerateLightJSON
        : public CWebvizOperationGenerateJSON {
     public:
      nlohmann::json ApplyTo(CWebviz& c_webviz, CLightEntity& c_entity) {
        nlohmann::json cJson;

        cJson["type"] = c_entity.GetTypeDescription();
        cJson["id"] = c_entity.GetId();

        /* Get the position of the box */
        const argos::CVector3& cPosition = c_entity.GetPosition();

        /* Add it to json as => position:{x, y, z} */
        cJson["position"]["x"] = cPosition.GetX();
        cJson["position"]["y"] = cPosition.GetY();
        cJson["position"]["z"] = cPosition.GetZ();

        /* Get the orientation of the box */
        const argos::CQuaternion& cOrientation = c_entity.GetOrientation();

        cJson["orientation"]["x"] = cOrientation.GetX();
        cJson["orientation"]["y"] = cOrientation.GetY();
        cJson["orientation"]["z"] = cOrientation.GetZ();
        cJson["orientation"]["w"] = cOrientation.GetW();

        std::stringstream strColorStream;

        const CColor& cColor = c_entity.GetColor();
        /* Convert to hex color*/
        strColorStream << "0x" << std::setfill('0') << std::setw(6) << std::hex
                       << (cColor.GetRed() << 16 | cColor.GetGreen() << 8 |
                           cColor.GetBlue());

        cJson["color"] = strColorStream.str();

        return cJson;
      }
    };

    REGISTER_WEBVIZ_ENTITY_OPERATION(
      CWebvizOperationGenerateJSON,
      CWebvizOperationGenerateLightJSON,
      CLightEntity);

  }  // namespace Webviz
}  // namespace argos