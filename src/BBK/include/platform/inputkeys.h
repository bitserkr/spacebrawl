#ifndef _INPUTKEYS_H
#define _INPUTKEYS_H

namespace bbk
{
// Ripped from SDL_keysym.h
enum InputKey{
	/**
	 * \name
	 * ASCII-mapped keysymbols, chosen to map to ASCII
	 */
	KB_UNKNOWN      = 0,
	KB_FIRST        = 0,
	KB_BACKSPACE    = 8,
	KB_TAB          = 9,
	KB_CLEAR		= 12,
	KB_RETURN		= 13,
	KB_PAUSE		= 19,
	KB_ESCAPE		= 27,
	KB_SPACE		= 32,
	KB_EXCLAIM      = 33,
	KB_QUOTEDBL     = 34,
	KB_HASH         = 35,
	KB_DOLLAR		= 36,
	KB_AMPERSAND	= 38,
	KB_QUOTE		= 39,
	KB_LEFTPAREN    = 40,
	KB_RIGHTPAREN   = 41,
	KB_ASTERISK     = 42,
	KB_PLUS         = 43,
	KB_COMMA		= 44,
	KB_MINUS		= 45,
	KB_PERIOD		= 46,
	KB_SLASH		= 47,
	KB_0            = 48,
	KB_1            = 49,
	KB_2            = 50,
	KB_3            = 51,
	KB_4            = 52,
	KB_5            = 53,
	KB_6            = 54,
	KB_7            = 55,
	KB_8            = 56,
	KB_9            = 57,
	KB_COLON		= 58,
	KB_SEMICOLON	= 59,
	KB_LESS		= 60,
	KB_EQUALS		= 61,
	KB_GREATER	= 62,
	KB_QUESTION	= 63,
	KB_AT			= 64,
	KB_A			= 65,
	KB_B			= 66,
	KB_C			= 67,
	KB_D			= 68,
	KB_E			= 69,
	KB_F			= 70,
	KB_G			= 71,
	KB_H			= 72,
	KB_I			= 73,
	KB_J			= 74,
	KB_K			= 75,
	KB_L			= 76,
	KB_M			= 77,
	KB_N			= 78,
	KB_O			= 79,
	KB_P			= 80,
	KB_Q			= 81,
	KB_R			= 82,
	KB_S			= 83,
	KB_T			= 84,
	KB_U			= 85,
	KB_V			= 86,
	KB_W			= 87,
	KB_X			= 88,
	KB_Y			= 89,
	KB_Z			= 90,
	KB_LEFTBRACKET  = 91,
	KB_BACKSLASH	= 92,
	KB_RIGHTBRACKET	= 93,
	KB_CARET		= 94,
	KB_UNDERSCORE	= 95,
	KB_BACKQUOTE	= 96,
	KB_a			= 97,
	KB_b			= 98,
	KB_c			= 99,
	KB_d			= 100,
	KB_e			= 101,
	KB_f			= 102,
	KB_g			= 103,
	KB_h			= 104,
	KB_i			= 105,
	KB_j			= 106,
	KB_k			= 107,
	KB_l			= 108,
	KB_m			= 109,
	KB_n			= 110,
	KB_o			= 111,
	KB_p			= 112,
	KB_q			= 113,
	KB_r			= 114,
	KB_s			= 115,
	KB_t			= 116,
	KB_u			= 117,
	KB_v			= 118,
	KB_w			= 119,
	KB_x			= 120,
	KB_y			= 121,
	KB_z			= 122,
	KB_DELETE		= 127,
	/* End of ASCII mapped keysyms */
	//\}

	/**
	 * \name International keyboard syms
	 *///\{
	KB_WORLD_0		= 160,		/* 0xA0 */
	KB_WORLD_1		= 161,
	KB_WORLD_2		= 162,
	KB_WORLD_3		= 163,
	KB_WORLD_4		= 164,
	KB_WORLD_5		= 165,
	KB_WORLD_6		= 166,
	KB_WORLD_7		= 167,
	KB_WORLD_8		= 168,
	KB_WORLD_9		= 169,
	KB_WORLD_10		= 170,
	KB_WORLD_11		= 171,
	KB_WORLD_12		= 172,
	KB_WORLD_13		= 173,
	KB_WORLD_14		= 174,
	KB_WORLD_15		= 175,
	KB_WORLD_16		= 176,
	KB_WORLD_17		= 177,
	KB_WORLD_18		= 178,
	KB_WORLD_19		= 179,
	KB_WORLD_20		= 180,
	KB_WORLD_21		= 181,
	KB_WORLD_22		= 182,
	KB_WORLD_23		= 183,
	KB_WORLD_24		= 184,
	KB_WORLD_25		= 185,
	KB_WORLD_26		= 186,
	KB_WORLD_27		= 187,
	KB_WORLD_28		= 188,
	KB_WORLD_29		= 189,
	KB_WORLD_30		= 190,
	KB_WORLD_31		= 191,
	KB_WORLD_32		= 192,
	KB_WORLD_33		= 193,
	KB_WORLD_34		= 194,
	KB_WORLD_35		= 195,
	KB_WORLD_36		= 196,
	KB_WORLD_37		= 197,
	KB_WORLD_38		= 198,
	KB_WORLD_39		= 199,
	KB_WORLD_40		= 200,
	KB_WORLD_41		= 201,
	KB_WORLD_42		= 202,
	KB_WORLD_43		= 203,
	KB_WORLD_44		= 204,
	KB_WORLD_45		= 205,
	KB_WORLD_46		= 206,
	KB_WORLD_47		= 207,
	KB_WORLD_48		= 208,
	KB_WORLD_49		= 209,
	KB_WORLD_50		= 210,
	KB_WORLD_51		= 211,
	KB_WORLD_52		= 212,
	KB_WORLD_53		= 213,
	KB_WORLD_54		= 214,
	KB_WORLD_55		= 215,
	KB_WORLD_56		= 216,
	KB_WORLD_57		= 217,
	KB_WORLD_58		= 218,
	KB_WORLD_59		= 219,
	KB_WORLD_60		= 220,
	KB_WORLD_61		= 221,
	KB_WORLD_62		= 222,
	KB_WORLD_63		= 223,
	KB_WORLD_64		= 224,
	KB_WORLD_65		= 225,
	KB_WORLD_66		= 226,
	KB_WORLD_67		= 227,
	KB_WORLD_68		= 228,
	KB_WORLD_69		= 229,
	KB_WORLD_70		= 230,
	KB_WORLD_71		= 231,
	KB_WORLD_72		= 232,
	KB_WORLD_73		= 233,
	KB_WORLD_74		= 234,
	KB_WORLD_75		= 235,
	KB_WORLD_76		= 236,
	KB_WORLD_77		= 237,
	KB_WORLD_78		= 238,
	KB_WORLD_79		= 239,
	KB_WORLD_80		= 240,
	KB_WORLD_81		= 241,
	KB_WORLD_82		= 242,
	KB_WORLD_83		= 243,
	KB_WORLD_84		= 244,
	KB_WORLD_85		= 245,
	KB_WORLD_86		= 246,
	KB_WORLD_87		= 247,
	KB_WORLD_88		= 248,
	KB_WORLD_89		= 249,
	KB_WORLD_90		= 250,
	KB_WORLD_91		= 251,
	KB_WORLD_92		= 252,
	KB_WORLD_93		= 253,
	KB_WORLD_94		= 254,
	KB_WORLD_95		= 255, /* 0xFF */
	//\}

	/**
	 * \name Numeric keypad
	 *///\{
	KB_KP0          = 256,
	KB_KP1          = 257,
	KB_KP2          = 258,
	KB_KP3          = 259,
	KB_KP4          = 260,
	KB_KP5          = 261,
	KB_KP6          = 262,
	KB_KP7          = 263,
	KB_KP8          = 264,
	KB_KP9          = 265,
	KB_KP_PERIOD    = 266,
	KB_KP_DIVIDE    = 267,
	KB_KP_MULTIPLY  = 268,
	KB_KP_MINUS     = 269,
	KB_KP_PLUS      = 270,
	KB_KP_ENTER     = 271,
	KB_KP_EQUALS    = 272,
	//\}

	/**
	 * \name Arrows + Home/End pad
	 *///\{
	KB_UP       = 273,
	KB_DOWN     = 274,
	KB_RIGHT    = 275,
	KB_LEFT     = 276,
	KB_INSERT   = 277,
	KB_HOME     = 278,
	KB_END      = 279,
	KB_PAGEUP   = 280,
	KB_PAGEDOWN = 281,
	//\}

	/**
	 * \name Function keys
	 *///\{
	KB_F1  = 282,
	KB_F2  = 283,
	KB_F3  = 284,
	KB_F4  = 285,
	KB_F5  = 286,
	KB_F6  = 287,
	KB_F7  = 288,
	KB_F8  = 289,
	KB_F9  = 290,
	KB_F10 = 291,
	KB_F11 = 292,
	KB_F12 = 293,
	KB_F13 = 294,
	KB_F14 = 295,
	KB_F15 = 296,
	//\}

	/**
	 * \name
	 * Modifier keys
	 *///\{
	KB_NUMLOCK   = 300,
	KB_CAPSLOCK  = 301,
	KB_SCROLLOCK = 302,
	KB_RSHIFT    = 303,
	KB_LSHIFT    = 304,
	KB_RCTRL     = 305,
	KB_LCTRL     = 306,
	KB_RALT      = 307,
	KB_LALT      = 308,
	KB_RMETA     = 309,
	KB_LMETA     = 310,
	KB_LSUPER    = 311, /**< Left "Windows" key    */
	KB_RSUPER    = 312, /**< Right "Windows" key   */
	KB_MODE      = 313, /**< "Alt Gr" key          */
	KB_COMPOSE   = 314, /**< Multi-key compose key */
	//\}

	/**
	 * \name
	 * Miscellaneous function keys
	 *///\{
	KB_HELP   = 315,
	KB_PRINT  = 316,
	KB_SYSREQ = 317,
	KB_BREAK  = 318,
	KB_MENU   = 319,
	KB_POWER  = 320,		/**< Power Macintosh power key */
	KB_EURO   = 321,		/**< Some european keyboards */
	KB_UNDO   = 322,		/**< Atari keyboard has Undo */
	//\}
	
	/* Add any other keys here */
	KB_NUM_KEYS,

	/**
	 * \name
	 * Mouse buttons
	 *///\{
	MOUSE_LMB = KB_NUM_KEYS,
	MOUSE_MMB,
	MOUSE_RMB,
	MOUSE_WHEELUP,
	MOUSE_WHEELDOWN,

	MOUSE_NUM_CTRLS = (MOUSE_WHEELDOWN - MOUSE_LMB) + 1
	//\}
};

/** Enumeration of valid key mods (possibly OR'd together for logical sum) */
enum InputModKeyMask{
	KB_MOD_NONE     = 0x0000,
	KB_MOD_LSHIFT   = 0x0001,
	KB_MOD_RSHIFT   = 0x0002,
	KB_MOD_LCTRL    = 0x0004,
	KB_MOD_RCTRL    = 0x0008,
	KB_MOD_LALT     = 0x0010,
	KB_MOD_RALT     = 0x0020,
	KB_MOD_LMETA    = 0x0040,
	KB_MOD_RMETA    = 0x0080,
	KB_MOD_NUM      = 0x0100,
	KB_MOD_CAPS     = 0x0200,
	KB_MOD_MODE     = 0x0400,
	KB_MOD_RESERVED = 0x0800
};
} // namespace bbk

#endif /* _INPUTKEYS_H */
