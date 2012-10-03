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

#include "libpal/pal_pipe_stream.h"

palPipeStream::palPipeStream() {
  _pipe = NULL;
  _can_read = false;
  _can_write = false;
}

palPipeStream::~palPipeStream() {
}

int palPipeStream::CreateReadStream(palPipe* pipe) {
  if (!pipe) {
    return PAL_PIPE_STREAM_INVALID_STREAM;
  }
  if (pipe->IsReadOpen() == false) {
    return PAL_PIPE_STREAM_INVALID_STREAM;
  }
  _pipe = pipe;
  _can_read = true;
  _can_write = false;
  return 0;
}

int palPipeStream::CreateWriteStream(palPipe* pipe) {
  if (!pipe) {
    return PAL_PIPE_STREAM_INVALID_STREAM;
  }
  if (pipe->IsWriteOpen() == false) {
    return PAL_PIPE_STREAM_INVALID_STREAM;
  }
  _pipe = pipe;
  _can_read = false;
  _can_write = true;
  return 0;
}

int palPipeStream::Reset() {
  _pipe = NULL;
  _can_write = false;
  _can_read = false;
  return 0;
}

bool palPipeStream::CanRead() const {
  return _can_read;
}

bool palPipeStream::CanWrite() const {
  return _can_write;
}

bool palPipeStream::CanSeek() const {
  return false;
}

size_t palPipeStream::GetPosition() const {
  return 0;
}

size_t palPipeStream::GetLength() const {
  return 0;
}

void palPipeStream::Flush() {
  if (_pipe) {
    if (_can_write) {
      _pipe->FlushWrite();
    }
    if (_can_read) {
      _pipe->FlushRead();
    }
  }
}

int palPipeStream::SetLength(size_t length) {
  return PAL_STREAM_ERROR_CANT_SEEK;
}

int palPipeStream::Seek(ssize_t offset, palStreamSeekOrigin origin) {
  return PAL_STREAM_ERROR_CANT_SEEK;
}

int palPipeStream::Read(void* buffer, size_t buffer_offset, size_t count_bytes, size_t* bytes_read) {
  uintptr_t bp = (uintptr_t)buffer;
  bp += (uintptr_t)buffer_offset;
  buffer = (void*)bp;
  if (_can_read) {
    int r;
    r = _pipe->Read(buffer, count_bytes, bytes_read);
    return r;
  } else {
    return PAL_STREAM_ERROR_CANT_READ;
  }
}

int palPipeStream::Write(const void* buffer,  size_t buffer_offset, size_t count_bytes, size_t* bytes_written) {
  uintptr_t bp = (uintptr_t)buffer;
  bp += (uintptr_t)buffer_offset;
  buffer = (void*)bp;
  if (_can_write) {
    int r;
    r = _pipe->Write(buffer, count_bytes, bytes_written);
    return r;
  } else {
    return PAL_STREAM_ERROR_CANT_WRITE;
  }
}

int palPipeStream::OffsetRead(void* buffer, size_t buffer_offset, size_t count_bytes, size_t stream_offset, size_t* bytes_read) {
  return PAL_STREAM_ERROR_CANT_SEEK;
}

int palPipeStream::OffsetWrite(const void* buffer, size_t buffer_offset, size_t count_bytes, size_t stream_offset, size_t* bytes_written) {
  return PAL_STREAM_ERROR_CANT_SEEK;
}
