
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

#include "libpal/pal_web_socket_server.h"
#include "libpal/pal_memory_stream.h"
#include "libpal/pal_algorithms.h"
#include "libpal/pal_string.h"
#include "libpal/pal_endian.h"
#include "libpal/pal_sha1.h"
#include "libpal/pal_tokenizer.h"

#define kPalWebSocketServerMessageType__MASK 0x8000000000000000
#define kPalWebSocketServerMessageType__SHIFT 63
#define kPalWebSocketServerMessageTypeText 0x0
#define kPalWebSocketServerMessageTypeBinary 0x1
#define kPalWebSocketServerMessageSize__MASK ~(kPalWebSocketServerMessageType__MASK)

#define kWS_CONTINUATION 0
#define kWS_TEXT_FRAME 1
#define kWS_BINARY_FRAME 2
#define kWS_CLOSED_OP 8
#define kWS_PING_OP 9
#define kWS_PONG_OP 10

#define kWS_MESSAGE_FRAME_START 0
#define kWS_MESSAGE_FRAME_END 2
#define kWS_CONTROL_FRAME_START 8
#define kWS_CONTROL_FRAME_END 0xF

static const char* op_code_name[16] = {
  "CONTINUATION", // 0
  "TEXT FRAME", // 1
  "BINARY FRAME", // 2
  "RESERVED 0x3", // 3
  "RESERVED 0x4", // 4
  "RESERVED 0x5", // 5
  "RESERVED 0x6", // 6
  "RESERVED 0x7", // 7
  "CONNECTION CLOSED", // 8
  "PING", // 9
  "PONG", // A
  "RESERVED 0xB", // B
  "RESERVED 0xC", // C
  "RESERVED 0xD", // D
  "RESERVED 0xE", // E
  "RESERVED 0xF", // F
};

#if 0
unsigned char test_messages[] = {
  0x89,0x05,0x48,0x65,0x6c,0x6c,0x6f, // ping with "Hello" as payload
  0x8a,0x05,0x48,0x65,0x6c,0x6c,0x6f, // pong with "Hello" as payload
  0x82, 0x7F, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, // unmaked binary message with 65536 bytes of data (data not present)
  0x82, 0x7E, 0x00, 0x01, // unmasked binary message with 256 of data (data not present)
  0x01,0x03,0x48,0x65,0x6c, // unmasked "Hel"
  0x80,0x02,0x6c,0x6f, // unmasked CONTINUATION "lo"
  0x81,0x85,0x37,0xfa,0x21,0x3d,0x7f,0x9f,0x4d,0x51,0x58, // masked "Hello"
  0x81,0x05,0x48,0x65,0x6c,0x6c,0x6f, // unmasked "Hello"
};
#endif

#define kWS_MAX_MESSAGE_HEADER_SIZE 10
struct WebSocketMessageHeader {
  union {
    struct {
      unsigned int OP_CODE : 4;
      unsigned int RSV1 : 1;
      unsigned int RSV2 : 1;
      unsigned int RSV3 : 1;
      unsigned int FIN : 1;
      unsigned int PAYLOAD : 7;
      unsigned int MASK : 1;
    } bits;
    uint16_t short_header;
  };

  bool IsFinal() const {
    return bits.FIN;
  }

  bool IsMasked() const {
    return bits.MASK == 1;
  }

  bool UsesExtendedPayload() const {
    return bits.PAYLOAD == 126;
  }

  bool UsesExtendedExtendedPayloadLength() const {
    return bits.PAYLOAD == 127;
  }

  uint8_t GetOpCode() const {
    return bits.OP_CODE;
  }

  size_t GetPayloadLength() const {
    uint16_t* chunks = (uint16_t*)&short_header;
    chunks++;
    if (UsesExtendedPayload()) {
      return *chunks;
    } else if (UsesExtendedExtendedPayloadLength()) {
      uint64_t r = (uint64_t)chunks[0] | (uint64_t)chunks[1] << 16 | (uint64_t)chunks[2] << 32 | (uint64_t)chunks[3] << 48;
      return (size_t)r;
    } else {
      return bits.PAYLOAD;
    }
    return 0;
  }

  uint16_t GetClosedCode() const {
    uint16_t* chunks = (uint16_t*)&short_header;
    chunks++;
    return *chunks;
  }

  uint32_t GetMask() const {
    uint16_t* chunks = (uint16_t*)&short_header;
    chunks++;
    uint32_t r = 0;
    if (IsMasked() == false) {
      return r;
    }
    if (UsesExtendedPayload()) {
      r = (uint32_t)chunks[1] | (uint32_t)chunks[2] << 16;
    } else if (UsesExtendedExtendedPayloadLength()) {
      r = (uint32_t)chunks[4] | (uint32_t)chunks[5] << 16;
    } else {
      r = (uint32_t)chunks[0] | (uint32_t)chunks[1] << 16;
    }
    return r;
  }

  size_t GetMessageLength() const {
    size_t r = GetPayloadLength();
    r += 2;
    if (UsesExtendedPayload()) {
      r += 2;
    }
    if (UsesExtendedExtendedPayloadLength()) {
      r += 8;
    }
    if (IsMasked()) {
      r += 4;
    }
    return r;
  }

  size_t GetPayloadOffset() const {
    size_t r = 0;
    r += 2;
    if (UsesExtendedPayload()) {
      r += 2;
    }
    if (UsesExtendedExtendedPayloadLength()) {
      r += 8;
    }
    if (IsMasked()) {
      r += 4;
    }
    return r;
  }

  size_t GetHeaderSize() const {
    return GetPayloadOffset();
  }

  template<typename T>
  T* GetPayload() {
    uintptr_t payload_addr = reinterpret_cast<uintptr_t>(this);
    payload_addr += (uintptr_t)GetPayloadOffset();
    return reinterpret_cast<T*>(payload_addr);
  }

  void Init() {
    short_header = 0;
  }

  void SetOpCode(uint8_t op_code) {
    bits.OP_CODE = op_code;
  }

  void SetFinal(bool final) {
    bits.FIN = final == true ? 1 : 0;
  }

  void SetMaskAndSize(uint32_t mask, uint64_t size) {
    uint16_t* chunks = (uint16_t*)&short_header;
    chunks++;
    if (size > 65536) {
      bits.PAYLOAD = 127;
      uint64_t* big_chunk = reinterpret_cast<uint64_t*>(chunks);
      *big_chunk = size;
    } else if (size > 125) {
      bits.PAYLOAD = 126;
      *chunks = (uint16_t)size;
    } else {
      bits.PAYLOAD = (uint8_t)size;
    }

    if (mask != 0) {
      bits.MASK = 1;
      uint16_t mask_hi = (mask >> 16) & 0xFFFF;
      uint16_t mask_lo = mask & 0xFFFF;
      if (UsesExtendedPayload()) {
        chunks[1] = mask_lo;
        chunks[2] = mask_hi;
      } else if (UsesExtendedExtendedPayloadLength()) {
        chunks[4] = mask_lo;
        chunks[5] = mask_hi;
      } else {
        chunks[0] = mask_lo;
        chunks[1] = mask_hi;
      }
    } else {
      bits.MASK = 0;
    }
  }

  void MakeNetworkOrder() {
    uint16_t* chunks = (uint16_t*)&short_header;
    chunks++;
    if (bits.PAYLOAD == 127) {
      uint64_t* big_chunk = reinterpret_cast<uint64_t*>(chunks);
      *big_chunk = palEndianByteSwap(*big_chunk);
    } else if (bits.PAYLOAD == 126) {
      *chunks = palEndianByteSwap(*chunks);
    }
    //short_header = palEndianByteSwap(short_header);
  }
};


static int FromTCPToRing(palTcpClient& client, palRingBlob* blob) {
  int r;
  size_t amount = (int)blob->GetConsecutiveWriteBlockSize();
  if (amount) {
    r = client.Receive(blob->GetWritePointer<unsigned char>(), &amount);
    if (r == 0) {
      blob->MoveWritePointer(amount);
    }
    return r;
  }
  return 0;
}

static int FromRingToTCP(palTcpClient& client, palRingBlob* blob) {
  int r;
  size_t amount = blob->GetConsecutiveReadBlockSize();
  if (amount) {
    r = client.Send(blob->GetReadPointer<const unsigned char>(), (int)amount);
    if (r != 0) {
      return r;
    }
    blob->Skip(amount);
    amount = blob->GetConsecutiveReadBlockSize();
    if (amount) {
      client.Send(blob->GetReadPointer<const unsigned char>(), (int)amount);
      if (r != 0) {
        return r;
      }
      blob->Skip(amount);
    }
  }
  return 0;
}

palWebSocketServer::palWebSocketServer() {
  SetIncomingBuffer(NULL, 0);
  SetOutgoingBuffer(NULL, 0);
  handshake_okay_ = false;
}

void palWebSocketServer::SetIncomingBuffer(unsigned char* incoming_buffer, size_t buffer_capacity) {
  _incoming_buffer = palRingBlob(incoming_buffer, buffer_capacity);
}

void palWebSocketServer::SetOutgoingBuffer(unsigned char* outgoing_buffer, size_t buffer_capacity) {
  _outgoing_buffer = palRingBlob(outgoing_buffer, buffer_capacity);
}

void palWebSocketServer::SetMessageBuffer(unsigned char* msg_buffer, size_t buffer_capacity) {
  _message_buffer = palAppendChopBlob(msg_buffer, buffer_capacity);
  _message_count = 0;
}

// total capacity of buffers
uint64_t palWebSocketServer::GetIncomingBufferCapacity() {
  return _incoming_buffer.GetCapacity();
}

uint64_t palWebSocketServer::GetOutgoingBufferCapacity() {
  return _outgoing_buffer.GetCapacity();
}

// amount currently used
uint64_t palWebSocketServer::GetIncomingBufferSize() {
  return _incoming_buffer.GetSize();
}

uint64_t palWebSocketServer::GetOutgoingBufferSize() {
  return _outgoing_buffer.GetSize();
}

int palWebSocketServer::AcceptConnection(palTcpListener* listener) {
  if (listener == NULL) {
    return PAL_WEBSOCKET_SERVER_NO_LISTENER;
  }
  if (client_.Connected()) {
    return PAL_WEBSOCKET_SERVER_ALREADY_CONNECTED;
  }
  // Paranoia
  CloseConnection();
  int r;
  r = listener->AcceptTcpClient(&client_);
  if (r != 0) {
    return r;
  }
  return 0;
}

int palWebSocketServer::CloseConnection() {
  client_.Close();
  _incoming_buffer.Clear();
  _outgoing_buffer.Clear();
  _message_buffer.Clear();
  _message_count = 0;
  handshake_okay_ = false;
  return 0;
}

bool palWebSocketServer::HasOpen() const {
  return has_open_;
}
bool palWebSocketServer::HasClose() const {
  return has_close_;
}
bool palWebSocketServer::HasError() const {
  return has_error_;
}

bool palWebSocketServer::Connected() const {
  return client_.Connected() && handshake_okay_;
}

// 1) accept/disconnect a connection
// 2) send any pending messages
// 3) receive any pending messages
void palWebSocketServer::Update() {
#define NETWORK_BUFFER_SIZE 512
  unsigned char network_buffer[NETWORK_BUFFER_SIZE];
  int r;

  // clear flags
  has_open_ = false;
  has_close_ = false;
  _closed_code = 0;
  has_error_ = false;

  if (client_.Connected() == false) {
    if (handshake_okay_ == true) {
      // We had been connected
      has_close_ = true;
      _closed_code = 0;
      handshake_okay_ = false;
      ClientDisconnect();
    }
  }

  if (client_.Connected() == true && handshake_okay_ == false) {
    // still need to handle handshake    
    if (client_.CanBeRead()) {
      if (_incoming_buffer.IsFull()) {
        // we have filled our entire buffer without finding a valid handshake
        // kick this client off
        has_error_ = true;
        client_.Close();
        _incoming_buffer.Clear();
        return;
      }
    } else if (_incoming_buffer.GetSize() > 0) {
      // if the 
      // if we can't read from the socket and we've read part of a handshake, quit.
      has_error_ = true;
      client_.Close();
      _incoming_buffer.Clear();
      return;
    }

    size_t bytes_to_read = NETWORK_BUFFER_SIZE;

    r = client_.Receive(&network_buffer[0], &bytes_to_read);
    if (bytes_to_read > 0) {
      _incoming_buffer.Append(&network_buffer[0], (size_t)bytes_to_read);
    } else {
      return;
    }

    bytes_to_read = NETWORK_BUFFER_SIZE;
    if (_incoming_buffer.GetSize() < NETWORK_BUFFER_SIZE) {
      bytes_to_read = (int)_incoming_buffer.GetSize();
    }

    _incoming_buffer.Peek(&network_buffer[0], bytes_to_read);

    if (HasCompleteHandshake(&network_buffer[0], bytes_to_read)) {
      r = ProcessHandshake(&network_buffer[0], bytes_to_read);
      if (r == 0) {
        // Got a connection.
        handshake_okay_ = true;
        has_open_ = true;
        _incoming_buffer.Clear();
        _outgoing_buffer.Clear();
        _message_count = 0;
        _message_buffer.Clear();
      } else {
        has_error_ = true;
        client_.Close();
        _incoming_buffer.Clear();
      }
    }
  }
  
  if (client_.Connected() == true && handshake_okay_ == true) {
    // have an active connection
    if (_outgoing_buffer.GetSize() > 0) {
      r = FromRingToTCP(client_, &_outgoing_buffer);
      if (r != 0) {
        ClientDisconnect();
        return;
      }
    }

    if (client_.CanBeRead()) {
      r = FromTCPToRing(client_, &_incoming_buffer);
      if (r != 0) {
        ClientDisconnect();
        return;
      }
    }

    if (_incoming_buffer.GetSize() > 0) {
      ParseMessages();
    }
  }
}

int palWebSocketServer::SendPing() {
  uint16_t header_temp[kWS_MAX_MESSAGE_HEADER_SIZE];
  WebSocketMessageHeader* header = (WebSocketMessageHeader*)&header_temp[0];
  header->Init();
  header->SetOpCode(kWS_PING_OP);
  header->SetFinal(true);
  header->SetMaskAndSize(0, 0);
  size_t header_size = header->GetHeaderSize();
  if (_outgoing_buffer.GetAvailableSize() < header_size) {
    return -1;
  }
  _outgoing_buffer.Append((void*)header, header_size);
  return 0;
}

int palWebSocketServer::SendTextMessage(const char* msg) {
  size_t len = palStringLength(msg);
  return SendTextMessage(msg, len);
}

int palWebSocketServer::SendTextMessage(const char* msg, size_t msg_length) {
  uint16_t header_temp[kWS_MAX_MESSAGE_HEADER_SIZE];
  WebSocketMessageHeader* header = (WebSocketMessageHeader*)&header_temp[0];
  header->Init();
  header->SetFinal(true);
  header->SetOpCode(kWS_TEXT_FRAME);
  header->SetMaskAndSize(0, msg_length);
  uint64_t packet_length = header->GetMessageLength();
  size_t header_size = header->GetHeaderSize();
  header->MakeNetworkOrder();
  if (packet_length <= _outgoing_buffer.GetAvailableSize()) {
    _outgoing_buffer.Append((void*)header, header_size);
    _outgoing_buffer.Append((void*)msg, msg_length);
  }
#if 0
  if (2 + msg_length + outgoing_buffer_cursor_ >= outgoing_buffer_length_) {
    return -1;
  }
  // copy message into outgoing buffer
  outgoing_buffer_[outgoing_buffer_cursor_] = 0x00;
  outgoing_buffer_cursor_++;
  palMemoryCopyBytes(&outgoing_buffer_[outgoing_buffer_cursor_], msg, msg_length);
  outgoing_buffer_cursor_ += msg_length;
  outgoing_buffer_[outgoing_buffer_cursor_] = 0xFF;
  outgoing_buffer_cursor_++;  
#endif
  return 0;
}

uint64_t palWebSocketServer::PendingMessageCount() const {
  return _message_count;
}

void palWebSocketServer::ProcessMessages(OnMessageDelegate del, void* userdata) {
  if (del.empty()) {
    return;
  }
  uint64_t num_messages = _message_count;
  size_t cursor = 0;
  while (num_messages != 0) {
    uint64_t* header = _message_buffer.GetPtr<uint64_t>(cursor);
    cursor += sizeof(uint64_t);

    size_t size = (size_t)(*header & kPalWebSocketServerMessageSize__MASK);
    bool binary_message = ((*header & (kPalWebSocketServerMessageSize__MASK << kPalWebSocketServerMessageType__SHIFT)) >> kPalWebSocketServerMessageType__SHIFT) == kPalWebSocketServerMessageTypeBinary;
    palMemBlob msg(_message_buffer.GetPtr(cursor), size);

    del(msg, binary_message, userdata);

    num_messages--;
    cursor += size;
  }
}

void palWebSocketServer::ClearMessages() {
  _message_buffer.Chop(_message_cursor);
  _message_count = 0;
  _message_cursor = 0;
}

void palWebSocketServer::ClientDisconnect() {
  client_.Close();
  ResetBuffers();
  has_open_ = false;
  handshake_okay_ = false;
  has_close_ = true;
}

void palWebSocketServer::ResetBuffers() {
  _incoming_buffer.Clear();
  _outgoing_buffer.Clear();
  _message_buffer.Clear();
  _message_count = 0;
}

static void UnmaskPayload(uint8_t* payload, uint64_t payload_length, uint32_t mask) {
  uint8_t* mask_octets = reinterpret_cast<uint8_t*>(&mask);
  for (uint64_t i = 0; i < payload_length; i++) {
    uint8_t j = i & 0x3;
    payload[i] = payload[i] ^ mask_octets[j];
  }
}

void palWebSocketServer::SendPong(size_t payload_size, uint32_t mask) {
  const uint64_t buff_size = 256;
  int r = 0;
  unsigned char buff[256];
  uint16_t header_temp[kWS_MAX_MESSAGE_HEADER_SIZE];
  WebSocketMessageHeader* header = (WebSocketMessageHeader*)&header_temp[0];
  header->SetOpCode(kWS_PONG_OP);
  header->SetFinal(true);
  header->SetMaskAndSize(mask, payload_size);
  _outgoing_buffer.Append((void*)header, header->GetHeaderSize());
  while (payload_size >= buff_size) {
    r = _incoming_buffer.Consume((void*)&buff[0], buff_size);
    palAssert(r == 0);
    _outgoing_buffer.Append((void*)&buff[0], buff_size);
    payload_size -= buff_size;
  }
  if (payload_size > 0) {
    r = _incoming_buffer.Consume((void*)&buff[0], payload_size);
    palAssert(r == 0);
    _outgoing_buffer.Append((void*)&buff[0], payload_size);
  }
}

static void palMemoryCopyBytes(palAppendChopBlob* dest, palRingBlob* src, size_t bytes) {
  const size_t buff_size = 256;
  uint8_t buff[256];
  int r;
  while (bytes >= buff_size) {
    r = src->Consume((void*)&buff[0], buff_size);
    palAssert(r == 0);
    dest->Append((void*)&buff[0], buff_size);
    bytes -= buff_size;
  }
  if (bytes > 0) {
    r = src->Consume((void*)&buff[0], bytes);
//     uint32_t z = (uint32_t)bytes;
//     palPrintf("CP: %.*s\n", z, (const char*)&buff[0]);
    palAssert(r == 0);
    dest->Append((void*)&buff[0], bytes);
  }
}

void palWebSocketServer::ParseMessage(size_t header_size) {
  uint16_t header_temp[kWS_MAX_MESSAGE_HEADER_SIZE];
  WebSocketMessageHeader* header = (WebSocketMessageHeader*)&header_temp[0];
  _incoming_buffer.Consume<uint16_t>(&header_temp[0], (int)header_size/sizeof(header_temp[0]));
  size_t payload_len = header->GetPayloadLength();
  uint8_t opcode = header->GetOpCode();
  uint64_t zz = 0;
  if (opcode >= kWS_CONTROL_FRAME_START && opcode <= kWS_CONTROL_FRAME_END) {
    // Control frame
    if (opcode == kWS_PING_OP) {
      // With a pong, we don't unmask the data, just send the ping payload with the same mask.
      SendPong(payload_len, header->GetMask());
    } else {
      // A control frame we don't care about.
      // Just skip the payload
      _incoming_buffer.Skip(payload_len);
      if (opcode == kWS_PONG_OP) {
        palPrintf("Pong.\n");
      }
    }
  } else {
    // Message frame
    const bool final = header->IsFinal(); // Final fragment of message?
    // Unmask the data
    UnmaskPayload(_incoming_buffer.GetReadPointer<uint8_t>(), payload_len, header->GetMask());
    
    if (opcode == kWS_CONTINUATION) {
      uint64_t* header = _message_buffer.GetPtr<uint64_t>(_message_cursor);
      size_t old_size = (size_t)(*header & kPalWebSocketServerMessageSize__MASK);
      // Zero out the size contained in the header
      *header &= ~(kPalWebSocketServerMessageSize__MASK);
      // Update the size of the message
      old_size += payload_len;
      *header |= old_size;
      // Copy the payload into the message buffer
      palMemoryCopyBytes(&_message_buffer, &_incoming_buffer, payload_len);
      zz += payload_len;
    } else {
      uint64_t message_header = payload_len;
      // Start of a new message
      if (opcode == kWS_TEXT_FRAME) {
        message_header |= ((uint64_t)kPalWebSocketServerMessageTypeText << kPalWebSocketServerMessageType__SHIFT);
      } else if (opcode == kWS_BINARY_FRAME) {
        message_header |= ((uint64_t)kPalWebSocketServerMessageTypeBinary << kPalWebSocketServerMessageType__SHIFT);
      }
      // Remember where the message started
      _message_cursor = _message_buffer.GetSize();
      // Append the header
      _message_buffer.Append(&message_header);
      // Copy the payload into the message buffer
      palMemoryCopyBytes(&_message_buffer, &_incoming_buffer, payload_len);
      zz += payload_len;
    }
    if (final) {
      // We added another message
      _message_count++;
      _message_cursor = _message_buffer.GetSize();
    }
  }
}

int palWebSocketServer::ParseMessages() {
  uint16_t header_temp[kWS_MAX_MESSAGE_HEADER_SIZE];
  WebSocketMessageHeader* header = NULL;
  int r;

  while (_incoming_buffer.GetSize() > 0) {
    // We peek the first 16-bits so we can determine the header size
    r = _incoming_buffer.Peek(&header_temp[0]);
    if (r != 0) {
      // Not enough data
      break;
    }
    header = (WebSocketMessageHeader*)&header_temp[0];
    size_t header_size = header->GetHeaderSize();
    if (_incoming_buffer.GetSize() < header_size) {
      // Not enough data to parse this message
      break;
    }
    if (header->IsMasked() == false) {
      has_error_ = true;
      has_close_ = true;
      ClientDisconnect();
      break;
    }
    // We peek the header size so we can determine the packet size
    r = _incoming_buffer.PeekN<uint16_t>(&header_temp[0], (int)header_size/sizeof(header_temp[0]));
    if (r != 0) {
      // Not enough data
      break;
    }
    uint64_t packet_len = header->GetMessageLength();
    if (_incoming_buffer.GetSize() < packet_len) {
      // Not enough data
      break;
    }
    uint64_t payload_len = header->GetPayloadLength();

    if (header->GetOpCode() >= kWS_MESSAGE_FRAME_START && header->GetOpCode() <= kWS_MESSAGE_FRAME_END) {
      if (_message_buffer.GetAvailableSize() <= payload_len) {
        // Not enough room in outgoing message buffer for this message.
        break;
      }
    }
    if (header->GetOpCode() == kWS_CLOSED_OP) {
      // Connection closed
      if (header->GetHeaderSize() >= 4) {
        _closed_code = header->GetClosedCode();
        palPrintf("WebSocket connection gracefully closed with code: %hd\n", _closed_code);
      } else {
        palPrintf("WebSocket connection gracefully closed.\n");
      }
      has_close_ = true;
      ClientDisconnect();
      break;
    } else {
      ParseMessage(header_size);
    }    
  }
  return 0;
}

static int ParseKey(palTokenizer* tokenizer, unsigned char* key) {  
  palDynamicString key_as_string;

  bool r = tokenizer->ReadRestOfLineAsString(&key_as_string);
  if (r == false) {
    return -1;
  }

  //key_as_string.Set("dGhlIHNhbXBsZSBub25jZQ==");
  key_as_string.Append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

  key_as_string.TrimWhiteSpaceFromStart();

  //palPrintf("Key: **%s**\n", key_as_string.C());

  {
    palSHA1 sha1;
    sha1.Reset();
    sha1.Update((unsigned char*)key_as_string.C(), key_as_string.GetLength());
    sha1.Finalize();
    sha1.GetSHA1(key);
  }

//   palPrintf("Dumping SHA-1: ");
//   for (int i = 0; i < 20; i++) {
//     palPrintf("%02x ", key[i]);
//   }
//   palPrintf("\n");

  return 0;
}

#define kKW_GET 1
#define kKW_HTTP_1_1 2
#define kKW_UpgradeColon 3
#define kKW_WebSocket 4
#define kKW_ConnectionColon 5
#define kKW_Upgrade 6
#define kKW_HostColon 7
#define kKW_OriginColon 8
#define kKW_key1 10
#define kKW_key2 11
#define kKW_key 12
#define kKW_WebSocketOriginColon 13
#define kKW_WebSocketProtocolColon 14
#define kKW_WebSocketVersionColon 15

static palTokenizerKeyword handshake_keywords[] = {
  {"GET", kKW_GET},
  {"HTTP/1.1", kKW_HTTP_1_1},
  {"Upgrade:", kKW_UpgradeColon},
  {"WebSocket", kKW_WebSocket},
  {"Connection:", kKW_ConnectionColon},
  {"Upgrade", kKW_Upgrade},
  {"Host:", kKW_HostColon},
  {"Origin:", kKW_OriginColon},
  {"Sec-WebSocket-Key1:", kKW_key1},
  {"Sec-WebSocket-Key2:", kKW_key2},
  {"Sec-WebSocket-Key:", kKW_key},
  {"Sec-WebSocket-Origin:", kKW_WebSocketOriginColon },
  {"Sec-WebSocket-Protocol:", kKW_WebSocketProtocolColon },
  {"Sec-WebSocket-Version:", kKW_WebSocketVersionColon },
  { NULL, -1 }
};

bool palWebSocketServer::HasCompleteHandshake(const unsigned char* s, size_t s_len) {
  // super simple test for a complete handshake
  const char* first = (const char*)s;
  //palPrintf("Checking handshake: %s\n", s);
  if (s_len < 12) {
    return false;
  }

  if (*first != 'G') {
    return false;
  }
  first++;
  if (*first != 'E') {
    return false;
  }
  first++;
  if (*first != 'T') {
    return false;
  }

  const char* final_bytes = (const char*)&s[s_len-4];
  // two cr+lf pairs
  if (*final_bytes != '\r') {
    return false;
  }
  final_bytes++;
  if (*final_bytes != '\n') {
    return false;
  }
  final_bytes++;
  if (*final_bytes != '\r') {
    return false;
  }
  final_bytes++;
  if (*final_bytes != '\n') {
    return false;
  }
  return true;
}

static const char Base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char Pad64 = '=';

int base64_encode(const char* data, size_t input_length, char* target, size_t* output_length) {
  size_t datalength = 0;
  u_char input[3];
  u_char output[4];
  size_t i;
  size_t srclength = input_length;
  palAssert(input_length == 20);
  *output_length = 28;
  const char* src = data;

  while (2 < srclength) {
    input[0] = *src++;
    input[1] = *src++;
    input[2] = *src++;
    srclength -= 3;

    output[0] = input[0] >> 2;
    output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
    output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
    output[3] = input[2] & 0x3f;
    palAssert(output[0] < 64);
    palAssert(output[1] < 64);
    palAssert(output[2] < 64);
    palAssert(output[3] < 64);

    if (datalength + 4 > *output_length)
      return (-1);

    target[datalength++] = Base64[output[0]];
    target[datalength++] = Base64[output[1]];
    target[datalength++] = Base64[output[2]];
    target[datalength++] = Base64[output[3]];
  }

  if (0 != srclength) {
    input[0] = input[1] = input[2] = '\0';
    for (i = 0; i < srclength; i++)
      input[i] = *src++;

    output[0] = input[0] >> 2;
    output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
    output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
    palAssert(output[0] < 64);
    palAssert(output[1] < 64);
    palAssert(output[2] < 64);

    if (datalength + 4 > *output_length)
      return (-1);
    target[datalength++] = Base64[output[0]];
    target[datalength++] = Base64[output[1]];
    if (srclength == 1)
      target[datalength++] = Pad64;
    else
      target[datalength++] = Base64[output[2]];
    target[datalength++] = Pad64;
  }
  return 0;
}


int palWebSocketServer::ProcessHandshake(const unsigned char* s, size_t s_len) {
  palMemBlob blob((void*)s, (size_t)s_len);
  palMemoryStream ms;
  int res;

  //palPrintf("Process Handshake %s\n", s);

  res = ms.Create(blob, false);
  if (res != 0) {
    return -2;
  }
  palTokenizer tokenizer;
  if (s_len < 0xae) {
    // not likely
    return -1;
  }
  res = tokenizer.UseStream(&ms);
  if (res != 0) {
    return -3;
  }
  tokenizer.SetKeywordArray(&handshake_keywords[0]);
  // don't detect punctuation or numbers
  tokenizer.SetNumberParsing(false);
  tokenizer.SetPunctuationParsing(false);
  tokenizer.SetNameSplitCharacters(" \n\r\t");
  palToken token;

  bool r;

  // parsing GET /resource HTTP/1.1
  r = tokenizer.FetchNextToken(&token);
  if (!r || token.type_flags != kKW_GET) {
    return -2;
  }
  r = tokenizer.FetchNextToken(&token);
  if (!r || token.type != kTokenName) {
    return -3;
  }
  palDynamicString resource_name(token.value_string);
  tokenizer.SkipRestOfLine();

  palDynamicString host_name;
  palDynamicString origin_name;
  unsigned char key[20];
  bool key_processed = false;
  do {
    r = tokenizer.FetchNextToken(&token);
    if (!r || token.type != kTokenKeyword) {
      return -4;
    }
    switch (token.type_flags) {
      case kKW_UpgradeColon:
        tokenizer.SkipRestOfLine();
      break;
      case kKW_ConnectionColon:
        tokenizer.SkipRestOfLine();
      break;
      case kKW_HostColon:
        tokenizer.FetchNextToken(&token);
        host_name = token.value_string;
        tokenizer.SkipRestOfLine();
      break;
      case kKW_OriginColon:
        tokenizer.FetchNextToken(&token);
        origin_name = token.value_string;
        tokenizer.SkipRestOfLine();
      break;
      case kKW_key:
        ParseKey(&tokenizer, &key[0]);
        key_processed = true;
      break;
      case kKW_WebSocketOriginColon:
        tokenizer.SkipRestOfLine();
      break;
      case kKW_WebSocketProtocolColon:
        tokenizer.SkipRestOfLine();
      break;
      case kKW_WebSocketVersionColon:
        tokenizer.SkipRestOfLine();
      break;
    }
  } while (key_processed == false);
  

  char accept_key[29];
  size_t accept_key_length;
  base64_encode((const char*)&key[0], 20, &accept_key[0], &accept_key_length);
  accept_key[28] = '\0';
  palDynamicString server_response;
  server_response.Append("HTTP/1.1 101 Switching Protocols\r\n");
  server_response.Append("Upgrade: websocket\r\n");
  server_response.Append("Connection: Upgrade\r\n");
  server_response.AppendPrintf("Sec-WebSocket-Accept: %s\r\n", &accept_key[0]);
  server_response.Append("\r\n");
  //server_response.Append("Sec-WebSocket-Protocol: \r\n");
#if 0
  server_response.Append("Sec-WebSocket-Location: ws://");
  server_response.Append(host_name.C());
  server_response.Append(resource_name.C());
  server_response.Append("\r\n");

  server_response.Append("Sec-WebSocket-Origin: ");
  server_response.Append(origin_name.C());
#endif
  
  
  //palPrintf("Response:\n%s\n", server_response.C());
  client_.Send((unsigned char*)server_response.C(), server_response.GetLength());
  return 0;
}
