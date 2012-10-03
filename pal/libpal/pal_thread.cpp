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

#include "libpal/pal_thread.h"

#if defined(PAL_PLATFORM_WINDOWS)
PAL_TLS palThread* current_thread;

#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
  DWORD dwType; // Must be 0x1000.
  LPCSTR szName; // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1=caller thread).
  DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void __SetThreadName(DWORD dwThreadID, const char* threadName)
{
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = threadName;
  info.dwThreadID = dwThreadID;
  info.dwFlags = 0;

  __try
  {
    RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
  }
}

#ifndef __CLR_OR_STD_CALL
#if defined (_M_CEE) || defined (MRTDLL)
#define __CLR_OR_STD_CALL   __clrcall
#else  /* defined (_M_CEE) || defined (MRTDLL) */
#define __CLR_OR_STD_CALL   __stdcall
#endif  /* defined (_M_CEE) || defined (MRTDLL) */
#endif  /* __CLR_OR_STD_CALL */

static  unsigned int __CLR_OR_STD_CALL ThreadRunner(void* arg) {
  current_thread = reinterpret_cast<palThread*>(arg);
  __SetThreadName(GetCurrentThreadId(), current_thread->GetName());
  current_thread->GetDescription().start_method(current_thread->GetDescription().start_value);
  return 0;
}

palThread* palThread::GetCurrentThread() {
  return current_thread;
}

void palThread::Sleep(palTimerTick ticks) {
}

void palThread::SpinWait(int iterations) {
}

void palThread::SpinYield() {
}

void palThread::Exit(int64_t thread_exit_value) {
  _endthreadex((unsigned int)thread_exit_value);
}

palThread::palThread() {
  _pdata.thread = INVALID_HANDLE_VALUE;
}

int palThread::Start(const palThreadDescription& desc, uintptr_t param) {
  _desc = desc;
  _desc.start_value = param;
  _pdata.thread = (HANDLE)_beginthreadex(NULL, _desc.stack_size, ThreadRunner, reinterpret_cast<void*>(this), 0, NULL);
  return 0;
}

int palThread::Join(int64_t* thread_return_value) {
  if (_pdata.thread != INVALID_HANDLE_VALUE) {
    WaitForSingleObject(reinterpret_cast<HANDLE>(_pdata.thread), INFINITE);
    CloseHandle(reinterpret_cast<HANDLE>(_pdata.thread));
    _pdata.thread = INVALID_HANDLE_VALUE;
  }
  return 0;
}

const char* palThread::GetName() const {
  return _desc.name;
}

const palThreadDescription& palThread::GetDescription() const {
  return _desc;
}

palThreadDescription& palThread::GetDescription() {
  return _desc;
}

palMutex::palMutex() {
}

int palMutex::Create(const palMutexDescription& desc) {
  _desc = desc;
  InitializeCriticalSection(&_pdata.mutex);
  return 0;
}

int palMutex::Acquire() {
  EnterCriticalSection(&_pdata.mutex);
  return 0;
}

int palMutex::Acquire(palTimerTick timeout) {
  EnterCriticalSection(&_pdata.mutex);
  return 0;
}

int palMutex::TryAcquire() {
  BOOL r = TryEnterCriticalSection(&_pdata.mutex);
  if (r) {
    return 0;
  } else {
    return PAL_THREAD_ERROR_TRY_AGAIN;
  }
}

int palMutex::Release() {
  LeaveCriticalSection(&_pdata.mutex);
  return 0;
}

int palMutex::Destroy() {
  DeleteCriticalSection(&_pdata.mutex);
  return 0;
}

palSemaphore::palSemaphore() {
  _pdata.semaphore = 0;
}

int palSemaphore::Create(const palSemaphoreDescription& desc) {
  _desc = desc;
  _pdata.semaphore = CreateSemaphoreEx(NULL, _desc.initial_reservation, _desc.maximum, _desc.name, 0, SEMAPHORE_ALL_ACCESS);
  if (_pdata.semaphore == 0) {
    return PAL_THREAD_ERROR_COULD_NOT_CREATE;
  }
  return 0;
}

int palSemaphore::Destroy() {
  if (_pdata.semaphore != 0) {
    CloseHandle(_pdata.semaphore);
  }
  
  return 0;
}

int palSemaphore::Acquire() {
  WaitForSingleObject(_pdata.semaphore, INFINITE);
  return 0;
}

int palSemaphore::Acquire(palTimerTick timeout) {
  DWORD r = WaitForSingleObject(_pdata.semaphore, palTimerTickGetMilliseconds(timeout));
  if (r == WAIT_TIMEOUT) {
    return PAL_THREAD_ERROR_TRY_AGAIN;
  } else if (r == WAIT_OBJECT_0) {
    return 0;
  }
  return PAL_THREAD_ERROR_GENERIC;
}

int palSemaphore::TryAcquire() {
  DWORD r = WaitForSingleObject(_pdata.semaphore, 0);
  if (r == WAIT_TIMEOUT) {
    return PAL_THREAD_ERROR_TRY_AGAIN;
  } else if (r == WAIT_OBJECT_0) {
    return 0;
  }
  return PAL_THREAD_ERROR_GENERIC;
}

int palSemaphore::Release() {
  BOOL r = ReleaseSemaphore(_pdata.semaphore, 1, NULL);
  if (r) {
    return 0;
  } 
  return PAL_THREAD_ERROR_GENERIC;
}

palReaderWriterLock::palReaderWriterLock() {
}

int palReaderWriterLock::Create(const palReaderWriterLockDescription& desc) {
  _desc = desc;
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::Destroy() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::AcquireReader() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::AcquireReader(palTimerTick timeout) {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::TryAcquireReader() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::ReleaseReader() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::AcquireWriter() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::AcquireWriter(palTimerTick timeout) {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::TryAcquireWriter() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::ReleaseWriter() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palThreadInit() {
  static palThreadDescription main_thread_description;
  main_thread_description.name = "Main Thread";
  main_thread_description.stack_size = 1024*1024;
  static palThread main_thread;
  main_thread.GetDescription() = main_thread_description;
  current_thread = &main_thread;
  __SetThreadName(GetCurrentThreadId(), "Main Thread");
  return 0;
}

#elif defined(PAL_PLATFORM_NACL)

PAL_TLS palThread* current_thread;

palThread* palThread::GetCurrentThread() {
  return current_thread;
}

void palThread::Sleep(palTimerTick ticks) {
}

void palThread::SpinWait(int iterations) {
}

void palThread::SpinYield() {
}

void palThread::Exit(int64_t thread_exit_value) {
}

palThread::palThread() {
}

int palThread::Start(const palThreadDescription& desc, uintptr_t param) {
  return 0;
}

int palThread::Join(int64_t* thread_return_value) {
  return 0;
}

const char* palThread::GetName() const {
  return _desc.name;
}

const palThreadDescription& palThread::GetDescription() const {
  return _desc;
}

palThreadDescription& palThread::GetDescription() {
  return _desc;
}

palMutex::palMutex() {
}

int palMutex::Create(const palMutexDescription& desc) {
  return 0;
}

int palMutex::Acquire() {
  return 0;
}

int palMutex::Acquire(palTimerTick timeout) {
  return 0;
}

int palMutex::TryAcquire() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palMutex::Release() {
  return 0;
}

int palMutex::Destroy() {
  return 0;
}

palSemaphore::palSemaphore() {
  _pdata.semaphore = 0;
}

int palSemaphore::Create(const palSemaphoreDescription& desc) {
  return 0;
}

int palSemaphore::Destroy() {
  return 0;
}

int palSemaphore::Acquire() {
  return 0;
}

int palSemaphore::Acquire(palTimerTick timeout) {
  return PAL_THREAD_ERROR_GENERIC;
}

int palSemaphore::TryAcquire() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palSemaphore::Release() {
  return PAL_THREAD_ERROR_GENERIC;
}

palReaderWriterLock::palReaderWriterLock() {
}

int palReaderWriterLock::Create(const palReaderWriterLockDescription& desc) {
  _desc = desc;
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::Destroy() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::AcquireReader() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::AcquireReader(palTimerTick timeout) {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::TryAcquireReader() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::ReleaseReader() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::AcquireWriter() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::AcquireWriter(palTimerTick timeout) {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::TryAcquireWriter() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palReaderWriterLock::ReleaseWriter() {
  return PAL_THREAD_ERROR_GENERIC;
}

int palThreadInit() {
  static palThreadDescription main_thread_description;
  main_thread_description.name = "Main Thread";
  main_thread_description.stack_size = 1024*1024;
  static palThread main_thread;
  main_thread.GetDescription() = main_thread_description;
  current_thread = &main_thread;
  //__SetThreadName(GetCurrentThreadId(), "Main Thread");
  return 0;
}
#endif