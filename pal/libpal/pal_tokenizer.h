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

#ifndef LIBPAL_PAL_TOKENIZER_H_
#define LIBPAL_PAL_TOKENIZER_H_

#include "libpal/pal_types.h"
#include "libpal/pal_string.h"
#include "libpal/pal_stream_interface.h"

/* token types:
 * NUMBER
 * WORD
 * PUNCTUATION
 * STRING
 * COLOUR
 */

enum palTokenType {
  kTokenNumber,
  kTokenKeyword,
  kTokenName,
  kTokenPunctuation,
  kTokenString,
  kTokenColour,
  kTokenEOL,
  kTokenEOS,
  kTokenERROR
};

const int kTokenNumberTypeInt = 0x00000001;
const int kTokenNumberTypeFloat = 0x00000002;
const int kTokenNumberTypeHex = 0x00000004;
const int kTokenNumberValueComputed = 0x00001000;

const int kTokenizerErrorNONE = 0x00000000;
const int kTokenizerErrorBadToken = 0x00000001;
const int kTokenizerErrorPushedTwice = 0x00000002;
const int kTokenizerErrorMissingQuote = 0x00000004;
const int kTokenizerErrorBadColour = 0x00000008;

enum palTokenizerPunctuationType {
  kPunctuationLOGICAL_AND,
  kPunctuationLOGICAL_OR,
  kPunctuationLOGICAL_GEQ,
  kPunctuationLOGICAL_LEQ,
  kPunctuationLOGICAL_EQ,
  kPunctuationLOGICAL_NOT_EQ,
  kPunctuationINC,
  kPunctuationDEC,
  kPunctuationMUL,
  kPunctuationPERCENT,
  kPunctuationDIV,
  kPunctuationSUB,
  kPunctuationADD,
  kPunctuationASSIGN,
  kPunctuationCOMMA,
  kPunctuationSEMICOLON,
  kPunctuationCOLON,
  kPunctuationQUESTIONMARK,
  kPunctuationBINARY_AND,
  kPunctuationBINARY_OR,
  kPunctuationBINARY_XOR,
  kPunctuationBINARY_NOT,
  kPunctuationLOGICAL_NOT,
  kPunctuationLOGICAL_GT,
  kPunctuationLOGICAL_LT,
  kPunctuationDOLLAR,
  kPunctuationHASH,
  kPunctuationPERIOD,
  kPunctuationPARENTHESES_OPEN,
  kPunctuationPARENTHESES_CLOSE,
  kPunctuationBRACE_OPEN,
  kPunctuationBRACE_CLOSE,
  kPunctuationSQUARE_BRACKET_OPEN,
  kPunctuationSQUARE_BRACKET_CLOSE
};

struct palToken
{
  palTokenType  type;
  uint32_t			type_flags;

  bool IsValueComputed();
  uint64_t GetIntegerValue();
  float    GetFloatValue();

  size_t start_index;
  size_t length;

  // values
  uint64_t value_integer;
  float value_float;
  char value_ch;
  palDynamicString value_string;
};

struct palTokenizerKeyword {
  const char* keyword;
  uint32_t keyword_id;
};


class palTokenizer {
public:
  palTokenizer();
  ~palTokenizer();

  void  SetKeywordArray(const palTokenizerKeyword* keywords);
  // by default, most punctuation characters will break a name apart
  void  SetNameSplitCharacters(const char* name_splitters);

  int UseStream(palStreamInterface* stream);
  
  void SetPunctuationParsing(bool on);
  void SetNumberParsing(bool on);
  void SetFloatParsing(bool on);
  
  bool	FetchNextToken(palToken* token);
  bool	FetchNextTokenTypeCheck(palTokenType type, palToken* token);

  bool	FetchInt(int* value);
  bool	FetchBool(bool* value);
  bool	FetchFloat(float* value);

  void	PushToken(palToken* token); // pushes a token so that next call to FetchNextToken will return it.

  bool ReadRestOfLineAsString(palDynamicString* result);

  bool	SkipRestOfLine();
  bool	SkipUntilKeyword(const char* keyword);
  bool	SkipUntilName(const char* word);

  int		GetStatus(); 
protected:
  void Reset();
  void ClearBuffer();
  void ReadNextToken(palToken* token);
  void ReadWhiteSpace(int* whitespace_count, int* newline_count);
  void ReadString(palToken* token);
  void ReadName(palToken* token);
  void ReadNumber(palToken* token);
  void ReadColour(palToken* token);
  void ReadPunctuation(palToken* token);
  int GetStreamIndex();
  bool DoublePeekNextCh(char& ch);
  bool PeekNextCh(char& ch);
  void SkipNextCh();
  void UnreadCh();
  bool ReadNextCh(char& ch);
  void CheckKeyword(palToken* token); 
  void SavePosition();
  void RestorePosition();
  palToken* PopToken();

  palStreamInterface* _stream;
  size_t _saved_position;
  // tokenizer state
  palToken* pushed_token_;
  bool disable_number_parsing_;
  bool disable_punctuation_parsing_;
  bool disable_float_parsing_;
  char* buffer_p_saved_;
  uint32_t status_;
  const palTokenizerKeyword* keyword_array_;
  const char* name_splitters_;
};

#endif  // LIBPAL_PAL_TOKENIZER_H_