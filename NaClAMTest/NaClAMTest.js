function testFloatSum(num) {
	var buff = new Float32Array(num+1);
	var i;
	for (i = 0; i < num+1; i++) {
		buff[i] = i;
	}
	aM.sendMessage('floatsum', {}, [buff.buffer]);
	var handler = function(msg) {
		var correctResult = num*(num+1)/2;
		var amResult = msg['header']['sum'];
		if (correctResult != amResult) {
			console.log('Error: Acceleration Module gave wrong result.');
		}
		console.log('Sum from 0 to ' + num + ' = ' + msg['header']['sum']);
		aM.removeEventListener('floatsum', handler);
	}
	aM.addEventListener('floatsum', handler);
}

function testFloatAdd(num, a, b) {
	var buff1 = new Float32Array(num);
	var buff2 = new Float32Array(num);
	var i;
	for (i = 0; i < num; i++) {
		buff1[i] = a;
		buff2[i] = b;
	}
	aM.sendMessage('addfloatarrays', {}, [buff1.buffer, buff2.buffer]);
	var handler = function(msg) {
		var result = new Float32Array(msg.frames[0]);
		var j;
		var correctResult = a + b;
		for (j = 0; j < result.length; j++) {
			if (result[j] != correctResult) {
				console.log('Got incorrect result at ' + j + 'result = ' + result[j]);
				break;
			}
		}
		if (j == result.length) {
			console.log('Arrays correctly added. Each element = ' + correctResult);
		}
		aM.removeEventListener('floatadd', handler);
	};
	aM.addEventListener('floatadd', handler);
}

function testFloatSub(num, a, b) {
	var buff1 = new Float32Array(num);
	var buff2 = new Float32Array(num);
	var i;
	for (i = 0; i < num; i++) {
		buff1[i] = a;
		buff2[i] = b;
	}
	aM.sendMessage('subfloatarrays', {}, [buff1.buffer, buff2.buffer]);
	var handler = function(msg) {
		var result = new Float32Array(msg.frames[0]);
		var j;
		var correctResult = a - b;
		for (j = 0; j < result.length; j++) {
			if (result[j] != correctResult) {
				console.log('Got incorrect result at ' + j + 'result = ' + result[j]);
				break;
			}
		}
		if (j == result.length) {
			console.log('Arrays correctly subtracted. Each element = ' + correctResult);
		}
		aM.removeEventListener('floatsub', handler);
	};
	aM.addEventListener('floatsub', handler);
}