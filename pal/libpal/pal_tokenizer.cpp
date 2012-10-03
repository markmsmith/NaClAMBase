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
#include <stdlib.h>
#include <string.h>
#include "libpal/pal_string.h"
#include "libpal/pal_debug.h"
#include "libpal/pal_tokenizer.h"

struct palLexerPunctuation {
  const char* string;
  int id;
};

static palLexerPunctuation default_pal_lexer_punctuation[] = {
  {"&&", kPunctuationLOGICAL_AND},
  {"||", kPunctuationLOGICAL_OR},
  {">=", kPunctuationLOGICAL_GEQ},
  {"<=", kPunctuationLOGICAL_LEQ},
  {"==", kPunctuationLOGICAL_EQ},
  {"!=", kPunctuationLOGICAL_NOT_EQ},
  {"++", kPunctuationINC},
  {"--", kPunctuationDEC},
  {"*", kPunctuationMUL},
  {"%", kPunctuationPERCENT},
  {"/", kPunctuationDIV},
  {"-", kPunctuationSUB},
  {"+", kPunctuationADD},
  {"=", kPunctuationASSIGN},
  {",", kPunctuationCOMMA},
  {";", kPunctuationSEMICOLON},
  {":", kPunctuationCOLON},
  {"?", kPunctuationQUESTIONMARK},
  {"&", kPunctuationBINARY_AND},
  {"|", kPunctuationBINARY_OR},
  {"^", kPunctuationBINARY_XOR},
  {"~", kPunctuationBINARY_NOT},
  {"!", kPunctuationLOGICAL_NOT},
  {">", kPunctuationLOGICAL_GT},
  {"<", kPunctuationLOGICAL_LT},
  {"$", kPunctuationDOLLAR},
  {"#", kPunctuationHASH},
  {".", kPunctuationPERIOD},
  {"(", kPunctuationPARENTHESES_OPEN},
  {")", kPunctuationPARENTHESES_CLOSE},
  {"{", kPunctuationBRACE_OPEN},
  {"}", kPunctuationBRACE_CLOSE},
  {"[", kPunctuationSQUARE_BRACKET_OPEN},
  {"]", kPunctuationSQUARE_BRACKET_CLOSE}
};

const char* default_name_splitters = " -=!@#$%^&*()_+.,<>?/:;{}[]\n\r";

static bool IsNameSplitter(char ch, const char* name_splitters) {
  const char* p = name_splitters;

  while (*p != '\0') {
    if (*p == ch)
      return true;
    p++;
  }

  return false;
}

bool palToken::IsValueComputed() {
  return (type_flags & kTokenNumberValueComputed) != 0;
}

uint64_t palToken::GetIntegerValue() {
  if (type != kTokenNumber || (type_flags & kTokenNumberTypeFloat))
    return 0;

  if (type_flags & kTokenNumberValueComputed)
    return value_integer;

  const char* str = value_string.C();
  if (type_flags & kTokenNumberTypeHex) {
    int base = 16;
    value_integer = 0;
    palAssert(*str == '0');
    str++;
    palAssert(*str == 'x' || *str == 'X');
    str++;
    while (*str != 0) {
      value_integer *= base;
      value_integer += palHexDigitValue(*str);
      str++;
    }
    type_flags |= kTokenNumberValueComputed;
  } else {
    int base = 10;
    value_integer = 0;
    while (*str != 0) {
      value_integer *= base;
      value_integer += palDigitValue(*str);
      str++;
    }
    type_flags |= kTokenNumberValueComputed;
  }

  return value_integer;
}

float palToken::GetFloatValue() {
  if (type != kTokenNumber || (type_flags & kTokenNumberTypeFloat) == 0)
    return 0.0f;

  if (type_flags & kTokenNumberValueComputed)
    return value_float;

  const char* str = value_string.C();
  value_float = static_cast<float>(atof(str));
  value_integer = static_cast<int>(value_float);
  type_flags |= kTokenNumberValueComputed;
  return value_float;
}

palTokenizer::palTokenizer() {
  pushed_token_ = NULL;
  _saved_position = 0;
  status_ = kTokenizerErrorNONE;
  keyword_array_ = NULL;
  name_splitters_ = default_name_splitters;
  disable_float_parsing_ = false;
  disable_number_parsing_ = false;
  disable_punctuation_parsing_ = false;
  _stream = NULL;
}

palTokenizer::~palTokenizer() {
  ClearBuffer();
}

void palTokenizer::Reset() {
  buffer_p_saved_ = NULL;
  pushed_token_ = NULL;
  status_ = kTokenizerErrorNONE;
  _stream = NULL;
}

void palTokenizer::ClearBuffer() {
  Reset();
}

void palTokenizer::SavePosition() {
  _saved_position = _stream->GetPosition();
}

void palTokenizer::RestorePosition() {
  _stream->Seek(_saved_position, kPalStreamSeekOriginBegin);
}

void palTokenizer::SetKeywordArray(const palTokenizerKeyword* keywords) {
  keyword_array_ = keywords;
}

void  palTokenizer::SetNameSplitCharacters(const char* name_splitters) {
  name_splitters_ = name_splitters;
}

int palTokenizer::UseStream(palStreamInterface* stream) {
  if (stream->CanSeek() == false || stream->CanRead() == false) {
    return -1;
  }
  _stream = stream;
  return 0;
}

void palTokenizer::SetPunctuationParsing(bool on) {
  disable_punctuation_parsing_ = !on;
}

void palTokenizer::SetNumberParsing(bool on) {
  disable_number_parsing_ = !on;
}

void palTokenizer::SetFloatParsing(bool on) {
  disable_float_parsing_ = !on;
}


void	palTokenizer::PushToken(palToken* token) {
  if (pushed_token_) {
    status_ |= kTokenizerErrorPushedTwice;
    return;
  }
  pushed_token_ = token;
}

palToken* palTokenizer::PopToken() {
  if (pushed_token_) {
    palToken* token = pushed_token_;
    pushed_token_ = NULL;
    return token;
  }
  return NULL;
}

int palTokenizer::GetStatus() {
  return status_;
}

int palTokenizer::GetStreamIndex() {
  return (int)_stream->GetPosition();
}

bool palTokenizer::DoublePeekNextCh(char& ch) {
  if (_stream) {
    size_t position;
    size_t size = _stream->GetLength();
    position = _stream->GetPosition();
    if (position == size || position+1 == size) {
      ch = 0;
      return false;
    }
    int r;
    _stream->Seek(1, kPalStreamSeekOriginCurrent);
    size_t read_bytes;
    r = _stream->Read(&ch, 0, 1, &read_bytes);
    _stream->Seek(position, kPalStreamSeekOriginBegin);
    if (r != 0) {
      ch = 0;
      return false;
    }
    return true;
  } else {
    return false;
  }
}

bool palTokenizer::PeekNextCh(char& ch) {
  if (_stream) {
    size_t position;
    size_t size;
    size = _stream->GetLength();
    position = _stream->GetPosition();
    if (position == size) {
      ch = 0;
      return false;
    }
    int r;
    size_t read_bytes;
    r = _stream->Read(&ch, 0, 1, &read_bytes);
    if (r != 0) {
      ch = 0;
      return false;
    }
    _stream->Seek(position, kPalStreamSeekOriginBegin);
    return true;
  } else {
    return false;
  }
}



void palTokenizer::SkipNextCh() {
  if (_stream) {
    _stream->Seek(1, kPalStreamSeekOriginCurrent);
  }
}

void palTokenizer::UnreadCh() {
  if (_stream) {
    _stream->Seek(-1, kPalStreamSeekOriginCurrent);
  }
}

bool palTokenizer::ReadNextCh(char& ch) {
  if (_stream) {
    size_t position;
    size_t size;
    position = _stream->GetPosition();
    size = _stream->GetLength();
    if (position == size) {
      ch = 0;
      return false;
    }
    int r;
    size_t read_bytes;
    r = _stream->Read(&ch, 0, 1, &read_bytes);
    if (r != 0) {
      ch = 0;
      return false;
    }
    return true;
  } else {
    return false;
  }
}

bool palTokenizer::FetchNextToken(palToken* token) {
  ReadNextToken(token);
  if (token->type == kTokenEOS)
    return false;

  return true;
}

bool palTokenizer::FetchNextTokenTypeCheck(palTokenType type, palToken* token) {
  ReadNextToken(token);

  if (token->type == type)
    return true;

  return false;
}

bool palTokenizer::FetchInt(int* value) {
  palToken token;

  ReadNextToken(&token);
  int sign = 1;
  if (token.type == kTokenPunctuation && token.value_string.Equals("-")) {
    sign = -1;
    ReadNextToken(&token);
  }

  if (token.type != kTokenNumber || (token.type_flags & kTokenNumberTypeFloat)) {
    return false;
  }

  if (value)
    *value = sign * (int)token.GetIntegerValue();

  return true;
}

bool	palTokenizer::FetchBool(bool* value) {
  palToken token;

  ReadNextToken(&token);
  if (token.type != kTokenNumber || (token.type_flags & kTokenNumberTypeFloat)) {
    return false;
  }

  if (value)
    *value = token.GetIntegerValue() != 0;

  return true;
}

bool palTokenizer::FetchFloat(float* value) {
  palToken token;

  ReadNextToken(&token);
  float sign = 1.0f;
  if (token.type == kTokenPunctuation && token.value_string.Equals("-")) {
    sign = -1.0f;
    ReadNextToken(&token);
  }

  if (token.type != kTokenNumber || (token.type_flags & kTokenNumberTypeFloat)) {
    return false;
  }

  if (value)
    *value = sign * token.GetFloatValue();

  return true;
}

bool palTokenizer::ReadRestOfLineAsString(palDynamicString* result) {
  result->SetLength(0);
  char ch;
  while (true) {
    bool r = PeekNextCh(ch);
    if (r == false) {
      return r;
    }
    if (ch == '\r') {
      SkipNextCh();
      PeekNextCh(ch);
      if (ch == '\n') {
        SkipNextCh();
      }
      break;
    } else {
      SkipNextCh();
      result->Append(ch);
    }
  }
  return true;
}

bool palTokenizer::SkipRestOfLine() {
  palToken token;
  while (true) {
    ReadNextToken(&token);
    if (token.type == kTokenEOL)
      return true;
    else if (token.type == kTokenEOS)
      return false;
  }
  return false;
}

bool palTokenizer::SkipUntilKeyword(const char* keyword) {
  palToken token;
  while (true) {
    ReadNextToken(&token);
    if (token.type == kTokenEOS)
      return false;
    if (token.type == kTokenKeyword) {
      if (token.value_string.Equals(keyword)) {
        return true;
      }
    }
  }
  return false;
}

bool	palTokenizer::SkipUntilName(const char* word) {
  palToken token;
  while (true) {
    ReadNextToken(&token);
    if (token.type == kTokenEOS)
      return false;
    if (token.type == kTokenName) {
      if (token.value_string.Equals(word)) {
        return true;
      }
    }
  }
  return false;
}

void palTokenizer::ReadNextToken(palToken* token) {
  palToken* pushed_token = PopToken();
  if (pushed_token) {
    // we saved a token onto the stack, just return it
    *token = *pushed_token;
    pushed_token = NULL;
    return;
  }

  // read the next token
  token->value_string.SetLength(0); // clear the string
  token->type = kTokenEOS; 
  token->type_flags = 0;
  token->start_index = GetStreamIndex();
  token->length = 0;
  char ch;
  char dpch;

  int whiteSpaceCount;
  int lineCrossCount;
  ReadWhiteSpace(&whiteSpaceCount, &lineCrossCount);
  if (lineCrossCount > 0) {
    token->type = kTokenEOL;
    token->type_flags = lineCrossCount;
    token->length = GetStreamIndex() - token->start_index;
    return;
  }

  // reset start_index after skipping whitespace
  token->start_index = GetStreamIndex();
  token->length = 0;

  if (PeekNextCh(ch) == false) {
    // keep returning EOS if we can't read anything.
    return;
  }

  DoublePeekNextCh(dpch);

  if ( palIsAlpha(ch) ||
      (disable_number_parsing_ == true && palIsDigit(ch)) ||
      (disable_number_parsing_ == true && disable_punctuation_parsing_ == true)) {
    // name
    ReadName(token);
  } else if (disable_number_parsing_ == false && (palIsDigit(ch) || (ch == '-' && palIsDigit(dpch)) || (disable_float_parsing_ == false && ch == '.' && palIsDigit(dpch)))) {
    // number
    ReadNumber(token);
  } else if (ch == '\"') {
    // string
    ReadString(token);
  } else if (ch == '#') {
    // colour
    ReadColour(token);
  } else if (disable_punctuation_parsing_ == false) {
    // punctuation
    if (ch == '.')
    {
      token->type = kTokenPunctuation;
      token->type_flags = kPunctuationPERIOD;
      token->value_string.Append('.');
      SkipNextCh();
    } else {
      ReadPunctuation(token);
    }	
  }
  token->length = GetStreamIndex() - token->start_index;
#if 0
  if (token->type == kTokenString) {
    // string starts after opening "
    token->start_index++;
    // string ends before closing "
    token->length -= 2;
  }
#endif
}

void palTokenizer::ReadWhiteSpace(int* whitespace_count, int* newline_count) {
  int ws_count = 0;
  int nl_count = 0;

  char ch;
  while (PeekNextCh(ch)) {
    if (palIsWhiteSpace(ch)) {
      // tabs, cr, lf, space, other non-printable characters.
      SkipNextCh();
      ws_count++;
      if (ch == '\n') {
        nl_count++;
      }
    } else {
      break; // non whitespace character.
    }
  }

  if (whitespace_count) {
    *whitespace_count = ws_count;
  }
  if (newline_count) {
    *newline_count = nl_count;
  }
}

void palTokenizer::ReadString(palToken* token) {
  char ch;

  // skip over the quote character
  ReadNextCh(ch);

  while (PeekNextCh(ch)) {
    SkipNextCh();
    if (ch == '\0' || ch == '\"') {
      break;
    }
    token->value_string.Append(ch);
  }
  if (ch != '\"')  {
    // the string was never closed.
    token->type = kTokenERROR;
    token->type_flags = kTokenString;
    status_ = kTokenizerErrorBadToken;
  } else {
    token->type = kTokenString;
  }
  return;
}

void palTokenizer::ReadName(palToken* token)
{
  char ch;
  while (PeekNextCh(ch)) {
    if (palIsAlpha(ch) || palIsDigit(ch) || IsNameSplitter(ch, name_splitters_) == false) {
      SkipNextCh();
      token->value_string.Append(ch);
    } else {
      break;
    }
  }
  token->type = kTokenName;
  if (keyword_array_) {
    CheckKeyword(token);
  }
}

void palTokenizer::ReadColour(palToken* token) {
  char ch;
  PeekNextCh(ch);
  palAssert(ch == '#');
  SkipNextCh();

  int count = 0;
  while (PeekNextCh(ch)) {
    if (palIsHexDigit(ch)) {
      SkipNextCh();
      token->value_string.Append(ch);
      count++;
    } else {
      break;
    }
  }

  if (count != 6 && count != 8) {
    token->type = kTokenERROR;
    token->type_flags = kTokenColour;
    return;
  }

  token->type = kTokenColour;
}

void palTokenizer::ReadNumber(palToken* token) {
  char ch;
  char dpch;

  PeekNextCh(ch);
  DoublePeekNextCh(dpch);

  if (ch == '0' && (dpch == 'x' || dpch == 'X'))
  {
    // hex integer number
    SkipNextCh();
    SkipNextCh();
    token->value_string.Append(ch);
    token->value_string.Append(dpch);
    // read rest of number...
    token->type = kTokenNumber;
    token->type_flags = kTokenNumberTypeHex;
    while (PeekNextCh(ch)) {
      if (palIsHexDigit(ch) == false) {
        break;
      }
      SkipNextCh();
      token->value_string.Append(ch);
    }
  } else {
    bool float_number = false;
    if (ch == '-') {
      // handle the negative outside the loop
      token->value_string.Append(ch);
      SkipNextCh();
    }
    while (PeekNextCh(ch)) {
      if (palIsDigit(ch)) {
        token->value_string.Append(ch);
        SkipNextCh();
      } else if (disable_float_parsing_ == false && ch == '.') {
        token->value_string.Append(ch);
        SkipNextCh();
        float_number = true;
      } else {
        break;
      }
    }
    float_number = float_number || (disable_float_parsing_ == false && ch == 'e');
    if (float_number) {
      // read exponent
      if (ch == 'e') {
        token->value_string.Append(ch);
        
        SkipNextCh();
        ReadNextCh(ch);
        if (ch == '-' || ch == '+') {
          token->value_string.Append(ch);
        } else if (palIsDigit(ch)) {
          token->value_string.Append(ch);
        }

        while (PeekNextCh(ch)) {
          if (palIsDigit(ch)) {
            SkipNextCh();
            token->value_string.Append(ch);
          } else {
            break;
          }
        }
      }
      token->type = kTokenNumber;
      token->type_flags = kTokenNumberTypeFloat;
    } else {
      token->type = kTokenNumber;
      token->type_flags = kTokenNumberTypeInt;
    }
  }
}

void palTokenizer::ReadPunctuation(palToken* token)
{
  for (int i = 0; default_pal_lexer_punctuation[i].string; i++) {
    SavePosition(); // we are going to be doing a bunch of trial and error, so save the position
    char ch;
    palLexerPunctuation& punctuation = default_pal_lexer_punctuation[i];
    int s;
    for (s = 0; punctuation.string[s] && ReadNextCh(ch); s++) {
      if (ch != punctuation.string[s])
      {
        break;
      }
    }
    if (punctuation.string[s] == 0)  {
      // we have scanned a punctuation
      token->type = kTokenPunctuation;
      token->type_flags = punctuation.id;
      token->value_string.Append(punctuation.string);
      return;
    }
    RestorePosition();
  }
  token->type = kTokenERROR;
  token->type_flags = kTokenPunctuation;
}

void palTokenizer::CheckKeyword(palToken* token) {
  if (!keyword_array_)
    return;

  for (int i = 0; keyword_array_[i].keyword; i++) {
    if (token->value_string.Equals(keyword_array_[i].keyword)) {
      token->type = kTokenKeyword;
      token->type_flags = keyword_array_[i].keyword_id;
      break;
    }
  }
}