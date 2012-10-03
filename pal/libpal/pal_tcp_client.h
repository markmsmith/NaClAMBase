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

#include "libpal/pal_errorcode.h"
#include "libpal/pal_socket.h"

#define PAL_TCP_CLIENT_DISCONNECTED palMakeErrorCode(PAL_ERROR_CODE_TCPCLIENT_GROUP, 1)
#define PAL_TCP_CLIENT_DISCONNECTED_EOF palMakeErrorCode(PAL_ERROR_CODE_TCPCLIENT_GROUP, 2)

class palTcpClient {
  palSocket socket_;
  bool connected_;
  PAL_DISALLOW_COPY_AND_ASSIGN(palTcpClient);
public:
  palTcpClient();
  palTcpClient(palSocket socket);
  ~palTcpClient();

  int Connect(palIpv4Address address, int port);
  int Close();

  bool Active() const; // active?
  bool Connected() const; // connected?
  bool CanBeRead() const; // can be read?

  void MakeNonBlocking();

  int Send(const unsigned char* buff, size_t buff_size);
  int Receive(unsigned char* buff, size_t* num_bytes_to_read);

  void SetSocket(palSocket socket); // sets the underlying socket
  palSocket GetSocket() const; // underlying socket
};