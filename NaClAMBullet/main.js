NaClAMModule = null;
aM = null;
statusText = 'NO-STATUS';

function pageDidLoad() {
  console.log('started');
  aM = new NaClAM('NaClAM');
  aM.enable();
  init();
  animate();
  NaClAMBulletInit();
  NaClAMBulletLoadScene(sceneDescription);
}

window.addEventListener("load", pageDidLoad, false);