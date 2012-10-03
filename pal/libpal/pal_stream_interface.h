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

enum palStreamSeekOrigin {
  kPalStreamSeekOriginBegin,
  kPalStreamSeekOriginCurrent,
  kPalStreamSeekOriginEnd,
};

#define PAL_STREAM_ERROR_CANT_SEEK palMakeErrorCode(PAL_ERROR_CODE_STREAM_GROUP, 1)
#define PAL_STREAM_ERROR_CANT_READ palMakeErrorCode(PAL_ERROR_CODE_STREAM_GROUP, 2)
#define PAL_STREAM_ERROR_CANT_WRITE palMakeErrorCode(PAL_ERROR_CODE_STREAM_GROUP, 3)

class palStreamInterface {
  PAL_DISALLOW_COPY_AND_ASSIGN(palStreamInterface);
public:
  palStreamInterface() {
  };

  virtual ~palStreamInterface() {
  }

  virtual bool CanRead() const = 0;
  virtual bool CanWrite() const = 0;
  virtual bool CanSeek() const = 0;

  virtual size_t GetPosition() const {
    return 0;
  }

  virtual size_t GetLength() const {
    return 0;
  }

  virtual void Flush() = 0;

  virtual int SetLength(size_t length) = 0;

  virtual int Seek(ssize_t offset, palStreamSeekOrigin origin) = 0;
  virtual int Read(void* buffer, size_t buffer_offset, size_t count_bytes, size_t* bytes_read) = 0;
  virtual int Write(const void* buffer, size_t buffer_offset, size_t count_bytes, size_t* bytes_written) = 0;
  // Stream must be seekable
  virtual int OffsetRead(void* buffer, size_t buffer_offset, size_t count_bytes, size_t stream_offset, size_t* bytes_read) = 0;
  virtual int OffsetWrite(const void* buffer, size_t buffer_offset, size_t count_bytes, size_t stream_offset, size_t* bytes_written) = 0;
};
