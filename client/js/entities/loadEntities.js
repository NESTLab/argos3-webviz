
/* Load All entities */
loadJS("/js/entities/Box.js", true)
loadJS("/js/entities/DefaultEntity.js", true)
loadJS("/js/entities/Light.js", true)
loadJS("/js/entities/Floor.js", true)

loadJS("/js/entities/Cylinder.js", true)
loadJS("/js/entities/Footbot.js", true)
loadJS("/js/entities/KheperaIV.js", true)

/* Add your custom entity here.. */


function GetEntity(entity, scale, callback) {
  /* You can use callback to get content synchronously */
  switch (entity.type) {
    case 'floor':
      new Floor(entity, scale, callback)
      break;
    case 'light':
      callback(new Light(entity, scale))
      break;
    case 'box':
      callback(new Box(entity, scale))
      break;
    case 'cylinder':
      callback(new Cylinder(entity, scale))
      break;
    case 'kheperaiv':
      return new KheperaIV(entity, scale, callback)
      break;
    case 'foot-bot':
      return new Footbot(entity, scale, callback)
      break;
    default:
      callback(new DefaultEntity(entity, scale))
      break;
  }
}
