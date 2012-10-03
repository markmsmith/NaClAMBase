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

#ifndef LIBPAL_PAL_MIN_HEAP_H__
#define LIBPAL_PAL_MIN_HEAP_H__

#include "libpal/pal_array.h"
#include "libpal/pal_algorithms.h"

template <typename T, typename CompareFuncLessThan = palCompareFuncLessThan<T>, uint32_t Alignment = PAL_ALIGNOF(T)>
class palMinHeap
{
public:
  /* Types and constants */
  typedef palMinHeap<T, CompareFuncLessThan, Alignment> this_type;
  typedef T element_type;
  static const uint32_t element_size = sizeof(T);
  static const uint32_t element_alignment = Alignment; 
protected:
  palArray<T, int32_t, Alignment> array_;
  CompareFuncLessThan LessThan_;

  /* Heap helpers */
	void HeapDown (int node) {
		while (!(node >= (array_.GetSize()/2) && (node < array_.GetSize()))) {
			int child = node * 2 + 1;
      if (child < array_.GetSize() - 1 && LessThan_(array_[child+1], array_[child]))
			{
				child++;
			}

			if (LessThan_(array_[child], array_[node])) {
        array_.Swap(node, child);
				node = child;
			} else {
				return;
			}
		}
	}

	void HeapUp (int node) {
		while (node > 0) {
      int parent = node / 2;
      if (LessThan_(array_[node],array_[parent]))
      {
        array_.Swap(node, parent);
        node = parent;
      } else {
        return;
      }
		}
	}

public:

  palMinHeap() : array_(), LessThan_(CompareFuncLessThan()) {

  }

  palMinHeap(int capacity) : array_(capacity), LessThan_(CompareFuncLessThan()) {
  }

  palMinHeap(const palMinHeap<T>& mh) : array_(mh.array_), LessThan_(mh.LessThan_) {

  }

  void SetAllocator(palAllocatorInterface* allocator) {
    array_.SetAllocator(allocator);
  }

  palAllocatorInterface* GetAllocator() const {
    return array_.GetAllocator();
  }
  palMinHeap<T>& operator=(const palMinHeap<T>& rhs) {
    array_ = rhs.array_;
    LessThan_ = rhs.LessThan_;
    return *this;
  }

  void Insert(const T& element) {
    array_.push_back(element);
    HeapUp(array_.GetSize()-1);
  }

  T& FindMin() {
    return array_[0];
  }

  const T& FindMin() const {
    return array_[0];
  }

  void DeleteMin() {
    array_.Swap(0, array_.GetSize()-1);
    array_.pop_back();
    if (array_.GetSize() != 0)
      HeapDown(0);
  }

  bool IsEmpty() {
    return array_.IsEmpty();
  }

  void Clear() {
    array_.Clear();
  }

  void Reset() {
    array_.Reset();
  }
};


#endif  // LIBPAL_PAL_MIN_HEAP_H__