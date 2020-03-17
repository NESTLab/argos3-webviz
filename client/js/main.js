
/* Server IP and port */
var server = window.location.hostname + ":3000";
// You need to change this port if 


/* Define function to run after all files are loaded */
var onAllFilesLoaded = function () {
  console.log('All JS files loaded');
  /* On Jquery load */
  $(function () {
    /* main panel-layout of the page */
    $('#layout').w2layout({
      name: 'app_layout',
      padding: 4,
      panels: [
        { type: 'top', size: 50, resizable: false, content: 'top' },
        { type: 'left', size: 200, resizable: true, content: 'left', hidden: true },
        { type: 'main', content: '<div id="main_panel"></div>' },
        { type: 'right', size: "20%", resizable: true, content: 'right' }
      ]
    });
    /* Log layout */
    $().w2layout({
      name: 'log_layout',
      panels: [{
        type: 'top',
        size: "50%",
        resizable: true,
        style: "padding:4px 8px;background-color:#ffffff",
        content: 'top'
      }, {
        type: 'main',
        size: "50%",
        resizable: true,
        style: "padding:0px;background-color:#ffffff",
        content: 'main'
      }]
    });

    /* Make them nested */
    w2ui['app_layout'].content('right', w2ui['log_layout']);

    /* Start next code */
    ConnectWebSockets();
  });
}

var ConnectWebSockets = function () {

}

/* Load Jquery - sequentially */
loadJS("/js/jquery.min.js", true)
loadJS("/js/w2ui-1.5.rc1.min.js", true)

/* Load Websockets code */
loadJS("/js/websocket-as-promised.js", true);
loadJS("/js/robust-websocket.js", true);

/* Load Three.js code */
loadJS("/js/three.min.js", true);
loadJS("/js/OrbitControls.js", true);
loadJS("/js/GLTFLoader.js", true);
loadJS("/js/stats.min.js", true);
loadJS("/js/GLTFLoader.js", true);

/* Start running javascript after all files are loaded */
loadJS("/js/rivets.bundled.min.js", onAllFilesLoaded, true);
