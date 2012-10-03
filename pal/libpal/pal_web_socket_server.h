#pragma once

/*
  Copyright (c) 2011 John McCutchan <john@johnmccutchan.com>

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
  claim that you wrote the original software. If you use this software
  in a product, an acknowledgment in the product documentation would be
  appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.

  3. This notice may not be removed or altered from any source
  distribution.
*/

#include "libpal/pal_array.h"
#include "libpal/pal_mem_blob.h"
#include "libpal/pal_tcp_listener.h"
#include "libpal/pal_event.h"
#include "libpal/pal_delegate.h"

#define PAL_WEBSOCKET_SERVER_ALREADY_CONNECTED palMakeErrorCode(PAL_ERROR_CODE_WEBSOCKET_GROUP, 1)
#define PAL_WEBSOCKET_SERVER_NO_LISTENER palMakeErrorCode(PAL_ERROR_CODE_WEBSOCKET_GROUP, 2)

class palWebSocketServer {
public:
  palWebSocketServer();

  void SetIncomingBuffer(unsigned char* incoming_buffer, size_t buffer_capacity);
  void SetOutgoingBuffer(unsigned char* outgoing_buffer, size_t buffer_capacity);
  void SetMessageBuffer(unsigned char* msg_buffer, size_t buffer_capacity);

  // total capacity of buffers
  uint64_t GetIncomingBufferCapacity();
  uint64_t GetOutgoingBufferCapacity();

  // amount currently used
  uint64_t GetIncomingBufferSize();
  uint64_t GetOutgoingBufferSize();

  // Attempt to take a connection
  int AcceptConnection(palTcpListener* listener);
  int CloseConnection();

  // The state these functions reference are updated by calling Update
  bool HasOpen() const;
  bool HasClose() const;
  bool HasError() const;

  bool Connected() const;

  int ConnectionError();

  // 1) accept/disconnect a connection
  // 2) send any pending messages
  // 3) receive any pending messages
  // 4) sets HasOpen, Close, Error flags
  void Update();

  int SendPing();

  int SendTextMessage(const char* msg);
  int SendTextMessage(const char* msg, size_t msg_length);

  typedef palDelegate<void (palMemBlob msg, bool binary_message, void* userdata)> OnMessageDelegate;
  uint64_t PendingMessageCount() const;
  void ProcessMessages(OnMessageDelegate del, void* userdata);
  void ClearMessages();
protected:
  void ClientDisconnect();
  void ResetBuffers();
  void SendPong(size_t payload_size, uint32_t mask);
  void ParseMessage(size_t header_size);
  int ParseMessages();
  bool HasCompleteHandshake(const unsigned char* s, size_t s_len);
  int ProcessHandshake(const unsigned char* s, size_t s_len);

  bool handshake_okay_;
  bool has_open_;
  bool has_close_;
  uint16_t _closed_code;
  bool has_error_;

  palTcpListener listener_;
  palTcpClient client_;

  palRingBlob _incoming_buffer;
  palRingBlob _outgoing_buffer;
  palAppendChopBlob _message_buffer;
  uint64_t _message_count;
  size_t _message_cursor;
};
