# Writing a custom client

The ARGoS3-Webviz contains a websockets server which can be used to receive experiment state / control simulation.

The implemented Websockets protocol is very basic, and hence may not be used with something complex like `sockets.io`. You can develop a client application in any technology given it can communicate to Argos3-webviz over websockets.

## Connecting to server
A websocket client can connect to the server with its IP and port (Note: the port is what is defined in the experiement.argos file. Default: 3000). ex: `ws://localhost:3000`

By default, when no `GET` parameters are defined, the client is *subscribed* to all the **topics** listed below,
 - broadcasts
 - events
 - logs

The client can selectively **subscribe** to some of or all the topics while connecting like,
- `ws://localhost:3000?broadcasts`
- `ws://localhost:3000?events,broadcasts,logs`

**NOTE:** Events are not realtime, they are published in next cycle of Broadcast (which runs at frequency defined in `broadcast_frequency`, default: 10 Hz)

Every message on any topic will be of type **JSON** (with MIME-TYPE `application/json`) of the format,
```json
{
  "type": "broadcast",
  "...": []
}
```
where type can be any of `broadcast`,`event`,`log`

**NOTE:** topic names are plural (broadcasts, events, logs) and each message-type is singular (broadcast, event, log)

### Topic: broadcasts
Messages on the topic `broadcasts` contains the full experiment state, which is emitted at the rate defined in experiment file by parameter `broadcast_frequency` (default: 10 Hz).

Format:
```json
{
  "type": "broadcast",
  "state": "EXPERIMENT_PLAYING",
  "steps": 24692,
  "timestamp": 1584200000000,
  "arena": {
    "center": {
      "x": 0,
      "y": 0,
      "z": 0
    },
    "size": {
      "x": 5,
      "y": 5,
      "z": 1
    }
  },
  "entities": [
    {
      "id": "wall_north",
      "is_movable": false,
      "orientation": {
        "w": 1,
        "x": 0,
        "y": 0,
        "z": 0
      },
      "position": {
        "x": 0,
        "y": 2,
        "z": 0
      },
      "scale": {
        "x": 4,
        "y": 0.1,
        "z": 0.5
      },
      "type": "box"
    },
    {
      "id": "fb0",
      "leds": [
        "0x000000",
        "0x000000",
        "0x000000",
        "0x000000",
        "0x000000",
        "0x000000",
        "0x000000",
        "0x000000",
        "0x000000",
        "0x000000",
        "0x000000",
        "0x000000"
      ],
      "orientation": {
        "w": 0.17354664003293813,
        "x": 0,
        "y": 0,
        "z": -0.9848256514395215
      },
      "points": [],
      "position": {
        "x": 1,
        "y": 0,
        "z": 0
      },
      "rays": [
        "false:0.0843093,0.0110995,0.06:0.183454,0.0241521,0.06",
        "false:0.0843093,-0.0110995,0.06:0.183454,-0.0241521,0.06"
      ],
      "type": "foot-bot"
    }
  ]
}
```
Every *broadcast* message contains a parameter `Entities` which contains *JSON*ified state of all the entities in the experiment. Each Entity has some mandatory parameters,
```json
{
  "type": "box",
  "id": "wall_north",
  "orientation": {
    "w": 1,
    "x": 0,
    "y": 0,
    "z": 0
  },
  "position": {
    "x": 0,
    "y": 0,
    "z": 0
  },
  "..": "..."
}
```
Where "type" is the static string which is used throughout ARGoS to identify the entity type.

All other optional parameters may or may not follow any standard (as long as server and client both know the format), to make the size of final JSON payload small (Like as shown in example above, each `ray` is just one line with `bool:start_x,start_y,start_z:end_x,end_y,end_z`).

### Topic: events
Messages on the topic `events` contain any control event happened in the experiment (like *play/pause/stop/step/done* of experiment). These are not realtime, but are emitted in next cycle of Broadcast (which runs at frequency defined in `broadcast_frequency`, default: 10 Hz).

### Topic: logs
Messages on the topic `logs` contain any log message from the experiment/or argos, which are accumulated in a single `log` message, and emitted at the rate defined in experiment file by parameter `broadcast_frequency` (default: 10 Hz).

```json
{
  "type":"log",
  "timestamp":1584640119430,
  "messages":[
    {
      "log_type":"LOG",
      "log_message":"1 client connected (Total: 1)",
      "step":"8981"
    },
    {
      "log_type":"LOGERR",
      "log_message":"Robot fb0 stuck!",
      "step":"8981"
    }
  ]
}
```
Where `log_type` can be either `LOG` or `LOGERR` and the *messages* can contain any number of messages accumulated from the last sent logs.

`step` is the simulation step at which the log was triggered.
