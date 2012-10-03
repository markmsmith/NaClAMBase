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

#include "libpal/pal_platform.h"
#include "libpal/pal_socket.h"
#include "libpal/pal_tcp_client.h"

#define kTcpListenerOk 0
#define kTCPListenerError -1

class palTcpListener {
  palSocket socket_;
  palIPPort port_;
  PAL_DISALLOW_COPY_AND_ASSIGN(palTcpListener);
public:
  palTcpListener(palIPPort port);
  palTcpListener();

  ~palTcpListener();

  void SetPort(palIPPort port);
  int StartListening(int pending_connection_queue_size = 1);
  int StopListening();

  bool Active(); // is this listener active?
  bool PendingConnections(); // any pending connections?

  int AcceptTcpClient(palTcpClient* client);

  palSocket GetSocket() const;  // get raw socket
  palIPPort GetPort() const; // get port
};