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

#include "libpal/dlmalloc/dlmalloc.h"
#include "libpal/pal_heap_allocator.h"
#include "libpal/pal_page_allocator.h"
  
int palHeapAllocator::Create(void* mem, uint64_t size) {
  internal_ = create_mspace_with_base(mem, (size_t)size);
  if (internal_ == NULL) {
    return PAL_HEAP_ALLOCATOR_COULD_NOT_CREATE;
  }
  return 0;
}

int palHeapAllocator::Create(palPageAllocator* page_allocator) {
  internal_ = create_mspace(0, page_allocator);
  if (internal_ == NULL) {
    return PAL_HEAP_ALLOCATOR_COULD_NOT_CREATE;
  }
  mspace_track_large_chunks(internal_, 1);
  return 0;
}

int palHeapAllocator::Destroy() {
  if (internal_) {
    destroy_mspace(internal_);
    internal_ = 0;
  }
  return 0;
}


void* palHeapAllocator::Allocate(size_t size, size_t alignment) {
  void* ptr = mspace_memalign(internal_, alignment, (size_t)size);
  if (ptr) {
    size_t reported_size = mspace_usable_size(ptr);
    ReportMemoryAllocation(ptr, reported_size);
  }
  return ptr;
}

void palHeapAllocator::Deallocate(void* ptr) {
  if (ptr) {
    size_t reported_size = mspace_usable_size(ptr);
    mspace_free(internal_, ptr);
    ReportMemoryDeallocation(ptr, reported_size);
  }
}

size_t palHeapAllocator::GetSize(void* ptr) const {
  return mspace_usable_size(ptr);
}
