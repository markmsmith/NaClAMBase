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

#include "pal_utf8.h"
#include "pal_unicode_tables.h"
#include "libpal/pal_memory.h"

#define BREAK_PROP_PART1(Page, Char) \
	((break_property_table_part1[Page] >= PAL_UNICODE_MAX_TABLE_INDEX) \
	? (break_property_table_part1[Page] - PAL_UNICODE_MAX_TABLE_INDEX) \
	: (break_property_data[break_property_table_part1[Page]][Char]))

#define BREAK_PROP_PART2(Page, Char) \
	((break_property_table_part2[Page] >= PAL_UNICODE_MAX_TABLE_INDEX) \
	? (break_property_table_part2[Page] - PAL_UNICODE_MAX_TABLE_INDEX) \
	: (break_property_data[break_property_table_part2[Page]][Char]))

#define BREAK_PROP(Char) \
	(((Char) <= PAL_UNICODE_LAST_CHAR_PART1) \
	? BREAK_PROP_PART1 ((Char) >> 8, (Char) & 0xff) \
	: (((Char) >= 0xe0000 && (Char) <= PAL_UNICODE_LAST_CHAR) \
	? BREAK_PROP_PART2 (((Char) - 0xe0000) >> 8, (Char) & 0xff) \
	: PAL_UNICODE_BREAK_UNKNOWN))

#define ATTR_TABLE(Page) (((Page) <= PAL_UNICODE_LAST_PAGE_PART1) \
	? attr_table_part1[Page] \
	: attr_table_part2[(Page) - 0xe00])

#define ATTTABLE(Page, Char) \
	((ATTR_TABLE(Page) == PAL_UNICODE_MAX_TABLE_INDEX) ? 0 : (attr_data[ATTR_TABLE(Page)][Char]))

#define TTYPE_PART1(Page, Char) \
	((type_table_part1[Page] >= PAL_UNICODE_MAX_TABLE_INDEX) \
	? (type_table_part1[Page] - PAL_UNICODE_MAX_TABLE_INDEX) \
	: (type_data[type_table_part1[Page]][Char]))

#define TTYPE_PART2(Page, Char) \
	((type_table_part2[Page] >= PAL_UNICODE_MAX_TABLE_INDEX) \
	? (type_table_part2[Page] - PAL_UNICODE_MAX_TABLE_INDEX) \
	: (type_data[type_table_part2[Page]][Char]))

#define TYPE(Char) \
	(((Char) <= PAL_UNICODE_LAST_CHAR_PART1) \
	? TTYPE_PART1 ((Char) >> 8, (Char) & 0xff) \
	: (((Char) >= 0xe0000 && (Char) <= PAL_UNICODE_LAST_CHAR) \
	? TTYPE_PART2 (((Char) - 0xe0000) >> 8, (Char) & 0xff) \
	: PAL_UNICODE_UNASSIGNED))

#define IS(Type, Class)	(((uint32_t)1 << (Type)) & (Class))
#define OR(Type, Rest)	(((uint32_t)1 << (Type)) | (Rest))

#define ISALPHA(Type)	IS ((Type),				\
	OR (PAL_UNICODE_LOWERCASE_LETTER,	\
	OR (PAL_UNICODE_UPPERCASE_LETTER,	\
	OR (PAL_UNICODE_TITLECASE_LETTER,	\
	OR (PAL_UNICODE_MODIFIER_LETTER,	\
	OR (PAL_UNICODE_OTHER_LETTER,		0))))))

#define ISALDIGIT(Type)	IS ((Type),				\
	OR (PAL_UNICODE_DECIMAL_NUMBER,	\
	OR (PAL_UNICODE_LETTER_NUMBER,	\
	OR (PAL_UNICODE_OTHER_NUMBER,		\
	OR (PAL_UNICODE_LOWERCASE_LETTER,	\
	OR (PAL_UNICODE_UPPERCASE_LETTER,	\
	OR (PAL_UNICODE_TITLECASE_LETTER,	\
	OR (PAL_UNICODE_MODIFIER_LETTER,	\
	OR (PAL_UNICODE_OTHER_LETTER,		0)))))))))

#define ISMARK(Type)	IS ((Type),				\
	OR (PAL_UNICODE_NON_SPACING_MARK,	\
	OR (PAL_UNICODE_COMBINING_MARK,	\
	OR (PAL_UNICODE_ENCLOSING_MARK,	0))))

#define ISZEROWIDTHTYPE(Type)	IS ((Type),			\
	OR (PAL_UNICODE_NON_SPACING_MARK,	\
	OR (PAL_UNICODE_ENCLOSING_MARK,	\
	OR (PAL_UNICODE_FORMAT,		0))))

bool pal_unichar_isalnum   (pal_unichar c)
{
	return ISALDIGIT(c) != 0;
}

bool pal_unichar_isalpha   (pal_unichar c)
{
	return ISALPHA(c) != 0;
}

bool pal_unichar_iscntrl   (pal_unichar c)
{
	return TYPE (c) == PAL_UNICODE_CONTROL;
}

bool pal_unichar_isdigit   (pal_unichar c)
{
  return TYPE (c) == PAL_UNICODE_DECIMAL_NUMBER;
}

bool pal_unichar_isgraph   (pal_unichar c)
{
	return !IS (TYPE(c),
		OR (PAL_UNICODE_CONTROL,
		OR (PAL_UNICODE_FORMAT,
		OR (PAL_UNICODE_UNASSIGNED,
		OR (PAL_UNICODE_SURROGATE,
		OR (PAL_UNICODE_SPACE_SEPARATOR,
		0))))));
}

bool pal_unichar_islower   (pal_unichar c)
{
  return TYPE (c) == PAL_UNICODE_LOWERCASE_LETTER;
}
bool pal_unichar_isprint   (pal_unichar c)
{
	return !IS (TYPE(c),
		OR (PAL_UNICODE_CONTROL,
		OR (PAL_UNICODE_FORMAT,
		OR (PAL_UNICODE_UNASSIGNED,
		OR (PAL_UNICODE_SURROGATE,
		0)))));
}

bool pal_unichar_ispunct   (pal_unichar c)
{
	return IS (TYPE(c),
		OR (PAL_UNICODE_CONNECT_PUNCTUATION,
		OR (PAL_UNICODE_DASH_PUNCTUATION,
		OR (PAL_UNICODE_CLOSE_PUNCTUATION,
		OR (PAL_UNICODE_FINAL_PUNCTUATION,
		OR (PAL_UNICODE_INITIAL_PUNCTUATION,
		OR (PAL_UNICODE_OTHER_PUNCTUATION,
		OR (PAL_UNICODE_OPEN_PUNCTUATION,
		OR (PAL_UNICODE_CURRENCY_SYMBOL,
		OR (PAL_UNICODE_MODIFIER_SYMBOL,
		OR (PAL_UNICODE_MATH_SYMBOL,
		OR (PAL_UNICODE_OTHER_SYMBOL,
		0)))))))))))) != 0;
}

bool pal_unichar_isspace   (pal_unichar c)
{
	switch (c)
	{
		/* special-case these since Unicode thinks they are not spaces */
	case '\t':
	case '\n':
	case '\r':
	case '\f':
		return true;
		break;

	default:
		{
			return IS (TYPE(c),
				OR (PAL_UNICODE_SPACE_SEPARATOR,
				OR (PAL_UNICODE_LINE_SEPARATOR,
				OR (PAL_UNICODE_PARAGRAPH_SEPARATOR,
				0)))) != 0;
		}
		break;
	}
}
bool pal_unichar_isupper   (pal_unichar c)
{
  return TYPE (c) == PAL_UNICODE_UPPERCASE_LETTER;
}
bool pal_unichar_ishexdigit  (pal_unichar c)
{
	return ((c >= 'a' && c <= 'f')
		|| (c >= 'A' && c <= 'F')
		|| (TYPE (c) == PAL_UNICODE_DECIMAL_NUMBER));
}

#define N_ELEMENTS(arr) (sizeof (arr) / sizeof ((arr)[0]))

bool pal_unichar_istitle   (pal_unichar c)
{
	unsigned int i;
	for (i = 0; i < N_ELEMENTS (title_table); ++i)
		if (title_table[i][0] == c)
			return true;
	return false;
}
bool pal_unichar_isdefined (pal_unichar c)
{
	return !IS (TYPE(c),
		OR (PAL_UNICODE_UNASSIGNED,
		OR (PAL_UNICODE_SURROGATE,
		0)));
}

bool pal_unichar_iszerowidth(pal_unichar c)
{
	if (c == 0x00AD)
		return false;

	if (ISZEROWIDTHTYPE (TYPE (c)))
		return true;

	if ((c >= 0x1160 && c < 0x1200) || c == 0x200B)
		return true;

	return false;
}
bool pal_unichar_ismark     (pal_unichar c)
{
  return ISMARK (TYPE (c)) != 0;
}

pal_unichar pal_unichar_toupper (pal_unichar c)
{
  int t = TYPE (c);
  if (t == PAL_UNICODE_LOWERCASE_LETTER)
    {
      pal_unichar val = ATTTABLE (c >> 8, c & 0xff);
      if (val >= 0x1000000)
	{
	  const char *p = special_case_table + val - 0x1000000;
          val = pal_utf8_get_unichar (p);
	}
      /* Some lowercase letters, e.g., U+000AA, FEMININE ORDINAL INDICATOR,
       * do not have an uppercase equivalent, in which case val will be
       * zero. 
       */
      return val ? val : c;
    }
  else if (t == PAL_UNICODE_TITLECASE_LETTER)
    {
      unsigned int i;
      for (i = 0; i < N_ELEMENTS (title_table); ++i)
	{
	  if (title_table[i][0] == c)
	    return title_table[i][1];
	}
    }
  return c;

}
pal_unichar pal_unichar_tolower (pal_unichar c)
{
  int t = TYPE (c);
  if (t == PAL_UNICODE_UPPERCASE_LETTER)
    {
      pal_unichar val = ATTTABLE (c >> 8, c & 0xff);
      if (val >= 0x1000000)
	{
	  const char *p = special_case_table + val - 0x1000000;
	  return pal_utf8_get_unichar (p);
	}
      else
	{
	  /* Not all uppercase letters are guaranteed to have a lowercase
	   * equivalent.  If this is the case, val will be zero. */
	  return val ? val : c;
	}
    }
  else if (t == PAL_UNICODE_TITLECASE_LETTER)
    {
      unsigned int i;
      for (i = 0; i < N_ELEMENTS (title_table); ++i)
	{
	  if (title_table[i][0] == c)
	    return title_table[i][2];
	}
    }
  return c;
}
pal_unichar pal_unichar_totitle (pal_unichar c)
{
	unsigned int i;
	for (i = 0; i < N_ELEMENTS (title_table); ++i)
	{
		if (title_table[i][0] == c || title_table[i][1] == c
			|| title_table[i][2] == c)
			return title_table[i][0];
	}

	if (TYPE (c) == PAL_UNICODE_LOWERCASE_LETTER)
		return pal_unichar_toupper (c);

	return c;
}
int pal_unichar_digit_value (pal_unichar c)
{
	if (TYPE (c) == PAL_UNICODE_DECIMAL_NUMBER)
		return ATTTABLE (c >> 8, c & 0xff);
	return -1;
}

int pal_unichar_xdigit_value (pal_unichar c)
{
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (TYPE (c) == PAL_UNICODE_DECIMAL_NUMBER)
		return ATTTABLE (c >> 8, c & 0xff);
	return -1;
}

PALUnicodeType pal_unichar_type (pal_unichar c)
{
  return (PALUnicodeType)TYPE (c);
}

PALUnicodeBreakType pal_unichar_break_type (pal_unichar c)
{
	return (PALUnicodeBreakType)BREAK_PROP(c);
}

static int _utf8_skip_data[256] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

static void _pal_utf8_length_and_mask (const char* utf8, int& length, int& mask)
{
	if ((unsigned char)*utf8 < 128)
	{
		length = 1;
		mask = 0x7f;
	} else if ((*utf8 & 0xe0) == 0xc0) {
		length = 2;
		mask = 0x1f;
	} else if ((*utf8 & 0xf0) == 0xe0) {
		length = 3;
		mask = 0x0f;
	} else if ((*utf8 & 0xf8) == 0xf0) {
		length = 4;
		mask = 0x07;
	} else if ((*utf8 & 0xfc) == 0xf8) {
		length = 5;
		mask = 0x03;
	} else if ((*utf8 & 0xfe) == 0xfc) {
		length = 6;
		mask = 0x01;
	} else
		length = -1;
}


int			pal_utf8_length (const char* utf8)
{
	int length;
	int mask;
	_pal_utf8_length_and_mask(utf8, length, mask);
	return length;
}

int			pal_utf8_mask (const char* utf8)
{
	int length;
	int mask;
	_pal_utf8_length_and_mask(utf8, length, mask);
	return mask;
}

pal_unichar pal_utf8_get_unichar (const char* utf8)
{
	int length;
	int mask;
	_pal_utf8_length_and_mask(utf8, length, mask);
	pal_unichar result;
	result = utf8[0] & mask;
	for (int count = 1; count < length; count++)
	{
		if ((utf8[count] & 0xc0) != 0x80)
		{
			result = -1;
			break;
		}
		result <<= 6;
		result |= utf8[count] & 0x3f;
	}
	return result;
}

int pal_utf8_put_unichar (pal_unichar ch, char* utf8)
{
	int len = 0;    
	int first;

	if (ch < 0x80)
	{
		first = 0;
		len = 1;
	}
	else if (ch < 0x800)
	{
		first = 0xc0;
		len = 2;
	}
	else if (ch < 0x10000)
	{
		first = 0xe0;
		len = 3;
	}
	else if (ch < 0x200000)
	{
		first = 0xf0;
		len = 4;
	}
	else if (ch < 0x4000000)
	{
		first = 0xf8;
		len = 5;
	}
	else
	{
		first = 0xfc;
		len = 6;
	}

	if (utf8)
	{
		for (int i = len - 1; i > 0; i--)
		{
			utf8[i] = (ch & 0x3f) | 0x80;
			ch >>= 6;
		}
		utf8[0] = ch | first;
		utf8[len] = 0; // zero it off
	}

	return len;
}

bool		pal_utf8_valid_unichar (const pal_unichar ch)
{
	return (ch < 0x110000 && ((ch & 0xFFFFF800) != 0xD800) && (ch < 0xFDD0 || ch > 0xFDEF) && (ch & 0xFFFE) != 0xFFFE);
}

char* pal_utf8_prev_char (char* utf8_start, char* utf8)
{
	utf8--;
	for (; utf8 >= utf8_start; utf8--)
	{
		if ((*utf8 & 0xc0) != 0x80)
			return utf8;
	}
	return 0;
}

const char*	pal_utf8_prev_char (const char* utf8_start, char* utf8)
{
	utf8--;
	for (; utf8 >= utf8_start; utf8--)
	{
		if ((*utf8 & 0xc0) != 0x80)
			return utf8;
	}
	return 0;
}

char*		pal_utf8_next_char (char* utf8)
{
	return utf8 + _utf8_skip_data[*utf8];
}

const char* pal_utf8_next_char (const char* utf8)
{
	return utf8 + _utf8_skip_data[*utf8];
}