
function NaClAMBulletStartScene(sceneDescription) {
	var requestId = aM.sendMessage('loadscene', sceneDescription);
	aM.addEventListener(requestId, function(msg) {
		console.log('Scene loaded.');
	});
}

function NaClAMBulletStepScene() {
	var requestId = aM.sendMessage('stepscene', {});
	aM.addEventListener(requestId, function(msg) {
		var i;
		var j;
		var numTransforms = 0;
		if (msg.header.cmd != 'sceneupdate') {
			animate();
			return;
		}
		TransformBuffer = new Float32Array(msg.frames[0]);
		numTransforms = TransformBuffer.length/16;
		for (i = 0; i < numTransforms; i++) {
			for (j = 0; j < 16; j++) {
				objects[i].matrixWorld.elements[j] = TransformBuffer[i*16+j];
			}
		}
		animate();
	});
}