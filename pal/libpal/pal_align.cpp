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

#include <stdlib.h>
#include "libpal/pal_debug.h"
#include "libpal/pal_align.h"

void* palAlign(void* ptr, uint32_t alignment) {
  return (void*)(((uintptr_t)((char*)ptr+alignment-1)) & ~(alignment-1));
}

uintptr_t palAlign(uintptr_t x, uintptr_t alignment) {
  if (alignment == 0) {
    return x;
  }
  uintptr_t step0 = x+alignment-1;
  uintptr_t step1 = ~(alignment-1);
  uintptr_t step2 = step0 & step1;
  return step2;
}


bool palIsAligned(void* ptr, uint32_t alignment) {
  if ((uintptr_t)ptr & (alignment-1))
  {
    return false;
  }
  return true;
}

bool palIsAligned4(void* ptr) {
  return palIsAligned(ptr, 4);
}

bool palIsAligned16(void* ptr) {
  return palIsAligned(ptr, 16);
}

bool palIsAligned128(void* ptr) {
  return palIsAligned(ptr, 128);
}

