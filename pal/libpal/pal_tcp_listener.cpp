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

#include "libpal/pal_tcp_listener.h"

palTcpListener::palTcpListener(palIPPort port) : port_(port), socket_(kpalSocketInitializer) {
}

palTcpListener::palTcpListener() : port_(0), socket_(kpalSocketInitializer) {
}

palTcpListener::~palTcpListener() {
  if (socket_ != kpalSocketInitializer)
  {
    StopListening();
  }
}

void palTcpListener::SetPort(palIPPort port) {
  port_ = port;
}

int palTcpListener::StartListening(int pending_connection_queue_size) {
  socket_ = palSocketOpenStream();
  int r;
  
  r = palSocketBind(socket_, kIpv4AddressAny, port_);
  if (r < 0) {
    return r;
  }
  r = palSocketListen(socket_, pending_connection_queue_size);
  
  return r;
}

int palTcpListener::StopListening() {
  palSocketClose(socket_);
  socket_ = kpalSocketInitializer;
  return 0;
}

bool palTcpListener::Active() {
  return socket_ != kpalSocketInitializer;
}

bool palTcpListener::PendingConnections() {
  if (socket_ == kpalSocketInitializer)
    return false;

  palSocketSet readable;

  readable.Clear();
  readable.Add(socket_);

  int r = palSocketSetSelectImmediate(socket_+1, &readable, NULL, NULL);

  return r > 0;
}

int palTcpListener::AcceptTcpClient(palTcpClient* client) {
  palSocket client_socket = palSocketAccept(socket_);

  if (client_socket == kpalSocketInitializer) {
    return -1;
  } else {
    client->SetSocket(client_socket);
    return 0;
  }
}

palSocket palTcpListener::GetSocket() const {
  return socket_;
}

palIPPort palTcpListener::GetPort() const {
  return port_;
}