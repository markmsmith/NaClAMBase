#include "libpal/pal_page_allocator.h"

#define PAGE_ALLOCATOR_MAGIC 0xdeadbeef

palPageAllocator::palPageAllocator() : palAllocatorInterface("System Memory Page Allocator") {
#if defined(PAL_PLATFORM_WINDOWS)
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  _page_size = si.dwPageSize;
#else
#endif
}

void* palPageAllocator::Allocate(size_t size, size_t alignment) {
#if defined(PAL_PLATFORM_WINDOWS)
  palAssert(alignment == _page_size);
  palAssert((size & (alignment-1)) == 0);
  unsigned char* p = NULL;
  p = (unsigned char*)VirtualAlloc(NULL, (SIZE_T)(size+_page_size), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (p == NULL) {
    DWORD e = GetLastError();
    palPrintf("Error = %d,%x\n", e, e);
  }
  palAssert(p != NULL);
  {
    *((uint32_t*)p) = PAGE_ALLOCATOR_MAGIC;
    *((size_t*)(p + sizeof(uint32_t))) = size;
  }
  ReportMemoryAllocation(p+_page_size, size+_page_size);
  return p+_page_size;
#else
  return NULL;
#endif
}

void palPageAllocator::Deallocate(void* ptr) {
#if defined(PAL_PLATFORM_WINDOWS)
  unsigned char* p = reinterpret_cast<unsigned char*>(ptr);
  p -= _page_size;
  uint32_t magic = *((uint32_t*)p);
  p += 4;
  size_t size = *((size_t*)p);
  palAssert(magic == PAGE_ALLOCATOR_MAGIC);
  p -= 4;
  VirtualFree(p, 0, MEM_RELEASE);
  ReportMemoryDeallocation(ptr, size+_page_size);
#else
#endif
}

size_t palPageAllocator::GetSize(void* ptr) const {
#if defined(PAL_PLATFORM_WINDOWS)
  unsigned char* p = reinterpret_cast<unsigned char*>(ptr);
  p -= _page_size;
  uint32_t magic = *((uint32_t*)p);
  p += 4;
  size_t size = *((size_t*)p);
  palAssert(magic == PAGE_ALLOCATOR_MAGIC);
  return size;
#else
  return 0;
#endif
}

size_t palPageAllocator::GetPageSize() const {
#if defined(PAL_PLATFORM_WINDOWS)  
  return _page_size;
#else
  return 0;
#endif
}

#define MAX_SIZE_T (~(size_t)0)
#define MFAIL ((void*)(MAX_SIZE_T))
void* palPageAllocator::mmap(size_t size) {
#if defined(PAL_PLATFORM_WINDOWS)  
  void* ptr = VirtualAlloc(NULL, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  if (ptr) {
    ReportMemoryAllocation(ptr, size);
    return ptr;
  } else {
    return MFAIL;
  }
#endif
  return MFAIL;
}

int palPageAllocator::munmap(void* ptr, size_t size) {
#if defined(PAL_PLATFORM_WINDOWS)
  MEMORY_BASIC_INFORMATION minfo;
  char* cptr = (char*)ptr;
  while (size) {
    if (VirtualQuery(cptr, &minfo, sizeof(minfo)) == 0)
      return -1;
    if (minfo.BaseAddress != cptr || minfo.AllocationBase != cptr ||
      minfo.State != MEM_COMMIT || minfo.RegionSize > size)
      return -1;
    if (VirtualFree(cptr, 0, MEM_RELEASE) == 0)
      return -1;
    ReportMemoryDeallocation(cptr, minfo.RegionSize);
    cptr += minfo.RegionSize;
    size -= minfo.RegionSize;
  }
#endif
  return 0;
}
