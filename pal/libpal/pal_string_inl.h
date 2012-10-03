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

#pragma once

#include "libpal/pal_platform.h"
#include <stdarg.h>
#include "libpal/pal_memory.h"
#include "libpal/pal_algorithms.h"
#include "libpal/pal_debug.h"



template<int PAL_STRING_DEFAULT_CAPACITY>
palString<PAL_STRING_DEFAULT_CAPACITY>::palString(const palString<PAL_STRING_DEFAULT_CAPACITY>& that) {
  str_capacity_ = that.str_capacity_;
  str_len_ = that.str_len_;
  if (that.str_ == &that.default_buffer_[0]) {
    str_ = &default_buffer_[0];
    palStringCopy(&default_buffer_[0], &that.default_buffer_[0]);
  } else {
    str_ = static_cast<char*>(palMalloc(str_capacity_));
    palStringCopy(str_, that.str_);
  }
}

template<int PAL_STRING_DEFAULT_CAPACITY>
palString<PAL_STRING_DEFAULT_CAPACITY>& palString<PAL_STRING_DEFAULT_CAPACITY>::operator = (const palString<PAL_STRING_DEFAULT_CAPACITY>& that) {
  if (this != &that) {
    // clean up any memory we've been using
    if (str_ != &default_buffer_[0]) {
      palFree(str_);
      str_ = &default_buffer_[0];
    }
    str_capacity_ = that.str_capacity_;
    str_len_ = that.str_len_;
    if (that.str_ == &that.default_buffer_[0]) {
      str_ = &default_buffer_[0];
      palStringCopy(&default_buffer_[0], &that.default_buffer_[0]);
    } else {
      str_ = static_cast<char*>(palMalloc(str_capacity_));
      palStringCopy(str_, that.str_);
    }
  }
  return *this;
}

template<int PAL_STRING_DEFAULT_CAPACITY>
template<int OTHER_STRING_CAPACITY>
palString<PAL_STRING_DEFAULT_CAPACITY>& palString<PAL_STRING_DEFAULT_CAPACITY>::operator = (const palString<OTHER_STRING_CAPACITY>& that)
{
  palAssert((void*)this != (void*)&that);
  {
    // clean up any memory we've been using
    if (str_ != &default_buffer_[0]) {
      palFree(str_);
      str_ = &default_buffer_[0];
    }
    str_capacity_ = PAL_STRING_DEFAULT_CAPACITY;
    str_len_ = 0;
    Append(that.str_);
  }
  return *this;
}

template<int PAL_STRING_DEFAULT_CAPACITY>
bool palString<PAL_STRING_DEFAULT_CAPACITY>::operator == (const palString<PAL_STRING_DEFAULT_CAPACITY>& that)
{
  if (this->Length() != that.Length())
    return false;

  int len = this->Length();
  for (int i = 0; i < len; i++)
  {
    if (str_[i] != that.str_[i])
    {
      return false;
    }
  }
  return true;
}

template<int PAL_STRING_DEFAULT_CAPACITY>
template<int OTHER_STRING_CAPACITY>
bool palString<PAL_STRING_DEFAULT_CAPACITY>::operator == (const palString<OTHER_STRING_CAPACITY>& that)
{
  if (this->Length() != that.Length())
    return false;
  int len = this->Length();
  for (int i = 0; i < len; i++)
  {
    if (str_[i] != that.str_[i])
    {
      return false;
    }
  }
  return true;
}

template<int PAL_STRING_DEFAULT_CAPACITY>
bool palString<PAL_STRING_DEFAULT_CAPACITY>::operator != (const palString<PAL_STRING_DEFAULT_CAPACITY>& that)
{
  if (this->Length() != that.Length())
    return true;

  int len = this->Length();
  for (int i = 0; i < len; i++)
  {
    if (str_[i] != that.str_[i])
    {
      return true;
    }
  }
  return false;
}

template<int PAL_STRING_DEFAULT_CAPACITY>
template<int OTHER_STRING_CAPACITY>
bool palString<PAL_STRING_DEFAULT_CAPACITY>::operator != (const palString<OTHER_STRING_CAPACITY>& that)
{
  if (this->Length() != that.Length())
    return true;

  int len = this->Length();
  for (int i = 0; i < len; i++)
  {
    if (str_[i] != that.str_[i])
    {
      return true;
    }
  }
  return false;
}

template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::Set(const char* str) {
  if (this->str_ != str) {
    Truncate(0);
    Append(str);
  }
}

template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::SetSize(int new_size) {
  if (new_size >= str_capacity_) {
    expandIfNeeded(new_size - str_len_);
  }
  str_len_ = new_size;
  str_[new_size] = '\0';
}

template<int PAL_STRING_DEFAULT_CAPACITY>
int palString<PAL_STRING_DEFAULT_CAPACITY>::Truncate(int trunc_len) {
  str_len_ = palMin(str_len_, trunc_len);
  str_[str_len_] = '\0';
  return str_len_;
}

template<int PAL_STRING_DEFAULT_CAPACITY>
const char* palString<PAL_STRING_DEFAULT_CAPACITY>::C() const {
  return str_;
}
template<int PAL_STRING_DEFAULT_CAPACITY>
int palString<PAL_STRING_DEFAULT_CAPACITY>::Length() const {
  return str_len_;
}
template<int PAL_STRING_DEFAULT_CAPACITY>
bool palString<PAL_STRING_DEFAULT_CAPACITY>::Equals(const char* test) const {
  if (!test)
    return false;

  int test_len = palStringLength(test);
  if (str_len_ != test_len) {
    return false;
  }

  for (int i = 0; i < test_len; i++) {
    if (str_[i] != test[i])
      return false;
  }

  return true;
}
template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::Clear() {
  if (str_ != &default_buffer_[0]) {
    palFree(str_);
    str_ = NULL;
  }
  str_len_ = 0;
  str_capacity_ = PAL_STRING_DEFAULT_CAPACITY;
  str_ = &default_buffer_[0];
  str_[0] = '\0';
}

template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::Append(const char* append_str) {
  Insert(-1, append_str);
}
template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::AppendPrintf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  char* temp_string = palStringAllocatingPrintfInternal(format, args);
  Insert(-1, temp_string);
  palFree(temp_string);
  va_end(args);
}
template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::AppendChar(const char ch) {
  InsertChar(-1, ch);
}
template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::AppendLength(const char* append_str, int length) {
  InsertLength(-1, append_str, length);
}
template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::Prepend(const char* prepend_str) {
  Insert(0, prepend_str);
}
template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::PrependPrintf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  char* temp_string = palStringAllocatingPrintfInternal(format, args);
  Insert(0, temp_string);
  palFree(temp_string);
  va_end(args);
}
template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::PrependChar(const char ch) {
  InsertChar(0, ch);
}
template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::PrependLength(const char* prepend_str, int length) {
  InsertLength(0, prepend_str, length);
}
template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::Insert(int position, const char* insert_str) {
  InsertLength(position, insert_str, palStringLength(insert_str));
}
template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::InsertPrintf(int position, const char* format, ...) {
  va_list args;
  va_start(args, format);
  char* temp_string = palStringAllocatingPrintfInternal(format, args);
  Insert(position, temp_string);
  palFree(temp_string);
  va_end(args);
}
template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::InsertChar(int position, const char ch) {
  expandIfNeeded(1);

  if (position < 0)
    position = str_len_;

  if (position > str_len_)
    return;


  /* create gap for new string by
   * moving the tail (read: string->str_len_ - position) of the original string str_len_ bytes forward
   */
  if (position < str_len_) {
    palMemoryCopyBytes(str_ + position + 1, str_ + position, str_len_ - position);
  }

  str_[position] = ch;
  str_len_ += 1;
  str_[str_len_] = '\0';
}
template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::InsertLength(int position, const char* insert_str, int insert_length) {
  if (insert_length == 0)
    return;

  if (position < 0)
    position = str_len_;

  // make sure position is valid
  if (position > str_len_)
    return;

  expandIfNeeded(insert_length);

  /* create gap for new string by
   * moving the tail (read: str_len_ - position) of the original string str_len_ bytes forward
   */
  if (position < str_len_) {
    palMemoryCopyBytes(str_ + position + insert_length, str_ + position, str_len_ - position);
  }

  /* insert new string */
  if (insert_length == 1) {
    // single character
    str_[position] = *insert_str;
  } else {
    // > 1 character
    palMemoryCopyBytes(str_ + position, insert_str, insert_length);
  }

  str_len_ += insert_length;
  str_[str_len_] = 0;
}

template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::expandIfNeeded(int extraLength) {
  if (str_len_ + extraLength >= str_capacity_) {
    resize(palRoundToPowerOfTwo(str_len_ + extraLength + 1));
  }
}
template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::resize(int newSize) {
  palAssert(newSize >= str_capacity_);  // we should only ever grow palStringClear() can shrink
  str_capacity_ = newSize;
  char* newbuff = static_cast<char*>(palMalloc(str_capacity_));
  str_[str_len_] = 0;  // make sure it is zero filled
  palStringCopy(newbuff, str_);  // copy old str_ into newbuffer
  if (str_ != &default_buffer_[0]) {
    palFree(str_);
  }
  str_ = newbuff;
}

template<int PAL_STRING_DEFAULT_CAPACITY>
void palString<PAL_STRING_DEFAULT_CAPACITY>::Remove(int position, int remove_length) {
  if (remove_length < 0) {
    str_len_ = str_len_ - position;
  } else {
    if (position + remove_length > str_len_)
      return;

    if (position + remove_length < str_len_)
      palMemoryCopyBytes(str_ + position, str_ + position + remove_length, str_len_ - (position + remove_length));
  }
  str_len_ -= remove_length;
  str_[str_len_] = 0;
}
