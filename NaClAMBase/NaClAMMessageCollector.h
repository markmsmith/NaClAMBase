#pragma once

#include "NaClAMBase.h"
#include "NaClAMMessage.h"

class NaClAMMessageCollector {
  int _stateCode;
  int _framesLeft;
  bool _messageReady;
  NaClAMMessage _preparedMessage;
  palJSONObjectParser _parser;

  int ParseHeader(palMemBlob blob);
  void HandleString(PP_Var message);
  void HandleBuffer(PP_Var buffer);
public:
  NaClAMMessageCollector();
  ~NaClAMMessageCollector();

  void Init();

  void Collect(PP_Var message);
  bool IsMessageReady();
  NaClAMMessage GrabMessage();
  void ClearMessage();
};