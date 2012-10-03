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

#if defined(PAL_BUILD_DEBUG)

#if defined(PAL_COMPILER_MICROSOFT) && defined(PAL_CPU_X86) && defined(PAL_ARCH_32BIT)
void palBreakHere() {
  __asm int 3
}
#elif defined(PAL_COMPILER_MICROSOFT) && defined(PAL_CPU_X86) && defined(PAL_ARCH_64BIT)
#include <windows.h>
void palBreakHere() {
  DebugBreak();
}
#elif defined(PAL_CPU_X86) && defined(PAL_COMPILER_GNU)
void palBreakHere() {
  __builtin_trap():
}
#endif

#else // defined(PAL_BUILD_DEBUG)

void palBreakHere() {
  return;
}

#endif
