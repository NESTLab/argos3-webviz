class DefaultEntity {
    constructor(entity, scale) {



        try {
            var dot = new THREE.Mesh(new THREE.BoxBufferGeometry(
                1,
                1,
                1
            ), new THREE.MeshPhongMaterial({
                color: 0xffa23f,
                flatShading: false
            }));

            dot.rotation.setFromQuaternion(new THREE.Quaternion(
                entity.orientation.x,
                entity.orientation.y,
                entity.orientation.z,
                entity.orientation.w));

            dot.position.x = entity.position.x;
            dot.position.y = entity.position.y;
            dot.position.z = entity.position.z;

            this.mesh = dot;
        } catch (ignored) {
            this.mesh = null; // dont create anything
        }


    }

    update(entity, scale) {
        try {
            this.mesh.position.x = entity.position.x * scale;
            this.mesh.position.y = entity.position.y * scale;

            this.mesh.rotation.setFromQuaternion(new THREE.Quaternion(
                entity.orientation.x,
                entity.orientation.y,
                entity.orientation.z,
                entity.orientation.w));
        } catch (ignored) {

        }
    }
}