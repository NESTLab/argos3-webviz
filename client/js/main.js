

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
        { type: 'left', size: "10%", resizable: true, content: 'left', hidden: true },
        { type: 'main', resizable: true, },
        { type: 'right', size: "30%", style: "background-color:#fff;border:0px", resizable: true, content: 'right' }
      ]
    });
    /* Log layout */
    $().w2layout({
      name: 'log_layout',
      padding: 5,
      panels: [{
        type: 'top',
        size: "50%",
        resizable: true,
        style: "padding:4px 8px",
        content: 'LOG'
      }, {
        type: 'main',
        size: "50%",
        resizable: true,
        style: "padding:4px 8px",
        content: 'LOGERR'
      }]
    });

    /* Make them nested */
    w2ui['app_layout'].content('right', w2ui['log_layout']);

    /* Load main logic code sub-files - sequentially */

    /* Load all entities */
    loadJS("/js/entities/loadEntities.js", true);

    /* load threejs scene */
    loadJS("/js/three_scene.js", function () {
      /* Setup scene */
      var renderer = IntializeThreejs()

      /* Get the panel from layout */
      var threejs_panel = $("#layout_app_layout_panel_main .w2ui-panel-content")

      renderer.setSize(threejs_panel.width(), threejs_panel.height());

      /* Define aspect ratio to be used later */
      window.threejs_aspect_ratio = threejs_panel.width() / threejs_panel.height()

      /* Add canvas to page */
      threejs_panel.append(renderer.domElement);
    }, true);

    /* Load websockets and connect to server */
    loadJS("/js/websockets.js", function () {
      ConnectWebSockets()
    }, true);

    /* On Threejs panel Resize */
    w2ui['app_layout'].on('resize', function (event) {
      // console.log('Event: ' + event.type + ' Target: ' + event.target);
      // console.log(event);

      /* When resizing is complete */
      event.onComplete = function () {
        if (window.camera) {
          var threejs_panel = $("#layout_app_layout_panel_main .w2ui-panel-content")

          window.threejs_aspect_ratio = threejs_panel.width() / threejs_panel.height()
          camera.aspect = window.threejs_aspect_ratio
          camera.updateProjectionMatrix();
          renderer.setSize(threejs_panel.width(), threejs_panel.height());
        }
      }
    });
  });
}

/* Load Jquery - sequentially */
loadJS("/js/libs/jquery.min.js", true)
loadJS("/js/libs/w2ui-1.5.rc1.min.js", true)

/* Load Websockets code */
loadJS("/js/libs/websocket-as-promised.js", true);
loadJS("/js/libs/robust-websocket.js", true);

/* Load Three.js code */
loadJS("/js/libs/three.min.js", true);
loadJS("/js/libs/OrbitControls.js", true);
loadJS("/js/libs/GLTFLoader.js", true);
loadJS("/js/libs/stats.min.js", true);
loadJS("/js/libs/GLTFLoader.js", true);

/* Start running javascript after all files are loaded */
loadJS("/js/libs/rivets.bundled.min.js", onAllFilesLoaded, true);
