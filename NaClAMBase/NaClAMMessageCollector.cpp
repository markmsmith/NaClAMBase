#include "NaClAMBase.h"
#include "NaClAMMessageCollector.h"

#define STATE_CODE_WAITING_FOR_HEADER 0
#define STATE_CODE_COLLECTING_FRAMES 1

NaClAMMessageCollector::NaClAMMessageCollector() {
  _stateCode = -1;
  _framesLeft = -1;
  _messageReady = false;
}

NaClAMMessageCollector::~NaClAMMessageCollector() {

}

void NaClAMMessageCollector::Init() {
  _stateCode = STATE_CODE_WAITING_FOR_HEADER;
  _framesLeft = 0;
  _messageReady = false;
  _parser = palJSONObjectParser(g_DefaultHeapAllocator);
}

void NaClAMMessageCollector::Collect( PP_Var message ) {
  if (message.type == PP_VARTYPE_STRING) {
    HandleString(message);
  } else if (message.type == PP_VARTYPE_ARRAY_BUFFER) {
    HandleBuffer(message);
  }
  if (_stateCode == STATE_CODE_COLLECTING_FRAMES && _framesLeft == 0) {
    _stateCode = STATE_CODE_WAITING_FOR_HEADER;
    _messageReady = true;
  }
}

bool NaClAMMessageCollector::IsMessageReady() {
  return _messageReady;
}

NaClAMMessage NaClAMMessageCollector::GrabMessage() {
  _messageReady = false;
  return _preparedMessage;
}

void NaClAMMessageCollector::ClearMessage() {
  moduleInterfaces.var->Release(_preparedMessage.headerMessage);
  for (int i = 0; i < _preparedMessage.frameCount; i++) {
    moduleInterfaces.var->Release(_preparedMessage.frames[i]);
  }
  _preparedMessage.reset();
  _messageReady = false;
}

int NaClAMMessageCollector::ParseHeader(palMemBlob blob) {
  if (blob.GetBufferSize() == 0) {
    return -1;
  }
  _parser.Parse(&blob);
  palJSONObject* root = _parser.GetRoot();
  if (root->type == 0) {
    _parser.Clear();
    return -2;
  }
  if (root->type != kJSONTokenTypeMap) {
    _parser.Clear();
    NaClAMPrintf("NaCl AM Error: Header was not a map.");
    return -3;
  }
  palJSONObject* cmd = (*root)["cmd"];
  if (!cmd) {
    NaClAMPrintf("NaCl AM Error: Header did not contain a cmd");
    return -4;
  }
  if (cmd->type != kJSONTokenTypeValueString) {
    NaClAMPrintf("NaCl AM Error: Header cmd was not a string");
    return -5;
  }
  palJSONObject* request = (*root)["request"];
  if (!request) {
    NaClAMPrintf("NaCl AM Error: Header did contain a request");
    return -6;
  }
  if (request->type != kJSONTokenTypeValueNumber) {
    NaClAMPrintf("NaCl AM Error: Header request was not an int");
    return -7;
  }
  palJSONObject* frames = (*root)["frames"];
  if (!frames) {
    NaClAMPrintf("NaCl AM Error: Header did not contain a frames");
    return -8;
  }
  if (frames->type != kJSONTokenTypeValueNumber) {
    NaClAMPrintf("NaCl AM Error: Header frames was not an int");
    return -9;
  }
  _preparedMessage.headerRoot = root;
  _preparedMessage.requestId = request->GetAsInt();
  cmd->GetAsDynamicString(&_preparedMessage.cmdString);
  return frames->GetAsInt();
}

void NaClAMMessageCollector::HandleString(PP_Var message) {
  if (_stateCode == STATE_CODE_WAITING_FOR_HEADER) {
    uint32_t len = 0;
    const char* str = moduleInterfaces.var->VarToUtf8(message, &len);
    if (len == 0) {
      //messagePrintf("Received empty message.");
      return;
    }
    _preparedMessage.headerMessage = message;
    int frames = ParseHeader(palMemBlob((void*)str, len));
    _stateCode = STATE_CODE_COLLECTING_FRAMES;
    if (frames >= 0) {
      _framesLeft = frames;
    } else {
      NaClAMPrintf("Error parsing header: %d", frames);
      _framesLeft = 0;
    }
  } else if (_stateCode == STATE_CODE_COLLECTING_FRAMES) {
    _framesLeft--;
    _preparedMessage.AppendFrame(message);
  }
}

void NaClAMMessageCollector::HandleBuffer(PP_Var buffer) {
  if (_stateCode == STATE_CODE_COLLECTING_FRAMES) {
    _framesLeft--;
    _preparedMessage.AppendFrame(buffer);
  }
}

