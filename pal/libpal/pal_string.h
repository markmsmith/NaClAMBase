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

#pragma once

#include "libpal/pal_types.h"
#include "libpal/pal_memory.h"

bool  palIsAlpha(char ch);
bool  palIsDigit(char ch);
bool  palIsHexDigit(char ch);
bool  palIsWhiteSpace(char ch);
int   palDigitValue(char ch);
int   palHexDigitValue(char ch);

size_t   palStringLength(const char* str);
char* palStringDuplicate(const char* str);
void palStringDuplicateDeallocate(const char* str);
void  palStringCopy(char* dest, const char* src);
int   palStringCompare(const char* a, const char* b);
int   palStringCompareN(const char* a, const char* b, size_t n);
bool  palStringEquals(const char* a, const char* b);
bool  palStringEqualsN(const char* a, const char* b, size_t n);
size_t   palStringPrintf(char* str, uint32_t size, const char* format, ...);
char* palStringAllocatingPrintf(const char* format, ...);
void palStringAllocatingPrintfDeallocate(char* buff);

char* palStringAllocatingPrintfInternal(const char* format, va_list args);
void palStringAllocatingPrintfInternalDeallocate(char* buff);
size_t palStringPrintfInternal(char* str, size_t size, const char* format, va_list args);

size_t   palStringFindCh(const char* str, char ch);
size_t   palStringFindChFromRight(const char* str, char ch);
bool  palStringFindString(const char* str, const char* findstr, size_t* start, size_t* end);

int palStringToInteger(const char* str);
float palStringToFloat(const char* str);

class palDynamicString {
public:
  palDynamicString();
  palDynamicString(const char* init_string);
  palDynamicString(const palDynamicString& other);
  ~palDynamicString();

  void SetCapacity(size_t capacity);
  void SetLength(size_t new_length);

  size_t GetCapacity() const;
  size_t GetLength() const;

  void Reset();

  const char* C() const;
  char* C();

  void TrimWhiteSpaceFromStart();
  void TrimWhiteSpaceFromEnd();

  bool Equals(const char* str) const;
  bool Equals(const palDynamicString& str) const;

  int Compare(const char* str) const;
  int Compare(const palDynamicString& str) const;

  void Set(const char ch);
  void Set(const char* str);
  void Set(const char* str, size_t length);
  void Set(const palDynamicString& str);
  void Set(const palDynamicString& str, size_t start, size_t count);
  void SetPrintf(const char* format, ...);
  
  void Append(const char ch);
  void Append(const char* str);
  void Append(const char* str, size_t length);
  void Append(const palDynamicString& str);
  void Append(const palDynamicString& str, size_t start, size_t count);
  void AppendPrintf(const char* format, ...);
  void AppendPrintfInternal(const char* format, va_list ap);

  void Prepend(const char ch);
  void Prepend(const char* str);
  void Prepend(const char* str, int length);
  void Prepend(const palDynamicString& str);
  void Prepend(const palDynamicString& str, size_t start, size_t count);
  void PrependPrintf(const char* format, ...);
  void PrependPrintfInternal(const char* format, va_list ap);

  /* If start == -1, inserted into end of string */
  void Insert(size_t start, const char* str);
  void Insert(size_t start, const char* str, size_t count);
  void Insert(size_t start, const char ch);
  void Insert(size_t start, const palDynamicString& str);
  void Insert(size_t start, const palDynamicString& str, size_t str_start, size_t count);
  void InsertPrintf(size_t start, const char* format, ...);
  void InsertPrintfInternal(size_t start, const char* format, va_list ap);

  void Cut(size_t start, size_t count);
  void Cut(size_t start, size_t count, char* target_str);
  void Cut(size_t start, size_t count, palDynamicString* target_str);

  void Copy(size_t start, size_t count, char* target_str);
  void Copy(size_t start, size_t count, palDynamicString* target_str);

  void SearchAndReplace(size_t start, size_t count, char search, char replace);
  void SearchAndReplace(size_t start, char search, char replace);
  palDynamicString& operator=(const palDynamicString& str);
  palDynamicString& operator=(const char* str);
private:
  char* _buffer;
  size_t _length;
  size_t _capacity;
protected:
  void ExpandCapacityIfNeeded(size_t added_chars);
  void Resize(size_t new_capacity);
};

bool operator==(const palDynamicString& A, const palDynamicString& B);
bool operator==(const palDynamicString& A, const char* B);
bool operator==(const char* A, const palDynamicString& B);
bool operator!=(const palDynamicString& A, const palDynamicString& B);
bool operator!=(const palDynamicString& A, const char* B);
bool operator!=(const char* B, const palDynamicString& A);