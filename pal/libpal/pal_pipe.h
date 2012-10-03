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

#pragma once

#include "libpal/pal_platform.h"
#include "libpal/pal_types.h"
#include "libpal/pal_mem_blob.h"
#include "libpal/pal_errorcode.h"

#if defined(PAL_PLATFORM_WINDOWS)
#include "libpal/windows/pal_pipe_windows.h"
#elif defined(PAL_PLATFORM_NACL)
#include "libpal/posix/pal_pipe_posix.h"
#else
#error Need to implement palPipe classes for this platform
#endif

#define PAL_PIPE_ALREADY_CREATED palMakeErrorCode(PAL_ERROR_CODE_PIPE_GROUP, 1)
#define PAL_PIPE_CANT_CLOSE_READ palMakeErrorCode(PAL_ERROR_CODE_PIPE_GROUP, 2)
#define PAL_PIPE_CANT_CLOSE_WRITE palMakeErrorCode(PAL_ERROR_CODE_PIPE_GROUP, 3)
#define PAL_PIPE_CANT_CREATE palMakeErrorCode(PAL_ERROR_CODE_PIPE_GROUP, 4)
#define PAL_PIPE_CLOSED_WRITE palMakeErrorCode(PAL_ERROR_CODE_PIPE_GROUP, 5)
#define PAL_PIPE_CLOSED_READ palMakeErrorCode(PAL_ERROR_CODE_PIPE_GROUP, 6)
#define PAL_PIPE_FAILED_READ palMakeErrorCode(PAL_ERROR_CODE_PIPE_GROUP, 7)
#define PAL_PIPE_FAILED_WRITE palMakeErrorCode(PAL_ERROR_CODE_PIPE_GROUP, 8)

class palPipe {
  palPipePlatformData _pdata;
  PAL_DISALLOW_COPY_AND_ASSIGN(palPipe);
public:
  palPipe();
  ~palPipe();

  int Create(bool inherited_by_spawned_process);

  int Write(const void* buffer, size_t bytes_to_write, size_t* bytes_written);
  int Read(void* buffer, size_t bytes_to_read, size_t* bytes_read);

  int Close();

  int CloseRead();
  int CloseWrite();

  bool IsReadOpen() const;
  bool IsWriteOpen() const;

  void FlushRead();
  void FlushWrite();

  void SetReadInheritanceFlag(bool inherit);
  void SetWriteInheritanceFlag(bool inherit);

  palPipePlatformData GetPlatformData();
};
