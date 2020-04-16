
var camera, controls, renderer, stats, menuRenderer;
var scale;
var uuid2idMap = {};
var selectedEntities = {}

var dom_menu;

var scene = new THREE.Scene();

window.isInitialized = false;
window.isLoadingModels = false;

/* Initialize Three.js scene */
THREE.Object3D.DefaultUp.set(0, 0, 1);
scene.background = new THREE.Color(0x007f7f);


/* ----------------------- */
var sceneEntities = [];

var IntializeThreejs = function (threejs_panel) {
  var _width = threejs_panel.width();
  var _height = threejs_panel.height();

  /* WebGLRenderer */
  renderer = new THREE.WebGLRenderer({
    antialias: true
  });
  renderer.setPixelRatio(window.devicePixelRatio);
  renderer.outputEncoding = THREE.sRGBEncoding;

  /* Add canvas to page */
  renderer.setSize(_width, _height);
  threejs_panel.append(renderer.domElement);

  /* Right click menu renderer */
  menuRenderer = new THREE.CSS2DRenderer();
  dom_menu = menuRenderer.domElement;
  dom_menu.id = "dom_menu"

  /* Add canvas, and menu renderers to page */
  menuRenderer.setSize(_width, _height);
  threejs_panel.append(menuRenderer.domElement);

  /* Lights */
  var light = new THREE.DirectionalLight(0xffffff, 1);
  light.position.set(1, -1, 1);
  light.layers.enable(0);// enabled by default
  light.layers.enable(1);// All selectable objects
  scene.add(light);

  var light = new THREE.DirectionalLight(0x222222);
  light.position.set(-1, 1, -1);
  light.layers.enable(0);// enabled by default
  light.layers.enable(1);// All selectable objects
  scene.add(light);

  var light = new THREE.AmbientLight(0x333333);
  light.position.set(0, 0, 1);
  light.layers.enable(0);// enabled by default
  light.layers.enable(1);// All selectable objects
  scene.add(light);

  /* Add event hander for mouse left click */
  threejs_panel.click(onThreejsPanelMouseClick);

  /* Add event hander for mouse right click */
  threejs_panel.contextmenu(onThreejsPanelMouseContextMenu);
}

function initSceneWithScale(_scale) {
  scale = _scale;

  camera = new THREE.PerspectiveCamera(45, window.threejs_panel.width() / window.threejs_panel.height(), 0.01, scale * 2500);

  camera.position.set(-scale * 3, 0, scale * 5);

  camera.layers.enable(0); // enabled by default
  camera.layers.enable(1); // All selectable objects

  // Controls
  // Possible types: OrbitControls, MapControls
  controls = new THREE.OrbitControls(camera, renderer.domElement);

  controls.enableDamping = true; // an animation loop is required when either damping or auto-rotation are enabled

  controls.dampingFactor = 0.05;
  controls.maxPolarAngle = Math.PI / 2;
  controls.screenSpacePanning = false;

  controls.minDistance = scale / 3;
  controls.maxDistance = scale * 2 * Math.max(window.experiment.data.arena.size.y,
    window.experiment.data.arena.size.x);

  var floor_found = false;
  window.experiment.data.entities.map(function (entity) {
    if (entity.type == "floor") {
      floor_found = true
    }
  })

  /* If no floor entity in experiment */
  if (!floor_found) {
    /* Ground plane */
    var plane_geometry = new THREE.BoxBufferGeometry(
      window.experiment.data.arena.size.x * scale,
      window.experiment.data.arena.size.y * scale,
      0.01
    );

    /* Bring to on top of zero*/
    plane_geometry.translate(0, 0, -0.005 * scale);

    new THREE.TextureLoader().load("/images/ground.png", function (texture) {
      texture.minFilter = THREE.LinearFilter;
      texture.wrapS = texture.wrapT = THREE.RepeatWrapping;
      texture.repeat.set(scale / 2.5, scale / 2.5);

      var material = new THREE.MeshPhongMaterial({
        map: texture
      })
      var plane = new THREE.Mesh(plane_geometry, material);
      plane.layers.set(0);

      scene.add(plane)
    });
  }
}

function cleanUpdateScene() {
  window.isLoadingModels = true;
  /* Remove all meshes */
  Object.keys(sceneEntities).map((i) => {
    const object = scene.getObjectByProperty('uuid', i.mesh);
    if (object) {
      object.geometry.dispose();
      object.material.dispose();
      scene.remove(object);
    }
  });
  /* reset Map */
  uuid2idMap = {};
  selectedEntities = {}

  var count = window.experiment.data.entities.length
  window.experiment.data.entities.map((entity) => {

    if (entity) { //Neglect Null Entities
      GetEntity(entity, scale, function (entityObject) {
        if (entityObject) {
          /* Copy basic properties from Argos entities to threejs objects */
          entityObject.id = entity.id;
          entityObject.type_description = entity.type;

          /* UUID to ID map */
          uuid2idMap[entityObject.mesh.uuid] = entity.id;

          sceneEntities[entity.id] = entityObject;

          /* Its not an object with "is_movable", so considering it movable(robots) */
          if (typeof entity.is_movable === 'undefined' || entity.is_movable === null) {
            /* Hardcoded floor entity in non-selectable entity */
            if (entity.type == "floor") {
              /* Non selectable */
              entityObject.mesh.layers.set(0);
            } else {
              /* Add to selectable layer */
              entityObject.mesh.layers.set(1);
              entityObject.mesh.traverse(function (child) { child.layers.set(1) })
            }
          } else {
            /* If "is_movable" is true */
            if (entity.is_movable && entity.is_movable === true) {
              /* Add to selectable layer */
              entityObject.mesh.layers.set(1);
            } else {
              /* Non selectable */
              entityObject.mesh.layers.set(0);
            }
          }

          scene.add(entityObject.mesh);
        }

        /*
          Comment this to not give a "Point light"
          for a light-entity
        */
        if (entity.type == "light") {
          scene.add(entityObject.light);
        }

        count--;

        if (count == 0) { // Finished loading all models
          window.isLoadingModels = false;
        }
      });
    } else {
      console.error("Entity is null");
    }
  })
}

function onThreejsPanelResize() {
  var _width = window.threejs_panel.width();
  var _height = window.threejs_panel.height();

  camera.aspect = _width / _height
  camera.updateProjectionMatrix();

  renderer.setSize(_width, _height);
  menuRenderer.setSize(_width, _height);
}

function onThreejsPanelMouseContextMenu(event) {
  var mouse = new THREE.Vector2();
  var raycaster = new THREE.Raycaster();
  raycaster.layers.set(1);// Allow to select only in layer 1

  // Convert to -1 to +1
  mouse.x = (event.offsetX / window.threejs_panel.width()) * 2 - 1;
  mouse.y = - (event.offsetY / window.threejs_panel.height()) * 2 + 1;


  // update the picking ray with the camera and mouse position
  raycaster.setFromCamera(mouse, camera);


  // calculate objects intersecting the picking ray // true for recursive(nested)
  var intersects = raycaster.intersectObjects(scene.children, true);

  if (intersects.length > 0) {

    /* Get only the top object */
    var object = intersects[0].object
    /* Get root object, whole parent is Scene */
    while (object.parent.type != "Scene") {
      object = object.parent
    }

    /* Already selected */
    if (selectedEntities[object.uuid]) {
      $.contextMenu({
        selector: '#menu_point',
        trigger: 'none',
        events: {
          hide: function (options) {
            $(this).hide()
            $("#menu_point").hide()
            $("#dom_menu").hide()
          }
        },
        callback: function (key, options) {
          var m = "clicked: " + key;
          console.log(m);
        },
        items: {
          "": ""
        }
      })

      $("#dom_menu").css({
        position: "absolute",
        top: "0",
        "background-color": "rgba(0,0,0,0.3)"
      }).empty()
        .append($("<div/>")
          .attr("id", "menu_point")
          .css({
            position: "absolute",
            top: event.offsetY,
            left: event.offsetX,
            width: "0px",
            height: "0px",
          })
        )
        .show()

      $("#menu_point").contextMenu()
    }
  }
}

function onThreejsPanelMouseClick(event) {
  var mouse = new THREE.Vector2();
  var raycaster = new THREE.Raycaster();
  raycaster.layers.set(1);// Allow to select only in layer 1


  // Convert to -1 to +1
  mouse.x = (event.offsetX / window.threejs_panel.width()) * 2 - 1;
  mouse.y = - (event.offsetY / window.threejs_panel.height()) * 2 + 1;


  // update the picking ray with the camera and mouse position
  raycaster.setFromCamera(mouse, camera);

  // calculate objects intersecting the picking ray // true for recursive(nested)
  var intersects = raycaster.intersectObjects(scene.children, true);

  if (intersects.length > 0) {
    /* Get only the top object */
    var object = intersects[0].object
    /* Get root object, whole parent is Scene */
    while (object.parent.type != "Scene") {
      object = object.parent
    }

    /* Already selected */
    if (selectedEntities[object.uuid]) {
      if (event.shiftKey) {
        /* remove from selection */
        var boundingBox = selectedEntities[object.uuid]

        boundingBox.geometry.dispose();
        boundingBox.material.dispose();
        scene.remove(boundingBox);

        delete selectedEntities[object.uuid];
      }
    } else {/* Object not already selected */
      if (event.shiftKey) {
        /* If multiple select is not there,
         * deselect all currently selected entities
         */
        if (!event.ctrlKey) {
          for (const uuid in selectedEntities) {
            if (selectedEntities.hasOwnProperty(uuid)) {
              const boundingBox = selectedEntities[uuid];

              boundingBox.geometry.dispose();
              boundingBox.material.dispose();
              scene.remove(boundingBox);

              delete selectedEntities[uuid]
            }
          }
        }

        /* Check if new entity is of same type as previously selected */
        if (Object.keys(selectedEntities).length > 0) {
          prev_object_type = sceneEntities[uuid2idMap[Object.keys(selectedEntities)[0]]].type_description
          if (prev_object_type != sceneEntities[uuid2idMap[object.uuid]].type_description) {
            return;// Do not allow different type to be selected
          }

          if (prev_object_type == "box") {
            return; // Disable multiple selection for boxes
          }
        }

        /* Add to selection */
        var boundingBox = new THREE.BoxHelper(object, 0x000000);
        selectedEntities[object.uuid] = boundingBox

        scene.add(boundingBox);
      }
    }
  } else { // No object intersected
    var ids = [];

    for (const uuid in selectedEntities) {
      if (selectedEntities.hasOwnProperty(uuid)) {
        if (uuid2idMap[uuid]) { // Found object
          ids.push(uuid2idMap[uuid]);
        }// selected object is no more in scene, dont know what to do...
      }
    }


    /* Move object if  only control pressed, and one object selected */
    if (event.ctrlKey &&
      !event.altKey &&
      !event.shiftKey &&
      ids.length == 1) {

      var pos = get2DProjectedPosition(mouse, sceneEntities[ids[0]]);

      window.wsp.sendPacked({
        command: 'moveEntity',
        entity_id: ids[0],
        position: {
          x: pos.x,
          y: pos.y,
          z: pos.z
        },
        orientation: {
          x: sceneEntities[ids[0]].mesh.quaternion._x,
          y: sceneEntities[ids[0]].mesh.quaternion._y,
          z: sceneEntities[ids[0]].mesh.quaternion._z,
          w: sceneEntities[ids[0]].mesh.quaternion._w
        }
      });
    }
  }
}

function get2DProjectedPosition(mouse, object) {
  /* Object's Z plane */
  var z_plane = object.mesh.position.z / scale;

  var mouse_point = new THREE.Vector3();
  mouse_point.x = mouse.x;
  mouse_point.y = mouse.y;
  mouse_point.z = z_plane;

  var pos = new THREE.Vector3();


  /* Get point under the mouse */
  mouse_point.unproject(camera);
  mouse_point.sub(camera.position).normalize();
  var distance = (z_plane - camera.position.z) / mouse_point.z;
  pos.copy(camera.position).add(mouse_point.multiplyScalar(distance));

  /* divide by scale to convert back to units from server */
  pos.divideScalar(scale)

  return pos
}

function animate() {
  requestAnimationFrame(animate);
  controls.update();
  render();
}


function render() {
  /* Experiment is initialized */
  if (window.experiment &&
    window.experiment.data &&
    window.experiment.data.entities &&
    window.isLoadingModels == false) {

    /* Entities count changed, clean and render again */
    if (window.experiment.data.entities.length != Object.keys(sceneEntities).length) {
      cleanUpdateScene();
      return; // Go to load models, do not update
    }

    /* Call update of each entity */
    window.experiment.data.entities.map((entity) => {
      sceneEntities[entity.id].update(entity, scale);
    });

    /* Update all bounding boxes */
    for (const uuid in selectedEntities) {
      if (selectedEntities.hasOwnProperty(uuid)) {
        selectedEntities[uuid].update();
      }
    }
  }

  renderer.render(scene, camera);
  menuRenderer.render(scene, camera);
}
