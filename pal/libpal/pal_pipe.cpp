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
#include "libpal/pal_pipe.h"

#if defined(PAL_PLATFORM_WINDOWS)
#include <windows.h>
#endif

palPipe::palPipe() {
}

palPipe::~palPipe() {
  Close();
}

int palPipe::Create(bool inherited_by_spawned_process) {
#if defined(PAL_PLATFORM_WINDOWS)
  if (_pdata._write_handle != INVALID_HANDLE_VALUE || _pdata._read_handle != INVALID_HANDLE_VALUE) {
    // already created
    return PAL_PIPE_ALREADY_CREATED;
  }
  BOOL r;
  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(sa);
  sa.bInheritHandle = inherited_by_spawned_process;
  sa.lpSecurityDescriptor = NULL;
  r = CreatePipe(&_pdata._read_handle, &_pdata._write_handle, &sa, 0);
  if (r != 0) {
    return 0;
  }
  return PAL_PIPE_CANT_CREATE;
#endif
}

int palPipe::Write(const void* buffer, size_t bytes_to_write, size_t* bytes_written) {
#if defined(PAL_PLATFORM_WINDOWS)
  BOOL r;
  DWORD _bytes_written;
  if (_pdata._write_handle == INVALID_HANDLE_VALUE) {
    return PAL_PIPE_CLOSED_WRITE;
  }
  r = WriteFile(_pdata._write_handle, buffer, (DWORD)bytes_to_write, &_bytes_written, NULL);
  if (r != 0) {
    *bytes_written = _bytes_written;
    return 0;
  }
  *bytes_written = 0;
  return PAL_PIPE_FAILED_WRITE;
#endif
}

int palPipe::Read(void* buffer, size_t bytes_to_read, size_t* bytes_read) {
#if defined(PAL_PLATFORM_WINDOWS)
  BOOL r;
  DWORD _bytes_read;
  if (_pdata._read_handle == INVALID_HANDLE_VALUE) {
    return PAL_PIPE_CLOSED_READ;
  }
  r = ReadFile(_pdata._read_handle, buffer, (DWORD)bytes_to_read, &_bytes_read, NULL);
  if (r != 0) {
    *bytes_read = _bytes_read;
    return 0;
  }
  *bytes_read = 0;
  return PAL_PIPE_FAILED_READ;
#endif
}

int palPipe::Close() {
  int r;
  r = CloseRead();
  if (r) {
    return r;
  }
  r = CloseWrite();
  return r;
}

int palPipe::CloseRead() {
#if defined(PAL_PLATFORM_WINDOWS)
  if (_pdata._read_handle != INVALID_HANDLE_VALUE) {
    CloseHandle(_pdata._read_handle);
    _pdata._read_handle = INVALID_HANDLE_VALUE;
    return 0;
  }
  return PAL_PIPE_CLOSED_READ;
#endif
}

int palPipe::CloseWrite() {
#if defined(PAL_PLATFORM_WINDOWS)
  if (_pdata._write_handle != INVALID_HANDLE_VALUE) {
    CloseHandle(_pdata._write_handle);
    _pdata._write_handle = INVALID_HANDLE_VALUE;
    return 0;
  }
  return PAL_PIPE_CLOSED_WRITE;
#endif
}

palPipePlatformData palPipe::GetPlatformData() {
  return _pdata;
}

bool palPipe::IsReadOpen() const {
#if defined(PAL_PLATFORM_WINDOWS)
  return _pdata._read_handle != INVALID_HANDLE_VALUE;
#endif
}

bool palPipe::IsWriteOpen() const {
#if defined(PAL_PLATFORM_WINDOWS)
  return _pdata._write_handle != INVALID_HANDLE_VALUE;
#endif
}

void palPipe::FlushRead() {
#if defined(PAL_PLATFORM_WINDOWS)
  if (_pdata._read_handle != INVALID_HANDLE_VALUE) {
    FlushFileBuffers(_pdata._read_handle);
  }
#endif
}

void palPipe::FlushWrite() {
#if defined(PAL_PLATFORM_WINDOWS)
  if (_pdata._write_handle != INVALID_HANDLE_VALUE) {
    FlushFileBuffers(_pdata._write_handle);
  }
#endif
}

void palPipe::SetReadInheritanceFlag(bool inherit) {
#if defined(PAL_PLATFORM_WINDOWS)
  if (_pdata._read_handle != INVALID_HANDLE_VALUE) {
    SetHandleInformation(_pdata._read_handle, HANDLE_FLAG_INHERIT, inherit ? 1 : 0);
  }
#endif
}

void palPipe::SetWriteInheritanceFlag(bool inherit) {
#if defined(PAL_PLATFORM_WINDOWS)
  if (_pdata._write_handle != INVALID_HANDLE_VALUE) {
    SetHandleInformation(_pdata._write_handle, HANDLE_FLAG_INHERIT, inherit ? 1 : 0);
  }
#endif
}
