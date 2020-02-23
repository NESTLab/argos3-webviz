/**
 * @file
 * <argos3/plugins/simulator/visualizations/network-api/entity/networkapi_box.cpp>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#include "networkapi_box.h"

namespace argos {
  namespace NetworkAPI {

    /****************************************/
    /****************************************/

    CNetworkAPIBox::CNetworkAPIBox() {}

    /****************************************/
    /****************************************/

    CNetworkAPIBox::~CNetworkAPIBox() {}

    /****************************************/
    /****************************************/

    nlohmann::json CNetworkAPIBox::GenerateJSON(const CBoxEntity& c_entity) {
      return nlohmann::json();
    }

    /****************************************/
    /****************************************/

    nlohmann::json CNetworkAPIBox::GenerateJSONLEDs(CBoxEntity& c_entity) {
      return nlohmann::json();
    }

    /****************************************/
    /****************************************/

    class CNetworkAPIOperationGenerateBoxJSON
        : public CNetworkAPIOperationGenerateJSON {
     public:
      nlohmann::json ApplyTo(CNetworkAPI& c_networkapi, CBoxEntity& c_entity) {
        static CNetworkAPIBox m_cModel;
        // LOG << "boxentity \"" << c_entity.GetId() << "\"" << std::endl;
        // c_visualization.DrawEntity(c_entity.GetEmbodiedEntity());
        m_cModel.GenerateJSON(c_entity);
        m_cModel.GenerateJSONLEDs(c_entity);
        return nlohmann::json();
      }
    };

    REGISTER_NETWORKAPI_ENTITY_OPERATION(
      CNetworkAPIOperationGenerateJSON,
      CNetworkAPIOperationGenerateBoxJSON,
      CBoxEntity);

  }  // namespace NetworkAPI
}  // namespace argos