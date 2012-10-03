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

#include "libpal/pal_spinlock.h"

typedef uintptr_t palStackAllocatorMarker;

class palStackAllocator {
  uintptr_t stack_bottom_;
  uintptr_t stack_top_;
  uintptr_t stack_cursor_;
  palSpinlock lock_;
  const char* name_;
 public:

  palStackAllocator() : stack_bottom_(0), stack_top_(0), stack_cursor_(0) {
    palSpinlockInit(&lock_);
    name_ = "palStackAllocator";
  }
  
  palStackAllocator(void* mem, uint32_t size) {
    palSpinlockInit(&lock_);
    Create(mem, size);
    name_ = "palStackAllocator";
  }

  void Create(void* mem, uint32_t size) {
    stack_bottom_ = reinterpret_cast<uintptr_t>(mem);
    stack_top_ = stack_bottom_ + size;
    stack_cursor_ = stack_bottom_;
  }

  void* Allocate(size_t size, int flags = kPalAllocationFlagNone) {
    palSpinlockTake(&lock_);
    palAssert(stack_cursor_ + size < stack_top_);
    void* p = reinterpret_cast<void*>(stack_cursor_);
    stack_cursor_ += size;
    palSpinlockRelease(&lock_);
    return p;
  }

  void* Allocate(size_t size, size_t alignment, size_t alignment_offset = 0, int flags = kPalAllocationFlagNone) {
    const int pointerSize = sizeof(void*);
    const int requestedSize = size + alignment - 1 + pointerSize;
    void* p = Allocate(requestedSize, flags);
    void* aligned = palAlign(p, alignment);
    return aligned;
  }

  void  Deallocate(void* p, size_t size) {
    palAssert(false);
  }

  const char* GetName() const {
    return name_;
  }

  void        SetName(const char* name) {
    name_ = name;
  }

  palStackAllocatorMarker GetMarker() {
    palSpinlockTake(&lock_);
    palStackAllocatorMarker m = stack_cursor_;
    palSpinlockRelease(&lock_);
    return m;
  }

  void FreeToMarker(palStackAllocatorMarker marker) {
    palAssert(marker >= stack_bottom_ && marker < stack_top_);
    palSpinlockTake(&lock_);
    stack_cursor_ = marker;
    palSpinlockRelease(&lock_);
  }

  void FreeAll() {
    palSpinlockTake(&lock_);
    stack_cursor_ = stack_bottom_;
    palSpinlockRelease(&lock_);
  }
};