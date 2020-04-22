# Adding a custom entity to Webviz: Server side

You need to write a subclass of `CWebvizOperationGenerateJSON` like shown below.

You can keep the file (and compile) anywhere in the system as it uses `REGISTER_WEBVIZ_ENTITY_OPERATION` function (as shown in the last lines of the code below) to register with Webviz.

**NOTE:** replace 4 underlines (`____`) with your Entity name in the code below, for example `CBoxEntity`.

Check implementation of other entities in folder [src/plugins/simulator/visualizations/webviz/entity](../src/plugins/simulator/visualizations/webviz/entity)


```cpp
#include <argos3/plugins/simulator/visualizations/webviz/webviz.h>
#include <nlohmann/json.hpp>

namespace argos {
  namespace Webviz {

    class CWebvizOperationGenerate____JSON
        : public CWebvizOperationGenerateJSON {
     public:
      nlohmann::json ApplyTo(CWebviz& c_webviz, C____Entity& c_entity) {
        nlohmann::json cJson;

        cJson["type"] = c_entity.GetTypeDescription();
        cJson["id"] = c_entity.GetId();

        /* Get the position of the ____ */
        const argos::CVector3& cPosition = c_entity.GetPosition();

        /* Add it to json as => position:{x, y, z} */
        cJson["position"]["x"] = cPosition.GetX();
        cJson["position"]["y"] = cPosition.GetY();
        cJson["position"]["z"] = cPosition.GetZ();

        /* Get the orientation of the ____ */
        const argos::CQuaternion& cOrientation = c_entity.GetOrientation();

        cJson["orientation"]["x"] = cOrientation.GetX();
        cJson["orientation"]["y"] = cOrientation.GetY();
        cJson["orientation"]["z"] = cOrientation.GetZ();
        cJson["orientation"]["w"] = cOrientation.GetW();

        /* Other eintity specific information here */
        cJson["color"] = "Some color";

        return cJson;
      }
    };

    REGISTER_WEBVIZ_ENTITY_OPERATION(
      CWebvizOperationGenerateJSON,
      CWebvizOperationGenerate____JSON,
      C____Entity);

  }  // namespace Webviz
}  // namespace argos
```

All the parameters shown above (including `type`, `id`, `orientation` and `position`) are mandatory.
