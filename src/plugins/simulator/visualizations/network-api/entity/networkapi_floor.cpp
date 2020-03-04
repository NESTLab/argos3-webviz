/**
 * @file
 * <argos3/plugins/simulator/visualizations/network-api/entity/networkapi_floor.cpp>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#include <argos3/core/simulator/entity/floor_entity.h>
#include <argos3/plugins/simulator/visualizations/network-api/networkapi.h>
#include <iomanip>
#include <nlohmann/json.hpp>

namespace argos {
  namespace NetworkAPI {

    /****************************************/
    /****************************************/

    class CNetworkAPIOperationGenerateFloorJSON
        : public CNetworkAPIOperationGenerateJSON {
     public:
      nlohmann::json ApplyTo(
        CNetworkAPI& c_networkapi, CFloorEntity& c_entity) {
        nlohmann::json cJson;

        cJson["type"] = c_entity.GetTypeDescription();
        cJson["id"] = c_entity.GetId();

        // cArenaSize.GetX()
        // c_entity.GetColorAtPoint();
        // TODO implement Floor image
        return cJson;
      }
    };

    REGISTER_NETWORKAPI_ENTITY_OPERATION(
      CNetworkAPIOperationGenerateJSON,
      CNetworkAPIOperationGenerateFloorJSON,
      CFloorEntity);

  }  // namespace NetworkAPI
}  // namespace argos