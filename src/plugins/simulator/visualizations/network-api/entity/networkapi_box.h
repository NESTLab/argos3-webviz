/**
 * @file
 * <argos3/plugins/simulator/visualizations/network-api/entity/networkapi_box.h>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#ifndef NETWORKAPI_BOX_H
#define NETWORKAPI_BOX_H

namespace argos {
  class CBoxEntity;

  class CNetworkAPI;
}  // namespace argos

#include <argos3/plugins/simulator/entities/box_entity.h>
#include <argos3/plugins/simulator/visualizations/network-api/networkapi.h>
#include <nlohmann/json.hpp>

namespace argos {
  namespace NetworkAPI {
    class CNetworkAPIBox {
     public:
      CNetworkAPIBox();

      virtual ~CNetworkAPIBox();

      virtual nlohmann::json GenerateJSONLEDs(CBoxEntity& c_entity);
      virtual nlohmann::json GenerateJSON(const CBoxEntity& c_entity);
    };
  };  // namespace NetworkAPI
}  // namespace argos

#endif