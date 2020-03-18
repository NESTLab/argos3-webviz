

/* Define function to run after all files are loaded */
var onAllFilesLoaded = function () {

  /* On Jquery load */
  $(function () {
    /* main panel-layout of the page */
    $('#layout').w2layout({
      name: 'app_layout',
      padding: 4,
      panels: [
        { type: 'top', size: 50, resizable: false },
        { type: 'left', size: "10%", resizable: true, content: 'left', hidden: true },
        { type: 'main', resizable: true, },
        { type: 'right', size: "30%", style: "background-color: #f2f2f2;border:0px", resizable: true, content: 'right' }
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
        title: "Log",
        style: "padding:4px 8px;background:white",
        content: '<div id="contentAreaLog" class="clusterize-content"></div>'
      }, {
        type: 'main',
        size: "50%",
        title: "LogErr",
        resizable: true,
        style: "padding:4px 8px;background:white",
        content: '<div id="contentAreaLogErr" class="clusterize-content"></div>'
      }]
    });

    /* Make them nested */
    w2ui['app_layout'].content('right', w2ui['log_layout']);


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

      /* Add styling for log divs */
      $("#layout_log_layout_panel_top>div.w2ui-panel-content")
        .attr("id", "scrollAreaLog")
        .addClass("clusterize-scroll")

      $("#layout_log_layout_panel_main>div.w2ui-panel-content")
        .attr("id", "scrollAreaLogErr")
        .addClass("clusterize-scroll")

      /* Initialize Log objects */
      window.log_clusterize = new Clusterize({
        show_no_data_row: false,
        scrollId: "scrollAreaLog",
        contentId: 'contentAreaLog'
      });

      window.logerr_clusterize = new Clusterize({
        show_no_data_row: false,
        scrollId: "scrollAreaLogErr",
        contentId: 'contentAreaLogErr'
      });


      /* Add button on top panel */
      $("#layout_app_layout_panel_top>div.w2ui-panel-content")
        .addClass('toolbar-flex-container')
        .append($("<div/>")
          .addClass('toolbar_counter')
          .attr("title", "Step counter")
          .prop("title", "Step counter")//for IE
          .html("{experiment.counter}")
        )
        /* Divider */
        .append($("<div/>")
          .addClass('toolbar_divider')
        )
        .append($("<div/>")
          .addClass('button')
          .addClass('step-button')
          .attr("title", "Step experiment")
          .prop("title", "Step experiment")//for IE
          .click(function () {
            window.wsp.send('step')
          })
        )
        .append($("<div/>")
          .addClass('button')
          .addClass('play-button')
          .attr('id', 'play_button')
          .attr("title", "Play experiment")
          .prop("title", "Play experiment")//for IE
          .click(function () {
            window.wsp.send('step')
            if (window.experiment.state != "EXPERIMENT_PLAYING" &&
              window.experiment.state != "EXPERIMENT_FAST_FORWARDING") {
              window.wsp.send('play')
            } else {
              window.wsp.send('pause')
            }
          })
        )
        .append($("<div/>")
          .addClass('button')
          .addClass('ff-button')
          .attr('id', 'ff_button')
          .attr("title", "Fast forward experiment")
          .prop("title", "Fast forward experiment")//for IE
          .click(function () {
            window.wsp.send('fastforward')
          })
        )
        /* Divider */
        .append($("<div/>")
          .addClass('toolbar_divider')
        )
        // .append($("<div/>")
        //   .addClass('button')
        //   .addClass('stop-button')
        //   .attr("title", "Terminate experiment")
        //   .prop("title", "Terminate experiment")//for IE
        //   .click(function () {
        //     // window.wsp.send('step')
        //   })
        // )
        .append($("<div/>")
          .addClass('button')
          .addClass('reset-button')
          .attr("title", "Reset experiment")
          .prop("title", "Reset experiment")//for IE
          .click(function () {
            window.wsp.send('reset')
          })
        )

        /* Spacer */
        .append($("<div/>").addClass('toolbar-spacer'))

        /* Right side of toolbar */
        .append($("<div/>")
          .addClass("toolbar_status")
          .html("{experiment.status}")
        )

      window.experiment = {}

      /* Bind data using rivets */
      rivets.bind($('#experiment'), { experiment: window.experiment })
      ConnectWebSockets()
    }, true);
  });
}

/* Load Jquery - sequentially */
loadJS("/js/libs/jquery.min.js", true)
loadJS("/js/libs/w2ui-1.5.rc1.min.js", true) /* Panels */
loadJS("/js/libs/clusterize.min.js", true) /* Better scroll for logs */

/* Load Websockets code */
loadJS("/js/libs/websocket-as-promised.js", true); /* basic websockets */
loadJS("/js/libs/robust-websocket.js", true); /* auto Reconnect */

/* Load Three.js code */
loadJS("/js/libs/three.min.js", true);
loadJS("/js/libs/OrbitControls.js", true);
loadJS("/js/libs/GLTFLoader.js", true);
loadJS("/js/libs/stats.min.js", true);
loadJS("/js/libs/GLTFLoader.js", true);

/* Start running javascript after all files are loaded */
loadJS("/js/libs/rivets.bundled.min.js", onAllFilesLoaded, true);
