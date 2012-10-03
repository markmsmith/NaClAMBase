#pragma once

#include "libpal/libpal.h"

#define MAX_FRAMES 16
struct NaClAMMessage {
  const palJSONObject* headerRoot;
  palDynamicString cmdString;
  int requestId;
  PP_Var headerMessage;
  PP_Var frames[MAX_FRAMES];
  int frameCount;

  NaClAMMessage() {
    headerRoot = NULL;
    frameCount = 0;
  }

  void reset() {
    headerRoot = NULL;
    frameCount = 0;
  }

  void AppendFrame(PP_Var frame) {   
    frames[frameCount] = frame;
    frameCount++;
  }
};