/**
 * @file <client/js/entities/loadEntities.js>
 * 
 * @author Prajankya Sonar - <prajankya@gmail.com>
 * 
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 * 
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

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
    case 'cylinder':
      return new Cylinder(entity, scale, callback)
    case 'kheperaiv':
      return new KheperaIV(entity, scale, callback)
    case 'foot-bot':
      return new Footbot(entity, scale, callback)
    default:
      return new DefaultEntity(entity, scale, callback)
  }
}
