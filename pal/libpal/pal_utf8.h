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

#ifndef __PAL_UTF8_H__
#define __PAL_UTF8_H__

#include "libpal/pal_types.h"

/* These are the possible character classifications.
 * See http://www.unicode.org/Public/UNIDATA/UCD.html#General_Category_Values
 */
typedef enum
{
	PAL_UNICODE_CONTROL,
	PAL_UNICODE_FORMAT,
	PAL_UNICODE_UNASSIGNED,
	PAL_UNICODE_PRIVATE_USE,
	PAL_UNICODE_SURROGATE,
	PAL_UNICODE_LOWERCASE_LETTER,
	PAL_UNICODE_MODIFIER_LETTER,
	PAL_UNICODE_OTHER_LETTER,
	PAL_UNICODE_TITLECASE_LETTER,
	PAL_UNICODE_UPPERCASE_LETTER,
	PAL_UNICODE_COMBINING_MARK,
	PAL_UNICODE_ENCLOSING_MARK,
	PAL_UNICODE_NON_SPACING_MARK,
	PAL_UNICODE_DECIMAL_NUMBER,
	PAL_UNICODE_LETTER_NUMBER,
	PAL_UNICODE_OTHER_NUMBER,
	PAL_UNICODE_CONNECT_PUNCTUATION,
	PAL_UNICODE_DASH_PUNCTUATION,
	PAL_UNICODE_CLOSE_PUNCTUATION,
	PAL_UNICODE_FINAL_PUNCTUATION,
	PAL_UNICODE_INITIAL_PUNCTUATION,
	PAL_UNICODE_OTHER_PUNCTUATION,
	PAL_UNICODE_OPEN_PUNCTUATION,
	PAL_UNICODE_CURRENCY_SYMBOL,
	PAL_UNICODE_MODIFIER_SYMBOL,
	PAL_UNICODE_MATH_SYMBOL,
	PAL_UNICODE_OTHER_SYMBOL,
	PAL_UNICODE_LINE_SEPARATOR,
	PAL_UNICODE_PARAGRAPH_SEPARATOR,
	PAL_UNICODE_SPACE_SEPARATOR
} PALUnicodeType;

/* These are the possible line break classifications.
 * Note that new types may be added in the future.
 * Implementations may regard unknown values like PAL_UNICODE_BREAK_UNKNOWN
 * See http://www.unicode.org/unicode/reports/tr14/
 */
typedef enum
{
	PAL_UNICODE_BREAK_OPEN_PUNCTUATION,
	PAL_UNICODE_BREAK_CLOSE_PUNCTUATION,
	PAL_UNICODE_BREAK_CLOSE_PARENTHESIS,
	PAL_UNICODE_BREAK_QUOTATION,
	PAL_UNICODE_BREAK_NON_BREAKING_GLUE,
	PAL_UNICODE_BREAK_NON_STARTER,
	PAL_UNICODE_BREAK_EXCLAMATION,
	PAL_UNICODE_BREAK_SYMBOL,
	PAL_UNICODE_BREAK_INFIX_SEPARATOR,
	PAL_UNICODE_BREAK_PREFIX,
	PAL_UNICODE_BREAK_POSTFIX,
	PAL_UNICODE_BREAK_NUMERIC,
	PAL_UNICODE_BREAK_ALPHABETIC,
	PAL_UNICODE_BREAK_IDEOGRAPHIC,
	PAL_UNICODE_BREAK_INSEPARABLE,
	PAL_UNICODE_BREAK_HYPHEN,
	PAL_UNICODE_BREAK_AFTER,
	PAL_UNICODE_BREAK_BEFORE,
	PAL_UNICODE_BREAK_BEFORE_AND_AFTER,
	PAL_UNICODE_BREAK_ZERO_WIDTH_SPACE,
	PAL_UNICODE_BREAK_COMBINING_MARK,
	PAL_UNICODE_BREAK_WORD_JOINER,
	PAL_UNICODE_BREAK_HANGUL_LV_SYLLABLE,
	PAL_UNICODE_BREAK_HANGUL_LVT_SYLLABLE,
	PAL_UNICODE_BREAK_HANGUL_L_JAMO,
	PAL_UNICODE_BREAK_HANGUL_V_JAMO,
	PAL_UNICODE_BREAK_HANGUL_T_JAMO, // 26
	PAL_UNICODE_BREAK_MANDATORY,
	PAL_UNICODE_BREAK_CARRIAGE_RETURN,
	PAL_UNICODE_BREAK_LINE_FEED,
	PAL_UNICODE_BREAK_SURROGATE,  
	PAL_UNICODE_BREAK_CONTINGENT,
	PAL_UNICODE_BREAK_SPACE,
	PAL_UNICODE_BREAK_COMPLEX_CONTEXT,
	PAL_UNICODE_BREAK_AMBIGUOUS,
	PAL_UNICODE_BREAK_UNKNOWN,
	PAL_UNICODE_BREAK_NEXT_LINE,  
} PALUnicodeBreakType;

typedef uint32_t pal_unichar;

bool pal_unichar_isalnum   (pal_unichar c);
bool pal_unichar_isalpha   (pal_unichar c);
bool pal_unichar_iscntrl   (pal_unichar c);
bool pal_unichar_isdigit   (pal_unichar c);
bool pal_unichar_isgraph   (pal_unichar c);
bool pal_unichar_islower   (pal_unichar c);
bool pal_unichar_isprint   (pal_unichar c);
bool pal_unichar_ispunct   (pal_unichar c);
bool pal_unichar_isspace   (pal_unichar c);
bool pal_unichar_isupper   (pal_unichar c);
bool pal_unichar_ishexdigit  (pal_unichar c);
bool pal_unichar_istitle   (pal_unichar c);
bool pal_unichar_isdefined (pal_unichar c);
bool pal_unichar_iswide    (pal_unichar c);
bool pal_unichar_iswide_cjk(pal_unichar c);
bool pal_unichar_iszerowidth(pal_unichar c);
bool pal_unichar_ismark     (pal_unichar c);
pal_unichar pal_unichar_toupper (pal_unichar c);
pal_unichar pal_unichar_tolower (pal_unichar c);
pal_unichar pal_unichar_totitle (pal_unichar c);
int pal_unichar_digit_value (pal_unichar c);
int pal_unichar_xdigit_value (pal_unichar c);
PALUnicodeType pal_unichar_type (pal_unichar c);
PALUnicodeBreakType pal_unichar_break_type (pal_unichar c);

int			pal_utf8_length (const char* utf8);
int			pal_utf8_mask (const char* utf8);
pal_unichar pal_utf8_get_unichar (const char* utf8);
int			pal_utf8_put_unichar (pal_unichar ch, char* utf8);
bool		pal_utf8_valid_unichar (const pal_unichar ch);
char*		pal_utf8_prev_char (char* utf8_start, char* utf8);
const char*	pal_utf8_prev_char (const char* utf8_start, char* utf8);
char*		pal_utf8_next_char (char* utf8);
const char* pal_utf8_next_char (const char* utf8);

#endif