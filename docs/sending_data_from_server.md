# Sending data from Server

To send data from server, you need to add a user function in your argos experiment file like

```xml
.
.
<visualization>
    <webviz>
      <user_functions library="build/testing/loop_functions/libuser_loop_functions" label="test_user_functions" />
    </webviz>
    <!-- <qt-opengl /> -->
  </visualization>
.
.
```

Where you need to subclass `CWebvizUserFunctions` class from `argos3/plugins/simulator/visualizations/webviz/webviz_user_functions.h`
 
 In your subclass, you could override `sendUserData()` function to add data to experiment JSON as `user_data` parameter.
```cpp
nlohmann::json sendUserData();
```

which will be added to the JSON message in broadcast topic like,
```json
{
  "type": "broadcast",
  "state": "EXPERIMENT_PLAYING",
  "steps": ...,
  "timestamp": ...,
  "arena": { ... },
  "entities": [ ... ],

  "user_data": {
    "YOUR": "USER_DATA",
    "..": "..."
  }
}
```

You can follow [https://github.com/nlohmann/json](https://github.com/nlohmann/json) to build your json, or check the example at [src/testing/loop_functions/user_loop_functions.cpp](../src/testing/loop_functions/user_loop_functions.cpp)


To send data per Entity, you can register an function like,

```cpp
.
.
..
CTestUserFunctions::CTestUserFunctions() {
  RegisterWebvizUserFunction<CTestUserFunctions, CFootBotEntity>(
    &CTestUserFunctions::sendRobotData);
}
..
.
const nlohmann::json CTestUserFunctions::sendRobotData(CFootBotEntity &robot) {
  nlohmann::json outJson;
}
..
.
```

Which will be added to entity json in broadcast topic like,
```json
{
  "type": "broadcast",
  "state": "EXPERIMENT_PLAYING",
  "steps": ...,
  "timestamp": ...,
  "arena": { ... },
  "entities": [
    {
      "type": "box",
      "id": "..",
      "orientation": { ... },
      "position": { ... },
      
      "user_data": {
        "YOUR": "USER_DATA",
        "..": "..."
      }
    }
  ],
}
```

You can check example at [src/testing/loop_functions/user_loop_functions.cpp](../src/testing/loop_functions/user_loop_functions.cpp)

