
/* Load All entities */
loadJS("/js/libs/jquery.min.js", true)
loadJS("/js/libs/w2ui-1.5.rc1.min.js", true)


loadJS("/js/entities/Box.js", true)
loadJS("/js/entities/DefaultEntity.js", true)
loadJS("/js/entities/Light.js", true)

loadJS("/js/entities/Cylinder.js", true)
loadJS("/js/entities/Footbot.js", true)
loadJS("/js/entities/KheperaIV.js", true)

function GetEntity(entity, scale, callback) {
  switch (entity.type) {
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
    case 'light':
      callback(new Light(entity, scale, callback))
      break;
    default:
      callback(new DefaultEntity(entity, scale))
      break;
  }
}
