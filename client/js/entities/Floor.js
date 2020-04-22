/**
 * @file <client/js/entities/Floor.js>
 * 
 * @author Prajankya Sonar - <prajankya@gmail.com>
 * 
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 * 
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

class Floor {
    constructor(entity, scale, EntityLoadingFinishedFn) {
        this.scale = scale;
        this.entity = entity;

        var geometry = new THREE.BoxBufferGeometry(
            window.experiment.data.arena.size.x * scale,
            window.experiment.data.arena.size.y * scale,
            0.01
        );

        /* Bring to on top of zero*/
        geometry.translate(0, 0, -0.005 * scale);

        this.mesh = undefined;

        if (entity.floor_image) {
            var that = this

            new THREE.TextureLoader().load(entity.floor_image, function (texture) {
                texture.minFilter = THREE.LinearFilter;
                var material = new THREE.MeshPhongMaterial({
                    map: texture
                })
                var floor = new THREE.Mesh(geometry, material);
                that.mesh = floor;

                EntityLoadingFinishedFn(that);
            });
        } else {
            EntityLoadingFinishedFn(this);
        }
    }

    getMesh() {
        return this.mesh;
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