#include "NaClAMBase/NaClAMBase.h"

/**
 * This function is called at module initialization time.
 * moduleInterfaces and moduleInstance are already initialized.
 */
void NaClAMModuleInit() {
  NaClAMPrintf("AM Running.");
  NaClAMPrintf("Build %s %s", __DATE__, __TIME__);
}

/**
 * This function is called at 60hz.
 * @param microseconds A monotonically increasing clock
 */
void NaClAMModuleHeartBeat(uint64_t microseconds) {
}

/* Computes the sum of the frame containing floating point numbers */
float floatsum(const NaClAMMessage& message) {
  if (message.frames[0].type != PP_VARTYPE_ARRAY_BUFFER) {
    return 0.0;
  }
  uint32_t len = 0;
  moduleInterfaces.varArrayBuffer->ByteLength(message.frames[0], &len);
  float* buf = (float*)moduleInterfaces.varArrayBuffer->Map(message.frames[0]);
  float sum = 0.0;
  uint32_t numFloats = len/sizeof(float);
  for (uint32_t i = 0; i < numFloats; i++) {
    sum += buf[i];
  }
  moduleInterfaces.varArrayBuffer->Unmap(message.frames[0]);
  return sum;
}

void floatsumReply(const NaClAMMessage& message, float sum) {
  palDynamicString jsonMessage;
  palJSONBuilder builder;
  builder.Start(&jsonMessage);
  builder.PushObject();
  builder.Map("frames", 0, true);
  builder.Map("request", message.requestId, true);
  builder.Map("cmd", "reply", true);
  builder.Map("sum", sum, false);
  builder.PopObject();
  PP_Var msgVar = moduleInterfaces.var->VarFromUtf8(jsonMessage.C(), 
                                                    jsonMessage.GetLength());
  NaClAMSendMessage(msgVar, NULL, 0);
  moduleInterfaces.var->Release(msgVar);
}

/**
 * This function is called for each message received from JS
 * @param message A complete message sent from JS
 */
void NaClAMModuleHandleMessage(const NaClAMMessage& message) {
  if (message.cmdString.Equals("floatsum")) {
    float sum = floatsum(message);
    floatsumReply(message, sum);
  } else {
    NaClAMPrintf("Got message I don't understand: %s", message.cmdString.C());
  }
}