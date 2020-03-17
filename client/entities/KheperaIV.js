class KheperaIV {
    constructor(entity, scale, callback) {
        this.scale = scale;
        this.lines = [];

        /* Scale to convert from mm to scale used here */
        var UNIT_SCALE = 0.001 * scale

        var loader = new THREE.GLTFLoader();
        var that = this;

        loader.load('/models/KheperaIV.gltf', function(gltf) {
            var kheperaiv_bot = gltf.scene.children[0];
            var boundingBox = new THREE.Box3().setFromObject(kheperaiv_bot);

            for (let i = 0; i < kheperaiv_bot.children.length; i++) {
                /* The scale in model were not right */
                kheperaiv_bot.children[i].geometry.scale(0.825, 0.825, 1);

                /* Move above the surface */
                kheperaiv_bot.children[i].geometry.translate(0, 0, boundingBox.getSize().z + 4.7 * UNIT_SCALE);

                kheperaiv_bot.children[i].material = new THREE.MeshBasicMaterial({
                    map: kheperaiv_bot.children[i].material.map,
                    color: 0xffffff,
                    side: THREE.SingleSide

                });
            }
            kheperaiv_bot.rotateZ(1.572);


            // --- LED 1
            var led1Geom = new THREE.SphereBufferGeometry(
                0.2,
                4,
                4
            );
            led1Geom.translate(40 * UNIT_SCALE, 25 * UNIT_SCALE, 54 * UNIT_SCALE)

            var led1 = new THREE.Mesh(led1Geom, new THREE.MeshLambertMaterial({
                emissive: 0x00ff00,
                color: 0x000000
            }));

            // --- LED 2
            var led2Geom = new THREE.SphereBufferGeometry(
                0.2,
                4,
                4
            );
            led2Geom.translate(-50 * UNIT_SCALE, 0, 54 * UNIT_SCALE)
            var led2 = new THREE.Mesh(led2Geom, new THREE.MeshLambertMaterial({
                emissive: 0xff0000,
                color: 0x000000
            }));

            // --- LED 3
            var led3Geom = new THREE.SphereBufferGeometry(
                0.2,
                4,
                4
            );
            led3Geom.translate(40 * UNIT_SCALE, -25 * UNIT_SCALE, 54 * UNIT_SCALE)
            var led3 = new THREE.Mesh(led3Geom, new THREE.MeshLambertMaterial({
                emissive: 0x000000,
                color: 0x000000
            }));

            var meshParent = new THREE.Mesh();

            /* Add mesh components */
            meshParent.add(kheperaiv_bot);
            meshParent.add(led1);
            meshParent.add(led2);
            meshParent.add(led3);

            /* Add Intersection Points */
            var pointsGeom = new THREE.BufferGeometry();
            pointsGeom.setAttribute('position', new THREE.BufferAttribute(
                new Float32Array(8 * 3), // 8 points * 3 axis per point
                3
            ));

            var points = new THREE.Points(pointsGeom, new THREE.PointsMaterial({
                color: 0x000000
            }));
            meshParent.add(points);

            /* Add lines for rays */
            for (let i = 0; i < 8; i++) {
                var lineGeom = new THREE.BufferGeometry();

                // attributes
                var linesPos = new Float32Array(2 * 3); //2 points per line * 3 axis per point
                lineGeom.setAttribute('position', new THREE.BufferAttribute(linesPos, 3));

                var line = new THREE.Line(lineGeom);

                meshParent.add(line);
                that.lines.push(line);
            }

            /* Update mesh parent */
            meshParent.position.x = entity.position.x * scale;
            meshParent.position.y = entity.position.y * scale;
            meshParent.position.z = entity.position.z * scale;

            that.mesh = meshParent;

            callback(that)
        });
    }

    update(entity, scale) {
        if (this.mesh) {
            this.mesh.position.x = entity.position.x * scale;
            this.mesh.position.y = entity.position.y * scale;

            this.mesh.rotation.setFromQuaternion(new THREE.Quaternion(
                entity.orientation.x,
                entity.orientation.y,
                entity.orientation.z,
                entity.orientation.w));

            if (entity.leds) {
                /* Update LED colors */
                this.mesh.children[1].material.color.setHex(entity.leds[0]);
                this.mesh.children[1].material.emissive.setHex(entity.leds[0]);

                this.mesh.children[2].material.color.setHex(entity.leds[1]);
                this.mesh.children[2].material.emissive.setHex(entity.leds[1]);

                this.mesh.children[3].material.color.setHex(entity.leds[2]);
                this.mesh.children[3].material.emissive.setHex(entity.leds[2]);
            }

            var pointMesh = this.mesh.children[4];

            if (entity.points.length > 0) {
                var points = pointMesh.geometry.getAttribute('position').array

                for (let i = 0; i < entity.points.length; i++) {
                    var pointVals = entity.points[i].split(",")
                    points[3 * i] = pointVals[0] * scale
                    points[3 * i + 1] = pointVals[1] * scale
                    points[3 * i + 2] = pointVals[2] * scale
                }
                pointMesh.geometry.getAttribute('position').needsUpdate = true;
            }

            /* Only draw given points, and hide all previous points */
            pointMesh.geometry.setDrawRange(0, entity.points.length);

            /* Draw rays */
            if (entity.rays.length > 0) {
                for (let i = 0; i < entity.rays.length; i++) {
                    /*
                        For each ray as a string,
                        format -> "BoolIsChecked:Vec3StartPoint:Vec3EndPoint"
                        For example -> "true:1,2,3:1,2,4"
                    */

                    var rayArr = entity.rays[i].split(":")
                    var start = rayArr[1].split(",")
                    var end = rayArr[2].split(",")

                    var line = this.mesh.children[5 + i];

                    if (line) {
                        if (rayArr[0] == "true") {
                            line.material.color.setHex(0xff00ff);
                        } else {
                            line.material.color.setHex(0x00ffff);
                        }

                        var positions = line.geometry.getAttribute('position').array

                        positions[0] = start[0] * scale
                        positions[1] = start[1] * scale
                        positions[2] = start[2] * scale

                        positions[3] = end[0] * scale
                        positions[4] = end[1] * scale
                        positions[5] = end[2] * scale

                        line.geometry.getAttribute('position').needsUpdate = true;
                        line.geometry.setDrawRange(0, 2);
                    }
                }
            }
            /* Hide all the previous lines */
            /* 5 are the number of objects in meshParent before rays */
            for (let i = 5 + entity.rays.length; i < this.mesh.children.length; i++) {
                this.mesh.children[i].geometry.setDrawRange(0, 0);
            }
        }
    }
}