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

#include "libpal/pal_memory_stream.h"

palMemoryStream::palMemoryStream() : _fixed_blob(NULL, 0), _growing_blob(NULL) {
  _own_growing_blob = false;
  _growable = false;
  _can_read = false;
  _can_seek = false;
  _can_write = false;
}

palMemoryStream::~palMemoryStream() {
  Reset();
}

int palMemoryStream::Create(const palMemBlob& blob, bool can_write) {
  _fixed_blob = blob;
  _can_read = true;
  _can_seek = true;
  _can_write = can_write;
  _position = 0;
  return 0;
}

int palMemoryStream::Create(palAllocatorInterface* allocator, size_t capacity) {
  _growable = true;
  _own_growing_blob = true;
  _growing_blob = palGrowingMemoryBlob(allocator);
  _growing_blob.IncreaseCapacity(capacity);
  _position = 0;
  _can_read = true;
  _can_seek = true;
  _can_write = true;
  return 0;
}

int palMemoryStream::Reset() {
  _fixed_blob = palMemBlob(NULL, 0);
  if (_growable && _own_growing_blob) {
    _growing_blob.Reset(true);
  }
  _own_growing_blob = false;
  _growable = false;
  _can_read = false;
  _can_seek = false;
  _can_write = false;
  return 0;
}

void palMemoryStream::GetBlob(palMemBlob* blob) {
  if (_growable) {
    _growing_blob.GetBlob(blob);
  } else {
    *blob = _fixed_blob;
  }
}

void palMemoryStream::StealBlob(palMemBlob* blob) {
  if (_growable) {
    _growing_blob.GetBlob(blob);
    _growing_blob.Reset(false);
  } else {
    *blob = _fixed_blob;
  }
  Reset();
}

bool palMemoryStream::CanRead() const {
  return _can_read;
}

bool palMemoryStream::CanWrite() const {
  return _can_write;
}

bool palMemoryStream::CanSeek() const {
  return _can_seek;
}

size_t palMemoryStream::GetPosition() const {
  return _position;
}

size_t palMemoryStream::GetLength() const {
  if (_growable) {
    return _growing_blob.GetBufferSize();
  } else {
    return _fixed_blob.GetBufferSize();
  }
  return 0;
}

void palMemoryStream::Flush() {
  return;
}

int palMemoryStream::SetLength(size_t length) {
  if (_growable) {
    _growing_blob.SetSize(length);
    return 0;
  }
  return -1;
}

int palMemoryStream::Seek(ssize_t offset, palStreamSeekOrigin origin) {
  switch (origin) {
  case kPalStreamSeekOriginBegin:
    _position = offset;
  break;
  case kPalStreamSeekOriginEnd:
    _position = GetLength()-offset;
  break;
  case kPalStreamSeekOriginCurrent:
    _position += offset;
  break;
  }
  if (_position > GetLength()) {
    _position = GetLength();
  }
  return 0;
}

int palMemoryStream::Read(void* buffer, size_t buffer_offset, size_t count_bytes, size_t* bytes_read) {
  uintptr_t bp = (uintptr_t)buffer;
  bp += (uintptr_t)buffer_offset;
  if (_growable) {
    palMemoryCopyBytes((void*)bp, _growing_blob.GetPtr(_position), count_bytes);
    *bytes_read = count_bytes;
  } else {
    palMemoryCopyBytes((void*)bp, _fixed_blob.GetPtr(_position), count_bytes);
    *bytes_read = count_bytes;
  }
  _position += count_bytes;
  return 0;
}

int palMemoryStream::Write(const void* buffer, size_t buffer_offset, size_t count_bytes, size_t* bytes_written) {
  if (_can_write == false) {
    return PAL_STREAM_ERROR_CANT_WRITE;
  }
  uintptr_t bp = (uintptr_t)buffer;
  bp += (uintptr_t)buffer_offset;
  if (_growable) {
    palMemoryCopyBytes(_growing_blob.GetPtr(_position), (void*)bp, count_bytes);
    *bytes_written = count_bytes;
  } else {
    palMemoryCopyBytes(_fixed_blob.GetPtr(_position), (void*)bp, count_bytes);
    *bytes_written = count_bytes;
  }
  _position += count_bytes;
  return 0;
}

int palMemoryStream::OffsetRead(void* buffer, size_t buffer_offset, size_t count_bytes, size_t stream_offset, size_t* bytes_read) {
  uintptr_t bp = (uintptr_t)buffer;
  bp += (uintptr_t)buffer_offset;
  if (_growable) {
    palMemoryCopyBytes((void*)bp, _growing_blob.GetPtr(stream_offset), count_bytes);
    *bytes_read = count_bytes;
  } else {
    palMemoryCopyBytes((void*)bp, _fixed_blob.GetPtr(stream_offset), count_bytes);
    *bytes_read = count_bytes;
  }
  return 0;
}

int palMemoryStream::OffsetWrite(const void* buffer, size_t buffer_offset, size_t count_bytes, size_t stream_offset, size_t* bytes_written) {
  if (_can_write == false) {
    return PAL_STREAM_ERROR_CANT_WRITE;
  }
  uintptr_t bp = (uintptr_t)buffer;
  bp += (uintptr_t)buffer_offset;
  if (_growable) {
    palMemoryCopyBytes(_growing_blob.GetPtr(stream_offset), (void*)bp, count_bytes);
    *bytes_written = count_bytes;
  } else {
    palMemoryCopyBytes(_fixed_blob.GetPtr(stream_offset), (void*)bp, count_bytes);
    *bytes_written = count_bytes;
  }
  return 0;
}
