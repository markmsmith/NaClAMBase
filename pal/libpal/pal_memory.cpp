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

#include <string.h>
#include <stdlib.h>

#include "libpal/pal_debug.h"
#include "libpal/pal_align.h"
#include "libpal/pal_allocator.h"
#include "libpal/pal_memory.h"

void palMemoryCopyBytes(void* destination, const void* source, size_t bytes) {
  uintptr_t d_start = reinterpret_cast<uintptr_t>(destination);
  uintptr_t d_end = d_start + (uintptr_t)bytes;
  uintptr_t s_start = reinterpret_cast<uintptr_t>(source);
  uintptr_t s_end = s_start + (uintptr_t)bytes;
  if ((s_start >= d_start && s_start < d_end) ||
    (d_start >= s_start && d_start < s_end)) {
    // regions overlap
    memmove(destination, source, (size_t)bytes);
  } else {
    memcpy(destination, source, (size_t)bytes);
  }
}

void palMemoryZeroBytes(void* destination, size_t bytes) {
  memset(destination, 0, (size_t)bytes);
}

void palMemorySetBytes(void* destination, unsigned char byte, size_t bytes) {
  memset(destination, byte, (size_t)bytes);
}

size_t palRoundToPowerOfTwo(size_t x) {
  const size_t start = 1;
  size_t n = start;

  while (n < x) {
    n <<= 1;
  }

  return n;
}

// globally override new operator
void* operator new(size_t size, palAllocatorInterface* allocator) {
  void* p = allocator->Allocate(size, 8);
  return p;
}

void* operator new(size_t size, palAllocatorInterface* allocator, size_t alignment) {
  void* p = allocator->Allocate(size, alignment);
  return p;
}

void operator delete(void* p, palAllocatorInterface* allocator) {
  allocator->Deallocate(p);
}

/*
void* operator new(size_t size) { 
  return g_StdProxyAllocator->Allocate(size, 16);
}

void* operator new[](size_t size) {
  return g_StdProxyAllocator->Allocate(size, 16);
}

void operator delete[](void* p) {
  g_StdProxyAllocator->Deallocate(p);
}

void operator delete(void* p) {
  g_StdProxyAllocator->Deallocate(p);
}*/

void* operator new(size_t size, void* p) {
  return p;
}