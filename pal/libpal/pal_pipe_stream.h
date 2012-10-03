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

#include "libpal/pal_file.h"
#include "libpal/pal_stream_interface.h"
#include "libpal/pal_pipe.h"

#define PAL_PIPE_STREAM_INVALID_STREAM palMakeErrorCode(PAL_ERROR_CODE_STREAM_GROUP, 200)

class palPipeStream : public palStreamInterface {
  palPipe* _pipe;
  bool _can_read;
  bool _can_write;
public:
  palPipeStream();
  virtual ~palPipeStream();

  int CreateReadStream(palPipe* pipe);
  int CreateWriteStream(palPipe* pipe);

  int Reset();

  virtual bool CanRead() const;
  virtual bool CanWrite() const;
  virtual bool CanSeek() const;

  virtual size_t GetPosition() const;

  virtual size_t GetLength() const;

  virtual void Flush();

  virtual int SetLength(size_t length);

  virtual int Seek(ssize_t offset, palStreamSeekOrigin origin);
  virtual int Read(void* buffer, size_t buffer_offset, size_t count_bytes, size_t* bytes_read);
  virtual int Write(const void* buffer, size_t buffer_offset, size_t count_bytes, size_t* bytes_written);

  virtual int OffsetRead(void* buffer, size_t buffer_offset, size_t count_bytes, size_t stream_offset, size_t* bytes_read);
  virtual int OffsetWrite(const void* buffer, size_t buffer_offset, size_t count_bytes, size_t stream_offset, size_t* bytes_written);
};