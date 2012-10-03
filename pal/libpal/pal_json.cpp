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

#include <cstdlib>
#include "libpal/pal_array.h"
#include "libpal/pal_string.h"
#include "libpal/pal_json.h"

const char* json_token_type_strings[NUM_palJSONTokenType] = {
  "parse error",
  "map",
  "map entry",
  "array",
  "value string",
  "value number",
  "value true",
  "value false",
  "value null"
};

palTokenizerKeyword JSON_keywords[] = {
  {"true", kJSONTokenTypeValueTrue},
  {"false", kJSONTokenTypeValueFalse},
  {"null", kJSONTokenTypeValueNull},
  NULL, 0
};

palJSONTokenType palJSONToken::GetTypeOfValue() const {
  char ch = *(JSON_str+value_first_index);

  if (ch == 't') {
    return kJSONTokenTypeValueTrue;
  } else if (ch == 'f') {
    return kJSONTokenTypeValueFalse;
  } else if (ch == 'n') {
    return kJSONTokenTypeValueNull;
  } else if (ch == '[') {
    return kJSONTokenTypeArray;
  } else if (ch == '{') {
    return kJSONTokenTypeMap;
  } else if (ch == '\"') {
    return kJSONTokenTypeValueString;
  } else if (palIsDigit(ch) || ch == '-') {
    return kJSONTokenTypeValueNumber;
  }

  return kJSONTokenTypeParseError;
}

bool palJSONToken::NameMatch(const char* name) const {
  return false;
}

float palJSONToken::GetAsFloat() const {
  return (float)atof(JSON_str+value_first_index);
}

int palJSONToken::GetAsInt() const {
  return atoi(JSON_str+value_first_index);
}

bool palJSONToken::GetAsBool() const {
  return (type == kJSONTokenTypeValueTrue);
}

void* palJSONToken::GetAsPointer() const {
  return NULL;
}

void palJSONToken::GetAsString(palDynamicString* str) const {
  str->Set("");
  str->Append(JSON_str+value_first_index+1, value_length-2);
}

void palJSONToken::GetName(palDynamicString* name) const {
  name->Set("");
  name->Append(JSON_str+name_first_index+1, name_length-2);
}

void palJSONToken::DebugPrintf() const {
  palDynamicString debug_str;

  switch (type) {
  case kJSONTokenTypeValueFalse:
    palPrintf("false");
    break;
  case kJSONTokenTypeValueTrue:
    palPrintf("true");
    break;
  case kJSONTokenTypeValueNull:
    palPrintf("null");
    break;
  case kJSONTokenTypeArray:
    break;
  case kJSONTokenTypeMap:
    break;
  case kJSONTokenTypeMapEntry:
    debug_str.Append(JSON_str+name_first_index, name_length);
    if (GetTypeOfValue() == kJSONTokenTypeValueFalse) {
      palPrintf("%s : false", debug_str.C());
    } else if (GetTypeOfValue() == kJSONTokenTypeValueTrue) {
      palPrintf("%s : true", debug_str.C());
    } else if (GetTypeOfValue() == kJSONTokenTypeValueNull) {
      palPrintf("%s : null", debug_str.C());
    } else if (GetTypeOfValue() == kJSONTokenTypeValueNumber) {
      palPrintf("%s : %f", debug_str.C(), GetAsFloat());
    } else if (GetTypeOfValue() == kJSONTokenTypeValueString) {
      palPrintf("%s : ", debug_str.C());
      debug_str.Reset();
      debug_str.Append(JSON_str+value_first_index, value_length);
      palPrintf("%s", debug_str.C());
    } else {
      palPrintf("%s : \n", debug_str.C());
    }
    break;
  case kJSONTokenTypeValueNumber:
    palPrintf("%f", GetAsFloat());
    break;
  case kJSONTokenTypeValueString:
    debug_str.Append(JSON_str+value_first_index, value_length);
    palPrintf("%s", debug_str.C());
    break;
  case kJSONTokenTypeParseError:
    palPrintf("parsing error\n");
    break;
  default:
    palBreakHere();
    break;
  }
}

palJSONToken* palJSONFindTokenWithName(palJSONToken* token_buffer, size_t token_buffer_length, const char* name) {
  for (size_t i = 0; i < token_buffer_length; i++) {
    if (token_buffer[i].name_first_index == -1) {
      break;
    }
    const char* name_string = token_buffer[i].JSON_str+(token_buffer[i].name_first_index+1);
    if (palStringEqualsN(name, name_string, (token_buffer[i].name_length-2))) {
      return &token_buffer[i];
    }
  }

  return NULL;
}

palJSONParser::palJSONParser() {
  JSON_str_ = 0;
  JSON_str_len_ = 0;
  parse_current_index_ = 0;
  parse_end_index_ = 0;
  tokenizer_.SetKeywordArray(&JSON_keywords[0]);
}

void palJSONParser::Init(const char* JSON_str) {
  JSON_str_ = JSON_str;
  JSON_str_len_ = palStringLength(JSON_str_);
  parse_current_index_ = 0;
  parse_end_index_ = 0;
}

void palJSONParser::StartParse() {
  buffer_offset_ = 0;
  parse_current_index_ = 0;
  parse_end_index_ = JSON_str_len_+1;
  _memory_stream.Reset();
  _memory_stream.Create(palMemBlob((void*)JSON_str_, (uint64_t)JSON_str_len_), false);
  tokenizer_.UseStream(&_memory_stream);
}
  
void palJSONParser::StartParse(size_t start_index, size_t length) {
  buffer_offset_ = start_index;
  parse_current_index_ = start_index;
  parse_end_index_ = start_index+length;
  _memory_stream.Reset();
  _memory_stream.Create(palMemBlob((void*)(JSON_str_+start_index), (size_t)length), false);
  tokenizer_.UseStream(&_memory_stream);
}

palJSONTokenType palJSONParser::SkipValue(size_t* start_index, size_t* length) {
  if (!start_index || !length) {
    return kJSONTokenTypeParseError;
  }

  palToken token;

  tokenizer_.FetchNextToken(&token);

  *start_index = buffer_offset_ + token.start_index;

  int nest_count = 0;
  if (token.type == kTokenPunctuation && token.type_flags == kPunctuationBRACE_OPEN) {
    // handle map value
    nest_count = 1;
    while (nest_count > 0) {
      tokenizer_.FetchNextToken(&token);
      if (token.type == kTokenPunctuation && token.type_flags == kPunctuationBRACE_OPEN) {
        nest_count++;
      } else if (token.type == kTokenPunctuation && token.type_flags == kPunctuationBRACE_CLOSE) {
        nest_count--;
      } else if (token.type == kTokenEOS) {
        return kJSONTokenTypeParseError;
      }
    }
    *length = token.start_index + token.length - *start_index + buffer_offset_;
    return kJSONTokenTypeMap;
  } else if (token.type == kTokenPunctuation && token.type_flags == kPunctuationSQUARE_BRACKET_OPEN) {
    // handle array value
    nest_count = 1;
    while (nest_count > 0) {
      tokenizer_.FetchNextToken(&token);
      if (token.type == kTokenPunctuation && token.type_flags == kPunctuationSQUARE_BRACKET_OPEN) {
        nest_count++;
      } else if (token.type == kTokenPunctuation && token.type_flags == kPunctuationSQUARE_BRACKET_CLOSE) {
        nest_count--;
      } else if (token.type == kTokenEOS) {
        return kJSONTokenTypeParseError;
      }
    }

    *length = token.start_index + token.length - *start_index + buffer_offset_;
    return kJSONTokenTypeArray;
  } else {
    *length = token.length;
    if (token.type == kTokenNumber) {
      return kJSONTokenTypeValueNumber;
    } else if (token.type == kTokenString) {
      return kJSONTokenTypeValueString;
    } else if (token.type == kTokenKeyword) {
      return (palJSONTokenType)token.type_flags;
    }

    return kJSONTokenTypeParseError;
  }
}

size_t palJSONParser::ParseMapEntries(palJSONToken* token_buffer, size_t token_buffer_size) {
  size_t token_buffer_insert_index = 0;
  int brace_count = 1;

  palToken token;
  while (brace_count > 0) {  
    tokenizer_.FetchNextToken(&token);

    /* map entries look like:
       "string" : value...,
       "string2" : value...,
     */

    if (token.type == kTokenEOL) {
      continue;
    }
    if (token.type == kTokenPunctuation && token.type_flags == kPunctuationBRACE_CLOSE) {
      brace_count--;
      break;
    }

    if (token.type == kTokenPunctuation || token.type_flags == kPunctuationCOMMA) {
      // next token.
      continue;
    }

    if (token.type != kTokenString) {
      // the first token must be a string
      break;
    }

    // we have a map entry
    token_buffer[token_buffer_insert_index].type = kJSONTokenTypeMapEntry;
    token_buffer[token_buffer_insert_index].JSON_str = NULL;

    token_buffer[token_buffer_insert_index].first_index = buffer_offset_ + token.start_index;

    token_buffer[token_buffer_insert_index].name_first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].name_length = token.length;

    tokenizer_.FetchNextToken(&token);

    if (token.type != kTokenPunctuation || token.type_flags != kPunctuationCOLON) {
      // after the name, we require a colon.
      break;
    }

    // skip over the value for this map entry, recording the first index and length of the value
    SkipValue(&token_buffer[token_buffer_insert_index].value_first_index, &token_buffer[token_buffer_insert_index].value_length);

    token_buffer_insert_index++;
    if (token_buffer_insert_index == token_buffer_size) {
      // we have filled up the token buffer, return.
      return token_buffer_insert_index;
    }
  }

  if (brace_count) {
    // add parse error token
    palBreakHere();
  }

  return token_buffer_insert_index;
}

size_t palJSONParser::ParseArrayEntries(palJSONToken* token_buffer, size_t token_buffer_size) {
  size_t token_buffer_insert_index = 0;
  int brace_count = 1;

  palToken token;
  while (brace_count > 0) {  
    tokenizer_.FetchNextToken(&token);

    /* array entries look like
      value1, value2, value3
     */

    if (token.type == kTokenEOL) {
      continue;
    }
    if (token.type == kTokenPunctuation && token.type_flags == kPunctuationSQUARE_BRACKET_CLOSE) {
      brace_count--;
      break;
    }

    if (token.type == kTokenPunctuation && token.type_flags == kPunctuationCOMMA) {
      // next value
      continue;
    }

    // we are ready to parse the value
    // push the last read token onto this buffer
    tokenizer_.PushToken(&token);

    token_buffer[token_buffer_insert_index].type = kJSONTokenTypeParseError;
    token_buffer[token_buffer_insert_index].JSON_str = NULL;

    token_buffer[token_buffer_insert_index].first_index = buffer_offset_ + token.start_index;

    token_buffer[token_buffer_insert_index].name_first_index = -1;
    token_buffer[token_buffer_insert_index].name_length = -1;

    // skip over the value
    token_buffer[token_buffer_insert_index].type = SkipValue(&token_buffer[token_buffer_insert_index].value_first_index, &token_buffer[token_buffer_insert_index].value_length);

    token_buffer_insert_index++;

    if (token_buffer_insert_index == token_buffer_size) {
      // we have filled up the token buffer, return.
      return token_buffer_insert_index;
    }
  }

  if (brace_count) {
    // add parse error token
    palBreakHere();
  }

  return token_buffer_insert_index;
}

size_t palJSONParser::Parse(palJSONToken* token_buffer, size_t token_buffer_size) {
  size_t token_buffer_insert_index = 0;

  const int kStateStart = 1;

  const int kStateObject = 2;
  const int kStateArray = 3;
  const int kStateValueTrue = 4;
  const int kStateValueFalse = 5;
  const int kStateValueNull = 6;
  const int kStateValueNumber = 7;
  const int kStateValueString = 8;

  char last_ch = 0;
  int state = kStateStart;
  int lbrace_count = 0;
  int lbracket_count = 0;

  palToken token;

  tokenizer_.FetchNextToken(&token);

  if (token.type == kTokenEOS || token.type == kTokenERROR) {
    // handle error
    return 0;
  }

  if (token.type == kTokenKeyword) {
    // we parsed a keyword
    token_buffer[token_buffer_insert_index].JSON_str = JSON_str_;
    token_buffer[token_buffer_insert_index].first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].length = token.length;
    token_buffer[token_buffer_insert_index].value_first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].value_length = token.length;
    token_buffer[token_buffer_insert_index].type = (palJSONTokenType)token.type_flags;
    token_buffer[token_buffer_insert_index].name_first_index = -1;
    token_buffer[token_buffer_insert_index].name_length = -1;
    token_buffer_insert_index++;
    return token_buffer_insert_index;
  }

  if (token.type == kTokenString) {
    // we parsed a string
    token_buffer[token_buffer_insert_index].JSON_str = JSON_str_;
    token_buffer[token_buffer_insert_index].first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].length = token.length;
    token_buffer[token_buffer_insert_index].value_first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].value_length = token.length;
    token_buffer[token_buffer_insert_index].type = kJSONTokenTypeValueString;
    token_buffer[token_buffer_insert_index].name_first_index = -1;
    token_buffer[token_buffer_insert_index].name_length = -1;
    token_buffer_insert_index++;
    return token_buffer_insert_index;
  }

  if (token.type == kTokenNumber) {
    // we parsed a number
    token_buffer[token_buffer_insert_index].JSON_str = JSON_str_;
    token_buffer[token_buffer_insert_index].first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].length = token.length;
    token_buffer[token_buffer_insert_index].value_first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].value_length = token.length;
    token_buffer[token_buffer_insert_index].type = kJSONTokenTypeValueNumber;
    token_buffer[token_buffer_insert_index].name_first_index = -1;
    token_buffer[token_buffer_insert_index].name_length = -1;
    token_buffer_insert_index++;
    return token_buffer_insert_index;    
  }

  /* There are only two JSON types left, the map and the array */
  if (token.type == kTokenPunctuation && token.type_flags == kPunctuationBRACE_OPEN) {
    /* First token is a "{", each token we return will be a MAP_ENTRY token. */
    size_t parsed_tokens = ParseMapEntries(token_buffer, token_buffer_size);
    for (size_t i = 0; i < parsed_tokens; i++) {
      token_buffer[i].JSON_str = JSON_str_;
    }
    return parsed_tokens;
  } else if (token.type == kTokenPunctuation && token.type_flags == kPunctuationSQUARE_BRACKET_OPEN) {
    /* First token is a "[", each token we return will be an array entry */
    size_t parsed_tokens = ParseArrayEntries(token_buffer, token_buffer_size);
    for (size_t i = 0; i < parsed_tokens; i++) {
      token_buffer[i].JSON_str = JSON_str_;
    }
    return parsed_tokens;
  } else {
    palBreakHere();
    return -1;
  }
}

void indent(size_t depth) {
  for (size_t i = 0; i < depth; i++) {
    palPrintf("  ");
  }
}

void palJSONPrettyPrintInternal(const char* str, size_t depth, size_t start_index, size_t length) {
  palJSONParser parser;
  parser.Init(str);
  parser.StartParse(start_index, length);

  const int num_json_tokens = 50;
  palJSONToken tokens[num_json_tokens];
  size_t num_json_tokens_parsed = 0;

  num_json_tokens_parsed = parser.Parse(&tokens[0], num_json_tokens);

  if (num_json_tokens_parsed > 0 && tokens[0].type == kJSONTokenTypeMapEntry) {
    indent(depth);
    palPrintf("{\n");
    depth++;
  }
  for (size_t i = 0; i < num_json_tokens_parsed; i++) {
    bool need_comma = i < num_json_tokens_parsed-1;
    indent(depth);
    tokens[i].DebugPrintf();
    if (depth >= 0) {
      if (tokens[i].GetTypeOfValue() == kJSONTokenTypeMap) {
        palJSONPrettyPrintInternal(str, depth+1, tokens[i].value_first_index, tokens[i].value_length);
      } else if (tokens[i].GetTypeOfValue() == kJSONTokenTypeArray) {
        indent(depth);
        palPrintf("[\n");
        palJSONPrettyPrintInternal(str, depth+1, tokens[i].value_first_index, tokens[i].value_length);
        indent(depth);
        palPrintf("]");
      } 
    }
    
    if (need_comma) {
      palPrintf(",\n");
    } else {
      palPrintf("\n");
    }
  }

  if (num_json_tokens_parsed > 0 && tokens[0].type == kJSONTokenTypeMapEntry) {
    depth--;
    indent(depth);
    palPrintf("}\n");
  }
}

void palJSONPrettyPrint(const char* JSON_str) {
  size_t len = palStringLength(JSON_str);
  palJSONPrettyPrintInternal(JSON_str, 0, 0, len);
}

palJSONReader::palJSONReader() {
}

void palJSONReader::Init(const char* JSON_str) {
  _parser.Init(JSON_str);
}

struct JsonQueryNode {
  palDynamicString name;
  // or
  size_t array_index;
  JsonQueryNode() {
    array_index = -1;
  }
};

static void BuildQueryNodes(const char* expr, palArray<JsonQueryNode>* nodes) {
  if (expr == NULL || *expr == '\0') {
    return;
  }

  if (*expr == '[') {
    JsonQueryNode& node = nodes->AddTail();
    node.array_index = palStringToInteger(expr+1);
    size_t first_rsquare = palStringFindCh(expr, ']');
    BuildQueryNodes(&expr[first_rsquare+1], nodes);
    return;
  } else if (*expr == '.') {
    //QueryNode& node = nodes->AddTail();
    //node.array_index = 0;
    BuildQueryNodes(expr+1, nodes);
    return;
  }

  size_t first_dot = palStringFindCh(expr, '.');
  size_t first_lsquare = palStringFindCh(expr, '[');

  if (first_dot >= 0 && first_lsquare >= 0) {
    if (first_dot < first_lsquare) {
      first_lsquare = -1;
    } else {
      first_dot = -1;
    }
  }

  if (first_dot > 0) {
    JsonQueryNode& node = nodes->AddTail();
    node.name.Append(expr, first_dot);
    BuildQueryNodes(&expr[first_dot], nodes);
    return;
  }

  if (first_lsquare > 0) {
    JsonQueryNode& node = nodes->AddTail();
    node.name.Append(expr, first_lsquare);
    BuildQueryNodes(&expr[first_lsquare], nodes);
    return;
  }

  JsonQueryNode& node = nodes->AddTail();
  node.name.Append(expr, palStringLength(expr));

  return;
}

int palJSONReader::GetPointerToValue(const char* expr, palJSONReaderPointer* pointer) {
  pointer->elements = NULL;
  pointer->size = 0;

  palArray<JsonQueryNode> nodes;

  BuildQueryNodes(expr, &nodes);

#if 0
  for (int i = 0; i < nodes.GetSize(); i++) {
    const QueryNode& node = nodes[i];
    int j = 0;
    while (j < i) {
      palPrintf(" ");
      j++;
    }
    if (node.array_index >= 0) {
      palPrintf("[%d]", node.array_index);
    } else {
      palPrintf("%s", node.name.C());
    }
    palPrintf("\n");
  }
#endif

  const int max_tokens = 32;
  palJSONToken tokens[max_tokens];
  palJSONToken* found_token = NULL;

  _parser.StartParse();
  for (int i = 0; i < nodes.GetSize(); i++) {
    const JsonQueryNode& node = nodes[i];
    size_t num_tokens = _parser.Parse(&tokens[0], max_tokens);
    if (node.array_index >= 0) {
      // array query
      if (node.array_index >= num_tokens) {
        found_token = NULL;
        break;
      }
      found_token = &tokens[node.array_index];
    } else {
      // name query
      found_token = palJSONFindTokenWithName(&tokens[0], num_tokens, node.name.C());
      if (found_token == NULL) {
        break;
      }
    }
    _parser.StartParse(found_token->value_first_index, found_token->value_length);
  }

  if (found_token) {
    // fill in pointer data
    pointer->elements = found_token->JSON_str+found_token->value_first_index;
    pointer->size = found_token->value_length;
    return 0;
  }

  return -1;
}



#if 0
{
  const char* j_i_name = "c:/temp/blah.json";
  uint64_t len;
  unsigned char* contents = palCopyFileContentsAsString(j_i_name, &len);
  if (!contents) {
    return -1;
  }
  palJSONReader j_reader;
  j_reader.Init((const char*)contents);

  char input[512];
  palStringPrintf(&input[0], 512, "menu.items[1].label");
  while (1) {
    palJSONReaderPointer pointer;
    int r = j_reader.GetPointerToValue(input, &pointer);
    if (r) {
      palPrintf("Error: could not find %s\n", input);
    } else {
      palPrintf("%.*s\n", pointer.size, pointer.elements);
    }

    continue;
  }
  palFree(contents);
  return 0;
}
#endif


int palJSONObject::GetNumChildren() const {
  if (!this) {
    return 0;
  }
  if (type == kJSONTokenTypeArray) {
    return _array.GetSize();
  }
  if (type == kJSONTokenTypeMap) {
    return _map.GetSize();
  }
  return 0;
}

const palArray<palJSONString>* palJSONObject::GetNameArray() const {
  if (!this) {
    return NULL;
  }
  if (type != kJSONTokenTypeMap) {
    return NULL;
  }
  return &_map.GetKeyArrayReference();
}

const palArray<palJSONObject*>* palJSONObject::GetChildArray() const {
  if (!this) {
    return NULL;
  }
  if (type == kJSONTokenTypeArray) {
    return &_array;
  }
  if (type == kJSONTokenTypeMap) {
    return &_map.GetValueArrayReference();
  }
  return NULL;
}

palArray<palJSONObject*>* palJSONObject::GetChildArray() {
  if (!this) {
    return NULL;
  }
  if (type == kJSONTokenTypeArray) {
    return &_array;
  }
  if (type == kJSONTokenTypeMap) {
    return &_map.GetValueArrayReference();
  }
  return NULL;
}

const palJSONObject* palJSONObject::GetChild(int i) const {
  if (!this) {
    return NULL;
  }
  const palArray<palJSONObject*>* children = GetChildArray();
  if (i < 0 || i >= children->GetSize()) {
    return NULL;
  }
  return children->operator[](i);
}

const palJSONObject* palJSONObject::GetChild(const char* key) const {
  if (!this) {
    return NULL;
  }
  if (type != kJSONTokenTypeMap) {
    return NULL;
  }
  palJSONString string;
  string.start = key;
  string.len = palStringLength(key);
  int r = _map.FindIndex(string);
  if (r == kPalHashNULL) {
    return NULL;
  }
  return *_map.GetValueAtIndex(r);
}

palJSONObject* palJSONObject::GetChild(int i) {
  if (!this) {
    return NULL;
  }
  palArray<palJSONObject*>* children = GetChildArray();
  if (i < 0 || i >= children->GetSize()) {
    return NULL;
  }
  return children->operator[](i);
}

palJSONObject* palJSONObject::GetChild(const char* key) {
  if (!this) {
    return NULL;
  }
  if (type != kJSONTokenTypeMap) {
    return NULL;
  }
  palJSONString string;
  string.start = key;
  string.len = palStringLength(key);
  int r = _map.FindIndex(string);
  if (r == kPalHashNULL) {
    return NULL;
  }
  return *_map.GetValueAtIndex(r);
}

const palJSONObject* palJSONObject::operator[](int i) const {
  return GetChild(i);
}

const palJSONObject* palJSONObject::operator[](const char* key) const {
  return GetChild(key);
}

const palJSONObject* palJSONObject::operator[](const palDynamicString& string) const {
  return GetChild(string.C());
}

palJSONObject* palJSONObject::operator[](int i) {
  return GetChild(i);
}

palJSONObject* palJSONObject::operator[](const char* key) {
  return GetChild(key);
}

palJSONObject* palJSONObject::operator[](const palDynamicString& string) {
  return GetChild(string.C());
}

float palJSONObject::GetAsFloat() const {
  if (!this) {
    return 0.0f;
  }
  if (type == kJSONTokenTypeValueNumber) {
    return palStringToFloat(internal_string.start);
  }
  return 0.0f;
}

int palJSONObject::GetAsInt() const {
  if (!this) {
    return 0;
  }
  if (type == kJSONTokenTypeValueNumber) {
    return palStringToInteger(internal_string.start);
  }
  return 0;
}

bool palJSONObject::GetAsBool() const {
  if (!this) {
    return false;
  }
  if (type == kJSONTokenTypeValueTrue) {
    return true;
  }
  return false;
}

void palJSONObject::GetAsDynamicString(palDynamicString* str) const {
  if (!this) {
    str->Set("");
  }
  str->Set(internal_string.start+1, internal_string.len-2);
}

const char* palJSONObject::C() const {
  if (!this) {
    return NULL;
  }
  if (type == kJSONTokenTypeValueString) {
    return noquotes_string.start;
  }
  return false;
}

size_t palJSONObject::StringLength() const {
  if (!this) {
    return 0;
  }
  if (type == kJSONTokenTypeValueString) {
    return noquotes_string.len;
  }
  return 0;
}


static char next(palJSONString input) {
  if (!input.start || input.len == 0) {
    return '\0';
  }
  return *input.start;
}

static palJSONString skipws(palJSONString input) {
  if (!input.start) {
    return input;
  }
  while (input.len != 0 && *input.start != '\0') {
    const char ch = *input.start;
    if (palIsWhiteSpace(ch) == false && ch != ',') {
      break;
    }
    input.start++;
    input.len--;
  }
  return input;
}

static palJSONString acceptkeyword(palJSONString input, const char* accept_string) {
  if (!input.start || !accept_string) {
    input.start = NULL;
    input.len = 0;
    return input;
  }
  while (input.len != 0 && *accept_string != '\0' && *input.start == *accept_string) {
    input.start++;
    input.len--;
    accept_string++;
  }
  if (*accept_string != '\0') {
    input.start = NULL;
    input.len = 0;
  }
  return input;
}

static bool numbercharacter(char ch) {
  return palIsDigit(ch) ||
         ch == '.' ||
         ch == 'e' ||
         ch == 'E' ||
         ch == '+' ||
         ch == '-';
}

static palJSONString acceptnumber(palJSONString input) {
  if (!input.start || input.len == 0 || *input.start == '\0') {
    input.start = NULL;
    input.len = 0;
    return input;
  }
  while (input.len != 0 && *input.start != '\0') {
    if (numbercharacter(*input.start)) {
      input.len--;
      input.start++;
    } else {
      break;
    }
  }
  return input;
}

static palJSONString acceptstring(palJSONString input) {
  if (!input.start || input.len == 0 || *input.start == '\0' || *input.start != '"') {
    input.start = NULL;
    input.len = 0;
    return input;
  }
  input.start++;
  input.len--;
  while (input.len != 0 && *input.start != '\0') {
    char ch = *input.start;
    input.start++;
    input.len--;
    if (ch == '"') {
      break;
    }
  }
  return input;
}

palJSONObjectParser::palJSONObjectParser(palAllocatorInterface* allocator /*= NULL*/) {
  _allocator = allocator;
  _root = NULL;
}

void palJSONObjectParser::SetAllocator(palAllocatorInterface* allocator) {
  _allocator = allocator;
}

palJSONObject* palJSONObjectParser::ConstructNode() {
  palJSONObject* object = NULL;
  object = _allocator->Construct<palJSONObject>();
  object->_map.SetAllocator(_allocator);
  object->_array.SetAllocator(_allocator);
  return object;
}

void palJSONObjectParser::PruneNode(palJSONObject* object) {
  if (object == NULL) {
    return;
  }
  const int num_children = object->GetNumChildren();
  if (num_children > 0) {
    palArray<palJSONObject*>* child_array = object->GetChildArray();
    for (int i = 0; i < num_children; i++) {
      PruneNode(child_array->operator[](i));
    }
  }
  object->_map.Clear();
  object->_array.Clear();
  _allocator->Destruct(object);
}

void palJSONObjectParser::Clear() {
  PruneNode(_root);
  _root = NULL;
}

int palJSONObjectParser::Parse(palMemBlob* blob) {
  _text = palJSONString(blob->GetPtr<const char>(0), blob->GetBufferSize());
  Parse(_text);
  return 0;
}

void palJSONObjectParser::Parse(palJSONString string) {
  Clear();
  _root = ParseNode(string);
}

palJSONObject* palJSONObjectParser::ParseNode(palJSONString string) {
  string = skipws(string);
  if (string.len == 0) {
    return NULL;
  }

  palJSONObject* node = NULL;

  switch (next(string)) {
  case '{':
    node = ParseObject(string);
    return node;
    break;
  case '[':
    node = ParseArray(string);
    return node;
    break;
  case '"':
    {
      palJSONString t = acceptstring(string);
      if (t.start == NULL) {
        break;
      }
      node = ConstructNode();
      node->type = kJSONTokenTypeValueString;
      node->internal_string.start = string.start;
      node->internal_string.len = (t.start - string.start);
      node->noquotes_string = node->internal_string;
      node->noquotes_string.start++;
      node->noquotes_string.len -= 2;
      return node;
    }
    break;
  case 't':
    {
      palJSONString t = acceptkeyword(string, "true");
      if (t.start == NULL) {
        break;
      }
      node = ConstructNode();
      node->type = kJSONTokenTypeValueTrue;
      node->internal_string.start = string.start;
      node->internal_string.len = 4;
      return node;
    }
    break;
  case 'f':
    {
      palJSONString t = acceptkeyword(string, "false");
      if (t.start == NULL) {
        break;
      }
      node = ConstructNode();
      node->type = kJSONTokenTypeValueFalse;
      node->internal_string.start = string.start;
      node->internal_string.len = 5;
      return node;
    }
    break;
  case 'n':
    {
      palJSONString type = acceptkeyword(string, "null");
      if (type.start == NULL) {
        break;
      }
      node = ConstructNode();
      node->type = kJSONTokenTypeValueNull;
      node->internal_string.start = string.start;
      node->internal_string.len = 4;
      return node;
    }
    break;
  case '-':
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    {
      palJSONString t = acceptnumber(string);
      if (t.start == NULL) {
        break;
      }
      node = ConstructNode();
      node->type = kJSONTokenTypeValueNumber;
      node->internal_string.start = string.start;
      node->internal_string.len = (t.start - string.start);
      return node;
    }
    break;
  case '\0':
  default:
    break;
  }

  return node;
}

palJSONObject* palJSONObjectParser::ParseObject(palJSONString string) {
  palJSONString start_string = string;
  string = acceptkeyword(string, "{");
  string = skipws(string);
  if (string.len == 0) {
    return NULL;
  }
  palJSONObject* node = ConstructNode();
  node->type = kJSONTokenTypeMap;
  while (next(string) != '}') {
    palJSONString t = acceptstring(string);
    // Name
    palJSONString name;
    name.start = string.start;
    name.len = (t.start - string.start);
    // Cut off the quotes
    name.start++;
    name.len--;
    name.len--;
    // Continue parsing
    string = t;
    string = skipws(string);
    string = acceptkeyword(string, ":");
    string = skipws(string);
    if (string.len == 0) {
      return node;
    }
    // Parse object itself
    palJSONObject* child_node = ParseNode(string);
    if (child_node == NULL) {
      // Failed to parse child...
      return node;
    }
    node->_map.Insert(name, child_node);
    // Move start string forward
    string.start = child_node->internal_string.start + child_node->internal_string.len;
    string.len -= child_node->internal_string.len;
    string = skipws(string);
  }
  string = acceptkeyword(string, "}");
  node->internal_string.start = start_string.start;
  node->internal_string.len = (string.start - start_string.start);
  return node;
}

palJSONObject* palJSONObjectParser::ParseArray(palJSONString string) {
  palJSONString start_string = string;
  string = acceptkeyword(string, "[");
  string = skipws(string);
  if (string.len == 0) {
    return NULL;
  }
  palJSONObject* node = ConstructNode();
  node->type = kJSONTokenTypeArray;
  while (next(string) != ']') {
    palJSONObject* child_node = ParseNode(string);
    node->_array.push_back(child_node);
    string.start = child_node->internal_string.start + child_node->internal_string.len;
    string.len -= child_node->internal_string.len;
    string = skipws(string);
    if (string.len == 0) {
      return node;
    }
  }
  string = acceptkeyword(string, "]");
  node->internal_string.start = start_string.start;
  node->internal_string.len = (string.start - start_string.start);
  return node;
}

palJSONObject* palJSONObjectParser::GetRoot() {
  return _root;
}

const palJSONObject* palJSONObjectParser::GetRoot() const {
  return _root;
}

void palJSONObjectPrinter::PrintObject(const palJSONObject* object, int depth /*= 0*/) {
  switch (object->type) {
  case kJSONTokenTypeValueFalse:
    palPrintf("false");
  break;
  case kJSONTokenTypeValueTrue:
    palPrintf("true");
  break;
  case kJSONTokenTypeValueNull:
    palPrintf("null");
  break;
  case kJSONTokenTypeValueString:
    palPrintf("\"%.*s\"", object->StringLength(), object->C());
  break;
  case kJSONTokenTypeValueNumber:
    {
      float f = object->GetAsFloat();
      palPrintf("%f", f);
    }
  break;
  case kJSONTokenTypeArray:
    {
      indent(depth);
      palPrintf("[\n");
      const palArray<palJSONObject*>* children = object->GetChildArray();
      const int num_children = children->GetSize();
      if (num_children > 0) {
        palJSONObject* child_object = NULL;
        for (int i = 0; i < num_children-1; i++) {
          child_object = children->operator[](i);
          PrintObject(child_object, depth+1);
          palPrintf(",");
          palPrintf("\n");
        }
        child_object = children->operator[](num_children-1);
        PrintObject(child_object, depth+1);
        palPrintf("\n");
      }
      indent(depth);
      palPrintf("]");
    }
  break;
  case kJSONTokenTypeMap:
    {
      indent(depth);
      palPrintf("{\n");
      const palArray<palJSONObject*>* children = object->GetChildArray();
      const palArray<palJSONString>* names = object->GetNameArray();
      const int num_children = children->GetSize();
      if (num_children > 0) {
        palJSONObject* child_object = NULL;
        palJSONString child_name;
        for (int i = 0; i < num_children-1; i++) {
          child_object = children->operator[](i);
          child_name = names->operator[](i);
          indent(depth+1);
          palPrintf("\"%.*s\" : ", child_name.len, child_name.start);
          PrintObject(child_object, depth+1);
          palPrintf(",");
          palPrintf("\n");
        }
        child_object = children->operator[](num_children-1);
        child_name = names->operator[](num_children-1);
        indent(depth+1);
        palPrintf("\"%.*s\" : ", child_name.len, child_name.start);
        PrintObject(child_object, depth+1);
        palPrintf("\n");
      }      
      indent(depth);
      palPrintf("}");
    }
  break;
  }
}

palJSONBuilder::palJSONBuilder() {
  _string = NULL;
}

void palJSONBuilder::Start(palDynamicString* string) {
  _string = string;
  _array_count = 0;
  _map_count = 0;
}

const char* palJSONBuilder::GetJSON() {
  if (!_string) {
    return NULL;
  }
  if (_array_count == 0 && _map_count == 0) {
    return _string->C();
  }
  return NULL;
}

int palJSONBuilder::PushObject() {
  _string->Append('{');
  _map_count++;
  return _map_count;
}

int palJSONBuilder::PopObject() {
  _string->Append('}');
  _map_count--;
  return _map_count;
}

void palJSONBuilder::Map(const char* name, uint32_t value, bool comma /*= false*/) {
  Map(name);
  Value(value);
  if (comma) {
    Comma();
  }
}

void palJSONBuilder::Map(const char* name, uint64_t value, bool comma /*= false*/) {
  Map(name);
  Value(value);
  if (comma) {
    Comma();
  }
}
void palJSONBuilder::Map(const char* name, int value, bool comma /*= false*/) {
  Map(name);
  Value(value);
  if (comma) {
    Comma();
  }
}

void palJSONBuilder::Map(const char* name, float value, bool comma /*= false*/) {
  Map(name);
  Value(value);
  if (comma) {
    Comma();
  }
}

void palJSONBuilder::Map(const char* name, const char* value, bool comma /*= false*/) {
  Map(name);
  Value(value);
  if (comma) {
    Comma();
  }
}

void palJSONBuilder::Map(const char* name) {
  _string->Append('"');
  _string->Append(name);
  _string->Append('"');
  _string->Append(':');
}

void palJSONBuilder::Map(const char* name, bool value, bool comma /*= false*/) {
  Map(name);
  Value(value);
  if (comma) {
    Comma();
  }
}

void palJSONBuilder::MapPrintf(const char* name, const char* fmt, ...) {
  va_list args;
  Map(name);
  _string->Append('"');
  va_start(args, fmt);
  _string->AppendPrintfInternal(fmt, args);
  va_end(args);
  _string->Append('"');
}

void palJSONBuilder::MapPrintfComma(const char* name, const char* fmt, ...) {
  va_list args;
  Map(name);
  _string->Append('"');
  va_start(args, fmt);
  _string->AppendPrintfInternal(fmt, args);
  va_end(args);
  _string->Append('"');
  Comma();
}

void palJSONBuilder::Comma() {
  _string->Append(',');
}

void palJSONBuilder::Value(uint32_t value, bool comma /*= false*/) {
  _string->AppendPrintf("%d", value);
  if (comma) {
    Comma();
  }
}

void palJSONBuilder::Value(uint64_t value, bool comma /*= false*/) {
  _string->AppendPrintf("%lld", value);
  if (comma) {
    Comma();
  }
}

void palJSONBuilder::Value(int value, bool comma /*= false*/) {
  _string->AppendPrintf("%d", value);
  if (comma) {
    Comma();
  }
}

void palJSONBuilder::Value(bool value, bool comma /*= false*/) {
  if (value) {
    _string->Append("true");
  } else {
    _string->Append("false");
  }
  if (comma) {
    Comma();
  }
}

extern void messagePrintf(const char*, ...);

void palJSONBuilder::Value(float value, bool comma /*= false*/) {
  _string->AppendPrintf("%f", value);
  if (comma) {
    Comma();
  }
}

void palJSONBuilder::Value(const char* value, bool comma /*= false*/) {
  _string->Append('"');
  _string->Append(value);
  _string->Append('"');
  if (comma) {
    Comma();
  }
}

void palJSONBuilder::ValuePrintf(const char* fmt, ...) {
  va_list args;
  _string->Append('"');
  va_start(args, fmt);
  _string->AppendPrintfInternal(fmt, args);
  va_end(args);
  _string->Append('"');
}

void palJSONBuilder::ValuePrintfComma(const char* fmt, ...) {
  va_list args;
  _string->Append('"');
  va_start(args, fmt);
  _string->AppendPrintfInternal(fmt, args);
  va_end(args);
  _string->Append('"');
  Comma();
}

int palJSONBuilder::PushArray() {
  _string->Append('[');
  _array_count++;
  return _array_count;
}

int palJSONBuilder::PopArray() {
  _string->Append(']');
  _array_count--;
  return _array_count;
}


