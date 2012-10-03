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
#include "libpal/pal_page_allocator.h"
#include "libpal/pal_heap_allocator.h"
#include "libpal/pal_proxy_allocator.h"
#include "libpal/pal_tracking_allocator.h"
#include "libpal/pal_system_heap_allocator.h"
#include "libpal/pal_array.h"

extern palAllocatorInterface* g_PageAllocator; // page allocator
extern palAllocatorInterface* g_DefaultHeapAllocator; // default heap
extern palAllocatorInterface* g_StdProxyAllocator; // Calls to new, new[], etc go through here
extern palAllocatorInterface* g_StringProxyAllocator; // string related allocations
extern palAllocatorInterface* g_FileProxyAllocator; // palFile related allocations
extern palAllocatorInterface* g_OpticsAllocator;
extern palAllocatorInterface* g_FontAllocator;

int palAllocatorInit();
int palAllocatorShutdown();

struct palTrackedAllocator {
  palArray<palTrackedAllocator> children;
  palAllocatorInterface* allocator;
  palAllocatorInterface* parent;
  palTrackedAllocator() {
    allocator = NULL;
    parent = NULL;
  }
  ~palTrackedAllocator() {
    children.Reset();
  }
};

class palJSONBuilder;

class palAllocatorTracker {
  palTrackedAllocator _root;
  palAllocatorInterface* _allocator;

  palTrackedAllocator* AddAllocator(palTrackedAllocator* root, palAllocatorInterface* allocator);
  palTrackedAllocator* LocateParentAllocator(palTrackedAllocator* root, palAllocatorInterface* allocator);
  void ConsoleDump(int level, palTrackedAllocator* root);
  void GetStats(palJSONBuilder* builder, palTrackedAllocator* root, bool* first);
public:
  palAllocatorTracker();
  ~palAllocatorTracker();

  void SetAllocator(palAllocatorInterface* allocator);

  void RegisterAllocator(palAllocatorInterface* allocator, palAllocatorInterface* parent_allocator);

  void ConsoleDump();
  void GetStats(palJSONBuilder* builder);
};

extern palAllocatorTracker* g_AllocatorTracker;