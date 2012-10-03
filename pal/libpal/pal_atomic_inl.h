/*
	Copyright (c) 2009 John McCutchan <john@johnmccutchan.com>

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

#ifndef LIBPAL_PAL_ATOMIC_INL_H_
#define LIBPAL_PAL_ATOMIC_INL_H_

#include "libpal/pal_platform.h"
#include "libpal/pal_atomic.h"

#if defined(PAL_PLATFORM_WINDOWS)
#include "libpal/windows/pal_atomic_windows.h"
#elif defined(PAL_PLATFORM_APPLE)
#include "libpal/apple/pal_atomic_apple.h"
#elif defined(PAL_PLATFORM_NACL)
#include "libpal/posix/pal_atomic_gcc.h"
#else
#error no pal_atomic for your platform
#endif

#endif  // LIBPAL_PAL_ATOMIC_INL_H_