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

template<class T>
void palSwap(T& a, T& b) {
  T temp(a);
  a = b;
  b = temp;
}

template<class T>
const T& palMin( const T& a, const T& b ) {
  return (a<b)?a:b;
}

template<class T>
const T& palMax( const T& a, const T& b ) {
  return (b<a)?a:b;
}

template<class T>
const T& palClamp(const T& a, const T& min, const T& max) {
  return (a < min) ? min : (a > max) ? max : a;
}

template<class T>
int palBinarySearchLowerBound(const T* data, const int num_data, const T& needle, const T** hit) {
  *hit = NULL;

  int len = num_data;
  int mid = len;
  int offset = 0;
  bool once = false;
  while( mid > 0 ) {
    mid = len >> 1;
    if ( data[offset+mid] < needle ) {
      offset += mid;
      once = true;
    }
    len -= mid;
  }
  if (once) {
    *hit = &data[offset];
    return 0;
  }
  return -1;
}

template<class T>
int palBinarySearchUpperBound(const T* data, const int num_data, const T& needle, const T** hit) {
  *hit = NULL;
  bool once = false;

#if 1
  int len = num_data;
  int mid = len;
  int offset = 0;
  int res = 0;
  while( mid > 0 ) {
    mid = len >> 1;
    if ( data[offset+mid] > needle ) {
      once = true;
      res = 0;
    } else {
      offset += mid;
      res = 1;
    }
    len -= mid;
  }
  offset += res;
#else
  int len = num_data;
  int mid = len;
  int offset = 0;
  bool once = false;
  while( mid > 0 ) {
    mid = len >> 1;
    if ( data[offset+mid] > needle ) {
      offset += mid;
      once = true;
    }
    len -= mid;
  }
#endif
  if (once) {
    *hit = &data[offset];
    return 0;
  }
  return -1;

}

template<class T>
int palBinarySearch(const T* data, const int num_data, const T& needle, const T** hit) {
  int first = 0;
  int last = num_data - 1;

  *hit = NULL;

  if (num_data == 0) {
    return -1;
  }

  while (first <= last) {
    int mid = ((unsigned int)first + (unsigned int)last) >> 1;
    if (needle < data[mid]) {
      last = mid - 1;
    } else if (data[mid] < needle) {
      first = mid+1;	
    } else {
      *hit = &data[mid];
      return 0;
    }
  }
  return -1;
}

template<class T>
int palBinarySearchGT(const T* data, const int num_data, const T& needle) {
  
}

template<typename T>
class palCompareFuncLessThan {
public:
  bool operator()(const T& a, const T& b) {
    return (a < b);
  }
};

template<typename T>
class palCompareFuncLessThanDereference {
public:
  bool operator()(const T& a, const T& b) {
    return (*a < *b);
  }
};

template <class T, typename CompareFuncLessThan>
void palQuickSortInternal(T* buffer, CompareFuncLessThan LessThan, int lo, int hi) {
  //  lo is the lower index, hi is the upper index
  //  of the region of array a that is to be sorted
  int i = lo, j = hi;
  T x= buffer[(lo+hi)/2];

  //  partition
  do {    
    while (LessThan(buffer[i],x)) 
      i++; 
    while (LessThan(x,buffer[j])) 
      j--;
    if (i<=j) {
      T temp = buffer[i];
      buffer[i] = buffer[j];
      buffer[j] = temp;
      i++; j--;
    }
  } while (i<=j);

  //  recursion
  if (lo<j) 
    palQuickSortInternal(buffer, LessThan, lo, j);
  if (i<hi) 
    palQuickSortInternal(buffer, LessThan, i, hi);
}

template <class T, typename CompareFuncLessThan>
void palHeapSortInternal(T *buffer, CompareFuncLessThan LessThan, int k, int n) {
  /* PRE: a[k+1..N] is a heap */
  /* POST:  a[k..N]  is a heap */

  T temp = buffer[k - 1];
  /* k has child(s) */
  while (k <= n/2)  {
    int child = 2*k;

    if ((child < n) && LessThan(buffer[child - 1] , buffer[child])) {
      child++;
    }
    /* pick larger child */
    if (LessThan(temp , buffer[child - 1])) {
      /* move child up */
      buffer[k - 1] = buffer[child - 1];
      k = child;
    } else {
      break;
    }
  }
  buffer[k - 1] = temp;
}

template<class T, typename CompareFuncLessThan>
void palQuickSort(T* buffer, int buffer_size, CompareFuncLessThan LessThan) {
  if (buffer_size > 1) {
    palQuickSortInternal(buffer, LessThan,0,buffer_size-1);
  }
}

template<class T, typename CompareFuncLessThan>
void palHeapSort(T* buffer, int buffer_size, CompareFuncLessThan LessThan) {
  /* sort a[0..N-1],  N.B. 0 to N-1 */
  int k;
  int n = buffer_size;
  for (k = n/2; k > 0; k--)  {
    palHeapSortInternal(buffer, LessThan, k, n);
  }

  /* a[1..N] is now a heap */
  while (n >= 1)  {
    T temp = buffer[0];
    buffer[0] = buffer[n-1];
    buffer[n-1] = temp;

    n = n - 1;
    /* restore a[1..i-1] heap */
    palHeapSortInternal(buffer, LessThan, 1, n);
  } 
}

template<class T>
void palArrayReverse(T* buffer, int buffer_size) {
  int start = 0;
  int end = buffer_size-1;
  while (start<end) {
    palSwap(buffer[start], buffer[end]);
    start++;
    end--;
  }
}

template<class T>
T* palArrayFindFirstOf(T* buffer, int buffer_size, const T& needle) {
  int count = 0;
  while (count < buffer_size) {
    if (buffer[count] == needle) {
      return &buffer[count];
    }
    count++;
  }
  return NULL;
}

template<class T>
T* palArrayFindLastOf(T* buffer, int buffer_size, const T& needle) {
  int count = buffer_size-1;
  while (count >= 0) {
    if (buffer[count] == needle) {
      return &buffer[count];
    }
    count--;
  }
  return NULL;
}

template<class T>
int palArrayCountOf(T* buffer, int buffer_size, const T& needle) {
  int needle_count = 0;
  int count = 0;
  while (count < buffer_size) {
    if (buffer[count] == needle) {
      needle_count++;
    }
    count++;
  }
  return needle_count;
}

template<class T>
void palArrayCopy(T* dest_buffer,const T* src_buffer, int count) {
  int i = 0;
  while (i < count) {
    dest_buffer[i] = src_buffer[i];
  }
}
