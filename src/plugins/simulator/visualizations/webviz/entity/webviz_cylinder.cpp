/**
 * @file
 * <argos3/plugins/simulator/visualizations/webviz/entity/webviz_cylinder.cpp>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#include <argos3/plugins/simulator/entities/cylinder_entity.h>
#include <argos3/plugins/simulator/entities/led_equipped_entity.h>
#include <argos3/plugins/simulator/visualizations/webviz/webviz.h>

#include <iomanip>
#include <nlohmann/json.hpp>

namespace argos {
  namespace Webviz {

    /****************************************/
    /****************************************/

    class CWebvizOperationGenerateCylinderJSON
        : public CWebvizOperationGenerateJSON {
     public:
      nlohmann::json ApplyTo(CWebviz& c_webviz, CCylinderEntity& c_entity) {
        nlohmann::json cJson;

        cJson["type"] = c_entity.GetTypeDescription();
        cJson["id"] = c_entity.GetId();
        cJson["is_movable"] = c_entity.GetEmbodiedEntity().IsMovable();

        /* Get Size of the Cylinder */
        cJson["height"] = c_entity.GetHeight();
        cJson["radius"] = c_entity.GetRadius();

        /* Get the position of the box */
        const argos::CVector3& cPosition =
          c_entity.GetEmbodiedEntity().GetOriginAnchor().Position;

        /* Add it to json as => position:{x, y, z} */
        cJson["position"]["x"] = cPosition.GetX();
        cJson["position"]["y"] = cPosition.GetY();
        cJson["position"]["z"] = cPosition.GetZ();

        /* Get the orientation of the box */
        const argos::CQuaternion& cOrientation =
          c_entity.GetEmbodiedEntity().GetOriginAnchor().Orientation;

        cJson["orientation"]["x"] = cOrientation.GetX();
        cJson["orientation"]["y"] = cOrientation.GetY();
        cJson["orientation"]["z"] = cOrientation.GetZ();
        cJson["orientation"]["w"] = cOrientation.GetW();

        CLEDEquippedEntity& cLEDEquippedEntity =
          c_entity.GetLEDEquippedEntity();

        if (cLEDEquippedEntity.GetLEDs().size() > 0) {
          /* Building a string of all led colors */

          for (UInt32 i = 0; i < cLEDEquippedEntity.GetLEDs().size(); i++) {
            nlohmann::json cLedJson;

            std::stringstream strLEDStream;
            const CColor& cColor = cLEDEquippedEntity.GetLED(i).GetColor();

            strLEDStream << "#" << std::setfill('0') << std::setw(6) << std::hex
                         << (cColor.GetRed() << 16 | cColor.GetGreen() << 8 |
                             cColor.GetBlue());
            cLedJson["color"] = strLEDStream.str();

            /* Get the position of the box */
            const argos::CVector3& cLedPosition =
              cLEDEquippedEntity.GetLED(i).GetPosition();

            /* Add it to json as => position:{x, y, z} */
            cLedJson["position"]["x"] = cLedPosition.GetX();
            cLedJson["position"]["y"] = cLedPosition.GetY();
            cLedJson["position"]["z"] = cLedPosition.GetZ();

            cJson["leds"].push_back(cLedJson);
          }
        }

        return cJson;
      }
    };

    REGISTER_WEBVIZ_ENTITY_OPERATION(
      CWebvizOperationGenerateJSON,
      CWebvizOperationGenerateCylinderJSON,
      CCylinderEntity);

  }  // namespace Webviz
}  // namespace argos