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

#include "libpal/pal_memory.h"
#include "libpal/pal_errorcode.h"
class palAllocatorInterface;
//#include "libpal/pal_allocator.h"

#define PAL_MEM_BLOB_NO_ROOM palMakeErrorCode(PAL_ERROR_CODE_BLOB_GROUP, 1)
#define PAL_MEM_BLOB_NO_DATA palMakeErrorCode(PAL_ERROR_CODE_BLOB_GROUP, 2)

struct palMemBlob {
protected:
  void* buffer;
  size_t buffer_size; // in bytes
public:
  palMemBlob() {
    buffer_size = 0;
    buffer = NULL;
  }

  ~palMemBlob() {
    buffer = NULL;
    buffer_size = 0;
  }

  palMemBlob(void* buffer, size_t buffer_len) {
    this->buffer = buffer;
    this->buffer_size = buffer_len;
  }

  template<typename T>
  T* GetPtr(size_t byte_offset = 0) const {
    uintptr_t addr = reinterpret_cast<uintptr_t>(buffer);
    return reinterpret_cast<T*>(addr+byte_offset);
  }

  void* GetPtr(size_t byte_offset = 0) const {
    uintptr_t addr = reinterpret_cast<uintptr_t>(buffer);
    return reinterpret_cast<void*>(addr+byte_offset);
  }

  size_t GetBufferSize() const {
    return buffer_size;
  }

  void Reset() {
    buffer_size = 0;
    buffer = NULL;
  }
};

template<typename T>
struct palTypeBlob {
  T* elements;
  size_t size;

  palTypeBlob<T>() {
    elements = NULL;
    size = 0;
  }

  palTypeBlob<T>(T* e, size_t num_e) {
    elements = e;
    size = num_e;
  }
};

class palGrowingMemoryBlob {
  void* buffer;
  size_t buffer_capacity;
  size_t buffer_size;
  palAllocatorInterface* allocator_;
public:
  palGrowingMemoryBlob() {
    buffer = NULL;
    buffer_size = 0;
    buffer_capacity = 0;
    allocator_ = NULL;
  }
  palGrowingMemoryBlob(palAllocatorInterface* allocator) {
    buffer = NULL;
    buffer_capacity = 0;
    buffer_size = 0;
    allocator_ = allocator;
  }

  void SetAllocator(palAllocatorInterface* allocator) {
    allocator_ = allocator;
  }

  ~palGrowingMemoryBlob() {
    if (allocator_ != NULL && buffer != NULL) {
      allocator_->Deallocate(buffer);
    } 
  }

  void IncreaseCapacity(size_t new_capacity) {
    if (buffer_capacity == 0) {
      // initial growth
      buffer = allocator_->Allocate(new_capacity);
      buffer_capacity = new_capacity;
    } else if (new_capacity > buffer_capacity) {
      // create new buffer
      void* new_buffer = allocator_->Allocate(new_capacity);
      // copy old buffer into new buffer
      palMemoryCopyBytes(new_buffer, buffer, buffer_size);
      // free old buffer
      allocator_->Deallocate(buffer);

      buffer = new_buffer;
      buffer_capacity = new_capacity;
    }
  }

  void SetSize(size_t new_size) {
    if (new_size > buffer_capacity) {
      IncreaseCapacity(new_size);
    }
    buffer_size = new_size;
  }

  size_t Append(void* data, size_t data_size) {
    if (buffer_size + data_size >= buffer_capacity) {
      // need more room in buffer
      size_t new_capacity = 2*buffer_capacity;
      if (new_capacity == 0) {
        new_capacity = 64;
      }
      while (new_capacity <= (buffer_size+data_size)) {
        new_capacity *= 2;
      }
      IncreaseCapacity(new_capacity);
    }
    uintptr_t target_address = reinterpret_cast<uintptr_t>(buffer) + (uintptr_t)buffer_size;
    palMemoryCopyBytes((void*)target_address, data, data_size);
    buffer_size += data_size;
    uintptr_t base_address = reinterpret_cast<uintptr_t>(buffer);
    return target_address - base_address;
  }

  void Clear() {
    buffer_size = 0;
  }

  void Reset(bool deallocate_buffer = true) {
    if (allocator_ != NULL && buffer != NULL && deallocate_buffer == false) {
      allocator_->Deallocate(buffer);
    }
    buffer = NULL;
    buffer_capacity = 0;
    buffer_size = 0;
  }

  void* GetPtr(size_t offset) {
    uintptr_t byte_address = reinterpret_cast<uintptr_t>(buffer);
    return reinterpret_cast<void*>(byte_address+offset);
  }

  size_t GetBufferSize() const {
    return buffer_size;
  }

  size_t GetBufferCapacity() const {
    return buffer_capacity;
  }

  void* GetPtr() {
    return buffer;
  }

  void GetBlob(palMemBlob* blob) const {
    *blob = palMemBlob(buffer, buffer_size);
  }
};

class palRingBlob {
  void* _buffer;
  uint8_t* _read_pointer;
  size_t _buffer_capacity;
  size_t _buffer_size;

  void Read(void* data, size_t data_size) {
    uint8_t* _start = reinterpret_cast<uint8_t*>(_buffer);
    uintptr_t read_offset = _read_pointer - _start;
    if (read_offset + data_size > _buffer_capacity) {
      // The read must be broken into two parts
      uintptr_t slop = (read_offset + (uintptr_t)data_size) - (uintptr_t)_buffer_capacity;
      uintptr_t first = (uintptr_t)data_size - slop;
      uintptr_t data_offset = reinterpret_cast<uintptr_t>(data);
      data_offset += first;
      palMemoryCopyBytes(data, _read_pointer, first);
      palMemoryCopyBytes(reinterpret_cast<void*>(data_offset), _buffer, slop);
    } else {
      // The read can be done in one
      palMemoryCopyBytes(data, _read_pointer, data_size);
    }
  }

  void Write(const void* data, size_t data_size) {
    uint8_t* _write_start = reinterpret_cast<uint8_t*>(MakeWritePointer());
    uint8_t* _start = reinterpret_cast<uint8_t*>(_buffer);
    uintptr_t write_offset = _write_start - _start;
    if (write_offset + data_size > _buffer_capacity) {
      // The write must be broken into two parts
      uintptr_t slop = (write_offset + (uintptr_t)data_size) - (uintptr_t)_buffer_capacity;
      uintptr_t first = (uintptr_t)data_size - slop;
      uintptr_t data_offset = reinterpret_cast<uintptr_t>(data);
      data_offset += first;
      palMemoryCopyBytes(_write_start, data, first);
      palMemoryCopyBytes(_buffer, reinterpret_cast<const void*>(data_offset), slop);
    } else {
      // The write can be done in one
      palMemoryCopyBytes(_write_start, data, data_size);
    }
  }

  void* MakeWritePointer() {
    size_t offset = _buffer_size % _buffer_capacity;
    uintptr_t start = reinterpret_cast<uintptr_t>(_read_pointer);
    return reinterpret_cast<void*>(start+offset);
  }

  void MoveReadPointer(size_t bytes) {
    size_t read_offset = _read_pointer - reinterpret_cast<uint8_t*>(_buffer);
    read_offset += bytes;
    read_offset = read_offset % _buffer_capacity;
    _read_pointer = reinterpret_cast<uint8_t*>(_buffer) + read_offset;
  }
public:
  palRingBlob() {
    _buffer = NULL;
    _read_pointer = NULL;
    _buffer_size = 0;
    _buffer_capacity = 0;
  }

  ~palRingBlob() {
  }

  palRingBlob(void* buffer, size_t buffer_capacity) {
    Reset(buffer, buffer_capacity);
  }

  palRingBlob(const palMemBlob& blob) {
    Reset(blob.GetPtr(), blob.GetBufferSize());
  }

  size_t GetCapacity() const {
    return _buffer_capacity;
  }

  size_t GetSize() const {
    return _buffer_size;
  }

  size_t GetAvailableSize() const {
    return _buffer_capacity - _buffer_size;
  }

  bool IsFull() const {
    return _buffer_size == _buffer_capacity;
  }

  bool IsEmpty() const {
    return _buffer_size == 0;
  }

  void Reset(void* buffer, size_t buffer_capacity) {
    _buffer = buffer;
    _buffer_capacity = buffer_capacity;
    _read_pointer = (uint8_t*)_buffer;
    _buffer_size = 0;
  }

  void Clear() {
    _buffer_size = 0;
    _read_pointer = reinterpret_cast<uint8_t*>(_buffer);
  }

  int Append(const void* data, size_t data_size) {
    if (_buffer_size + data_size > _buffer_capacity) {
      return PAL_MEM_BLOB_NO_ROOM;
    }
    Write(data, data_size);
    _buffer_size += data_size;
    return 0;
  }

  template<typename T>
  int Append(const T* item) {
    return Append(reinterpret_cast<const void*>(item), sizeof(T));
  }

  template<typename T>
  int AppendN(const T* items, int item_count) {
    return Append(reinterpret_cast<const void*>(items), sizeof(T)*item_count);
  }

  int Consume(void* data, size_t data_size) {
    if (_buffer_size < data_size) {
      return PAL_MEM_BLOB_NO_DATA;
    }
    Read(data, data_size);
    _buffer_size -= data_size;
    MoveReadPointer(data_size);
    return 0;
  }

  template<typename T>
  int Consume(T* item) {
    return Consume(reinterpret_cast<void*>(item), sizeof(T));
  }

  template<typename T>
  int Consume(T* item, int item_count) {
    return Consume(reinterpret_cast<void*>(item), sizeof(T)*item_count);
  }

  int Peek(void* data, size_t data_size) {
    if (_buffer_size < data_size) {
      return PAL_MEM_BLOB_NO_DATA;
    }
    Read(data, data_size);
    return 0;
  }

  template<typename T>
  int Peek(T* item) {
    return Peek(reinterpret_cast<void*>(item), sizeof(T));
  }

  template<typename T>
  int PeekN(T* item, int item_count) {
    return Peek(reinterpret_cast<void*>(item), sizeof(T)*item_count);
  }

  int Skip(size_t bytes) {
    if (_buffer_size < bytes) {
      return PAL_MEM_BLOB_NO_DATA;
    }
    MoveReadPointer(bytes);
    _buffer_size -= bytes;
    return 0;
  }

  int MoveWritePointer(size_t bytes) {
    if (_buffer_size + bytes >= _buffer_capacity) {
      return PAL_MEM_BLOB_NO_ROOM;
    }
    _buffer_size += bytes;
    return 0;
  }

  void* GetReadPointer() {
    return _read_pointer;
  }

  void* GetWritePointer() {
    return MakeWritePointer();
  }

  template<typename T>
  T* GetReadPointer() {
    return reinterpret_cast<T*>(_read_pointer);
  }

  template<typename T>
  T* GetWritePointer() {
    return reinterpret_cast<T*>(MakeWritePointer());
  }

  size_t GetConsecutiveReadBlockSize() {
    size_t data_size = GetSize();
    uint8_t* _start = reinterpret_cast<uint8_t*>(_buffer);
    uintptr_t read_offset = _read_pointer - _start;
    if (read_offset + data_size > _buffer_capacity) {
      // The read must be broken into two parts
      uintptr_t slop = (read_offset + (uintptr_t)data_size) - (uintptr_t)_buffer_capacity;
      uintptr_t first = (uintptr_t)data_size - slop;
      return first;
    } else {
      return data_size;
    }
  }

  size_t GetConsecutiveWriteBlockSize() {
    size_t data_size = GetCapacity()-GetSize();
    uint8_t* _write_start = reinterpret_cast<uint8_t*>(MakeWritePointer());
    uint8_t* _start = reinterpret_cast<uint8_t*>(_buffer);
    uintptr_t write_offset = _write_start - _start;
    if (write_offset + data_size > _buffer_capacity) {
      // The write must be broken into two parts
      uintptr_t slop = (write_offset + (uintptr_t)data_size) - (uintptr_t)_buffer_capacity;
      uintptr_t first = (uintptr_t)data_size - slop;
      return first;
    } else {
      return data_size;
    }
  }
};

class palAppendChopBlob {
  uintptr_t _buffer;
  size_t _buffer_capacity;
  size_t _buffer_size;
public:
  palAppendChopBlob() {
    _buffer = 0;
    _buffer_capacity = 0;
    _buffer_size = 0;
  }

  palAppendChopBlob(void* buffer, size_t buffer_capacity) {
    Reset(buffer, buffer_capacity);
  }

  size_t GetCapacity() const {
    return _buffer_capacity;
  }

  size_t GetSize() const {
    return _buffer_size;
  }

  size_t GetAvailableSize() const {
    return _buffer_capacity - _buffer_size;
  }

  bool IsFull() const {
    return _buffer_size == _buffer_capacity;
  }

  bool IsEmpty() const {
    return _buffer_size == 0;
  }

  void Reset(void* buffer, size_t buffer_capacity) {
    _buffer = reinterpret_cast<uintptr_t>(buffer);
    _buffer_capacity = buffer_capacity;
    _buffer_size = 0;
  }

  void Clear() {
    _buffer_size = 0;
  }

  int Append(const void* data, size_t data_size) {
    if (_buffer_size + data_size >= _buffer_capacity) {
      return PAL_MEM_BLOB_NO_ROOM;
    }
    palMemoryCopyBytes(reinterpret_cast<void*>(_buffer+_buffer_size), data, data_size);
    _buffer_size += data_size;
    return 0;
  }

  template<typename T>
  int Append(const T* item) {
    return Append(reinterpret_cast<const void*>(item), sizeof(T));
  }

  template<typename T>
  int Append(const T* items, int item_count) {
    return Append(reinterpret_cast<const void*>(items), sizeof(T)*item_count);
  }

  int Chop(size_t size) {
    if (size > _buffer_size) {
      return PAL_MEM_BLOB_NO_DATA;
    }
    size_t left_over = _buffer_size - size;
    palMemoryCopyBytes(reinterpret_cast<void*>(_buffer), reinterpret_cast<void*>(_buffer+size), left_over);
    _buffer_size = left_over;
    return 0;
  }

  template<typename T>
  T* GetPtr(size_t byte_offset) const {
    return reinterpret_cast<T*>(_buffer+byte_offset);
  }

  void* GetPtr(size_t byte_offset) const {
    return reinterpret_cast<void*>(_buffer+byte_offset);
  }

  template<typename T>
  T* GetAppendPtr() const {
    return reinterpret_cast<T*>(_buffer+_buffer_size);
  }

  void* GetAppendPtr() const {
    return reinterpret_cast<void*>(_buffer+_buffer_size);
  }

  int MoveAppendPtr(size_t bytes) {
    if (bytes + _buffer_size >= _buffer_capacity) {
      return PAL_MEM_BLOB_NO_ROOM;
    }

    _buffer_size += bytes;
    return 0;
  }
};
