

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
      /* When resizing is complete */
      event.onComplete = function () {
        if (window.threejs_panel) {
          onThreejsPanelResize();
        }
      }
    });

    /* Load main logic code sub-files - sequentially */
    /* load threejs scene */
    loadJS("/js/three_scene.js", function () {
      /* Get the panel from layout */
      window.threejs_panel = $("#layout_app_layout_panel_main .w2ui-panel-content")

      /* Setup scene */
      IntializeThreejs(threejs_panel)
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
          .addClass('icon-step')
          .attr("title", "Step experiment")
          .prop("title", "Step experiment")//for IE
          .click(function () {
            window.wsp.sendPacked({ command: 'step' })
          })
        )
        .append($("<div/>")
          .addClass('button')
          .addClass('icon-play')
          .attr('id', 'play_button')
          .attr("title", "Play experiment")
          .prop("title", "Play experiment")//for IE
          .click(function () {
            window.wsp.sendPacked({ command: 'play' })
          })
        )
        .append($("<div/>")
          .addClass('button')
          .addClass('icon-pause')
          .attr('id', 'pause_button')
          .attr("title", "Pause experiment")
          .prop("title", "Pause experiment")//for IE
          .click(function () {
            window.wsp.sendPacked({ command: 'pause' })
          })
        )
        .append($("<div/>")
          .addClass('button')
          .addClass('icon-ff')
          .attr('id', 'ff_button')
          .attr("title", "Fast forward experiment")
          .prop("title", "Fast forward experiment")//for IE
          .click(function () {

            var steps = parseInt($("#ff_steps_input").val());

            if (steps && steps >= 1 && steps <= 500) {
              $("#ff_steps_input").val(steps)
              window.wsp.sendPacked({ command: 'fastforward', steps: steps })
            } else {
              window.wsp.sendPacked({ command: 'fastforward' })
            }
          })
        )
        .append($("<input/>")
          .attr('type', 'number')
          .attr('id', 'ff_steps_input')
          .attr('min', '1')
          .attr('max', '500')
          .attr('value', '10')
          .attr("title", "Fast forward steps")
          .prop("title", "Fast forward steps")//for IE
        )
        /* Divider */
        .append($("<div/>")
          .addClass('toolbar_divider')
        )
        // .append($("<div/>")
        //   .addClass('button')
        //   .attr('id', 'stop_button')
        //   .addClass('icon-stop')
        //   .attr("title", "Terminate experiment")
        //   .prop("title", "Terminate experiment")//for IE
        //   .click(function () {
        //     // window.wsp.send('step')
        //   })
        // )
        .append($("<div/>")
          .addClass('button')
          .addClass('icon-reset')
          .attr('id', 'reset_button')
          .attr("title", "Reset experiment")
          .prop("title", "Reset experiment")//for IE
          .click(function () {
            window.wsp.sendPacked({ command: 'reset' })
          })
        )
        /* Divider */
        .append($("<div/>")
          .addClass('toolbar_divider')
        )
        // .append($("<div/>")
        //   .addClass('button')
        //   .addClass('icon-settings')
        //   .attr('id', 'settings_button')
        //   .attr("title", "Settings")
        //   .prop("title", "Settings")//for IE
        //   .click(function () {
        //   })
        // )
        .append($("<div/>")
          .addClass('button')
          .addClass('icon-help')
          .attr("title", "Help")
          .prop("title", "Help")//for IE
          .click(function () {
            $("#HelpModal").w2popup({
              title: 'Help',
              showClose: true,
              height: 300,
              width: 500
            })
            // window.wsp.sendPacked({ command: 'reset' })
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

      $("#preloader").fadeOut()
      ConnectWebSockets()
    }, true);
  });
}

/* Load Jquery - sequentially */
loadJS("/js/libs/jquery.min.js", true)
loadJS("/js/libs/w2ui-1.5.rc1.min.js", true) /* Panels */
loadJS("/js/libs/clusterize.min.js", true) /* Better scroll for logs */
loadJS("/js/libs/jquery.contextMenu.min.js", true); /* Right click */

/* Load Websockets code */
loadJS("/js/libs/websocket-as-promised.js", true); /* basic websockets */
loadJS("/js/libs/robust-websocket.js", true); /* auto Reconnect */

/* Load Three.js code */
loadJS("/js/libs/three.min.js", true);
loadJS("/js/libs/OrbitControls.js", true);

loadJS("/js/libs/CSS2DRenderer.js", true);

loadJS("/js/libs/stats.min.js", true);
loadJS("/js/libs/GLTFLoader.js", true);

/* Start running javascript after all files are loaded */
loadJS("/js/libs/rivets.bundled.min.js", onAllFilesLoaded, true);
