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

#include "libpal/pal_debug.h"
#include "libpal/pal_allocator_interface.h"
#include "libpal/pal_memory.h"
#include "libpal/pal_align.h"

#define kpalArrayDefaultGrowthCapacity 2.0f

template <typename T, typename INDEX_TYPE = ssize_t, size_t Alignment = 4>
class palArray {
public:
  /* Types and constants */
  typedef palArray<T, INDEX_TYPE, Alignment> this_type;
  typedef T element_type;
  static const uint32_t element_alignment = Alignment; 
protected:
  palAllocatorInterface* allocator_;
  float growth_factor_;
  T* buffer_;
  int capacity_;
  int size_;
  bool stolen_;

  void ShiftBufferLeft(int start) {
    for (int i = start; i < size_; i++) {
      buffer_[i-1] = buffer_[i];
    }
  }

  void ShiftBufferRight(int start) {
    for (int i = size_-1; i >= 0 && i >= start; i--) {
      buffer_[i+1] = buffer_[i];
    }
  }

  void FillBuffer(int start, int stop, const T& element)
  {
    for (int i = start; i < stop; i++) {
      new (&buffer_[i]) T(element);
    }
  }

  void CallDestructor(int start, int stop) {
    for (int i = start; i < stop; i++) {
      buffer_[i].~T();
    }
  }

  int NextCapacity(int capacity) {
    int ncapacity = (capacity > 0 ? (int)(growth_factor_ * capacity) : 1);
    if (ncapacity <= capacity) {
      /* At least grow by 1 */
      return capacity+1;
    }
    return ncapacity;
  }

  void DeallocateBuffer() {
    if (buffer_ != NULL) {
      allocator_->Deallocate(buffer_);
    }
    buffer_ = NULL;
    capacity_ = 0;
  }

  T* AllocateBuffer(int number) {
    return static_cast<T*>(allocator_->Allocate(number * sizeof(T), this_type::element_alignment));
  }
public:
  /* Copy constructor */
  palArray(const palArray<T>& array) {
    buffer_ = NULL;
    capacity_ = 0;
    size_ = 0;
    stolen_ = false;
    allocator_ = array.allocator_;
    growth_factor_ = array.growth_factor_;
    Reserve(array.GetCapacity());
    Resize(array.GetSize());
    for (int i = 0; i < array.GetSize(); i++) {
      new (&buffer_[i]) T(array[i]);
    }
  }

  /* Assignment operator */
  palArray<T>& operator=(const palArray<T>& array) {
    if (this == &array) return *this; // self assignment

    if (stolen_ == false && size_ > 0) {
      CallDestructor(0, size_);
    }
    if (stolen_ == false && capacity_ > 0) {
      DeallocateBuffer();
    }

    buffer_ = NULL;
    capacity_ = 0;
    size_ = 0;
    stolen_ = false;
    growth_factor_ = array.growth_factor_;
    Reserve(array.GetCapacity());
    Resize(array.GetSize());
    for (int i = 0; i < array.GetSize(); i++) {
      new (&buffer_[i]) T(array[i]);
    }
    return *this;
  }

  palArray() {
    buffer_ = NULL;
    capacity_ = 0;
    size_ = 0;
    stolen_ = false;
    allocator_ = NULL;
    growth_factor_ = kpalArrayDefaultGrowthCapacity;
  }

  ~palArray() {
    if (!stolen_) {
      CallDestructor(0, size_);
      DeallocateBuffer();
    }
  }

  void SetAllocator(palAllocatorInterface* allocator) {
    allocator_ = allocator;
  }

  palAllocatorInterface* GetAllocator() const {
    return allocator_;
  }

  T& operator[](int i) {
    return buffer_[i];
  }

  const T& operator[](int i) const {
    return buffer_[i];
  }

  void SetGrowthFactor(float growth_factor) {
    growth_factor_ = growth_factor;
  }

  T* GetPtr() {
    return buffer_;
  }

  const T* GetConstPtr() const {
    return buffer_;
  }

  T* StealBuffer() {
    stolen_ = true;
    return buffer_;
  }

  int GetCapacity() const {
    return capacity_;
  }

  int GetSize() const {
    return size_;
  }

  bool IsEmpty() const {
    return size_ == 0;
  }

  void EraseStable(const T& element) {
    int index = Find(element);
    if (index < size_) {
      RemoveStable(index);
    }
  }

  void EraseAllStable(const T& element) {
    int index = Find(element);
    while (index < size_) {
      RemoveStable(index);
      index = Find(element, index);
    }
  }

  void Erase(const T& element) {
    int index = Find(element);
    if (index < size_) {
      Remove(index);
    }
  }

  void EraseAll(const T& element) {
    int index = Find(element);
    while (index < size_) {
      Remove(index);
      index = Find(element, index);
    }
  }

  void RemoveStable(int position) {
    if (position < size_) {
      buffer_[position].~T();
      ShiftBufferLeft(position+1);
      size_--;
    }
  }

  void Remove(int start, int end) {
    for (int i = start; i < end; i++) {
      buffer_[i].~T();
      size_--;
    }
    int num = end - start;
    for (int i = start; i < size_; i++) {
      buffer_[i] = buffer_[i+num];
    }
  }

  void Remove(int position) {
    if (position < size_) {
      buffer_[position].~T();
      size_--;
      buffer_[position] = buffer_[size_];
    }
  }

  template <typename CompareFuncLessThan>
  void OrderedInsert(const T& element, CompareFuncLessThan LessThan) {
    int index = 0;
    // find position to insert
    while (index < size_ && LessThan(buffer_[index], element)) {
      index++;
    }
    InsertAtPosition(index, element);
  }

  void InsertAtPosition(int position, const T& element) {
    if (size_+1 >= capacity_) {
      Reserve(NextCapacity(capacity_));
    }

    if (position <= size_) {
      ShiftBufferRight(position);
      new (&buffer_[position]) T(element);
      size_++;
    } else {
      palAssert(false);
    }
  }

  void Swap(int i, int j) {
    T temp = buffer_[j];
    buffer_[j] = buffer_[i];
    buffer_[i] = temp;
  }

  void Clear() {
    CallDestructor(0, size_);
    size_ = 0;
  }

  void Reset() {
    CallDestructor(0, size_);
    DeallocateBuffer();
    capacity_ = 0;
    size_ = 0;
  }

  void Reserve(int new_capacity) {
    if (new_capacity > capacity_)
    {
      /* Growing */
      T* new_elements = AllocateBuffer(new_capacity);
      int i = 0;
      for (i = 0; i < size_; i++) {
        new (&new_elements[i]) T(buffer_[i]);
      }
      DeallocateBuffer();
      capacity_ = new_capacity;
      buffer_ = new_elements;
    }
  }

  void Resize(int new_size, const T& element = T()) {
    if (new_size < size_) {
      CallDestructor(new_size, size_);
      size_ = new_size;
    } else if (new_size > size_) {
      if (new_size > capacity_) {
        Reserve(new_size);
      }
      FillBuffer(size_, new_size, element);
      size_ = new_size;
    }
  }

  int Find(const T& element, int start = 0) {
    for (int i = start; i < size_; i++) {
      if (element == buffer_[i])
        return i;
    }
    return size_;
  }

  bool Contains(const T& element) {
    return Find(element) != size_;
  }

  template <typename CompareFuncLessThan>
  void QuickSort(CompareFuncLessThan LessThan) {
    palQuickSort(&buffer_[0], size_, LessThan);
  }

  template <typename CompareFuncLessThan>
  void HeapSort(CompareFuncLessThan LessThan) {
    palHeapSort(&buffer_[0], size_, LessThan);
  }

  void CopySlice(int start, int end, palArray<T>* target) const {
    for (int i = start; i < end; i++) {
      target->push_back(buffer_[i]);
    }
  }

  void CutSlice(int start, int end, palArray<T>* target) {
    CopySlice(start, end, target);
    Remove(start, end);
  }

  void Append(const palArray<T>& src, int src_start_index, int src_count) {
    int src_end_index = src_start_index + src_count;
    for (int i = src_start_index; i < src_end_index; i++) {
      push_back(src[i]);
    }
  }

  void push_front(const T& element) {
    InsertAtPosition(0, element);
  }

  T& AddTail() {
    if (size_ == capacity_) {
      Reserve(NextCapacity(capacity_));
    }
    new (&buffer_[size_++]) T();

    return buffer_[size_-1];
  }

  int push_back(const T& element) {
    if (size_ == capacity_) {
      Reserve(NextCapacity(capacity_));
    }
    new (&buffer_[size_++]) T(element);
    return size_-1;
  }

  void pop_front() {
    RemoveStable(0);
  }

  void pop_back() {
    size_--;
    buffer_[size_].~T();
  }
};

template <typename T, typename INDEX_TYPE, uint32_t Alignment>
bool operator==(const palArray<T, INDEX_TYPE, Alignment>& A, const palArray<T, INDEX_TYPE, Alignment>& B) {
  if (A.GetSize() != B.GetSize()) {
    return false;
  }

  const int size = A.GetSize();
  for (int i = 0; i < size; i++) {
    if (A[i] != B[i]) {
      return false;
    }
  }

  return true;
}

template <typename T, typename INDEX_TYPE, uint32_t Alignment>
bool operator!=(const palArray<T, INDEX_TYPE, Alignment>& A, const palArray<T, INDEX_TYPE, Alignment>& B) {
  return ! operator==(A,B);
}