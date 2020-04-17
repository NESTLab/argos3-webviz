/**
 * @file
 * <argos3/plugins/simulator/visualizations/webviz/entity/webviz_kheperaiv.cpp>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_proximity_sensor.h>
#include <argos3/plugins/robots/kheperaiv/control_interface/ci_kheperaiv_proximity_sensor.h>
#include <argos3/plugins/robots/kheperaiv/simulator/kheperaiv_entity.h>
#include <argos3/plugins/simulator/entities/led_equipped_entity.h>
#include <argos3/plugins/simulator/visualizations/webviz/webviz.h>

#include <iomanip>
#include <nlohmann/json.hpp>

namespace argos {
  namespace Webviz {

    /****************************************/
    /****************************************/

    class CWebvizOperationGenerateFootbotJSON
        : public CWebvizOperationGenerateJSON {
     private:
      CCI_KheperaIVProximitySensor* m_pcProximitySensor;

     private:
      CVector3 m_cRayVector;

     public:
      /**
       * @brief Function called to generate a JSON representation of KheperaIV
       *
       * @param c_webviz
       * @param c_entity
       * @return nlohmann::json
       */
      nlohmann::json ApplyTo(CWebviz& c_webviz, CKheperaIVEntity& c_entity) {
        nlohmann::json cJson;

        cJson["type"] = c_entity.GetTypeDescription();
        cJson["id"] = c_entity.GetId();

        /* Get the position of the foot-bot */
        const argos::CVector3& cPosition =
          c_entity.GetEmbodiedEntity().GetOriginAnchor().Position;

        /* Add it to json as => position:{x, y, z} */
        cJson["position"]["x"] = cPosition.GetX();
        cJson["position"]["y"] = cPosition.GetY();
        cJson["position"]["z"] = cPosition.GetZ();

        /* Get the orientation of the foot-bot */
        const argos::CQuaternion& cOrientation =
          c_entity.GetEmbodiedEntity().GetOriginAnchor().Orientation;

        cJson["orientation"]["x"] = cOrientation.GetX();
        cJson["orientation"]["y"] = cOrientation.GetY();
        cJson["orientation"]["z"] = cOrientation.GetZ();
        cJson["orientation"]["w"] = cOrientation.GetW();

        /* Actuators */
        CLEDEquippedEntity& cLEDEquippedEntity =
          c_entity.GetLEDEquippedEntity();

        if (cLEDEquippedEntity.GetLEDs().size() > 0) {
          /* Building a string of all led colors */
          for (UInt32 i = 0; i < 3; i++) {
            std::stringstream strLEDsStream;

            const CColor& cColor = cLEDEquippedEntity.GetLED(i).GetColor();
            /* Convert to hex color*/
            strLEDsStream << "0x" << std::setfill('0') << std::setw(6)
                          << std::hex
                          << (cColor.GetRed() << 16 | cColor.GetGreen() << 8 |
                              cColor.GetBlue());

            cJson["leds"].push_back(strLEDsStream.str());
          }
        }

        /* Rays */
        std::vector<std::pair<bool, CRay3>>& vecRays =
          c_entity.GetControllableEntity().GetCheckedRays();

        cJson["rays"] = json::array();  // Empty array

        /*
         * To make rays relative, negate the rotation of body along Z axis
         */
        CQuaternion cInvZRotation = cOrientation;
        cInvZRotation.SetZ(-cOrientation.GetZ());

        /*
          For each ray as a string,
          Output format -> "BoolIsChecked:Vec3StartPoint:Vec3EndPoint"
          For example -> "true:1,2,3:1,2,4"
        */
        for (UInt32 i = 0; i < vecRays.size(); ++i) {
          std::stringstream strRayStream;
          if (vecRays[i].first) {
            strRayStream << "true";
          } else {
            strRayStream << "false";
          }

          /* Substract the body position, to get relative position of ray */
          CVector3 cStartVec = vecRays[i].second.GetStart();
          cStartVec -= cPosition;
          CVector3 cEndVec = vecRays[i].second.GetEnd();
          cEndVec -= cPosition;

          cStartVec.Rotate(cInvZRotation);
          cEndVec.Rotate(cInvZRotation);

          /* append vectors to string */
          strRayStream << ":";
          strRayStream << cStartVec;
          strRayStream << ":";
          strRayStream << cEndVec;

          cJson["rays"].push_back(strRayStream.str());
        }

        std::vector<argos::CVector3>& vecPoints =
          c_entity.GetControllableEntity().GetIntersectionPoints();

        cJson["points"] = json::array();  // Empty array

        for (UInt32 i = 0; i < vecPoints.size(); ++i) {
          // NO NOT TAKE IT BY REFERENCE HERE, we are updating them below
          CVector3 cPoint = vecPoints[i];
          cPoint -= cPosition;
          cPoint.Rotate(cInvZRotation);

          std::stringstream strPointStream;
          strPointStream << cPoint.GetX();
          strPointStream << ",";
          strPointStream << cPoint.GetY();
          strPointStream << ",";
          strPointStream << cPoint.GetZ();

          cJson["points"].push_back(strPointStream.str());
        }
        return cJson;
      }
    };

    REGISTER_WEBVIZ_ENTITY_OPERATION(
      CWebvizOperationGenerateJSON,
      CWebvizOperationGenerateFootbotJSON,
      CKheperaIVEntity);

  }  // namespace Webviz
}  // namespace argos