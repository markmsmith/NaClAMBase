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

#include "libpal/pal_errorcode.h"
#include "libpal/pal_allocator_interface.h"
#include "libpal/pal_page_allocator.h"

class palProxyAllocator : public palAllocatorInterface {
  palAllocatorInterface* _target_allocator;
public:

  palProxyAllocator(const char* proxy_name, palAllocatorInterface* target_allocator) : palAllocatorInterface(proxy_name), _target_allocator(target_allocator) {
  }

  void SetTargetAllocator(palAllocatorInterface* target_allocator) {
    _target_allocator = target_allocator;
  }

  virtual void* Allocate(size_t size, size_t alignment  = 8) {
    void* p = _target_allocator->Allocate(size, alignment);
    if (p) {
      size_t size_p = _target_allocator->GetSize(p);
      ReportMemoryAllocation(p, size_p);
    }
    return p;
  }

  virtual void Deallocate(void* ptr) {
    if (ptr) {
      size_t size_p = _target_allocator->GetSize(ptr);
      _target_allocator->Deallocate(ptr);
      ReportMemoryDeallocation(ptr, size_p);
    }
  }

  virtual size_t GetSize(void* ptr) const {
    return _target_allocator->GetSize(ptr);
  }
};