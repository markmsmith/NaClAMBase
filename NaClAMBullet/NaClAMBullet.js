
function NaClAMBulletInit() {
	aM.addEventListener('sceneloaded', NaClAMBulletSceneLoadedHandler);
	aM.addEventListener('noscene', NaClAMBulletStepSceneHandler);
	aM.addEventListener('sceneupdate', NaClAMBulletStepSceneHandler);
}

function NaClAMBulletSceneLoadedHandler(msg) {
	console.log('Scene loaded.');
	console.log('Scene object count = ' + msg.header.sceneobjectcount);
}

function NaClAMBulletLoadScene(sceneDescription) {
	aM.sendMessage('loadscene', sceneDescription);
	aM.sendMessage('stepscene', {});
}

function NaClAMBulletStepSceneHandler(msg) {
	// Step the scene
	aM.sendMessage('stepscene', {});
	var i;
	var j;
	var numTransforms = 0;
	if (msg.header.cmd == 'sceneupdate') {
		TransformBuffer = new Float32Array(msg.frames[0]);
		numTransforms = TransformBuffer.length/16;
		for (i = 0; i < numTransforms; i++) {
			for (j = 0; j < 16; j++) {
				objects[i].matrixWorld.elements[j] = TransformBuffer[i*16+j];
			}
		}	
	}
	animate();
}