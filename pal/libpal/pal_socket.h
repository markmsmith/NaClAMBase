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

#include "libpal/pal_types.h"
#include "libpal/pal_errorcode.h"

#if defined(PAL_PLATFORM_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET palSocket;
#define kpalSocketInitializer INVALID_SOCKET
#else
typedef int palSocket;
#define kpalSocketInitializer -1
#endif

#define kIpv4AddressAny 0x0 // 0.0.0.0
#define kIpv4AddressLocalhost 0x7f000001 // 127.0.0.1
typedef uint32_t palIpv4Address;
typedef uint16_t palIPPort;

#define PAL_SOCKET_ERROR_WOULDBLOCK palMakeErrorCode(PAL_ERROR_CODE_SOCKET_GROUP, 1)
#define PAL_SOCKET_ERROR_CONNECTION_ABORTED palMakeErrorCode(PAL_ERROR_CODE_SOCKET_GROUP, 2)

// application wide
void palSocketInit();
void palSocketFini();
palSocket palSocketInvalidSocket();
bool palSocketIsValid(palSocket socket);
int palSocketGetErrorNumber();

int palSocketGetLocalName(palSocket socket, palIpv4Address* local_ip, palIPPort* local_port);
int palSocketGetRemoteName(palSocket socket, palIpv4Address* remote_ip, palIPPort* remote_port);

palSocket palSocketOpenStream(); // streaming TCP socket
int palSocketSend(palSocket socket, const unsigned char* buf, size_t* bytes_to_send);
int palSocketReceive(palSocket socket, char unsigned* buf, size_t* bytes_to_read);
int palSocketClose(palSocket socket);

int palSocketAvailableBytes(palSocket socket);
int palSocketControlBlocking(palSocket socket, bool blocking);

int palSocketBind(palSocket socket, palIpv4Address local_ip, palIPPort local_port);
int palSocketListen(palSocket socket, int backlog);
palSocket palSocketAccept(palSocket listener_socket);

int palSocketConnect(palSocket socket, palIpv4Address remote_ip, palIPPort remote_port);

struct palSocketSet {
  fd_set set_;

  void Clear();
  void Add(palSocket socket);
  void Remove(palSocket socket);
  int IsMember(palSocket socket) const;
};

int palSocketSetSelectImmediate(palSocket max_socket, palSocketSet* can_read, palSocketSet* can_write, palSocketSet* has_error);
int palSocketSetSelectBlock(palSocket max_socket, palSocketSet* can_read, palSocketSet* can_write, palSocketSet* has_error);