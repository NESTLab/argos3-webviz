# Controlling experiment

Server accepts commands through Websockets "message".

Each message has the format,

```json
{
  "command": "play",
  "...": "..."
}
```
The parameter `command` is mandatory, all others are command specific.


### Play
Command to start/play the experiment.

```json
{ "command": "play" }
```
**Note:** It will not work if the state of experiment is `Playing`, `Fast-forwarding` or `Done`)

### Pause
Command to pause the experiment.

```json
{ "command": "pause" }
```
**Note:** It will not work if the state of experiment is `Paused` or `Done`)


### Step
Command to run one step in the experiment.

It will pause the experiment after one step is executed.
```json
{ "command": "step" }
```
**Note:** It will not work if the state of experiment is `Done`).



### Fast forward
Command to Fastforward the experiment.

```json
{
  "command": "fastforward",
  "steps": 10
}
```
`steps` is optional, and defaults to value defined in experiment(.argos) file, like
```xml
<visualization>
  <webviz ff_draw_frames_every=10 />
</visualization>
```
**Note:** It will not work if the state of experiment is `Fast-forwarding` or `Done`)

### Reset
Command to reset the experiment.

```json
{ "command": "reset" }
```

