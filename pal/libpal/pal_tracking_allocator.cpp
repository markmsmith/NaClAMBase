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

#include "libpal/pal_memory.h"
#include "libpal/pal_tracking_allocator.h"

#define MAX_SYMBOL_NAME_LENGTH 256
palTrackingAllocator::palTrackingAllocator(const char* name, palAllocatorInterface* parent_allocator) : palAllocatorInterface(name), _parent_allocator(parent_allocator) {
  _symbol_lookup_buffer = parent_allocator->Allocate(palDebugGetSizeForSymbolLookup(MAX_SYMBOL_NAME_LENGTH));
}

palTrackingAllocator::~palTrackingAllocator() {
  _parent_allocator->Deallocate(_symbol_lookup_buffer);
  ConsoleDump();
}

void* palTrackingAllocator::Allocate(size_t size, size_t alignment) {
  const size_t requested_size = size+sizeof(palTrackedAllocation);
  void* result = _parent_allocator->Allocate(requested_size, alignment);
  if (result) {
    const size_t received_size = GetSize(result);
    ReportMemoryAllocation(result, received_size);
    const size_t offset = received_size-sizeof(palTrackedAllocation);
    palAssert(offset >= size);
    uintptr_t tracked_allocation_pointer = (uintptr_t)result;
    tracked_allocation_pointer += (uintptr_t)offset;
    palTrackedAllocation* tracked_allocation = (palTrackedAllocation*)tracked_allocation_pointer;
    tracked_allocation->ptr = result;
    _tracked_allocations.AddTail(&tracked_allocation->list_node);
    palDebugCaptureCallstack(1, MAX_CALLSTACK_ENTRIES, &tracked_allocation->stacktrace[0]);
  }
  return result;
}

void palTrackingAllocator::Deallocate(void* ptr) {
  if (ptr) {
    const size_t received_size = GetSize(ptr);
    ReportMemoryDeallocation(ptr, received_size);
    const size_t offset = received_size-sizeof(palTrackedAllocation);
    uintptr_t tracked_allocation_pointer = (uintptr_t)ptr;
    tracked_allocation_pointer += (uintptr_t)offset;
    palTrackedAllocation* tracked_allocation = (palTrackedAllocation*)tracked_allocation_pointer;
    palAssert(tracked_allocation->ptr == ptr);
    _tracked_allocations.Remove(&tracked_allocation->list_node);
    _parent_allocator->Deallocate(ptr);
  }
}

size_t palTrackingAllocator::GetSize(void* ptr) const {
  return _parent_allocator->GetSize(ptr);
}

void palTrackingAllocator::ConsoleDump() const {
  palIListForeachDeclare(palTrackedAllocation, list_node) fe(const_cast<palIList*>(&_tracked_allocations));
  if (fe.Finished()) {
    return;
  }
  palPrintf("Dumping leaks from %s\n", GetName());
  while (fe.Finished() == false) {
    palTrackedAllocation* list_entry = fe.GetListEntry();
    palPrintf("%p [%d]\n", list_entry->ptr, GetSize(list_entry->ptr));
    int i = 0;
    while (list_entry->stacktrace[i] != 0) {
      palPrintf("        %d: %s %p\n", i, palDebugLookupSymbol(list_entry->stacktrace[i], MAX_SYMBOL_NAME_LENGTH, _symbol_lookup_buffer), list_entry->stacktrace[i]);
      i++;
    }
    fe.Next();
  }
}
