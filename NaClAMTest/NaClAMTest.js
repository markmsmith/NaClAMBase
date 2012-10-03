
function testFloatSum(num) {
	var buff = new Float32Array(num+1);
	var i;
	for (i = 0; i < num+1; i++) {
		buff[i] = i;
	}
	var requestId = aM.sendMessage('floatsum', {}, [buff.buffer]);
	aM.addEventListener(requestId, function(msg) {
		var correctResult = num*(num+1)/2;
		var amResult = msg['header']['sum'];
		if (correctResult != amResult) {
			console.log('Error: Acceleration Module gave wrong result.');
		}
		console.log('Sum from 0 to ' + num + ' = ' + msg['header']['sum']);
	});
}