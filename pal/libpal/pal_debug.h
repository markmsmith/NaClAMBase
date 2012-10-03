/*
	Copyright (c) 2010 John McCutchan <john@johnmccutchan.com>

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

/* This header file provides the following macros when compiled with PAL_BUILD_DEBUG:
 * palBreakHere - breaks into the debugger
 * palAssertBreak - breaks into debugger if the assertion fails
 * palAssert - kills the program if the assertion fails
 */
#include "libpal/pal_platform.h"
#include "libpal/pal_console.h"
#include "libpal/pal_types.h"

int palDebugInit();
int palDebugShutdown();

int palDebugCaptureCallstack(int entries_to_skip, int max_entries, uintptr_t* callstack);
int palDebugGetSizeForSymbolLookup(int name_length);
const char* palDebugLookupSymbol(uintptr_t pc, int name_length, void* buffer);

void palAbort();

#if defined(PAL_BUILD_DEBUG)
#define palAssert(expr) do { if (!(expr)) { palPrintf("Assertion failed: %s::%d\n", __FILE__, __LINE__); palAbort(); } } while (0)
#define palAssertBreak(expr) do { if (!(expr)) { palPrintf("Assertion failed: %s::%d\n", __FILE__, __LINE__); palBreakHere(); } } while (0)
#define palAssertNeverHit() do { palPrintf("Never hit assertion failed: %s::%d\n", __FILE__, __LINE__); palAbort(); } while (0)
#else
#define palAssertNeverHit() do {} while (0)
#define palAssert(expr) do {} while (0)
#define palAssertBreak(expr) do {} while(0)
#endif

