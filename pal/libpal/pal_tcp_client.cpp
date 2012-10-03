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

#include "libpal/pal_tcp_client.h"

palTcpClient::palTcpClient() : socket_(kpalSocketInitializer) {
  connected_ = false;
}

palTcpClient::palTcpClient(palSocket socket) : socket_(socket) {
  connected_ = true;
}

palTcpClient::~palTcpClient() {
  if (socket_ != kpalSocketInitializer)
    Close();
  connected_ = false;
}

int palTcpClient::Connect(palIpv4Address address, int port) {
  socket_ = palSocketOpenStream();
  if (socket_ <= 0)
    return (int)socket_;
  int r = palSocketConnect(socket_, address, port);
  connected_ = r == 0;
  return r;
}

int palTcpClient::Close() {
  palSocketClose(socket_);
  SetSocket(kpalSocketInitializer);
  return 0;
}

bool palTcpClient::Active() const {
  // have we ever been connected? 
  return socket_ != kpalSocketInitializer;
}

bool palTcpClient::Connected() const {
  return connected_;
}

bool palTcpClient::CanBeRead() const {
  if (connected_ == false) {
    return false;
  }
  palSocketSet readable;
  readable.Clear();
  readable.Add(socket_);
  int r = palSocketSetSelectImmediate(socket_+1, &readable, NULL, NULL);
  return r > 0;
}

void palTcpClient::MakeNonBlocking() {
  palSocketControlBlocking(socket_, false);
}

int palTcpClient::Send(const unsigned char* buff, size_t buff_size) {
  size_t bytes_to_send = buff_size;
  int r = palSocketSend(socket_, buff, &bytes_to_send);
  connected_ = r == 0;
  return r;
}

int palTcpClient::Receive(unsigned char* buff, size_t* num_bytes_to_read) {
  size_t bytes_to_read = *num_bytes_to_read;
  int r = palSocketReceive(socket_, buff, &bytes_to_read);
  if (r < 0) {
    int error = palSocketGetErrorNumber();
    if (error == PAL_SOCKET_ERROR_WOULDBLOCK) {
      r = 0;
    } else {
      connected_ = false;
      return PAL_TCP_CLIENT_DISCONNECTED;
    }
  } else if (r == 0 && bytes_to_read == 0) {
    // connection closed
    palSocketClose(socket_);
    connected_ = false;
    return PAL_TCP_CLIENT_DISCONNECTED_EOF;
  } else {
    connected_ = r == 0;
  }
  *num_bytes_to_read = bytes_to_read;
  return r;
}

void palTcpClient::SetSocket(palSocket socket) {
  socket_ = socket;
  connected_ = socket_ != kpalSocketInitializer;
}

palSocket palTcpClient::GetSocket() const {
  return socket_;
}