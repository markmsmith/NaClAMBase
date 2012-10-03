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

#include "libpal/pal_file_stream.h"

palFileStream::palFileStream() {
  _file = NULL;
  _can_write = false;
  _can_seek = false;
  _can_read = false;
  _own_file = false;
}

palFileStream::~palFileStream() {
  if (_own_file) {
    _internal_opened_file.Close();
    _own_file = false;
  }
}

int palFileStream::Create(palFile* file) {
  _file = file;
  _own_file = false;
  palFileAccess access = file->GetAccess();
  _can_seek = true;
  if (access == kFileAccessRead) {
    _can_read = true;
    _can_write = false;
  } else if (access == kFileAccessWrite) {
    _can_read = false;
    _can_write = true;
  } else if (access == kFileAccessReadWrite) {
    _can_read = true;
    _can_write = true;
  }
  return 0;
}

int palFileStream::Create(const char* filename, palFileMode mode, palFileAccess access) {
  int r;
  r = _internal_opened_file.Open(filename, mode, access);
  if (r != 0) {
    return r;
  }
  _own_file = true;
  _file = &_internal_opened_file;
  _can_seek = true;
  if (access == kFileAccessRead) {
    _can_read = true;
    _can_write = false;
  } else if (access == kFileAccessWrite) {
    _can_read = false;
    _can_write = true;
  } else if (access == kFileAccessReadWrite) {
    _can_read = true;
    _can_write = true;
  }
  return 0;
}

int palFileStream::Reset() {
  if (_own_file) {
    _internal_opened_file.Close();
  }
  _file = NULL;
  _own_file = false;
  _can_write = false;
  _can_read = false;
  _can_seek = false;
  return 0;
}

bool palFileStream::CanRead() const {
  return _can_read;
}

bool palFileStream::CanWrite() const {
  return _can_write;
}

bool palFileStream::CanSeek() const {
  return _can_seek;
}

size_t palFileStream::GetPosition() const {
  return (size_t)_file->GetPosition();
}

size_t palFileStream::GetLength() const {
  return (size_t)_file->GetSize();
}

void palFileStream::Flush() {
  _file->Flush();
}

int palFileStream::SetLength(size_t length) {
  uint64_t new_length = _file->SetFileSize(length);
  if (length == new_length) {
    return 0;
  } else {
    return -1;
  }
}

int palFileStream::Seek(ssize_t offset, palStreamSeekOrigin origin) {
  _file->Seek((palFileSeekOrigin)origin, offset);
  return 0;
}

int palFileStream::Read(void* buffer, size_t buffer_offset, size_t count_bytes, size_t* bytes_read) {
  uintptr_t bp = (uintptr_t)buffer;
  bp += (uintptr_t)buffer_offset;
  uint64_t _bytes_read = _file->Read((void*)bp, count_bytes);
  *bytes_read = (size_t)_bytes_read;
  return 0;
}

int palFileStream::Write(const void* buffer, size_t buffer_offset, size_t count_bytes, size_t* bytes_written) {
  uintptr_t bp = (uintptr_t)buffer;
  bp += (uintptr_t)buffer_offset;
  uint64_t _bytes_written = _file->Write((const void*)bp, count_bytes);
  *bytes_written = (size_t)_bytes_written;
  return 0;
}

int palFileStream::OffsetRead(void* buffer, size_t buffer_offset, size_t count_bytes, size_t stream_offset, size_t* bytes_read) {
  uintptr_t bp = (uintptr_t)buffer;
  bp += (uintptr_t)buffer_offset;
  uint64_t _bytes_read = _file->OffsetRead(stream_offset, (void*)bp, count_bytes);
  *bytes_read = (size_t)_bytes_read;
  return 0;
}

int palFileStream::OffsetWrite(const void* buffer, size_t buffer_offset, size_t count_bytes, size_t stream_offset, size_t* bytes_written) {
  uintptr_t bp = (uintptr_t)buffer;
  bp += (uintptr_t)buffer_offset;
  uint64_t _bytes_written = _file->OffsetWrite(stream_offset, (const void*)bp, count_bytes);
  *bytes_written = (size_t)_bytes_written;
  return 0;
}
