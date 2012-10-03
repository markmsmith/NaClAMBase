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

#include "libpal/pal_debug.h"
#include "libpal/pal_ilist.h"
#include "libpal/pal_compacting_allocator.h"

#define ALLOCATED_BIT (0x1)
#define SIZE_MASK (~0x1)
#define MINIMUM_SIZE (4)

struct gcaMemoryChunkHeader {
  uint32_t size;
  palGCAHandle handle;
  palIListNodeDeclare(gcaMemoryChunkHeader, chunk_list);

  bool IsAllocated() {
    return size & ALLOCATED_BIT;
  }

  void MarkAllocated() {
    size |= ALLOCATED_BIT;
  }

  void MarkFree() {
    size &= ~ALLOCATED_BIT;
  }

  uint32_t GetSize() {
    return size & SIZE_MASK;
  }

  void SetSize(uint32_t new_size) {
    uint32_t amask = size & ALLOCATED_BIT;
    size = new_size | amask;
  }
};

static gcaMemoryChunkHeader* HandlePointerToMemoryChunkHeader(void* p) {
  return reinterpret_cast<gcaMemoryChunkHeader*>(reinterpret_cast<unsigned char*>(p) - sizeof(gcaMemoryChunkHeader));
}

uint32_t palCompactingAllocator::GetNextHandle() {
  palGCAHandle handle = ++next_handle_;
  if (handle == 0) {
    handle = ++next_handle_;
  }
  return handle;
}

palCompactingAllocator::palCompactingAllocator() : chunk_list_head() {
  map_count_.Exchange(0);
  next_handle_.Exchange(0);
}

palCompactingAllocator::palCompactingAllocator(uint32_t size, void* memory) : chunk_list_head() {
  map_count_.Exchange(0);
  next_handle_.Exchange(0);
  Create(size, memory);
}

palCompactingAllocator::~palCompactingAllocator() {
  // NOP
}

void palCompactingAllocator::SetAllocator(palAllocatorInterface* allocator) {
  handle_to_address_.SetAllocator(allocator);
}

void palCompactingAllocator::Create(uint32_t size, void* memory) {
  memory_size_ = size;
  memory_ = (unsigned char*)memory;
  gcaMemoryChunkHeader* master_header = reinterpret_cast<gcaMemoryChunkHeader*>(memory);
  master_header->MarkFree();
  master_header->SetSize(size-sizeof(gcaMemoryChunkHeader));
  chunk_list_head.AddHead(&master_header->chunk_list);
}

palGCAHandle palCompactingAllocator::Malloc(uint32_t size) {
  if (size == 0) {
    return NULL;
  }

  // align up
  size = (size + 3) & ~3;

  // make sure that it's 
  if (size < MINIMUM_SIZE) {
    size = MINIMUM_SIZE;
  }

  void* returned_address = NULL;
  gcaMemoryChunkHeader* returned_chunk = NULL;

  // search all chunks_to_compact
  palIListForeachDeclare(gcaMemoryChunkHeader, chunk_list) chunks_iterator(&chunk_list_head);
  while (chunks_iterator.Finished() == false) {
    gcaMemoryChunkHeader* chunk = chunks_iterator.GetListEntry();
    if (chunk->IsAllocated() || chunk->GetSize() < size) {
      // not free or too small, skip
      chunks_iterator.Next();
      continue;
    }

    if (chunk->GetSize() >= size) {
      // found a suitable chunk
      // slice up chunk into allocated and next free section

      // left over amount for free slice
      uint32_t left_over = chunk->GetSize() - size;

      if (left_over < sizeof(*chunk)) {
        // not enough space left over for another header.
        // roll this space into returned block
        size += left_over;
        left_over = 0;

        chunk->MarkAllocated();

        // mark chunk as allocated
        returned_chunk = chunk;
        // chunk + sizeof(header) is returned to user
        returned_address = reinterpret_cast<unsigned char*>(chunk) + sizeof(*chunk);
      } else {
        chunk->SetSize(size);
        chunk->MarkAllocated();
        gcaMemoryChunkHeader* free_chunk = reinterpret_cast<gcaMemoryChunkHeader*>(reinterpret_cast<unsigned char*>(chunk) + sizeof(*chunk) + size);
        free_chunk->MarkFree();
        free_chunk->handle = 0;
        free_chunk->chunk_list = palIListNode();
        free_chunk->SetSize(left_over - sizeof(*chunk));
        chunks_iterator.AddAfter(&free_chunk->chunk_list);
        // mark chunk as allocated
        returned_chunk = chunk;
        // chunk + sizeof(header) is returned to user
        returned_address = reinterpret_cast<unsigned char*>(chunk) + sizeof(*chunk);
      }
      break;
    }
  }

  if (returned_address) {
    palGCAHandle handle = GetNextHandle();
    returned_chunk->handle = handle;
    handle_to_address_.Insert(handle, returned_address);
    return handle;
  }
  return NULL;
}

void palCompactingAllocator::Free(palGCAHandle handle) {
  if (handle == 0) {
    // handle == NULL is NULL..
    return;
  }

  void** ptr_to_address = handle_to_address_.Find(handle);
  if (!ptr_to_address) {
    // Invalid handle
    return;
  }
  void* ptr = *ptr_to_address;
  gcaMemoryChunkHeader* chunk = HandlePointerToMemoryChunkHeader(ptr);
  if (chunk->handle != handle) {
    // chunk from map was wrong
    palBreakHere();
  }

  // erase handle from handle -> address map
  handle_to_address_.Remove(handle);

  gcaMemoryChunkHeader* next_chunk = palIListNodeValue(chunk->chunk_list.next, gcaMemoryChunkHeader, chunk_list);
  gcaMemoryChunkHeader* previous_chunk = palIListNodeValue(chunk->chunk_list.prev, gcaMemoryChunkHeader, chunk_list);

  if (!chunk_list_head.IsRoot(&next_chunk->chunk_list) && !next_chunk->IsAllocated()) {
    // fold next chunk into this chunk
    chunk->SetSize(chunk->GetSize() + next_chunk->GetSize() + sizeof(*chunk));
    // delete next chunk node
    chunk_list_head.Remove(&next_chunk->chunk_list);
  }

  chunk->MarkFree();
  chunk->handle = 0;

  if (!chunk_list_head.IsRoot(&previous_chunk->chunk_list) && !previous_chunk->IsAllocated()) {
    // fold this chunk into previous chunk
    previous_chunk->SetSize(previous_chunk->GetSize() + chunk->GetSize() + sizeof(*chunk));
    chunk_list_head.Remove(&chunk->chunk_list);
  }
}

void* palCompactingAllocator::MapHandle(palGCAHandle handle) {
  void** ptr_to_address = handle_to_address_.Find(handle);
  if (!ptr_to_address) {
    // Invalid handle
    return NULL;
  }
  ++map_count_;
  return *ptr_to_address;
}

void palCompactingAllocator::UnmapHandle(palGCAHandle handle) {
  --map_count_;
  return;
}

void palCompactingAllocator::Compact(uint32_t chunks_to_compact) {
  if (map_count_.Load() != 0) {
    // someone called Compact while holding a mapping
    palBreakHere();
  }

  palIListForeachDeclare(gcaMemoryChunkHeader, chunk_list) chunks_iterator(&chunk_list_head);
  while (chunks_iterator.Finished() == false && chunks_to_compact > 0) {
    gcaMemoryChunkHeader* chunk = chunks_iterator.GetListEntry();
    gcaMemoryChunkHeader* next_chunk = palIListNodeValue(chunk->chunk_list.next, gcaMemoryChunkHeader, chunk_list);
    if (chunk->IsAllocated() == false && chunk_list_head.IsRoot(&next_chunk->chunk_list) == false) {
      // chunk is free and is our target
      // next_chunk is allocated and is going to be moved down, eventually compacting the heap

      // remove next chunk from list
      // this makes chunk->list_node connect with next_chunk->chunk_list->next
      chunk_list_head.Remove(&next_chunk->chunk_list);
      // save the size of the chunk we are going to move
      uint32_t moved_chunk_size = next_chunk->GetSize();
      // save the total size of the area including the moved chunk size the header and the size of the free chunk
      uint32_t free_chunk_size = chunk->GetSize();
      // save the handle of the chunk we are going to move
      palGCAHandle moved_chunk_handle = next_chunk->handle;

      // move the actual data store in next chunk down into target chunk
      palMemoryCopyBytes(reinterpret_cast<unsigned char*>(chunk) + sizeof(*chunk), 
                  reinterpret_cast<unsigned char*>(next_chunk) + sizeof(chunk),
                  next_chunk->GetSize());

      // update handle
      chunk->handle = moved_chunk_handle;
      // mark previously free chunk as allocated
      chunk->MarkAllocated();
      // update chunk size
      chunk->SetSize(moved_chunk_size);
      // update handle -> address map to point to new address
      handle_to_address_.Insert(chunk->handle, reinterpret_cast<unsigned char*>(chunk) + sizeof(*chunk));

      // new free chunk address
      gcaMemoryChunkHeader* new_free_chunk = reinterpret_cast<gcaMemoryChunkHeader*>(reinterpret_cast<unsigned char*>(chunk) + sizeof(*chunk) + chunk->GetSize());
      new_free_chunk->size = 0;
      new_free_chunk->handle = 0;
      new_free_chunk->chunk_list = palIListNode();
      new_free_chunk->SetSize(free_chunk_size);
      chunks_iterator.AddAfter(&new_free_chunk->chunk_list);
      gcaMemoryChunkHeader* after_new_free_chunk = palIListNodeValue(new_free_chunk->chunk_list.next, gcaMemoryChunkHeader, chunk_list);
      if (chunk_list_head.IsRoot(&after_new_free_chunk->chunk_list) == false && after_new_free_chunk->IsAllocated() == false) {
        // need to combine adjacent free chunks
        new_free_chunk->SetSize(new_free_chunk->GetSize() + after_new_free_chunk->GetSize() + sizeof(*chunk));
        chunk_list_head.Remove(&after_new_free_chunk->chunk_list);
      }

      chunks_to_compact--;
    }
    // move to next chunk
    chunks_iterator.Next();
  }
  return;
}

void palCompactingAllocator::DiagnosticDump() {
  palIListForeachDeclare(gcaMemoryChunkHeader, chunk_list) chunks_iterator(&chunk_list_head);
  palPrintf("Memory Base Address = %p\n", memory_);
  palPrintf("Memory size = %x\n", memory_size_);
  palPrintf("sizeof(gcaMemoryChunkHeader) = %x\n", sizeof(gcaMemoryChunkHeader));

  int chunk_id = 0;
  while (chunks_iterator.Finished() == false) {
    gcaMemoryChunkHeader* chunk = chunks_iterator.GetListEntry();
    palPrintf("Chunk [%d]\n", chunk_id);
    palPrintf("&header = %p\n", chunk);
    palPrintf("&user = %p\n", reinterpret_cast<unsigned char*>(chunk) + sizeof(*chunk));
    if (chunk->IsAllocated()) {
      palPrintf("Allocated\n");
    } else {
      palPrintf("Free\n");
    }
    palPrintf("Handle = %d\n", chunk->handle);
    palPrintf("Size = %x\n", chunk->GetSize());
    uintptr_t next_header = (uintptr_t)reinterpret_cast<unsigned char*>(chunk) + sizeof(*chunk) + chunk->GetSize();
    palPrintf("&next_header = %x\n", next_header);
    chunks_iterator.Next();
    chunk_id++;
    if (chunks_iterator.Finished() == false) {
      gcaMemoryChunkHeader* next_chunk = chunks_iterator.GetListEntry();
      if (next_header != (uintptr_t)next_chunk)
      {
        palPrintf("*** BROKEN LINKED LIST\n");
      }
    }
    
  }
  palPrintf("====\n");
}
