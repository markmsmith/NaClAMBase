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
#include "libpal/pal_pipe.h"

#if defined(PAL_PLATFORM_WINDOWS)
#include "libpal/windows/pal_process_windows.h"
#elif defined(PAL_PLATFORM_NACL)
#include "libpal/posix/pal_process_posix.h"
#else
#error Need to implement palProcess classes for this platform
#endif

struct palProcessStartDescription {
  palPipe* stdout_override;
  palPipe* stdin_override;
  palPipe* stderr_override;
  bool inherit_os_objects;
  char* command_line;

  palProcessStartDescription() {
    stderr_override = NULL;
    stdout_override = NULL;
    stdin_override = NULL;
    command_line = NULL;
    inherit_os_objects = true;
  }
};

class palProcess {
  palProcessPlatformData _pdata;
  PAL_DISALLOW_COPY_AND_ASSIGN(palProcess);
public:
  palProcess();
  ~palProcess();

  void Close();
  bool HasExited() const;
  void WaitForExit();
  int GetExitCode();

  static int Start(const palProcessStartDescription& desc, palProcess* started);
};
