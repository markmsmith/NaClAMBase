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


#ifndef LIBPAL_PAL_SPINLOCK_H__
#define LIBPAL_PAL_SPINLOCK_H__

#include "libpal/pal_types.h"
#include "libpal/pal_align.h"
#include "libpal/pal_atomic.h"

typedef palAtomicFlag palSpinlock;

PAL_INLINE void palSpinlockInit(palSpinlock* spinlock) {
  spinlock->flag_ = 0;
}

PAL_INLINE void palSpinlockTake(palSpinlock* spinlock) {
  while (spinlock->TestAndSet()) {
    // spin until we got it
    continue;
  }
}

PAL_INLINE void palSpinlockRelease(palSpinlock* spinlock) {
  spinlock->Clear();
}

#endif  // LIBPAL_PAL_SPINLOCK_H__
