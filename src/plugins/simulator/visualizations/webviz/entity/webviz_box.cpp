/**
 * @file <argos3/plugins/simulator/visualizations/webviz/entity/webviz_box.cpp>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#include <argos3/plugins/simulator/entities/box_entity.h>
#include <argos3/plugins/simulator/entities/led_equipped_entity.h>
#include <argos3/plugins/simulator/visualizations/webviz/webviz.h>

#include <iomanip>
#include <nlohmann/json.hpp>

namespace argos {
  namespace Webviz {

    /****************************************/
    /****************************************/

    class CWebvizOperationGenerateBoxJSON
        : public CWebvizOperationGenerateJSON {
     public:
      nlohmann::json ApplyTo(CWebviz& c_webviz, CBoxEntity& c_entity) {
        nlohmann::json cJson;

        cJson["type"] = c_entity.GetTypeDescription();
        cJson["id"] = c_entity.GetId();

        cJson["is_movable"] = c_entity.GetEmbodiedEntity().IsMovable();

        /* Get Scale of the box */
        const argos::CVector3& cScale = c_entity.GetSize();
        cJson["scale"]["x"] = cScale.GetX();
        cJson["scale"]["y"] = cScale.GetY();
        cJson["scale"]["z"] = cScale.GetZ();

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
            const argos::CVector3& cPosition =
              cLEDEquippedEntity.GetLED(i).GetPosition();

            /* Add it to json as => position:{x, y, z} */
            cLedJson["position"]["x"] = cPosition.GetX();
            cLedJson["position"]["y"] = cPosition.GetY();
            cLedJson["position"]["z"] = cPosition.GetZ();

            cJson["leds"].push_back(cLedJson);
          }
        }

        return cJson;
      }
    };

    REGISTER_WEBVIZ_ENTITY_OPERATION(
      CWebvizOperationGenerateJSON,
      CWebvizOperationGenerateBoxJSON,
      CBoxEntity);

  }  // namespace Webviz
}  // namespace argos