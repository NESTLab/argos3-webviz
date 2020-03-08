class Light {
    constructor(entity, scale_) {
        this.scale = scale_ * 0.001;

        var geometry = new THREE.SphereBufferGeometry(
            0.1 * scale,
            32, 32
        );

        var material = new THREE.MeshBasicMaterial({
            color: parseInt(entity.color),
        });

        var lightMesh = new THREE.Mesh(geometry, material);

        lightMesh.rotation.setFromQuaternion(new THREE.Quaternion(
            entity.orientation.x,
            entity.orientation.y,
            entity.orientation.z,
            entity.orientation.w));
        lightMesh.position.x = entity.position.x * scale;
        lightMesh.position.y = entity.position.y * scale;
        lightMesh.position.z = entity.position.z * scale;

        this.mesh = lightMesh;

        var pointLight = new THREE.PointLight(parseInt(entity.color), 1, 100, 2);
        pointLight.position.set(entity.position.x * scale,
            entity.position.y * scale,
            entity.position.z * scale);

        this.light = pointLight
    }

    update(entity) {
        // console.log(this.light);
        /* Do not update anything, considering the light doesn't move */
    }
}