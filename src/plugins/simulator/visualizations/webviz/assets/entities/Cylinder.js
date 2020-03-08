class Cylinder {
    constructor(entity, scale) {
        this.scale = scale;

        var geometry = new THREE.CylinderBufferGeometry(
            entity.radius * scale,
            entity.radius * scale,
            entity.height * scale,
            32
        );
        geometry.rotateX(-1.572);

        /* Bring to on top of zero*/
        geometry.translate(0, 0, entity.height * scale * 0.5);

        var color = null;
        if (entity.is_movable) {
            color = 0x00ff00;
        } else {
            color = 0x766e64
        }
        var material = new THREE.MeshPhongMaterial({
            color: color,
        });

        var cylinder = new THREE.Mesh(geometry, material);

        cylinder.rotation.setFromQuaternion(new THREE.Quaternion(
            entity.orientation.x,
            entity.orientation.y,
            entity.orientation.z,
            entity.orientation.w));
        cylinder.position.x = entity.position.x * scale;
        cylinder.position.y = entity.position.y * scale;
        cylinder.position.z = entity.position.z * scale;

        this.mesh = cylinder;
    }

    update(entity) {
        /* Do not update anything, considering the cylinders doesn't move */
        /* TODO: handle "is_movable" and move the cylinder */
    }
}