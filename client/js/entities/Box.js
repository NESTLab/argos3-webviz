class Box {
    constructor(entity, scale) {
        this.scale = scale;

        var geometry = new THREE.BoxBufferGeometry(
            entity.scale.x * scale,
            entity.scale.y * scale,
            entity.scale.z * scale
        );
        /* Bring to on top of zero*/
        geometry.translate(0, 0, entity.scale.z * scale * 0.5);

        var color = null;
        if (entity.is_movable) {
            color = 0xff0000;
        } else {
            color = 0x766e64
        }
        var material = new THREE.MeshPhongMaterial({
            color: color,
        });

        var box = new THREE.Mesh(geometry, material);

        box.rotation.setFromQuaternion(new THREE.Quaternion(
            entity.orientation.x,
            entity.orientation.y,
            entity.orientation.z,
            entity.orientation.w));
        box.position.x = entity.position.x * scale;
        box.position.y = entity.position.y * scale;
        box.position.z = entity.position.z * scale;

        this.mesh = box;
    }

    update(entity) {
        /* Do not update anything, considering the boxes doesn't move */
        /* TODO: handle "is_movable" and move the box */
    }
}