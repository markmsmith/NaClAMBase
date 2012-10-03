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

#ifndef LIBPAL_PAL_COMPACTING_ALLOCATOR_H_
#define LIBPAL_PAL_COMPACTING_ALLOCATOR_H_

#include "libpal/pal_platform.h"
#include "libpal/pal_hash_map.h"

#include "libpal/pal_types.h"
#include "libpal/pal_ilist.h"

#include "libpal/pal_atomic.h"

typedef uint32_t palGCAHandle;

class palCompactingAllocator {
 private:
  PAL_DISALLOW_COPY_AND_ASSIGN(palCompactingAllocator);

  palAtomicInt32 map_count_;
  palAtomicInt32 next_handle_;

  uint32_t GetNextHandle();

  unsigned char* memory_;
  uint32_t memory_size_;
  uint32_t memory_available_;

  /* handle -> address */
  palHashMap<palGCAHandle, void*> handle_to_address_;

  /* list of memory chunks */
  palIList chunk_list_head;
 public:
  palCompactingAllocator();
  palCompactingAllocator(uint32_t memory_size, void* memory);
  ~palCompactingAllocator();

  void SetAllocator(palAllocatorInterface* allocator);
  void Create(uint32_t size, void* memory);

  palGCAHandle Malloc(uint32_t size);
  void Free(palGCAHandle handle);

  void* MapHandle(palGCAHandle handle);
  void UnmapHandle(palGCAHandle handle);

  void Compact(uint32_t chunks_to_compact);

  void DiagnosticDump();
};

#endif  // LIBPAL_PAL_COMPACTING_ALLOCATOR_H_
