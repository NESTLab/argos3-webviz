/**
 * @file
 * <argos3/plugins/simulator/visualizations/webviz/entity/webviz_floor.cpp>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#include <argos3/core/simulator/entity/floor_entity.h>
#include <argos3/plugins/simulator/visualizations/webviz/utility/base64.h>
#include <argos3/plugins/simulator/visualizations/webviz/webviz.h>

#include <iomanip>
#include <iostream>
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

#ifdef ARGOS_WITH_FREEIMAGE
        /* If floor is updated */
        if (c_entity.HasChanged()) {
          std::string strFilePath =
            "/tmp/argos3_webviz_floor_" +
            std::to_string(
              std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count()) +
            ".png";

          /* TODO: below code is not at all optimized ...  */
          c_entity.SaveAsImage(strFilePath);

          /* Read the file */

          std::ifstream infile(
            strFilePath, std::ifstream::binary | std::ios::ate);

          std::streamsize size = infile.tellg();

          infile.seekg(0, std::ios::beg);

          // Allocate a string, make it large enough to hold the input
          std::string buffer;
          buffer.resize(size);

          // read the text into the string
          infile.read(&buffer[0], buffer.size());
          infile.close();

          std::string strFloorImage;
          Base64::Encode(buffer, &strFloorImage);
          cJson["floor_image"] = "data:image/png;base64," + strFloorImage;
        }
#endif

        return cJson;
      }
    };

    REGISTER_WEBVIZ_ENTITY_OPERATION(
      CWebvizOperationGenerateJSON,
      CWebvizOperationGenerateFloorJSON,
      CFloorEntity);

  }  // namespace Webviz
}  // namespace argos