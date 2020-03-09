# argos3-webclient
A webclient for ARGoS using https://github.com/NESTLab/argos3-webviz



# How to create websocket-as-promised js file

- Install parcel js
```bash
npm i websocket-as-promised
```
- Write a javascript file named `websocket-as-promised.js` with content
```javascript
window.WebSocketAsPromised = require('websocket-as-promised');
```
- run
```bash
parcel build websocket-as-promised.js
```

