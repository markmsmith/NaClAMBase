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
#include "libpal/pal_types.h"
#include "libpal/pal_timer.h"
#include "libpal/pal_delegate.h"
#include "libpal/pal_string.h"
#include "libpal/pal_errorcode.h"

#if defined(PAL_PLATFORM_WINDOWS)
#include "libpal/windows/pal_thread_windows.h"
#elif defined(PAL_PLATFORM_NACL)
#include "libpal/posix/pal_thread_posix.h"
#else
#error Need to implement palThread classes for this platform
#endif

#define PAL_THREAD_ERROR_GENERIC palMakeErrorCode(PAL_ERROR_CODE_THREAD_GROUP, 0)
#define PAL_THREAD_ERROR_TRY_AGAIN palMakeErrorCode(PAL_ERROR_CODE_THREAD_GROUP, 1)
#define PAL_THREAD_ERROR_COULD_NOT_CREATE palMakeErrorCode(PAL_ERROR_CODE_THREAD_GROUP, 2)

enum palThreadRecursionPolicy {
  kPalThreadRecursionPolicyNotAllowed,
  kPalThreadRecursionPolicyAllowed,
  NUM_palThreadRecursionPolicies
};

#define palThreadCPUMaskAll 0xffffffff

enum palThreadPriority {
  kPalThreadPriorityHighest,
  kPalThreadPriorityHigh,
  kPalThreadPriorityNormal,
  kPalThreadPriorityLow,
  kPalThreadPriorityLowest,
  NUM_palThreadPriorities,
};

typedef palDelegate<void (uintptr_t)> palThreadStart;

struct palThreadDescription {
  const char* name;
  palThreadStart start_method;
  uintptr_t start_value;
  palThreadPriority priority;
  uint32_t stack_size;
  uint32_t cpu_mask;
  palThreadDescription() : cpu_mask(palThreadCPUMaskAll), stack_size(128*1024), priority(kPalThreadPriorityNormal), name("Unnamed Thread") {
  }
};

class palThread {
  palThreadDescription _desc;
  palThreadPlatformData _pdata;
  PAL_DISALLOW_COPY_AND_ASSIGN(palThread);
public:
  static palThread* GetCurrentThread();
  static void Sleep(palTimerTick ticks);
  static void SpinWait(int iterations);
  static void SpinYield();
  static void Exit(int64_t thread_exit_value);

  palThread();

  int Start(const palThreadDescription& desc, uintptr_t param);
  int Join(int64_t* thread_exit_value);
  const char* GetName() const;
  const palThreadDescription& GetDescription() const;
  palThreadDescription& GetDescription();
};

struct palMutexDescription {
  const char* name;
  bool initial_ownership;
  palThreadRecursionPolicy recursion_policy;

  palMutexDescription() : initial_ownership(false), name(NULL), recursion_policy(kPalThreadRecursionPolicyNotAllowed) {
  }
};

class palMutex {
  palMutexDescription _desc;
  palMutexPlatformData _pdata;
  PAL_DISALLOW_COPY_AND_ASSIGN(palMutex);
public:
  palMutex();

  int Create(const palMutexDescription& desc);
  int Destroy();

  int Acquire();
  int Acquire(palTimerTick timeout);
  int TryAcquire();

  int Release();
};

struct palSemaphoreDescription {
  const char* name;
  uint32_t maximum;
  uint32_t initial_reservation;

  palSemaphoreDescription() : initial_reservation(0), maximum(1), name("Unnamed Semaphore") {
  }
};

class palSemaphore {
  palSemaphoreDescription _desc;
  palSemaphorePlatformData _pdata;
  PAL_DISALLOW_COPY_AND_ASSIGN(palSemaphore);
public:
  palSemaphore();

  int Create(const palSemaphoreDescription& desc);
  int Destroy();

  int Acquire();
  int Acquire(palTimerTick timeout);
  int TryAcquire();

  int Release();
};

struct palReaderWriterLockDescription {
  const char* name;
  palThreadRecursionPolicy recursion_policy;
};

class palReaderWriterLock {
  palReaderWriterLockDescription _desc;
  palReaderWriterLockPlatformData _pdata;
  PAL_DISALLOW_COPY_AND_ASSIGN(palReaderWriterLock);
public:
  palReaderWriterLock();

  int Create(const palReaderWriterLockDescription& desc);
  int Destroy();

  int AcquireReader();
  int AcquireReader(palTimerTick timeout);
  int TryAcquireReader();
  int ReleaseReader();

  int AcquireWriter();
  int AcquireWriter(palTimerTick timeout);
  int TryAcquireWriter();
  int ReleaseWriter();
};

class palScopedMutex {
  palMutex* _mutex;
  PAL_DISALLOW_COPY_AND_ASSIGN(palScopedMutex);
public:
  palScopedMutex(palMutex* mutex) : _mutex(mutex) {
    _mutex->Acquire();
  }
  ~palScopedMutex() {
    _mutex->Release();
  }
};

class palScopedReaderWriterLockReader {
  palReaderWriterLock* _rw;
public:
  palScopedReaderWriterLockReader(class palReaderWriterLock* rw) : _rw(rw) {
    _rw->AcquireReader();
  }
  ~palScopedReaderWriterLockReader() {
    _rw->ReleaseReader();
  }
};

class palScopedReaderWriterLockWriter {
  palReaderWriterLock* _rw;
public:
  palScopedReaderWriterLockWriter(palReaderWriterLock* rw) : _rw(rw) {
    _rw->AcquireWriter();
  }

  ~palScopedReaderWriterLockWriter() {
    _rw->ReleaseWriter();
  }
};

int palThreadInit();