function NaClAMMessage() {
	this.header = {};
	this.frames = new Array();
}

NaClAMMessage.prototype.reset = function() {
	this.header = {};
	this.frames = new Array();
}

function NaClAM(embedId, logId) {
	this.embedId = embedId;
	this.logId = logId;
	this.requestId = 0;
	this.message = new NaClAMMessage();
	this.state = 0;
	this.framesLeft = 0;
	this.callbacks_ = Object.create(null);
	this.handleMesssage_ = this.handleMesssage_.bind(this);
}

NaClAM.prototype.enable = function() {
	window.addEventListener('message', this.handleMesssage_, true);
}

NaClAM.prototype.disable = function() {
	window.removeEventListener('message', this.handleMesssage_, true);
}

NaClAM.prototype.log_ = function(msg) {
	logDiv = document.getElementById(this.logId);
    var old = logDiv.innerHTML;
    logDiv.innerHTML = "<p>";
    logDiv.innerHTML += msg;
    logDiv.innerHTML += "</p>";
    logDiv.innerHTML += old;  
}

NaClAM.prototype.handleMesssage_ = function(event) {
	var STATE_WAITING_FOR_HEADER = 0;
	var STATE_COLLECTING_FRAMES = 1;
	if (this.state == STATE_WAITING_FOR_HEADER) {
		var header = JSON.parse(String(event.data));
		// Special case our log print command
		if (header['cmd'] == 'NaClAMPrint') {
			this.log_(header['print'])
			return;
		}
		if (typeof(header['request']) != "number") {
			console.log('Header message requestId is not a number.');
			return;
		}
		if (typeof(header['frames']) != "number") {
			console.log('Header message frames is not a number.');
			return;
		}
		this.framesLeft = header['frames'];
		this.state = STATE_COLLECTING_FRAMES;
		this.message.header = header;
	} else if (this.state == STATE_COLLECTING_FRAMES) {
		this.framesLeft--;
		this.frames.push(event.data);
	}
	if (this.state == STATE_COLLECTING_FRAMES && this.framesLeft == 0) {
		// Dispatch message
		var requestId = this.message.header['request'];
		var callback = this.callbacks_[requestId];
		if (callback) {
			callback(this.message);
			this.message.reset();
			delete this.callbacks_[requestId];
		}
		this.state = STATE_WAITING_FOR_HEADER;
 	}
}

NaClAM.prototype.messageHeaderIsValid_ = function(header) {
	if (!header['cmd']) {
		console.log('NaClAM: Message header does not contain cmd.');
		return false;
	}
	if (typeof(header['cmd']) != "string") {
		console.log('NaClAm: Message cmd is not a string.');
		return false;
	}
	if (!header['frames']) {
		console.log('NaClAM: Message header does not contain frames.');
		return false;
	}
	if (typeof(header['frames']) != "number") {
		console.log('NaClAm: Message frames is not a number.');
		return false;
	}
	if (!header['request']) {
		console.log('NaClAM: Message header does not contain request.');
		return false;
	}
	if (typeof(header['request']) != "number") {
		console.log('NaClAm: Message request is not a number.');
		return false;
	}
	return true;
}

NaClAM.prototype.framesIsValid_ = function(frames) {
	var i;
	if (!frames) {
		// No frames.
		return true;
	}
	if (Array.isArray(frames) == false) {
		console.log('NaClAM: Frames must be an array.');
		return false;
	}
	for (i = 0; i < frames.length; i++) {
		var e = frames[i];
		if (typeof(e) == "string") {
			continue;
		}
		if ((e instanceof ArrayBuffer) == false) {
			console.log('NaClAM: Frame is not a string or ArrayBuffer');
			return false;
		}
	}
	return true;
}

NaClAM.prototype.framesLength_ = function(frames) {
	if (!frames) {
		// No frames.
		return 0;
	}
	return frames.length;
}

NaClAM.prototype.sendMessage = function(cmdName, arguments, frames) {
	if (this.framesIsValid_(frames) == false) {
		console.log('NaClAM: Not sending message because frames is invalid.');
		return undefined;
	}
	var numFrames = this.framesLength_(frames);
	this.requestId++;
	var msgHeader = {
		cmd: cmdName,
		frames: numFrames,
		request: this.requestId,
		args: arguments
	};
	if (this.messageHeaderIsValid_(msgHeader) == false) {
		console.log('NaClAM: Not sending message because header is invalid.');
		return undefined;
	}
	var AM = document.getElementById(this.embedId);
	if (!AM) {
		console.log('NaClAM: Not sending message because Acceleration Module is not there.');
		return undefined;
	}
	AM.postMessage(JSON.stringify(msgHeader));
	var i;
	for (i = 0; i < numFrames; i++) {
		AM.postMessage(frames[i]);
	}
	return this.requestId;
}

NaClAM.prototype.addEventListener = function(id, callback) {
	this.callbacks_[id] = callback;
}