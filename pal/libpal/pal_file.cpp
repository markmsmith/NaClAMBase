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
#include "libpal/pal_debug.h"
#include "libpal/pal_memory.h"
#include "libpal/pal_string.h"
#include "libpal/pal_mem_blob.h"
#include "libpal/pal_file.h"
#include "libpal/pal_allocator.h"

palFile::palFile() {
#if defined(PAL_PLATFORM_WINDOWS)
  _pdata._handle = INVALID_HANDLE_VALUE;
#endif
  _mode = NUM_palFileModes;
  _access = NUM_palFileAccess;
}

palFile::~palFile() {
  Close();
}

int palFile::Open(const char* filename, palFileMode mode, palFileAccess access) {
#if defined(PAL_PLATFORM_WINDOWS)
  const uint32_t access_array[NUM_palFileAccess] = { GENERIC_READ, GENERIC_WRITE, GENERIC_READ|GENERIC_WRITE };
  const uint32_t mode_array[NUM_palFileModes] = { CREATE_NEW, CREATE_ALWAYS, OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING, OPEN_ALWAYS };
  HANDLE fileh = CreateFile(filename, access_array[access], 0, NULL, mode_array[mode], FILE_ATTRIBUTE_NORMAL, NULL);
  if (fileh == INVALID_HANDLE_VALUE) {
    // need to expand error checking here
    return PAL_FILE_ERROR_OPENNING;
  }
  _mode = mode;
  _access = access;
  _pdata._handle = fileh;
  return 0;
#endif
  return -1;
}

int palFile::OpenForReading(const char* filename) {
  return Open(filename, kFileModeOpen, kFileAccessRead);
}

int palFile::OpenForWritingTruncate(const char* filename) {
  return Open(filename, kFileModeCreate, kFileAccessWrite);
}

void palFile::Close() {
#if defined(PAL_PLATFORM_WINDOWS)
  if (_pdata._handle != INVALID_HANDLE_VALUE) {
    CloseHandle(_pdata._handle);
    _pdata._handle = INVALID_HANDLE_VALUE;
    _mode = NUM_palFileModes;
    _access = NUM_palFileAccess;
  }
#endif
}

bool palFile::IsOpen() {
#if defined(PAL_PLATFORM_WINDOWS)
  return _pdata._handle != INVALID_HANDLE_VALUE;
#endif
}

palFileMode palFile::GetMode() {
  return _mode;
}

palFileAccess palFile::GetAccess() {
  return _access;
}

void palFile::CopyContentsAsString(palMemBlob* blob) {
  unsigned char *buf = NULL;
  buf = (unsigned char *)g_FileProxyAllocator->Allocate((size_t)GetSize()+1);
  if (!buf)
    return;
  uint64_t read_size = Read(buf, (size_t)GetSize());
  palAssert(read_size == GetSize());
  buf[GetSize()] = '\0';
  *blob = palMemBlob(buf, (size_t)GetSize()+1);
}

void palFile::CopyContents(palMemBlob* blob) {
  unsigned char *buf = NULL;
  buf = (unsigned char *)g_FileProxyAllocator->Allocate((size_t)GetSize());
  if (!buf)
    return;
  uint64_t read_size = Read(buf, (size_t)GetSize());
  palAssert(read_size == GetSize());
  *blob = palMemBlob(buf, (size_t)GetSize());
}

uint64_t palFile::Seek(palFileSeekOrigin p, int64_t offset ) {
#if defined(PAL_PLATFORM_WINDOWS)
  const uint32_t method_array[kFileSeekOriginNUM] = { FILE_BEGIN, FILE_CURRENT, FILE_END };
  uint32_t method = method_array[p];

  BOOL r;
  uint64_t result;

  LARGE_INTEGER i, o;
  i.QuadPart = offset;
  r = SetFilePointerEx(_pdata._handle, i, &o, method);
  result = o.QuadPart;
  return result;
#endif
  return -1;
}

uint64_t palFile::Read(void* buffer, size_t num_bytes) {
#if defined(PAL_PLATFORM_WINDOWS)
  BOOL r;
  uint32_t bytes_read;
  r = ReadFile(_pdata._handle, buffer, (DWORD)num_bytes, (LPDWORD)&bytes_read, NULL);
  return bytes_read;
#endif
  return -1;
}

uint64_t palFile::Write(const void* buffer, size_t num_bytes) {
#if defined(PAL_PLATFORM_WINDOWS)
  BOOL r;
  uint32_t bytes_written;

  r = WriteFile(_pdata._handle, buffer, (DWORD)num_bytes, (LPDWORD)&bytes_written, NULL);

  return bytes_written;
#endif
  return -1;
}

uint64_t palFile::Write(const char* str) {
  return Write(str, palStringLength(str));
}

uint64_t palFile::WritePrintf(const char* str, ...) {
  va_list argptr;
  va_start(argptr, str);
  char* to_write = palStringAllocatingPrintfInternal(str, argptr);
  va_end(argptr);
  uint64_t r = Write(to_write);
  palStringAllocatingPrintfInternalDeallocate(to_write);
  return r;
}

uint64_t palFile::OffsetRead(uint64_t offset, void* buffer, size_t num_bytes) {
#if defined(PAL_PLATFORM_WINDOWS)
  BOOL r;
  uint32_t bytes_read_;
  OVERLAPPED ol;
  ol.Offset = offset & 0xFFFFFFFF;
  ol.OffsetHigh = offset >> 32;
  r = ReadFile(_pdata._handle, buffer, (DWORD)num_bytes, (LPDWORD)&bytes_read_, &ol);
  uint64_t new_offset = ((uint64_t)ol.OffsetHigh << 32) | ol.Offset;
  return new_offset-offset;
#endif
  return -1;
}

uint64_t palFile::OffsetWrite(uint64_t offset, const void* buffer, size_t num_bytes) {
#if defined(PAL_PLATFORM_WINDOWS)
  BOOL r;
  uint32_t bytes_written_;
  OVERLAPPED ol;
  ol.Offset = offset & 0xFFFFFFFF;
  ol.OffsetHigh = offset >> 32;
  r = WriteFile(_pdata._handle, buffer, (DWORD)num_bytes, (LPDWORD)&bytes_written_, &ol);
  uint64_t new_offset = ((uint64_t)ol.OffsetHigh << 32) | ol.Offset;
  return new_offset-offset;
#endif
  return -1;
}

uint64_t palFile::GetPosition() {
  return Seek(kFileSeekCurrent, 0);
}

uint64_t palFile::GetSize() {
#if defined(PAL_PLATFORM_WINDOWS)
  if (_pdata._handle != INVALID_HANDLE_VALUE) {
    uint64_t size;
    uint32_t low_size;
    uint32_t high_size;
    low_size = GetFileSize(_pdata._handle, (LPDWORD)&high_size);
    size = low_size | ((uint64_t)high_size << 32);
    return size;
  } else {
    return 0;
  }
#endif
  return 0;
}

uint64_t palFile::SetFileSize(uint64_t new_size) {
  uint64_t new_position = Seek(kFileSeekBegin, new_size);
#if defined(PAL_PLATFORM_WINDOWS)
    SetEndOfFile(_pdata._handle);
#endif
  return new_position;
}

void palFile::Flush() {
#if defined(PAL_PLATFORM_WINDOWS)
  FlushFileBuffers(_pdata._handle);
#endif
}

int palFile::CopyFileContentsAsString(const char* filename, palMemBlob* blob) {
  palFile pf;
  int r;
  r = pf.OpenForReading(filename);
  if (r != 0) {
    *blob = palMemBlob(NULL, 0);
    return -1;
  }
  pf.CopyContentsAsString(blob);
  pf.Close();
  return 0;
}

int palFile::CopyFileContents(const char* filename, palMemBlob* blob) {
  palFile pf;
  int r;
  *blob = palMemBlob(NULL, 0);
  r = pf.OpenForReading(filename);
  if (r != 0) {
    *blob = palMemBlob(NULL, 0);
    return -1;
  }
  pf.CopyContents(blob);
  pf.Close();
  return 0;
}

void palFile::FreeFileContents(palMemBlob* blob) {
  if (blob && blob->GetPtr(0)) {
    g_FileProxyAllocator->Deallocate(blob->GetPtr(0));
  }
}

int palFile::WriteFileContents(const char* filename, palMemBlob blob) {
  palFile pf;
  int r;
  r = pf.OpenForWritingTruncate(filename);
  if (r != 0) {
    return r;
  }
  pf.Write(blob.GetPtr(0), blob.GetBufferSize());
  pf.Close();
  return 0;
}
