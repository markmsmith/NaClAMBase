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

#include "libpal/pal_mem_blob.h"
#include "libpal/pal_memory_stream.h"
#include "libpal/pal_tokenizer.h"
#include "libpal/pal_hash_map.h"


enum palJSONTokenType {
  kJSONTokenTypeParseError = 0,
  kJSONTokenTypeMap,
  kJSONTokenTypeMapEntry,
  kJSONTokenTypeArray,
  kJSONTokenTypeValueString,
  kJSONTokenTypeValueNumber,
  kJSONTokenTypeValueTrue,
  kJSONTokenTypeValueFalse,
  kJSONTokenTypeValueNull,
  NUM_palJSONTokenType,
};

struct palJSONToken {
  /* Type of token */
  palJSONTokenType type;

  /* String containing token */
  const char* JSON_str;

  /* Start and end of token in string */
  size_t first_index;
  size_t length;

  /* Token name, maybe -1 */
  size_t name_first_index;
  size_t name_length;

  /* The token value */
  size_t value_first_index;
  size_t value_length;

  palJSONTokenType GetTypeOfValue() const;

  bool NameMatch(const char* name) const;

  float GetAsFloat() const;
  int GetAsInt() const;
  bool GetAsBool() const;
  void* GetAsPointer() const;
  void GetAsString(palDynamicString* str) const;

  void GetName(palDynamicString* name) const;

  void DebugPrintf() const;
};

palJSONToken* palJSONFindTokenWithName(palJSONToken* token_buffer, size_t token_buffer_length, const char* name);

class palJSONParser {
  const char* JSON_str_;
  size_t buffer_offset_;
  size_t JSON_str_len_;
  size_t parse_current_index_;
  size_t parse_end_index_;
  palMemoryStream _memory_stream;
  palTokenizer tokenizer_;

  palJSONTokenType SkipValue(size_t* start_index, size_t* length);

  size_t ParseMapEntries(palJSONToken* token_buffer, size_t token_buffer_size);
  size_t ParseArrayEntries(palJSONToken* token_buffer, size_t token_buffer_size);
public:
  palJSONParser();

  /* Initialize this JSON parser with a string to parse */
  void Init(const char* JSON_str);

  /* Start parsing the whole string */
  void StartParse();
  /* Start parsing at start_index for length bytes */
  void StartParse(size_t start_index, size_t length);

  /* Returns the number of tokens detected,
   * if the return is the size of the tokens buffer,
   * you should call parse again */

  /*
  An example,
  palJSONToken token_buffer[8];
  json.Init(string);
  json.StartParse();
  do {
    int num_parsed = json.Parse(&token_buffer[0], 8);

    // do something with parsed tokens

    if (num_parsed < 8)
      break;
  } while(true);
  */
  size_t Parse(palJSONToken* token_buffer, size_t token_buffer_size);
};

void palJSONPrettyPrint(const char* JSON_str);

typedef palTypeBlob<const char> palJSONReaderPointer;

class palJSONReader {
  palJSONParser _parser;
public:

  palJSONReader();

  void Init(const char* JSON_str);

  int GetPointerToValue(const char* expr, palJSONReaderPointer* pointer);
};

/* A different approach */

struct palJSONString {
  palJSONString() { start = NULL, len = 0; }
  palJSONString(const char* string, size_t string_len) { start = string; len = string_len; }
  const char* start;
  size_t len;
};

template<>
struct palHashFunction<palJSONString> {
  unsigned int operator()(const palJSONString& key) const {
    return palMurmurHash(key.start, key.len);
  }
};

template<>
struct palHashEqual<palJSONString> {
  bool operator()(const palJSONString& x, const palJSONString& y) const {
    if (x.len != y.len) {
      return false;
    }
    return palStringEqualsN(x.start, y.start, x.len);
  }
};

struct palJSONObject {
  palJSONTokenType type;
  palJSONString internal_string;
  palJSONString noquotes_string;
  palHashMap<palJSONString, palJSONObject*> _map;
  palArray<palJSONObject*> _array;

  int GetNumChildren() const;

  const palArray<palJSONString>* GetNameArray() const;
  const palArray<palJSONObject*>* GetChildArray() const;

  palArray<palJSONObject*>* GetChildArray();
  
  const palJSONObject* GetChild(int i) const;
  const palJSONObject* GetChild(const char* key) const;

  palJSONObject* GetChild(int i);
  palJSONObject* GetChild(const char* key);

  const palJSONObject* operator[](int i) const;
  const palJSONObject* operator[](const char* key) const;
  const palJSONObject* operator[](const palDynamicString& string) const;

  palJSONObject* operator[](int i);
  palJSONObject* operator[](const char* key);
  palJSONObject* operator[](const palDynamicString& string);

  float GetAsFloat() const;
  int GetAsInt() const;
  bool GetAsBool() const;
  void GetAsDynamicString(palDynamicString* str) const;

  const char* C() const;
  size_t StringLength() const;
};

class palJSONObjectParser {
  palJSONObject* _root;
  palJSONString _text;
  palAllocatorInterface* _allocator;

  // Main parse entry point
  void Parse(palJSONString string);
  palJSONObject* ParseNode(palJSONString string);
  palJSONObject* ParseObject(palJSONString string);
  palJSONObject* ParseArray(palJSONString string);
  palJSONObject* ConstructNode();
  void PruneNode(palJSONObject* object);
public:
  palJSONObjectParser(palAllocatorInterface* allocator = NULL);
  void SetAllocator(palAllocatorInterface* allocator);
  int Parse(palMemBlob* blob);
  void Clear();
  palJSONObject* GetRoot();
  const palJSONObject* GetRoot() const;
};

class palJSONObjectPrinter {
public:
  static void PrintObject(const palJSONObject* object, int depth = 0);
};

class palJSONBuilder {
  palDynamicString* _string;
  int _map_count;
  int _array_count;
public:
  palJSONBuilder();

  void Start(palDynamicString* string);
  const char* GetJSON();

  /* { */
  int PushObject();
  /* } */
  int PopObject();

  /* "name" : value */
  void Map(const char* name, uint64_t value, bool comma = false);
  void Map(const char* name, uint32_t value, bool comma = false);
  void Map(const char* name, int value, bool comma = false);
  void Map(const char* name, bool value, bool comma = false);
  void Map(const char* name, float value, bool comma = false);
  void Map(const char* name, const char* value, bool comma = false);

  /* "name" : */
  void Map(const char* name);

  /* "name" : "printf(fmt, ...)" */
  void MapPrintf(const char* name, const char* fmt, ...);
  void MapPrintfComma(const char* name, const char* fmt, ...);

  /* , */
  void Comma();

  /* Literal values */
  void Value(uint32_t value, bool comma = false);
  void Value(uint64_t value, bool comma = false);
  void Value(int value, bool comma = false);
  void Value(bool value, bool comma = false);
  void Value(float value, bool comma = false);
  void Value(const char* value, bool comma = false);
  void ValuePrintf(const char* fmt, ...);
  void ValuePrintfComma(const char* fmt, ...);

  /* [ */
  int PushArray();
  /* ] */
  int PopArray();
};
