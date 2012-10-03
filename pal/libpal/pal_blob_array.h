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

#include "libpal/pal_types.h"
#include "libpal/pal_mem_blob.h"
#include "libpal/pal_errorcode.h"

#define PAL_BLOB_ARRAY_CANT_INIT palMakeErrorCode(PAL_ERROR_CODE_ARRAY_GROUP, 0)
#define PAL_BLOB_ARRAY_ERROR_INVALID_INDEX palMakeErrorCode(PAL_ERROR_CODE_ARRAY_GROUP, 1)
#define PAL_BLOB_ARRAY_ERROR_FULL palMakeErrorCode(PAL_ERROR_CODE_ARRAY_GROUP, 2)

template<typename T, typename INDEX_TYPE = int32_t, uint32_t ALIGNMENT = PAL_ALIGNOF(T)>
class palBlobArray {
  uintptr_t _blob;

  uintptr_t OffsetToBegin() const {
    uintptr_t first = _blob;
    first += sizeof(INDEX_TYPE) * 2;
    first = palAlign(first, ALIGNMENT);
    return first;
  }

  uintptr_t OffsetToEnd() const {
    uintptr_t first = OffsetToBegin();
    first += GetCapacity() * Stride();
    return first;
  }

  uintptr_t OffsetToNth(INDEX_TYPE i) const {
    return OffsetToBegin() + (Stride() * i);
  }

  uintptr_t Stride() const {
    return sizeof(T);
  }

  uintptr_t OffsetToCapacity() const {
    return 0;
  }

  uintptr_t OffsetToSize() const {
    return sizeof(INDEX_TYPE);
  }

  template<typename PTR_TYPE>
  PTR_TYPE* GetPtr(size_t offset) const {
    return reinterpret_cast<PTR_TYPE*>(_blob+offset);
  }

  T* GetPtrToNth(INDEX_TYPE i) {
    return reinterpret_cast<T*>(OffsetToNth(i));
  }

  const T* GetPtrToNth(INDEX_TYPE i) const {
    return reinterpret_cast<const T*>(OffsetToNth(i));
  }

  INDEX_TYPE* GetPtrToCapacity() const {
    return GetPtr<INDEX_TYPE>(OffsetToCapacity());
  }

  INDEX_TYPE* GetPtrToSize() const {
    return GetPtr<INDEX_TYPE>(OffsetToSize());
  }

  void ShiftBufferLeft(INDEX_TYPE start) {
    INDEX_TYPE size = GetSize();
    T* first = begin();
    for (INDEX_TYPE i = start; i < size; i++) {
      first[i-1] = first[i];
    }
  }

  void ShiftBufferRight(INDEX_TYPE start) {
    INDEX_TYPE size = GetSize();
    T* first = begin();
    for (INDEX_TYPE i = size-1; i >= 0 && i >= start; i--) {
      first[i+1] = first[i];
    }
  }

  void FillBuffer(INDEX_TYPE start, INDEX_TYPE stop, const T& element) {
    T* first = begin();
    for (INDEX_TYPE i = start; i < stop; i++) {
      new (&first[i]) T(element);
    }
  }
public:
  /* Types and constants */
  typedef palBlobArray<T, INDEX_TYPE, ALIGNMENT> this_type;
  typedef T element_type;

  palBlobArray() {
    _blob = NULL;
    // sizeof(T) is a multiple of ALIGNMENT
    //palAssert((sizeof(T) % ALIGNMENT) == 0);
  }

  ~palBlobArray() {
  }

  void CallDestructor(int start, int stop) {
    T* first = begin();
    for (int i = start; i < stop; i++) {
      first[i].~T();
    }
  }

  void CallDestructor() {
    CallDestructor(0, GetSize());
  }

  uintptr_t GetBlobAddress() const {
    return _blob;
  }

  INDEX_TYPE GetCapacity() const {
    if (_blob != NULL) {
      return *(GetPtr<INDEX_TYPE>(OffsetToCapacity()));
    } else {
      return 0;
    }
  }

  INDEX_TYPE GetSize() const {
    if (_blob != NULL) {
      return *(GetPtr<INDEX_TYPE>(OffsetToSize()));
    } else {
      return 0;
    }
  }

  bool IsEmpty() const {
    return GetSize() == 0;
  }

  bool IsFull() const {
    return GetSize() == GetCapacity();
  }

  int Init(palMemBlob blob) {
    _blob = reinterpret_cast<uintptr_t>(blob.GetPtr(0));
    if (_blob == NULL || blob.GetBufferSize() < sizeof(INDEX_TYPE) * 2) {
      _blob = NULL;
      return PAL_BLOB_ARRAY_CANT_INIT;
    }
    size_t element_size = sizeof(T);
    size_t capacity = static_cast<size_t>(blob.GetBufferSize());
    uintptr_t first_offset = OffsetToBegin();
    capacity -= first_offset;
    // Our capacity in terms of Ts
    capacity /= Stride();
    *(GetPtr<INDEX_TYPE>(OffsetToCapacity())) = capacity;
    *(GetPtr<INDEX_TYPE>(OffsetToSize())) = 0;
    return 0;
  }

  int Init(void* p, size_t p_size) {
    palMemBlob blob(p, p_size);
    return Init(blob);
  }

  int Init(const palBlobArray& ba) {
    _blob = ba.GetBlobAddress();
    return 0;
  }

  const T* begin() const {
    return reinterpret_cast<const T*>(OffsetToBegin());
  }

  T* begin() {
    return reinterpret_cast<T*>(OffsetToBegin());
  }

  const T* end() const {
    return reinterpret_cast<const T*>(OffsetToEnd());
  }

  T* end() {
    return reinterpret_cast<T*>(OffsetToEnd());
  }

  T& operator[](INDEX_TYPE i) {
    return *GetPtrToNth(i);
  }

  const T& operator[](INDEX_TYPE i) const {
    return *GetPtrToNth(i);
  }

  int RemoveStable(INDEX_TYPE position) {
    if (position < GetSize()) {
      GetPtrToNth(position)->~T();
      ShiftBufferLeft(position+1);
      (*GetPtrToSize())--;
      return 0;
    }
    return PAL_BLOB_ARRAY_ERROR_INVALID_INDEX;
  }

  int Remove(INDEX_TYPE position) {
    if (position < GetSize()) {
      GetPtrToNth(position)->~T();
      (*GetPtrToSize())--;
      *GetPtrToNth(position) = *GetPtrToNth(GetSize());
      return 0;
    }
    return PAL_BLOB_ARRAY_ERROR_INVALID_INDEX;
  }

  int Remove(INDEX_TYPE start, INDEX_TYPE end) {
    T* element = GetPtrToNth(start);
    for (int i = start; i < end; i++) {
      element->~T();
      element++;
    }
    *GetPtrToSize() -= (end - start);
    int num = end - start;
    INDEX_TYPE size = GetSize();
    element = GetPtrToNth(start);
    for (int i = start; i < size; i++) {
      *element = *(element+num);
      element++;
    }
    return 0;
  }

  void Swap(INDEX_TYPE i, INDEX_TYPE j) {
    T* i_ptr = GetPtrToNth(i);
    T* j_ptr = GetPtrToNth(j);
    T temp = *j_ptr;
    *j_ptr = *i_ptr;
    *i_ptr = temp;
  }

  void Clear() {
    if (_blob == NULL) {
      return;
    }
    CallDestructor(0, GetSize());
    *GetPtrToSize() = 0;
  }

  void push_front(const T& element) {
    InsertAtPosition(0, element);
  }

  INDEX_TYPE push_back(const T& element) {
    if (IsFull()) {
      return PAL_BLOB_ARRAY_ERROR_FULL;
    }
    T* buffer = begin();
    new (&buffer[GetSize()]) T(element);
    (*GetPtrToSize())++;
    return GetSize()-1;
  }

  void pop_front() {
    RemoveStable(0);
  }

  void pop_back() {
    (*GetPtrToSize())--;
    T* buffer = begin();
    buffer[GetSize()].~T();
  }

  template <typename CompareFuncLessThan>
  void OrderedInsert(const T& element, CompareFuncLessThan LessThan) {
    int index = 0;
    // find position to insert
    INDEX_TYPE size = GetSize();
    while (index < size && LessThan(operator[](index), element)) {
      index++;
    }
    InsertAtPosition(index, element);
  }

  int InsertAtPosition(int position, const T& element) {
    if (IsFull()) {
      return PAL_BLOB_ARRAY_ERROR_FULL;
    }
    T* buffer = begin();
    if (position <= GetSize()) {
      ShiftBufferRight(position);
      new (&buffer[position]) T(element);
      (*GetPtrToSize())++;
      return 0;
    } else {
      return PAL_BLOB_ARRAY_ERROR_INVALID_INDEX;
    }
  }

  void EraseStable(const T& element) {
    int index = Find(element);
    if (index < GetSize()) {
      RemoveStable(index);
    }
  }

  void EraseAllStable(const T& element) {
    int index = Find(element);
    while (index < GetSize()) {
      RemoveStable(index);
      index = Find(element, index);
    }
  }

  void Erase(const T& element) {
    int index = Find(element);
    if (index < GetSize()) {
      Remove(index);
    }
  }

  void EraseAll(const T& element) {
    int index = Find(element);
    while (index < GetSize()) {
      Remove(index);
      index = Find(element, index);
    }
  }

  INDEX_TYPE Find(const T& element, int start = 0) {
    T* buffer = begin();
    INDEX_TYPE size = GetSize();
    for (int i = start; i < size; i++) {
      if (element == buffer[i])
        return i;
    }
    return size;
  }

  void Resize(INDEX_TYPE new_size, const T& element = T()) {
    if (new_size < GetSize()) {
      CallDestructor(new_size, GetSize());
    } else if (new_size > GetSize()) {
      FillBuffer(GetSize(), new_size, element);
    }
    *GetPtrToSize() = new_size;
  }
};
