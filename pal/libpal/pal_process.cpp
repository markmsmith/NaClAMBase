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

#include "libpal/pal_process.h"


palProcess::palProcess() {
}

palProcess::~palProcess() {
  Close();
}

void palProcess::Close() {
#if defined(PAL_PLATFORM_WINDOWS)
  if (_pdata._process != INVALID_HANDLE_VALUE) {
    CloseHandle(_pdata._process);
    _pdata._process = INVALID_HANDLE_VALUE;
  }
  if (_pdata._thread != INVALID_HANDLE_VALUE) {
    CloseHandle(_pdata._thread);
    _pdata._thread = INVALID_HANDLE_VALUE;
  }
#endif
}

bool palProcess::HasExited() const {
#if defined(PAL_PLATFORM_WINDOWS)
  BOOL r;
  DWORD exit_code;
  r = GetExitCodeProcess(_pdata._process, &exit_code);
  if (r != 0 && exit_code == STILL_ACTIVE) {
    return false;
  }
  if (r == 0) {
    return false;
  }
  return true;
#endif
}

void palProcess::WaitForExit() {
#if defined(PAL_PLATFORM_WINDOWS)
  // Wait until child process exits.
  if (_pdata._process != INVALID_HANDLE_VALUE) {
    WaitForSingleObject(_pdata._process, INFINITE );
  }
#endif
}

int palProcess::GetExitCode() {
#if defined(PAL_PLATFORM_WINDOWS)
  BOOL r;
  DWORD exit_code;
  r = GetExitCodeProcess(_pdata._process, &exit_code);
  return exit_code;
#endif
}

int palProcess::Start(const palProcessStartDescription& desc, palProcess* started) {
  if (started == NULL) {
    return -1;
  }
  if (desc.command_line == NULL) {
    return -2;
  }
#if defined(PAL_PLATFORM_WINDOWS)
  SECURITY_ATTRIBUTES sa;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  BOOL r;
  sa.nLength = sizeof(sa);
  sa.bInheritHandle = desc.inherit_os_objects;
  sa.lpSecurityDescriptor = NULL;
  palMemoryZeroBytes(&pi, sizeof(pi));
  palMemoryZeroBytes(&si, sizeof(si));
  si.cb = sizeof(si);
  if (desc.inherit_os_objects) {
    if (desc.stdin_override) {
      // the spawned process shouldn't be able to write to STDIN
      desc.stdin_override->SetWriteInheritanceFlag(false);
      si.hStdInput = desc.stdin_override->GetPlatformData()._read_handle;
    }
    if (desc.stdout_override) {
      // the spawned process shouldn't be able to read from STDOUT
      desc.stdout_override->SetReadInheritanceFlag(false);
      si.hStdOutput = desc.stdout_override->GetPlatformData()._write_handle;
    }
    if (desc.stderr_override) {
      // the spawned process shouldn't be able to read from STDERR
      desc.stderr_override->SetReadInheritanceFlag(false);
      si.hStdError = desc.stderr_override->GetPlatformData()._write_handle;
    }
    si.dwFlags |= STARTF_USESTDHANDLES;
  }
  r = CreateProcess(NULL,
                    desc.command_line,
                    NULL,
                    NULL,
                    desc.inherit_os_objects ? 1 : 0,
                    0,
                    NULL,
                    NULL,
                    &si,
                    &pi);

  if (r == 0) {
    DWORD le;
    le = GetLastError();
    return -3;
  }

  started->_pdata._process = pi.hProcess;
  started->_pdata._thread = pi.hThread;

  return 0;
#endif
}
