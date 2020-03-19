
var camera, controls, scene, renderer, stats;
var scale;

window.isInitialized = false;
window.isLoadingModels = false;

/* Initialize Three.js scene */
THREE.Object3D.DefaultUp.set(0, 0, 1);
scene = new THREE.Scene();
scene.background = new THREE.Color(0x007f7f);


/* ----------------------- */
var sceneEntities = [];

function IntializeThreejs(params) {
  /* WebGLRenderer */
  renderer = new THREE.WebGLRenderer({
    antialias: true,
    // precision: "mediump"
  });
  renderer.setPixelRatio(window.devicePixelRatio);
  renderer.outputEncoding = THREE.sRGBEncoding;

  /* Lights */
  var light = new THREE.DirectionalLight(0xffffff, 1);
  light.position.set(1, -1, 1);
  scene.add(light);

  var light = new THREE.DirectionalLight(0x222222);
  light.position.set(-1, 1, -1);
  scene.add(light);

  var light = new THREE.AmbientLight(0x333333);
  light.position.set(0, 0, 1);
  scene.add(light);

  return renderer;
}

function initSceneWithScale(_scale) {
  scale = _scale;

  camera = new THREE.PerspectiveCamera(45, window.threejs_aspect_ratio, 0.01, scale * 2500);

  camera.position.set(-scale * 3, 0, scale * 5);

  // Controls
  // Possible types: OrbitControls, MapControls
  controls = new THREE.OrbitControls(camera, renderer.domElement);

  controls.enableDamping = true; // an animation loop is required when either damping or auto-rotation are enabled

  controls.dampingFactor = 0.05;
  controls.screenSpacePanning = false;
  controls.minDistance = scale / 3;
  controls.maxDistance = scale * 10;
  controls.maxPolarAngle = Math.PI / 2;

  /* Ground plane */
  var plane_geometry = new THREE.BoxBufferGeometry(
    window.experiment.data.arena.size.x * scale,
    window.experiment.data.arena.size.y * scale,
    0.01
  );
  new THREE.TextureLoader().load("/images/ground.png", function (texture) {
    texture.minFilter = THREE.LinearFilter;
    texture.wrapS = texture.wrapT = THREE.RepeatWrapping;
    texture.repeat.set(5, 5);

    var material = new THREE.MeshPhongMaterial({
      specular: 0x111111,
      shininess: 10,
      map: texture
    })
    var plane = new THREE.Mesh(plane_geometry, material);

    scene.add(plane)
  });
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

  var count = window.experiment.data.entities.length
  window.experiment.data.entities.map((entity) => {

    if (entity) { //Neglect Null Entities
      GetEntity(entity, scale, function (entityObject) {
        if (entityObject) {
          sceneEntities[entity.id] = entityObject;
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


function animate() {
  requestAnimationFrame(animate);
  controls.update(); // only required if controls.enableDamping = true, or if controls.autoRotate = true
  render();
  // stats.update();
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
  }

  renderer.render(scene, camera);
}
