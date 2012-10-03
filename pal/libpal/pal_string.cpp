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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "libpal/pal_platform.h"
#include "libpal/pal_algorithms.h"
#include "libpal/pal_string.h"
#include "libpal/pal_memory.h"
#include "libpal/pal_debug.h"
#include "libpal/pal_allocator.h"

bool    palIsAlpha(char ch) {
  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

bool    palIsDigit(char ch) {
  return ch >= '0' && ch <= '9';
}

bool    palIsHexDigit(char ch) {
  return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

bool  palIsWhiteSpace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

int palDigitValue(char ch) {
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  return 0;
}

int palHexDigitValue(char ch) {
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  else if (ch >= 'a' && ch <= 'f')
    return 10 + (ch - 'a');
  else if (ch >= 'A' && ch <= 'F')
    return 10 + (ch - 'A');
  return 0;
}

size_t palStringLength(const char* str) {
  if (!str || *str == '\0')
    return 0;
  return (int)strlen(str);
}

char* palStringDuplicate(const char* str) {
  if (!str)
    return NULL;

  char* new_str = (char*)g_StringProxyAllocator->Allocate(palStringLength(str)+1);
  palStringCopy(new_str, str);
  return new_str;
}

void palStringDuplicateDeallocate(const char* str) {
  g_StringProxyAllocator->Deallocate((void*)str);
}

void palStringCopy(char* dest, const char* src) {
  if (src == NULL || dest == NULL) {
    return;
  }
#if defined(PAL_COMPILER_MICROSOFT)
#pragma warning(push)
#pragma warning(disable : 4996)
  strcpy(dest, src);
#pragma warning(pop)
#else
  strcpy(dest, src);
#endif
}

int palStringCompare(const char* a, const char* b) {
  return strcmp(a, b);
}

int palStringCompareN(const char* a, const char* b, size_t n) {
  return strncmp(a, b, n);
}

bool palStringEquals(const char* a, const char* b) {
  return palStringCompare(a, b) == 0;
}

bool palStringEqualsN(const char* a, const char* b, size_t n) {
  return palStringCompareN(a, b, n) == 0;
}

size_t internal_pal_printf_upper_bound(const char* format, va_list args) {
#if defined(PAL_COMPILER_MICROSOFT)
  size_t n = _vscprintf(format, args);
#else
  size_t n = vsnprintf(NULL, 0, format, args);
#endif
  /* on some versions of windows vsnprintf will return -1 instead of the number of characters needed to be written */
  palAssert(n >= 0);
  return n;
}

size_t pal_printf_upper_bound(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  size_t n = internal_pal_printf_upper_bound(format, ap);
  va_end(ap);
  return n;
}

extern void messagePrintf(const char*, ...);

char* palStringAllocatingPrintfInternal(const char* format, va_list args) {
  va_list temp_args;
  va_copy(temp_args, args);
  size_t len = internal_pal_printf_upper_bound(format, args)+1;
  palAssert(len >= 0);
  char* str = static_cast<char*>(g_StringProxyAllocator->Allocate(len+1));
  size_t n = palStringPrintfInternal(str, len, format, temp_args);
  va_end(temp_args);
  return str;
}

void palStringAllocatingPrintfInternalDeallocate(char* buff) {
  g_StringProxyAllocator->Deallocate(buff);
}

char* palStringAllocatingPrintf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  char* str = palStringAllocatingPrintfInternal(format, args);
  va_end(args);
  return str;
}

void palStringAllocatingPrintfDeallocate(char* buff) {
  palStringAllocatingPrintfInternalDeallocate(buff);
}

size_t palStringPrintfInternal(char* str, size_t size, const char* format, va_list args) {
#if defined(PAL_COMPILER_MICROSOFT)
#pragma warning(push)
#pragma warning(disable : 4996)
  size_t n = _vsnprintf(str, size, format, args);
#pragma warning(pop)
#else
  size_t n = vsnprintf(str, size, format, args);
#endif
  str[size-1] = '\0';
  return n;
}

size_t palStringPrintf(char* str, uint32_t size, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  size_t n = palStringPrintfInternal(str, size, format, ap);
  va_end(ap);
  return n;
}

size_t palStringFindCh(const char* str, char ch) {
  if (!str)
    return -1;

  size_t index = 0;
  while (*str != 0) {
    if (*str == ch) {
      return index;
    }
    index++;
    str++;
  }
  return -1;
}

size_t palStringFindChFromRight(const char* str, char ch) {
  size_t length = palStringLength(str);
  while (length > 0) {
    if (str[length] == ch) {
      return length;
    }
    length--;
  }
  return -1;
}

bool palStringFindString(const char* str, const char* findstr, size_t* start_out, size_t* end_out) {
  size_t str_len = palStringLength(str);
  size_t findstr_len = palStringLength(findstr);
  size_t start;
  size_t end;

  if (findstr_len > str_len) {
    return false;
  } else if (findstr_len == str_len) {
    if (palStringCompareN(str, findstr, findstr_len) == 0) {
      start = 0;
      end = str_len;
      if (start_out)
        *start_out = start;
      if (end_out)
        *end_out = end;
      return true;
    }
    return false;
  }

  const char* search_point = str;
  for (size_t i = 0; i <= str_len - findstr_len; i++) {
    if (palStringCompareN(search_point, findstr, findstr_len) == 0) {
      start = search_point - str;
      end = start + findstr_len;
      if (start_out)
        *start_out = start;
      if (end_out)
        *end_out = end;
      return true;
    }
    search_point++;
  }
  return false;
}

int palStringToInteger(const char* str) {
  int r = atoi(str);
  return r;
}

float palStringToFloat(const char* str) {
  float f = (float)atof(str);
  return f;
}

bool operator==(const palDynamicString& A, const palDynamicString& B) {
  return A.Equals(B);
}

bool operator==(const palDynamicString& A, const char* B) {
  return A.Equals(B);
}

bool operator==(const char* A, const palDynamicString& B) {
  return B.Equals(A);
}

bool operator!=(const palDynamicString& A, const palDynamicString& B) {
  return A.Equals(B) == false;
}
bool operator!=(const palDynamicString& A, const char* B) {
  return A.Equals(B) == false;
}

bool operator!=(const char* B, const palDynamicString& A) {
  return A.Equals(B) == false;
}

static const int kPalDynamicStringMinimumCapacity = 16;
static const int kPalDynamicStringGrowthFactor = 2;
static size_t pal_dynamic_string_memory_used = 0;

palDynamicString::palDynamicString() : _buffer(NULL), _capacity(0), _length(0) {
}

palDynamicString::palDynamicString(const char* init_string) : _buffer(NULL), _capacity(0), _length(0) {
  if (init_string == NULL) {
    return;
  }
  Set(init_string);
}

palDynamicString::palDynamicString(const palDynamicString& other) : _buffer(NULL), _capacity(0), _length(0) {
  Set(other.C());
}

palDynamicString::~palDynamicString() {
  Resize(0);
  _length = 0;
}

void palDynamicString::SetCapacity(size_t capacity) {
  if (capacity < _length) {
    // we are shrinking
    SetLength(capacity);
  }
  Resize(capacity);
}

void palDynamicString::SetLength(size_t new_length) {
  if (new_length >= _capacity) {
    // can't grow longer than capacity
    return;
  }
  if (new_length > _length) {
    // string is being grown
    // fill with spaces
    palMemorySetBytes(&_buffer[_length], ' ', new_length - _length);
  }
  _buffer[new_length] = '\0';
  _length = new_length;
}

size_t palDynamicString::GetCapacity() const {
  return _capacity;
}

size_t palDynamicString::GetLength() const {
  return _length;
}

void palDynamicString::Reset() {
  Resize(0);
  _length = 0;
}

const char* palDynamicString::C() const {
  return _buffer;
}

char* palDynamicString::C() {
  return _buffer;
}

bool palDynamicString::Equals(const char* str) const {
  return Compare(str) == 0;
}

bool palDynamicString::Equals(const palDynamicString& str) const {
  return Compare(str) == 0;
}

int palDynamicString::Compare(const char* str) const {
  if (_buffer == NULL) {
    return 1;
  }
  return palStringCompare(_buffer, str);
}

int palDynamicString::Compare(const palDynamicString& str) const {
  if (_buffer == NULL) {
    return 1;
  }
  return palStringCompare(_buffer, str.C());
}

void palDynamicString::Set(const char ch) {
  SetLength(0);
  ExpandCapacityIfNeeded(1);
  _buffer[0] = ch;
  _buffer[1] = '\0';
  _length = 1;
}

void palDynamicString::Set(const char* str) {
  SetLength(0);
  size_t length = palStringLength(str);
  ExpandCapacityIfNeeded(length);
  palStringCopy(_buffer, str); 
  _length = length;
}

void palDynamicString::Set(const char* str, size_t length) {
  SetLength(0);
  ExpandCapacityIfNeeded(length);
  palMemoryCopyBytes(_buffer, str, length);
  _buffer[length] = '\0';
  _length = length;
}

void palDynamicString::Set(const palDynamicString& str) {
  SetLength(0);
  size_t length = str.GetLength();
  ExpandCapacityIfNeeded(length);
  palStringCopy(_buffer, str.C());
  _length = length;
}

void palDynamicString::Set(const palDynamicString& str, size_t start, size_t count) {
  SetLength(0);
  size_t str_length = str.GetLength();
  if (start == -1 || start >= str_length) {
    return;
  }
  Set(str.C()+start, count);
}

void palDynamicString::SetPrintf(const char* format, ...) {
  SetLength(0);
  va_list args;
  va_start(args, format);
  char* temp_string = palStringAllocatingPrintfInternal(format, args);
  Set(temp_string);
  g_StringProxyAllocator->Deallocate(temp_string);
  va_end(args);
}

void palDynamicString::Append(const char ch) {
  Insert(-1, ch);
}

void palDynamicString::Append(const char* str) {
  Insert(-1, str);
}

void palDynamicString::Append(const char* str, size_t length) {
  Insert(-1, str, length);
}

void palDynamicString::Append(const palDynamicString& str) {
  Insert(-1, str);
}

void palDynamicString::Append(const palDynamicString& str, size_t start, size_t count) {
  Insert(-1, str, start, count);
}

void palDynamicString::AppendPrintf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  char* temp_string = palStringAllocatingPrintfInternal(format, args);
  Insert(-1, temp_string);
  g_StringProxyAllocator->Deallocate(temp_string);
  va_end(args);
}

void palDynamicString::AppendPrintfInternal(const char* format, va_list args) {
  char* temp_string = palStringAllocatingPrintfInternal(format, args);
  Insert(-1, temp_string);
  g_StringProxyAllocator->Deallocate(temp_string);
}

void palDynamicString::Prepend(const char ch) {
  Insert(0, ch);
}

void palDynamicString::Prepend(const char* str) {
  Insert(0, str);
}

void palDynamicString::Prepend(const char* str, int length) {
  Insert(0, str, length);
}

void palDynamicString::Prepend(const palDynamicString& str) {
  Insert(0, str);
}

void palDynamicString::Prepend(const palDynamicString& str, size_t start, size_t count) {
  Insert(0, str, start, count);
}

void palDynamicString::PrependPrintf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  char* temp_string = palStringAllocatingPrintfInternal(format, args);
  Insert(0, temp_string);
  g_StringProxyAllocator->Deallocate(temp_string);
  va_end(args);
}

void palDynamicString::PrependPrintfInternal(const char* format, va_list args) {
  char* temp_string = palStringAllocatingPrintfInternal(format, args);
  Insert(0, temp_string);
  g_StringProxyAllocator->Deallocate(temp_string);
}


void palDynamicString::Insert(size_t start, const char ch) {
 ExpandCapacityIfNeeded(1);

  if (start == -1)
    start = _length;

  if (start > _length)
    return;

  /* create gap for new string by
   * moving the tail (read: string->str_len_ - position) of the original string str_len_ bytes forward
   */
  if (start < _length) {
    palMemoryCopyBytes(_buffer + start + 1, _buffer + start, _length - start);
  }

  _buffer[start] = ch;
  _length += 1;
  _buffer[_length] = '\0';
}

void palDynamicString::Insert(size_t start, const char* str) {
  Insert(start, str, palStringLength(str));
}

void palDynamicString::Insert(size_t start, const char* str, size_t count) {
  // all the real work gets forwarded to this function
  if (count == 0) {
    return;
  }

  if (start == -1) {
    start = _length;
  }

  // make sure position is valid
  if (start > _length)
    return;

  ExpandCapacityIfNeeded(count);

  /* create gap for new string by
   * moving the tail (read: str_len_ - position) of the original string str_len_ bytes forward
   */
  if (start < _length) {
    palMemoryCopyBytes(_buffer + start + count, _buffer + start, _length - start);
  }

  /* insert new string */
  if (count == 1) {
    // single character
    _buffer[start] = *str;
  } else {
    // > 1 character
    palMemoryCopyBytes(_buffer + start, str, count);
  }

  _length += count;
  _buffer[_length] = 0;
}

void palDynamicString::Insert(size_t start, const palDynamicString& str) {
  Insert(start, str.C(), str.GetLength());
}

void palDynamicString::Insert(size_t start, const palDynamicString& str, size_t str_start, size_t count) {
  Insert(start, str.C()+str_start, count);
}

void palDynamicString::InsertPrintf(size_t start, const char* format, ...) {
  va_list args;
  va_start(args, format);
  char* temp_string = palStringAllocatingPrintfInternal(format, args);
  Insert(start, temp_string);
  g_StringProxyAllocator->Deallocate(temp_string);
  va_end(args);
}

void palDynamicString::InsertPrintfInternal(size_t start, const char* format, va_list args) {
  char* temp_string = palStringAllocatingPrintfInternal(format, args);
  Insert(start, temp_string);
  g_StringProxyAllocator->Deallocate(temp_string);
}

void palDynamicString::Cut(size_t start, size_t count) {
  if (start == -1 || start >= _length) {
    return;
  }
  if (start+count > _length) {
    size_t allowed = start+count - _length;
    count = allowed;
  }
  palMemoryCopyBytes(_buffer+start, _buffer+start+count, _length-count);
  _length -= count;
}

void palDynamicString::Cut(size_t start, size_t count, char* target_str) {
  Copy(start, count, target_str);
  Cut(start, count);
}

void palDynamicString::Cut(size_t start, size_t count, palDynamicString* target_str) {
  target_str->SetLength(0);
  if (target_str->GetCapacity() > count) {
    target_str->ExpandCapacityIfNeeded(count);
  }
  Copy(start, count, target_str->C());
  Cut(start, count);
}

void palDynamicString::Copy(size_t start, size_t count, char* target_str) {
  if (start == -1 || start >= _length) {
    target_str[0] = '\0';
    return;
  }
  if (start+count > _length) {
    size_t allowed = start+count - _length;
    count = allowed;
  }
  palMemoryCopyBytes(target_str, _buffer+start, count);
  target_str[count+1] = '\0';
}

void palDynamicString::Copy(size_t start, size_t count, palDynamicString* target_str) {
  target_str->SetLength(0);
  if (target_str->GetCapacity() > count) {
    target_str->ExpandCapacityIfNeeded(count);
  }
  return Copy(start, count, target_str->C());
}

void palDynamicString::SearchAndReplace(size_t start, size_t count, char search, char replace) {
  if (start == -1 || start >= _length) {
    return;
  }
  if (start+count > _length) {
    size_t allowed = start+count-_length;
    count = allowed;
  }
  char* s = _buffer;
  for (size_t i = start; i < start+count; i++) {
    if (s[i] == search) {
      s[i] = replace;
    }
  }
}

void palDynamicString::SearchAndReplace(size_t start, char search, char replace) {
  if (start == -1 || start >= _length) {
    return;
  }
  char* s = _buffer;
  for (size_t i = start; _buffer[i] != '\0'; i++) {
    if (s[i] == search) {
      s[i] = replace;
    }
  }
}

palDynamicString& palDynamicString::operator=(const palDynamicString& str) {
  if (this != &str) {
    Set(str);
  }
  return *this;
}

palDynamicString& palDynamicString::operator=(const char* str) {
  if (_buffer != str) {
    Set(str);
  }
  return *this;
}

void palDynamicString::ExpandCapacityIfNeeded(size_t added_chars) {
  if (_length+added_chars >= _capacity) {
    size_t size = palRoundToPowerOfTwo(_length+added_chars+1);
    if (size < kPalDynamicStringMinimumCapacity) {
      size = kPalDynamicStringMinimumCapacity;
    }
    Resize(size);
  }
}

void palDynamicString::Resize(size_t new_capacity) {
  if (_buffer && new_capacity == 0) {
    // deleting the string
    pal_dynamic_string_memory_used -= _capacity;
    g_StringProxyAllocator->Deallocate(_buffer);
    _buffer = NULL;
    _capacity = 0;
  } else if (new_capacity > _capacity) {
    // growing the string
    char* new_buffer = (char*)g_StringProxyAllocator->Allocate(new_capacity);

    // track memory used by dynamic strings
    pal_dynamic_string_memory_used += new_capacity;
    pal_dynamic_string_memory_used -= _capacity;

    if (_buffer != NULL) {
      palStringCopy(new_buffer, _buffer);
    } else {
      new_buffer[0] = '\0';
    }
    g_StringProxyAllocator->Deallocate(_buffer);
    _buffer = new_buffer;
    _capacity = new_capacity;
  } else if (new_capacity < _capacity) {
    // shrinking the string
    char* new_buffer = (char*)g_StringProxyAllocator->Allocate(new_capacity);

    // track memory used by dynamic strings
    pal_dynamic_string_memory_used += new_capacity;
    pal_dynamic_string_memory_used -= _capacity;

    // copy beginning of old string into new string
    palAssert(_buffer != NULL);
    palMemoryCopyBytes(new_buffer, _buffer, new_capacity);
    g_StringProxyAllocator->Deallocate(_buffer);
    _buffer = new_buffer;
    _capacity = new_capacity;
  }
}

void palDynamicString::TrimWhiteSpaceFromStart() {
  size_t index = 0;
  size_t start_ws = -1;
  size_t ws_count = 0;

  while (index < GetLength()) {
    bool ws = palIsWhiteSpace(_buffer[index]);
    if (start_ws == -1 && ws) {
      start_ws = index;
      ws_count++;
      palAssert(start_ws == 0);
    } else if (ws) {
      ws_count++;
    } else {
      break;
    }
    index++;
  }

  Cut(start_ws, ws_count);
}

void palDynamicString::TrimWhiteSpaceFromEnd() {
  size_t index = GetLength()-1;
  size_t ws_count = 0;
  size_t start_ws = -1;

  while (index >= 0) {
    bool ws = palIsWhiteSpace(_buffer[index]);
    if (start_ws == -1 && ws) {
      start_ws = index;
      palAssert(start_ws == GetLength()-1);
    } else if (ws) {
      ws_count++;
    } else {
      break;
    }
    index--;
  }

  Cut(start_ws-ws_count, ws_count);
}
