#pragma once

typedef enum {
	/* The keyboard syms have been cleverly chosen to map to ASCII */
	KB_SYM_BACKSPACE = 0x08,
	KB_SYM_TAB = 0x09,
	KB_SYM_RETURN		= 0x0D,
	KB_SYM_ESCAPE		= 0x1B,
	KB_SYM_SPACE		= 0x20,
	
	KB_SYM_0			= '0',
	KB_SYM_1			= '1',
	KB_SYM_2			= '2',
	KB_SYM_3			= '3',
	KB_SYM_4			= '4',
	KB_SYM_5			= '5',
	KB_SYM_6			= '6',
	KB_SYM_7			= '7',
	KB_SYM_8			= '8',
	KB_SYM_9			= '9',

	KB_SYM_a			= 'A',
	KB_SYM_b			= 'B',
	KB_SYM_c			= 'C',
	KB_SYM_d			= 'D',
	KB_SYM_e			= 'E',
	KB_SYM_f			= 'F',
	KB_SYM_g			= 'G',
	KB_SYM_h			= 'H',
	KB_SYM_i			= 'I',
	KB_SYM_j			= 'J',
	KB_SYM_k			= 'K',
	KB_SYM_l			= 'L',
	KB_SYM_m			= 'M',
	KB_SYM_n			= 'N',
	KB_SYM_o			= 'O',
	KB_SYM_p			= 'P',
	KB_SYM_q			= 'Q',
	KB_SYM_r			= 'R',
	KB_SYM_s			= 'S',
	KB_SYM_t			= 'T',
	KB_SYM_u			= 'U',
	KB_SYM_v			= 'V',
	KB_SYM_w			= 'W',
	KB_SYM_x			= 'X',
	KB_SYM_y			= 'Y',
	KB_SYM_z			= 'Z',

	/* Arrows + Home/End pad */
	KB_SYM_UP			= 0x26,
	KB_SYM_DOWN		= 0x28,
	KB_SYM_RIGHT		= 0x27,
	KB_SYM_LEFT		= 0x25,

	KB_SYM_INSERT		= 0x2D,
  KB_SYM_DELETE = 0x2E,

	KB_SYM_PAGEUP		= 0x21,
	KB_SYM_PAGEDOWN		= 0x22,

  KB_SYM_HOME		= 0x24,
  KB_SYM_END		= 0x23,
  
  

	/* Function keys */
	KB_SYM_F1			= 0x70,
	KB_SYM_F2			= 0x71,
	KB_SYM_F3			= 0x72,
	KB_SYM_F4			= 0x73,
	KB_SYM_F5			= 0x74,
	KB_SYM_F6			= 0x75,
	KB_SYM_F7			= 0x76,
	KB_SYM_F8			= 0x77,
	KB_SYM_F9			= 0x78,  
	KB_SYM_F10		= 0x79,
	KB_SYM_F11		= 0x7A,
	KB_SYM_F12		= 0x7B,
	KB_SYM_F13		= 0x7C,
	KB_SYM_F14		= 0x7D,
	KB_SYM_F15		= 0x7E,

	/* Key state modifier keys */
  KB_SYM_SHIFT    = 0x10,
  KB_SYM_LSHIFT		= 0xA0,
	KB_SYM_RSHIFT		= 0xA1,
  KB_SYM_CTRL     = 0x11,
	KB_SYM_LCTRL		= 0xA2,
	KB_SYM_RCTRL		= 0xA3,
  KB_SYM_ALT    = 0x12,
	KB_SYM_LALT		= 0xA4,
  KB_SYM_RALT		= 0xA5,
	KB_SYM_LSUPER		= 0x5B,		/* Left "Windows" key */
	KB_SYM_RSUPER		= 0x5C,		/* Right "Windows" key */

	KB_SYM_LAST
} kb_sym_t;

/* Enumeration of valid key mods (possibly OR'd together) */
typedef enum {
	KB_MOD_NONE  = 0x0000,
	KB_MOD_LSHIFT= 0x0001,
	KB_MOD_RSHIFT= 0x0002,
	KB_MOD_LCTRL = 0x0004,
	KB_MOD_RCTRL = 0x0008,
	KB_MOD_LALT  = 0x0010,
	KB_MOD_RALT  = 0x0020,
	KB_MOD_LAST
} kb_mod_t;

#define KB_MOD_CTRL	(KB_MOD_LCTRL|KB_MOD_RCTRL)
#define KB_MOD_SHIFT	(KB_MOD_LSHIFT|KB_MOD_RSHIFT)
#define KB_MOD_ALT	(KB_MOD_LALT|KB_MOD_RALT)
