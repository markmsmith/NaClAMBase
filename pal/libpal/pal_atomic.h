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

#include "libpal/pal_types.h"
#include "libpal/pal_platform.h"

template <typename T> 
struct palAtomicIntegral {
private:
  // disable copying
  palAtomicIntegral(const palAtomicIntegral&);
  palAtomicIntegral& operator=(const palAtomicIntegral&);
public:
  palAtomicIntegral();
  palAtomicIntegral(T initial_value);

  /* Atomically stores new_value into *this */
  void Store(T new_value)  volatile;
  /* Atomically fetches the value stored in *this and returns it */
  T Load() const volatile;

  /* Atomically store a new value and return old value */
  T Exchange(T new_value) volatile;

  /* Atomically compare the value with expected, and store new_value if they are equal */
  /* Returns true if value was expected, false otherwise */
  /* Updates expected with value read */
  bool CompareExchange(T& expected, T new_value) volatile;

  /* Atomically fetches the value stored in *this and returns it */
  operator T() const volatile;

  /* Atomically fetch the value, perform the operation and return the original value */
  T FetchAdd(T i) volatile;
  T FetchSub(T i) volatile;
  T FetchAnd(T i) volatile;
  T FetchOr(T i) volatile;
  T FetchXor(T i) volatile;

  T operator=(T iv) volatile {
    Store(iv);
    return iv;
  }

  /* Atomically perform pre and post increment and decrement */
  T operator++() volatile;
  T operator++(int) volatile;
  T operator--() volatile;
  T operator--(int) volatile;

  /* Atomically perform the operations, returning the resulting value */
  T operator+=(T i) volatile;
  T operator-=(T i) volatile;
  T operator&=(T i) volatile;
  T operator|=(T i) volatile;
  T operator^=(T i) volatile;
};

struct palAtomicFlag {
  volatile long flag_;
private:
  // disable copying
  palAtomicFlag(const palAtomicFlag&) {}
  palAtomicFlag& operator=(const palAtomicFlag&) { return *this; }
public:
  PAL_INLINE palAtomicFlag();
  /* Initialization: */
  /* palAtomicFlag aflag = PAL_ATOMIC_FLAG_INIT; */
  /* Atomically set the flag and check whether or not it was set */
  /* Returns true if the flag was already set, false if it was already clear */
  PAL_INLINE bool TestAndSet() volatile;
  /* Atomically clears the flag */
  PAL_INLINE void Clear() volatile;
};

#define PAL_ATOMIC_FLAG_INIT { 0 }

struct palAtomicReferenceCount {
  volatile long count_;
private:
public:
  PAL_INLINE palAtomicReferenceCount();

  /* Atomically decrements reference count */
  /* Returns true if reference count hit 0 */
  PAL_INLINE int32_t Unref() volatile;

  /* Atomically increases reference count */
  PAL_INLINE int32_t Ref() volatile;

  /* Atomically loads and returns the reference count */
  PAL_INLINE int32_t Load() const volatile;
};

typedef palAtomicIntegral<int32_t> palAtomicInt32;
typedef palAtomicIntegral<int64_t> palAtomicInt64;

struct palAtomicAddress {
private:
  void* value_;
  // disable copying
  palAtomicAddress(const palAtomicAddress&);
  palAtomicAddress& operator=(const palAtomicAddress&);
public:
  palAtomicAddress();
  palAtomicAddress(void* ptr);

  /* Atomically stores new_value into *this */
  void Store(void* new_value)  volatile;
  /* Atomically fetches the value stored in *this and returns it */
  void* Load() const volatile;

  /* Atomically store a new value and return old value */
  void* Exchange(void* new_value) volatile;

  /* Atomically compare the value with expected, and store new_value if they are equal */
  /* Returns true if value was expected, false otherwise */
  /* Updates expected with value read */
  bool CompareExchange(void*& expected, void* new_value) volatile;

  /* Atomically fetches the value stored in *this and returns it */
  operator void*() const volatile;

  void* FetchAdd(ptrdiff_t i) volatile;
  void* FetchSub(ptrdiff_t i) volatile;

  void* operator=(void* iv) volatile {
    Store(iv);
    return iv;
  }

  void* operator+=(ptrdiff_t v) volatile {
    return FetchAdd(v);
  }

  void* operator-=(ptrdiff_t v) volatile {
    return FetchSub(v);
  }
};

PAL_INLINE void palAtomicMemoryBarrier();

#include "libpal/pal_atomic_inl.h"


