#pragma once

#include "libpal/pal_platform.h"

#define PAL_OK 0
#define palMakeErrorCode(group, code) (0x80000000|((group & 0xff) << 16)|(code & 0xffff))

#define PAL_ERROR_CODE_STREAM_GROUP 0x01
#define PAL_ERROR_CODE_THREAD_GROUP 0x2
#define PAL_ERROR_CODE_PIPE_GROUP 0x3
#define PAL_ERROR_CODE_ALLOCATOR_GROUP 0x4
#define PAL_ERROR_CODE_BLOB_GROUP 0x5
#define PAL_ERROR_CODE_TCPCLIENT_GROUP 0x6
#define PAL_ERROR_CODE_SOCKET_GROUP 0x7
#define PAL_ERROR_CODE_ARRAY_GROUP 0x8
#define PAL_ERROR_CODE_WEBSOCKET_GROUP 0x9