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

#include "libpal/pal_allocator_interface.h"
#include "libpal/pal_ilist.h"

#define MAX_CALLSTACK_ENTRIES 63
struct palTrackedAllocation {
  uintptr_t stacktrace[MAX_CALLSTACK_ENTRIES+1];
  void* ptr;
  palIListNodeDeclare(palTrackedAllocation, list_node);
};

class palTrackingAllocator : public palAllocatorInterface {
  palIList _tracked_allocations;
  palAllocatorInterface* _parent_allocator;
  void* _symbol_lookup_buffer;
public:
  palTrackingAllocator(const char* name, palAllocatorInterface* parent_allocator);

  ~palTrackingAllocator();

  virtual void* Allocate(size_t size, size_t alignment = 8);
  virtual void Deallocate(void* ptr);
  virtual size_t GetSize(void* ptr) const;

  void ConsoleDump() const;
};