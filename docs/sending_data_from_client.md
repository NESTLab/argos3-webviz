# Sending data from Client

You need to subclass `CWebvizUserFunctions` class from `argos3/plugins/simulator/visualizations/webviz/webviz_user_functions.h`

In your subclass, you can override `HandleCommandFromClient` function, which will be called on every JSON received from client except for the inbuilt commands described in [Controlling experiment](controlling_experiment.md).
```cpp
void HandleCommandFromClient(
      const std::string& str_ip, nlohmann::json c_json_command);
```

Examples of JSON which will not be forwarded to this function are:`{ "command": "play" }`, `{ "command": "pause" }`, etc. (All are listed at [Controlling experiment](controlling_experiment.md))

Some of the valid JSON which will be forwarded to this function are:
```json
{
  "Some": "json",
  "info": [
    "extra",
    "data"
  ]
}
```

```json
{
  "command": "custom command",
  "x" : 0.1,
  "is_testing": true
}
