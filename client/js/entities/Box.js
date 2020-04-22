/**
 * @file <client/js/entities/Box.js>
 * 
 * @author Prajankya Sonar - <prajankya@gmail.com>
 * 
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 * 
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

class Box {
    constructor(entity, scale, EntityLoadingFinishedFn) {
        this.scale = scale;
        this.entity = entity;

        var geometry = new THREE.BoxBufferGeometry(
            entity.scale.x * scale,
            entity.scale.y * scale,
            entity.scale.z * scale
        );

        /* Bring above ground */
        geometry.translate(0, 0, entity.scale.z * scale * 0.5);

        var color = null;
        if (entity.is_movable) {
            color = 0xff0000;
        } else {
            color = 0x766e64;
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

        EntityLoadingFinishedFn(this);
    }

    getMesh() {
        return this.mesh;
    }

    update(entity) {
        if (entity.is_movable) {
            try {
                this.mesh.position.x = entity.position.x * this.scale;
                this.mesh.position.y = entity.position.y * this.scale;

                this.mesh.rotation.setFromQuaternion(new THREE.Quaternion(
                    entity.orientation.x,
                    entity.orientation.y,
                    entity.orientation.z,
                    entity.orientation.w));
            } catch (ignored) { }
        }
    }
}