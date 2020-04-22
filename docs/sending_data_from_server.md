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
 
 In your subclass, you could override `sendExtraData()` function to add data to experiment JSON as `extra_data` parameter.
```cpp
nlohmann::json sendExtraData();
```

You can follow [https://github.com/nlohmann/json](https://github.com/nlohmann/json) to build your json, or check the example at [src/testing/loop_functions/user_loop_functions.cpp](../src/testing/loop_functions/user_loop_functions.cpp)


To send data per Entity, you can register an function like,

```cpp

```