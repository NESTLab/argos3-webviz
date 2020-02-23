/**
 * @file <argos3/plugins/simulator/visualizations/network-api/helpers/utils.h>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#ifndef ARGOS_NETWORKAPI_UTILITIES_H
#define ARGOS_NETWORKAPI_UTILITIES_H

#include <argos3/core/simulator/space/space.h>
#include <algorithm>
#include <nlohmann/json.hpp>

/****************************************/
/****************************************/

static inline nlohmann::json GetEntitiesAsJSON(argos::CSpace& c_space) {
  std::vector<nlohmann::json> vecEntitiesJson;

  argos::CEntity::TVector& cvecEntities = c_space.GetRootEntityVector();
  for (argos::CEntity::TVector::iterator itEntities = cvecEntities.begin();
       itEntities != cvecEntities.end();
       ++itEntities) {
    nlohmann::json cEntityJson;

    /* Get the type of the entity */
    cEntityJson["type"] = (*itEntities)->GetTypeDescription();

    /* Try to get embodied entity */

    /* Is the entity embodied itself? */
    argos::CEmbodiedEntity* pcEmbodiedEntity =
      dynamic_cast<argos::CEmbodiedEntity*>(*itEntities);

    if (pcEmbodiedEntity == NULL) {
      /* Is the entity composable with an embodied component? */
      argos::CComposableEntity* pcComposableTest =
        dynamic_cast<argos::CComposableEntity*>(*itEntities);
      if (pcComposableTest != NULL) {
        if (pcComposableTest->HasComponent("body")) {
          pcEmbodiedEntity =
            &(pcComposableTest->GetComponent<argos::CEmbodiedEntity>("body"));
        }
      }
    }

    if (pcEmbodiedEntity == NULL) {
      /* cannot find EmbodiedEntity */
      LOG_S(WARNING) << "Not an Embodied Entity :"
                     << (*itEntities)->GetTypeDescription();
      continue;
    }

    /* Get the position of the entity */
    const argos::CVector3& cPosition =
      pcEmbodiedEntity->GetOriginAnchor().Position;

    /* Add it to json as=>  position:{x, y, z} */
    cEntityJson["position"]["x"] = cPosition.GetX();
    cEntityJson["position"]["y"] = cPosition.GetY();
    cEntityJson["position"]["z"] = cPosition.GetZ();

    /* Get the orientation of the entity */
    const argos::CQuaternion& cOrientation =
      pcEmbodiedEntity->GetOriginAnchor().Orientation;

    cEntityJson["orientation"]["x"] = cOrientation.GetX();
    cEntityJson["orientation"]["y"] = cOrientation.GetY();
    cEntityJson["orientation"]["z"] = cOrientation.GetZ();
    cEntityJson["orientation"]["w"] = cOrientation.GetW();

    // m_cModel.DrawEntity(c_entity);
    vecEntitiesJson.push_back(cEntityJson);
  }
  return vecEntitiesJson;
}

/****************************************/
/****************************************/

template <typename T>
static inline void EraseFromVector(std::vector<T>& deque, T const& value) {
  deque.erase(std::remove(begin(deque), end(deque), value), end(deque));
};

static inline std::vector<std::string> SplitString(
  const std::string& str, const std::string& delims = " ") {
  std::vector<std::string> output;
  auto first = std::cbegin(str);

  while (first != std::cend(str)) {
    const auto second = std::find_first_of(
      first, std::cend(str), std::cbegin(delims), std::cend(delims));

    if (first != second) output.emplace_back(first, second);

    if (second == std::cend(str)) break;

    first = std::next(second);
  }

  return output;
}

// uses std::find_first_of rather than string_view::find_first_of
static inline std::vector<std::string_view> SplitSV(
  std::string_view strv, std::string_view delims = " ") {
  std::vector<std::string_view> output;
  // output.reserve(strv.length() / 4);
  auto first = strv.begin();

  while (first != strv.end()) {
    const auto second = std::find_first_of(
      first, std::cend(strv), std::cbegin(delims), std::cend(delims));
    // std::cout << first << ", " << second << '\n';
    if (first != second) {
      output.emplace_back(strv.substr(
        std::distance(strv.begin(), first), std::distance(first, second)));
    }

    if (second == strv.end()) break;

    first = std::next(second);
  }

  return output;
}
#endif