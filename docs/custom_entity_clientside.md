# Adding a custom entity to Webviz: Client side

In the folder [client/js/entities](../client/js/entities) you need to create a new javascript file like shown below,


```javascript

class NewEntity {
    constructor(entity, scale, EntityLoadingFinishedFn) {
        this.scale = scale;
        this.entity = entity;

        /* Here geometry of the new entity is generated, using THREE.js buffer geometries */
        var geometry = new THREE.BoxBufferGeometry(
            15 * scale, // For now, create a box with 15 size
            15 * scale,
            15 * scale
        );

        /* Bring above ground */
        geometry.translate(0, 0, 15 * scale * 0.5);

        var material = new THREE.MeshPhongMaterial({
            color: 0xff0000, // Red color
        });

        var mesh = new THREE.Mesh(geometry, material);

        mesh.rotation.setFromQuaternion(new THREE.Quaternion(
            entity.orientation.x,
            entity.orientation.y,
            entity.orientation.z,
            entity.orientation.w));

        mesh.position.x = entity.position.x * scale;
        mesh.position.y = entity.position.y * scale;
        mesh.position.z = entity.position.z * scale;

        this.mesh = mesh;

        EntityLoadingFinishedFn(this);
    }

    getMesh() {
        return this.mesh;
    }

    update(entity) {
        this.mesh.position.x = entity.position.x * this.scale;
        this.mesh.position.y = entity.position.y * this.scale;
        /* Considering object will not move along Z axis */
        
        this.mesh.rotation.setFromQuaternion(new THREE.Quaternion(
            entity.orientation.x,
            entity.orientation.y,
            entity.orientation.z,
            entity.orientation.w));
    }
}
```

And edit the file [client/js/entities/loadEntities.js](client/js/entities/loadEntities.js) to add your new file include, using `loadJS` as shown in the code below,

And also add an entry for the new class in `switch`, as shown.

```javascript

/* Load All entities */
loadJS("/js/entities/DefaultEntity.js")
loadJS("/js/entities/Light.js")
loadJS("/js/entities/Floor.js")

loadJS("/js/entities/Box.js")
loadJS("/js/entities/Cylinder.js")
loadJS("/js/entities/Footbot.js")

loadJS("/js/entities/KheperaIV.js")

/* Add your custom entity here.. */


function GetEntity(entity, scale, callback) {
  /* You can use callback to get content synchronously */
  switch (entity.type) {
    case 'floor':
      return new Floor(entity, scale, callback)
    case 'light':
      return new Light(entity, scale, callback)
    case 'box':
      return new Box(entity, scale, callback)
    /** .
     *  .
     *  other entities
     *  .
     *  .
     */
    case 'new-entity': // This is ARGoS Entity type
      return new NewEntity(entity, scale, callback)
    /** .
     *  .
     *  other entities
     *  .
     *  .
     */
    default:
      return new DefaultEntity(entity, scale, callback)
  }
}

```