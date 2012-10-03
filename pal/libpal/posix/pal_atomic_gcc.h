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

#pragma once

#include "libpal/pal_platform.h"
#include "libpal/pal_debug.h"

template <> 
struct palAtomicIntegral<int32_t> {
private:
  volatile int32_t value_;
  // disable copying
  palAtomicIntegral(const palAtomicIntegral&);
  palAtomicIntegral& operator=(const palAtomicIntegral&);
public:
  palAtomicIntegral() {
    value_ = 0;
    __sync_synchronize();
  }
  palAtomicIntegral(int32_t initial_value) {
    value_ = initial_value;
    __sync_synchronize();
  }

  /* Atomically stores new_value into *this */
  void Store(int32_t new_value) volatile {
    value_ = new_value;
    __sync_synchronize();
  }

  /* Atomically fetches the value stored in *this and returns it */
  int32_t Load() const volatile {
    return __sync_fetch_and_add(const_cast<volatile int32_t*>(&value_), 0);
  }

  /* Atomically store a new value and return old value */
  int32_t Exchange(int32_t new_value) volatile {
    value_ = new_value;
    __sync_synchronize();
  }

  /* Atomically compare the value with expected, and store new_value if they are equal */
  /* Returns true if value was expected, false otherwise */
  /* Updates expected with value read */
  bool CompareExchange(int32_t& expected, int32_t new_value) volatile {
    int32_t actual;
    actual = __sync_val_compare_and_swap(&value_, expected, new_value);
    bool success = actual == expected;
    expected = actual;
    return success;
  }

  /* Atomically fetches the value stored in *this and returns it */
  operator int32_t() const volatile {
    return __sync_fetch_and_add((volatile long*)&value_, 0);
  }

  /* Atomically fetch the value, perform the operation and return the original value */
  int32_t FetchAdd(int32_t i) volatile {
    return __sync_fetch_and_add(&value_, i);
  }

  int32_t FetchSub(int32_t i) volatile {
    return __sync_fetch_and_sub(&value_, i);
  }

  int32_t FetchAnd(int32_t i) volatile {
    return __sync_fetch_and_and(&value_, i);
  }
  int32_t FetchOr(int32_t i) volatile {
    return __sync_fetch_and_or(&value_, i);
  }
  int32_t FetchXor(int32_t i) volatile {
    return __sync_fetch_and_xor(&value_, i);
  }

  /* Atomically perform pre and post increment and decrement */
  int32_t operator++() volatile {
    return FetchAdd(1) + 1;
  }

  int32_t operator++(int) volatile {
    return FetchAdd(1);
  }

  int32_t operator--() volatile {
    return FetchSub(1) - 1;
  }

  int32_t operator--(int) volatile {
    return FetchSub(1);
  }

  /* Atomically perform the operations, returning the resulting value */
  int32_t operator+=(int32_t i) volatile {
    return FetchAdd(i) + i;
  }
  int32_t operator-=(int32_t i) volatile {
    return FetchSub(i) - i;
  }
  int32_t operator&=(int32_t i) volatile {
    return FetchAnd(i) & i;
  }
  int32_t operator|=(int32_t i) volatile {
    return FetchOr(i) | i;
  }
  int32_t operator^=(int32_t i) volatile {
    return FetchXor(i) ^ i;
  }
};

template <> 
struct palAtomicIntegral<int64_t> {
private:
  volatile int64_t value_;
  // disable copying
  palAtomicIntegral(const palAtomicIntegral&);
  palAtomicIntegral& operator=(const palAtomicIntegral&);
public:
  palAtomicIntegral() {
    value_ = 0;
    __sync_synchronize();
  }
  palAtomicIntegral(int64_t initial_value) {
    value_ = initial_value;
    __sync_synchronize();
  }

  /* Atomically stores new_value into *this */
  void Store(int64_t new_value)  volatile {
    value_ = new_value;
    __sync_synchronize();
  }

  /* Atomically fetches the value stored in *this and returns it */
  int64_t Load() const volatile {
    return __sync_fetch_and_add(const_cast<volatile int64_t*>(&value_), 0);
  }

  /* Atomically store a new value and return old value */
  int64_t Exchange(int64_t new_value) volatile {
    value_ = new_value;
    __sync_synchronize();
  }

  /* Atomically compare the value with expected, and store new_value if they are equal */
  /* Returns true if value was expected, false otherwise */
  /* Updates expected with value read */
  bool CompareExchange(int64_t& expected, int64_t new_value) volatile {
    int64_t actual;
    actual = __sync_val_compare_and_swap(&value_, expected, new_value);
    bool success = actual == expected;
    expected = actual;
    return success;
  }

  /* Atomically fetches the value stored in *this and returns it */
  operator int64_t() const volatile {
    return __sync_fetch_and_add((volatile long long*)&value_, 0);
  }

  /* Atomically fetch the value, perform the operation and return the original value */
  int64_t FetchAdd(int64_t i) volatile {
    return __sync_fetch_and_add(&value_, i);
  }

  int64_t FetchSub(int64_t i) volatile {
    return __sync_fetch_and_sub(&value_, i);
  }

  int64_t FetchAnd(int64_t i) volatile {
    return __sync_fetch_and_and(&value_, i);
  }

  int64_t FetchOr(int64_t i) volatile {
    return __sync_fetch_and_or(&value_, i);
  }

  int64_t FetchXor(int64_t i) volatile {
    return __sync_fetch_and_xor(&value_, i);
  }

  /* Atomically perform pre and post increment and decrement */
  int64_t operator++() volatile {
    return FetchAdd(1) + 1;
  }

  int64_t operator++(int) volatile {
    return FetchAdd(1);
  }

  int64_t operator--() volatile {
    return FetchSub(1) - 1;
  }

  int64_t operator--(int) volatile {
    return FetchSub(1);
  }

  /* Atomically perform the operations, returning the resulting value */
  int64_t operator+=(int64_t i) volatile {
    return FetchAdd(i) + i;
  }

  int64_t operator-=(int64_t i) volatile {
    return FetchSub(i) - i;
  }

  int64_t operator&=(int64_t i) volatile {
    return FetchAnd(i) & i;
  }
  int64_t operator|=(int64_t i) volatile {
    return FetchOr(i) | i;
  }
  int64_t operator^=(int64_t i) volatile {
    return FetchXor(i) ^ i;
  }
};

PAL_INLINE palAtomicFlag::palAtomicFlag() {
  flag_ = 0;
  __sync_synchronize();
}

PAL_INLINE bool palAtomicFlag::TestAndSet() volatile {
  return __sync_val_compare_and_swap(&flag_, 0, 1) == 1;
}

PAL_INLINE void palAtomicFlag::Clear() volatile {
  flag_ = 0;
  __sync_synchronize();
}

PAL_INLINE palAtomicReferenceCount::palAtomicReferenceCount() {
  count_ = 0;
  __sync_synchronize();
}

/* Atomically decrements reference count */
PAL_INLINE int32_t palAtomicReferenceCount::Unref() volatile {
  int32_t new_count = __sync_fetch_and_sub(&count_, 1);
  palAssert((new_count >= 0));
  return new_count;
}

/* Atomically increases reference count */
PAL_INLINE int32_t palAtomicReferenceCount::Ref() volatile {
  return __sync_fetch_and_add(&count_, 1);
}

/* Atomically loads and returns the reference count */
PAL_INLINE int32_t palAtomicReferenceCount::Load() const volatile {
  return __sync_fetch_and_add((volatile long*)&count_, 0);
}

PAL_INLINE palAtomicAddress::palAtomicAddress() {
  value_ = 0;
  __sync_synchronize();
}

PAL_INLINE palAtomicAddress::palAtomicAddress(void* ptr) {
  value_ = ptr;
  __sync_synchronize();
}

/* Atomically stores new_value into *this */
PAL_INLINE void palAtomicAddress::Store(void* new_value)  volatile {
  value_ = new_value;
  __sync_synchronize();
}

/* Atomically fetches the value stored in *this and returns it */
PAL_INLINE void* palAtomicAddress::Load() const volatile {
  return reinterpret_cast<void*>(__sync_fetch_and_add((volatile intptr_t*)&value_, 0));
}

/* Atomically store a new value and return old value */
PAL_INLINE void* palAtomicAddress::Exchange(void* new_value) volatile {
    value_ = new_value;
  __sync_synchronize();
}

/* Atomically compare the value with expected, and store new_value if they are equal */
/* Returns true if value was expected, false otherwise */
/* Updates expected with value read */
PAL_INLINE bool palAtomicAddress::CompareExchange(void*& expected, void* new_value) volatile {
  void* actual;
  actual = __sync_val_compare_and_swap(&value_, expected, new_value);
  bool success = actual == expected;
  expected = actual;
  return success;
}

/* Atomically fetches the value stored in *this and returns it */
PAL_INLINE palAtomicAddress::operator void*() const volatile {
  return reinterpret_cast<void*>(value_);
  //return reinterpret_cast<void*>(__sync_fetch_and_add(const_cast<volatile intptr_t*>(&value_), 0));
}

PAL_INLINE void* palAtomicAddress::FetchAdd(ptrdiff_t i) volatile {
  return reinterpret_cast<void*>(__sync_fetch_and_add(&value_, i));
}

PAL_INLINE void* palAtomicAddress::FetchSub(ptrdiff_t i) volatile {
  return reinterpret_cast<void*>(__sync_fetch_and_sub(&value_, i));
}

PAL_INLINE void palAtomicMemoryBarrier() {
  __sync_synchronize();
}
