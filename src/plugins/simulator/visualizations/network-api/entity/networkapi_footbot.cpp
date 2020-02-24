/**
 * @file
 * <argos3/plugins/simulator/visualizations/network-api/entity/networkapi_footbot.cpp>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include <argos3/plugins/simulator/entities/led_equipped_entity.h>
#include <argos3/plugins/simulator/visualizations/network-api/networkapi.h>
#include <iomanip>
#include <nlohmann/json.hpp>

namespace argos {
  namespace NetworkAPI {

    /****************************************/
    /****************************************/

    class CNetworkAPIOperationGenerateFootbotJSON
        : public CNetworkAPIOperationGenerateJSON {
     public:
      nlohmann::json ApplyTo(
        CNetworkAPI& c_networkapi, CFootBotEntity& c_entity) {
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

        CLEDEquippedEntity& cLEDEquippedEntity =
          c_entity.GetLEDEquippedEntity();

        if (cLEDEquippedEntity.GetLEDs().size() > 0) {
          std::stringstream str_LEDsStream;

          /* Building a string of all led colors */
          for (UInt32 i = 0; i < 12; i++) {
            const CColor& cColor = cLEDEquippedEntity.GetLED(i).GetColor();
            /* Convert to hex color*/
            str_LEDsStream << "#" << std::setfill('0') << std::setw(6)
                           << std::hex
                           << (cColor.GetRed() << 16 | cColor.GetGreen() << 8 |
                               cColor.GetBlue());

            str_LEDsStream << ";";
          }

          cJson["leds"] = str_LEDsStream.str();
        }

        return cJson;
      }
    };

    REGISTER_NETWORKAPI_ENTITY_OPERATION(
      CNetworkAPIOperationGenerateJSON,
      CNetworkAPIOperationGenerateFootbotJSON,
      CFootBotEntity);

  }  // namespace NetworkAPI
}  // namespace argos