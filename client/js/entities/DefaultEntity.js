/**
 * @file <client/js/entities/DefaultEntity.js>
 * 
 * @author Prajankya Sonar - <prajankya@gmail.com>
 * 
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 * 
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

class DefaultEntity {
    constructor(entity, scale, EntityLoadingFinishedFn) {
        this.scale = scale;
        this.entity = entity;

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

        EntityLoadingFinishedFn(this);
    }

    getMesh() {
        return this.mesh
    }


    update(entity) {
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