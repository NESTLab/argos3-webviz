class Floor {
    constructor(entity, scale, callback) {
        this.scale = scale;

        var geometry = new THREE.BoxBufferGeometry(
            window.experiment.data.arena.size.x * scale,
            window.experiment.data.arena.size.y * scale,
            0.01
        );

        /* Bring to on top of zero*/
        geometry.translate(0, 0, -0.005 * scale);

        var that = this
        if (entity.floor_image) {
            new THREE.TextureLoader().load(entity.floor_image, function (texture) {
                texture.minFilter = THREE.LinearFilter;
                var material = new THREE.MeshPhongMaterial({
                    map: texture
                })
                var floor = new THREE.Mesh(geometry, material);
                that.mesh = floor;
                callback(that)
            });
        }
        this.mesh = undefined
    }

    update(entity) {
        if (entity.floor_image && this.mesh) {
            var that = this
            new THREE.TextureLoader().load(entity.floor_image, function (texture) {
                texture.minFilter = THREE.LinearFilter;
                // in this example we create the material when the texture is loaded
                that.mesh.material.map = texture;
            });
        }
    }
}