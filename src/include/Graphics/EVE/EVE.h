/*
@file    EVE.h
@brief   Contains FT80x/FT81x/BT81x API definitions
@version 5.0
@date    2024-01-07
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2023 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


@author mikee47 Jan 2024

Tidied and strengthened for C++ only

*/

#pragma once

#include <cstdint>

namespace Graphics::EVE
{
/* Memory */
constexpr uint32_t EVE_RAM_G = 0x00000000;
constexpr uint32_t EVE_ROM_CHIPID = 0x000C0000;
constexpr uint32_t EVE_ROM_FONT = 0x001E0000;
constexpr uint32_t EVE_ROM_FONTROOT = 0x002FFFFC;
constexpr uint32_t EVE_RAM_DL = 0x00300000;
constexpr uint32_t EVE_RAM_REG = 0x00302000;
constexpr uint32_t EVE_RAM_CMD = 0x00308000;

/* Memory buffer sizes */
constexpr uint32_t EVE_RAM_G_SIZE = 1024 * 1024;
constexpr uint32_t EVE_CMDFIFO_SIZE = 4 * 1024;
constexpr uint32_t EVE_RAM_DL_SIZE = 8 * 1024;
constexpr uint32_t EVE_MEMORY_SIZE = 0x00400000;

/**
 * @brief Diplay list commands
 *
 * These form the first byte (most significant) of any DL command word.
 */
enum DisplayListCommand {
	DL_DISPLAY = 0x00,
	DL_BITMAP_SOURCE = 0x01,
	DL_CLEAR_COLOR_RGB = 0x02,
	DL_TAG = 0x03,
	DL_COLOR_RGB = 0x04,
	DL_BITMAP_HANDLE = 0x05,
	DL_CELL = 0x06,
	DL_BITMAP_LAYOUT = 0x07,
	DL_BITMAP_SIZE = 0x08,
	DL_ALPHA_FUNC = 0x09,
	DL_STENCIL_FUNC = 0x0A,
	DL_BLEND_FUNC = 0x0B,
	DL_STENCIL_OP = 0x0C,
	DL_POINT_SIZE = 0x0D,
	DL_LINE_WIDTH = 0x0E,
	DL_CLEAR_COLOR_A = 0x0F,
	DL_COLOR_A = 0x10,
	DL_CLEAR_STENCIL = 0x11,
	DL_CLEAR_TAG = 0x12,
	DL_STENCIL_MASK = 0x13,
	DL_TAG_MASK = 0x14,
	DL_BITMAP_TRANSFORM_A = 0x15,
	DL_BITMAP_TRANSFORM_B = 0x16,
	DL_BITMAP_TRANSFORM_C = 0x17,
	DL_BITMAP_TRANSFORM_D = 0x18,
	DL_BITMAP_TRANSFORM_E = 0x19,
	DL_BITMAP_TRANSFORM_F = 0x1A,
	DL_SCISSOR_XY = 0x1B,
	DL_SCISSOR_SIZE = 0x1C,
	DL_CALL = 0x1D,
	DL_JUMP = 0x1E,
	DL_BEGIN = 0x1F,
	DL_COLOR_MASK = 0x20,
	DL_END = 0x21,
	DL_SAVE_CONTEXT = 0x22,
	DL_RESTORE_CONTEXT = 0x23,
	DL_RETURN = 0x24,
	DL_MACRO = 0x25,
	DL_CLEAR = 0x26,
	DL_VERTEX_FORMAT = 0x27,
	DL_BITMAP_LAYOUT_H = 0x28,
	DL_BITMAP_SIZE_H = 0x29,
	DL_PALETTE_SOURCE = 0x2A,
	DL_VERTEX_TRANSLATE_X = 0x2B,
	DL_VERTEX_TRANSLATE_Y = 0x2C,
	DL_NOP = 0x2D,
	// Top 2 bits reserved for these commands
	DL_VERTEX2F = 0x40,
	DL_VERTEX2II = 0x80,
};

static inline constexpr uint32_t MAKE_CMD_WORD(DisplayListCommand cmd, uint32_t param)
{
	return (uint32_t(cmd) << 24) | param;
}

/* Host commands (6-bit) with prefix 0'b01 */
enum class HostCommand : uint8_t {
	ACTIVE = 0x00,		 // place EVE in active state
	STANDBY = 0x41,		 // place EVE in Standby (clk running)
	SLEEP = 0x42,		 // place EVE in Sleep (clk off)
	CLKEXT = 0x44,		 // select external clock source
	CLKINT = 0x48,		 // select internal clock source, not a valid option for BT817 / BT818
	PWRDOWN = 0x50,		 // place EVE in Power Down (core off)
	CLKSEL = 0x61,		 // configure system clock
	RST_PULSE = 0x68,	// reset core - all registers default and processors reset
	CORERST = 0x68,		 // reset core - all registers default and processors reset
	PINDRIVE = 0x70,	 // setup drive strength for various pins
	PIN_PD_STATE = 0x71, // setup how pins behave during power down
};

/* Graphic command defines */
enum class TestFunction : uint8_t {
	NEVER = 0,
	LESS = 1,
	LEQUAL = 2,
	GREATER = 3,
	GEQUAL = 4,
	EQUAL = 5,
	NOTEQUAL = 6,
	ALWAYS = 7,
};

/* Bitmap formats */
enum BitmapFormat {
	BMF_ARGB1555 = 0,
	BMF_L1 = 1,
	BMF_L4 = 2,
	BMF_L8 = 3,
	BMF_RGB332 = 4,
	BMF_ARGB2 = 5,
	BMF_ARGB4 = 6,
	BMF_RGB565 = 7,
	BMF_PALETTED = 8,
	BMF_TEXT8X8 = 9,
	BMF_TEXTVGA = 10,
	BMF_BARGRAPH = 11,
	BMF_PALETTED565 = 14,
	BMF_PALETTED4444 = 15,
	BMF_PALETTED8 = 16,
	BMF_L2 = 17,
};

/* DL_BITMAP_SIZE filter types */
enum class BitmapFilter {
	NEAREST = 0,
	BILINEAR = 1,
};

/* DL_BITMAP_SIZE wrap types */
enum class BitmapWrap {
	BORDER = 0,
	REPEAT = 1,
};

/* Stencil test actions */
enum class StencilOp : uint8_t {
	ZERO = 0,
	KEEP = 1,
	REPLACE = 2,
	INCR = 3,
	DECR = 4,
	INVERT = 5,
};

/* REG_DLSWAP parameter */
enum DlSwap : uint8_t {
	EVE_DLSWAP_DONE = 0,
	EVE_DLSWAP_LINE = 1,
	EVE_DLSWAP_FRAME = 2,
};

/* Interrupt bit masks */
enum Interrupt {
	EVE_INT_SWAP = 0x01,
	EVE_INT_TOUCH = 0x02,
	EVE_INT_TAG = 0x04,
	EVE_INT_SOUND = 0x08,
	EVE_INT_PLAYBACK = 0x10,
	EVE_INT_CMDEMPTY = 0x20,
	EVE_INT_CMDFLAG = 0x40,
	EVE_INT_CONVCOMPLETE = 0x80,
};

/* REG_TOUCH_MODE values */
enum TouchMode {
	EVE_TMODE_OFF = 0,
	EVE_TMODE_ONESHOT = 1, ///< Resistive only
	EVE_TMODE_FRAME = 2,   ///< Resistive only
	EVE_TMODE_CONTINUOUS = 3,
};

/* Alpha blending */
enum class BlendFunction {
	ZERO = 0,
	ONE = 1,
	SRC_ALPHA = 2,
	DST_ALPHA = 3,
	ONE_MINUS_SRC_ALPHA = 4,
	ONE_MINUS_DST_ALPHA = 5,
};

/* Graphics primitives (DL_BEGIN) */
enum GraphicsPrimitive {
	GP_BITMAPS = 1,
	GP_POINTS = 2,
	GP_LINES = 3,
	GP_LINE_STRIP = 4,
	GP_EDGE_STRIP_R = 5,
	GP_EDGE_STRIP_L = 6,
	GP_EDGE_STRIP_A = 7,
	GP_EDGE_STRIP_B = 8,
	GP_RECTS = 9,
};

/* Widget command options */
enum WidgetOption : uint16_t {
	EVE_OPT_MONO = 1,
	EVE_OPT_NODL = 2,
	EVE_OPT_FLAT = 256,
	EVE_OPT_CENTERX = 512,
	EVE_OPT_CENTERY = 1024,
	EVE_OPT_CENTER = EVE_OPT_CENTERX | EVE_OPT_CENTERY,
	EVE_OPT_NOBACK = 4096,
	EVE_OPT_NOTICKS = 8192,
	EVE_OPT_NOHM = 16384,
	EVE_OPT_NOPOINTER = 16384,
	EVE_OPT_NOSECS = 32768,
	EVE_OPT_NOHANDS = 49152,
	EVE_OPT_RIGHTX = 2048,
	EVE_OPT_SIGNED = 256,

	EVE_OPT_MEDIAFIFO = 16,
	EVE_OPT_FULLSCREEN = 8,
	EVE_OPT_NOTEAR = 4,
	EVE_OPT_SOUND = 32,
};

/* REG_TOUCH_ADC_MODE */
enum ResistiveTouchAdcMode {
	EVE_ADC_DIFFERENTIAL = 1,
	EVE_ADC_SINGLE_ENDED = 0,
};

/* Fonts */
constexpr unsigned EVE_NUMCHAR_PERFONT = 128; ///< number of font characters per bitmap handle
constexpr unsigned EVE_FONT_TABLE_SIZE = 148; ///< size of the font table - utilized for loopup by the graphics engine
constexpr uint32_t EVE_FONT_TABLE_POINTER =
	0xFFFFC; //< pointer to the inbuilt font tables starting from bitmap handle 16

/* REG_PLAYBACK_FORMAT Audio sample type */
enum class SampleFormat {
	LINEAR = 0, ///< 8bit signed
	ULAW = 1,   ///< 8bit ulaw
	ADPCM = 2,  ///< 4bit IMA ADPCM
};

/* Synthesized sound */
enum class Sound : uint8_t {
	SILENCE = 0x00,
	SQUAREWAVE = 0x01,
	SINEWAVE = 0x02,
	SAWTOOTH = 0x03,
	TRIANGLE = 0x04,
	BEEPING = 0x05,
	ALARM = 0x06,
	WARBLE = 0x07,
	CAROUSEL = 0x08,
	PIPS_1 = 0x10,
	PIPS_2 = 0x11,
	PIPS_3 = 0x12,
	PIPS_4 = 0x13,
	PIPS_5 = 0x14,
	PIPS_6 = 0x15,
	PIPS_7 = 0x16,
	PIPS_8 = 0x17,
	PIPS_9 = 0x18,
	PIPS_10 = 0x19,
	PIPS_11 = 0x1A,
	PIPS_12 = 0x1B,
	PIPS_13 = 0x1C,
	PIPS_14 = 0x1D,
	PIPS_15 = 0x1E,
	PIPS_16 = 0x1F,
	DTMF_HASH = 0x23,
	DTMF_STAR = 0x2C,
	DTMF_0 = 0x30,
	DTMF_1 = 0x31,
	DTMF_2 = 0x32,
	DTMF_3 = 0x33,
	DTMF_4 = 0x34,
	DTMF_5 = 0x35,
	DTMF_6 = 0x36,
	DTMF_7 = 0x37,
	DTMF_8 = 0x38,
	DTMF_9 = 0x39,
	HARP = 0x40,
	XYLOPHONE = 0x41,
	TUBA = 0x42,
	GLOCKENSPIEL = 0x43,
	ORGAN = 0x44,
	TRUMPET = 0x45,
	PIANO = 0x46,
	CHIMES = 0x47,
	MUSICBOX = 0x48,
	BELL = 0x49,
	CLICK = 0x50,
	SWITCH = 0x51,
	COWBELL = 0x52,
	NOTCH = 0x53,
	HIHAT = 0x54,
	KICKDRUM = 0x55,
	POP = 0x56,
	CLACK = 0x57,
	CHACK = 0x58,
	MUTE = 0x60,
	UNMUTE = 0x61,
};

/* Synthesized sound frequencies, midi note */
enum MidiNote {
	NOTE_A0 = 21,
	NOTE_A_0 = 22,
	NOTE_B0 = 23,
	NOTE_C1 = 24,
	NOTE_C_1 = 25,
	NOTE_D1 = 26,
	NOTE_D_1 = 27,
	NOTE_E1 = 28,
	NOTE_F1 = 29,
	NOTE_F_1 = 30,
	NOTE_G1 = 31,
	NOTE_G_1 = 32,
	NOTE_A1 = 33,
	NOTE_A_1 = 34,
	NOTE_B1 = 35,
	NOTE_C2 = 36,
	NOTE_C_2 = 37,
	NOTE_D2 = 38,
	NOTE_D_2 = 39,
	NOTE_E2 = 40,
	NOTE_F2 = 41,
	NOTE_F_2 = 42,
	NOTE_G2 = 43,
	NOTE_G_2 = 44,
	NOTE_A2 = 45,
	NOTE_A_2 = 46,
	NOTE_B2 = 47,
	NOTE_C3 = 48,
	NOTE_C_3 = 49,
	NOTE_D3 = 50,
	NOTE_D_3 = 51,
	NOTE_E3 = 52,
	NOTE_F3 = 53,
	NOTE_F_3 = 54,
	NOTE_G3 = 55,
	NOTE_G_3 = 56,
	NOTE_A3 = 57,
	NOTE_A_3 = 58,
	NOTE_B3 = 59,
	NOTE_C4 = 60, ///< Middle C, 261.6 Hz (slightly flat?)
	NOTE_C_4 = 61,
	NOTE_D4 = 62,
	NOTE_D_4 = 63,
	NOTE_E4 = 64,
	NOTE_F4 = 65,
	NOTE_F_4 = 66,
	NOTE_G4 = 67,
	NOTE_G_4 = 68,
	NOTE_A4 = 69, ///< 440Hz (equal-tempered scale)
	NOTE_A_4 = 70,
	NOTE_B4 = 71,
	NOTE_C5 = 72,
	NOTE_C_5 = 73,
	NOTE_D5 = 74,
	NOTE_D_5 = 75,
	NOTE_E5 = 76,
	NOTE_F5 = 77,
	NOTE_F_5 = 78,
	NOTE_G5 = 79,
	NOTE_G_5 = 80,
	NOTE_A5 = 81,
	NOTE_A_5 = 82,
	NOTE_B5 = 83,
	NOTE_C6 = 84,
	NOTE_C_6 = 85,
	NOTE_D6 = 86,
	NOTE_D_6 = 87,
	NOTE_E6 = 88,
	NOTE_F6 = 89,
	NOTE_F_6 = 90,
	NOTE_G6 = 91,
	NOTE_G_6 = 92,
	NOTE_A6 = 93,
	NOTE_A_6 = 94,
	NOTE_B6 = 95,
	NOTE_C7 = 96,
	NOTE_C_7 = 97,
	NOTE_D7 = 98,
	NOTE_D_7 = 99,
	NOTE_E7 = 100,
	NOTE_F7 = 101,
	NOTE_F_7 = 102,
	NOTE_G7 = 103,
	NOTE_G_7 = 104,
	NOTE_A7 = 105,
	NOTE_A_7 = 106,
	NOTE_B7 = 107,
	NOTE_C8 = 108,
};

/**
 * @brief Co-processor commands
 *
 * These form the last byte (least significant) of any command word, as 0xffffffxx
 */
enum CoproCommand {
	CMD_DLSTART = 0x00,
	CMD_SWAP = 0x01,
	CMD_INTERRUPT = 0x02,
	CMD_BGCOLOR = 0x09,
	CMD_FGCOLOR = 0x0A,
	CMD_GRADIENT = 0x0B,
	CMD_TEXT = 0x0C,
	CMD_BUTTON = 0x0D,
	CMD_KEYS = 0x0E,
	CMD_PROGRESS = 0x0F,
	CMD_SLIDER = 0x10,
	CMD_SCROLLBAR = 0x11,
	CMD_TOGGLE = 0x12,
	CMD_GAUGE = 0x13,
	CMD_CLOCK = 0x14,
	CMD_CALIBRATE = 0x15,
	CMD_SPINNER = 0x16,
	CMD_STOP = 0x17,
	CMD_MEMCRC = 0x18,
	CMD_REGREAD = 0x19,
	CMD_MEMWRITE = 0x1A,
	CMD_MEMSET = 0x1B,
	CMD_MEMZERO = 0x1C,
	CMD_MEMCPY = 0x1D,
	CMD_APPEND = 0x1E,
	CMD_SNAPSHOT = 0x1F,
	CMD_INFLATE = 0x22,
	CMD_GETPTR = 0x23,
	CMD_LOADIMAGE = 0x24,
	CMD_GETPROPS = 0x25,
	CMD_LOADIDENTITY = 0x26,
	CMD_TRANSLATE = 0x27,
	CMD_SCALE = 0x28,
	CMD_ROTATE = 0x29,
	CMD_SETMATRIX = 0x2A,
	CMD_SETFONT = 0x2B,
	CMD_TRACK = 0x2C,
	CMD_DIAL = 0x2D,
	CMD_NUMBER = 0x2E,
	CMD_SCREENSAVER = 0x2F,
	CMD_SKETCH = 0x30,
	CMD_LOGO = 0x31,
	CMD_COLDSTART = 0x32,
	CMD_GETMATRIX = 0x33,
	CMD_GRADCOLOR = 0x34,
	CMD_SETROTATE = 0x36,
	CMD_SNAPSHOT2 = 0x37,
	CMD_SETBASE = 0x38,
	CMD_MEDIAFIFO = 0x39,
	CMD_PLAYVIDEO = 0x3A,
	CMD_SETFONT2 = 0x3B,
	CMD_SETSCRATCH = 0x3C,
	CMD_ROMFONT = 0x3F,
	CMD_VIDEOSTART = 0x40,
	CMD_VIDEOFRAME = 0x41,
	CMD_SYNC = 0x42,
	CMD_SETBITMAP = 0x43,
};

static inline constexpr uint32_t MAKE_COPROC_CMD_WORD(CoproCommand cmd)
{
	return 0xffffff00UL | cmd;
}

/* Registers */
enum Register : uint32_t {
	REG_ID = 0x00302000,
	REG_FRAMES = 0x00302004,
	REG_CLOCK = 0x00302008,
	REG_FREQUENCY = 0x0030200c,
	REG_RENDERMODE = 0x00302010, // only listed in datasheet
	REG_SNAPY = 0x00302014,		 // only listed in datasheet
	REG_SNAPSHOT = 0x00302018,   // only listed in datasheet
	REG_SNAPFORMAT = 0x0030201c, // only listed in datasheet
	REG_CPURESET = 0x00302020,
	REG_TAP_CRC = 0x00302024,  // only listed in datasheet
	REG_TAP_MASK = 0x00302028, // only listed in datasheet
	REG_HCYCLE = 0x0030202c,
	REG_HOFFSET = 0x00302030,
	REG_HSIZE = 0x00302034,
	REG_HSYNC0 = 0x00302038,
	REG_HSYNC1 = 0x0030203c,
	REG_VCYCLE = 0x00302040,
	REG_VOFFSET = 0x00302044,
	REG_VSIZE = 0x00302048,
	REG_VSYNC0 = 0x0030204c,
	REG_VSYNC1 = 0x00302050,
	REG_DLSWAP = 0x00302054,
	REG_ROTATE = 0x00302058,
	REG_OUTBITS = 0x0030205c,
	REG_DITHER = 0x00302060,
	REG_SWIZZLE = 0x00302064,
	REG_CSPREAD = 0x00302068,
	REG_PCLK_POL = 0x0030206c,
	REG_PCLK = 0x00302070,
	REG_TAG_X = 0x00302074,
	REG_TAG_Y = 0x00302078,
	REG_TAG = 0x0030207c,
	REG_VOL_PB = 0x00302080,
	REG_VOL_SOUND = 0x00302084,
	REG_SOUND = 0x00302088,
	REG_PLAY = 0x0030208c,
	REG_GPIO_DIR = 0x00302090,
	REG_GPIO = 0x00302094,
	REG_GPIOX_DIR = 0x00302098,
	REG_GPIOX = 0x0030209c,
	// RESERVED 0x003020a0
	// RESERVED 0x003020a4
	REG_INT_FLAGS = 0x003020a8,
	REG_INT_EN = 0x003020ac,
	REG_INT_MASK = 0x003020b0,
	REG_PLAYBACK_START = 0x003020b4,
	REG_PLAYBACK_LENGTH = 0x003020b8,
	REG_PLAYBACK_READPTR = 0x003020bc,
	REG_PLAYBACK_FREQ = 0x003020c0,
	REG_PLAYBACK_FORMAT = 0x003020c4,
	REG_PLAYBACK_LOOP = 0x003020c8,
	REG_PLAYBACK_PLAY = 0x003020cc,
	REG_PWM_HZ = 0x003020d0,
	REG_PWM_DUTY = 0x003020d4,
	REG_MACRO_0 = 0x003020d8,
	REG_MACRO_1 = 0x003020dc,
	// RESERVED 0x003020e0
	// RESERVED 0x003020e4
	// RESERVED 0x003020e8
	// RESERVED 0x003020ec
	// RESERVED 0x003020f0
	// RESERVED 0x003020f4
	REG_CMD_READ = 0x003020f8,
	REG_CMD_WRITE = 0x003020fc,
	REG_CMD_DL = 0x00302100,
	REG_TOUCH_MODE = 0x00302104,
	REG_TOUCH_ADC_MODE = 0x00302108,
	REG_CTOUCH_EXTENDED = REG_TOUCH_ADC_MODE,
	REG_TOUCH_CHARGE = 0x0030210c,
	REG_TOUCH_SETTLE = 0x00302110,
	REG_TOUCH_OVERSAMPLE = 0x00302114,
	REG_TOUCH_RZTHRESH = 0x00302118,
	REG_TOUCH_RAW_XY = 0x0030211c,
	REG_CTOUCH_TOUCH1_XY = REG_TOUCH_RAW_XY,
	REG_TOUCH_RZ = 0x00302120,
	REG_CTOUCH_TOUCH4_Y = REG_TOUCH_RZ,
	REG_TOUCH_SCREEN_XY = 0x00302124,
	REG_CTOUCH_TOUCH_XY = REG_TOUCH_SCREEN_XY, // only listed in datasheet
	REG_TOUCH_TAG_XY = 0x00302128,
	REG_TOUCH_TAG = 0x0030212c,
	REG_TOUCH_TAG1_XY = 0x00302130, // only listed in datasheet
	REG_TOUCH_TAG1 = 0x00302134,	// only listed in datasheet
	REG_TOUCH_TAG2_XY = 0x00302138, // only listed in datasheet
	REG_TOUCH_TAG2 = 0x0030213c,	// only listed in datasheet
	REG_TOUCH_TAG3_XY = 0x00302140, // only listed in datasheet
	REG_TOUCH_TAG3 = 0x00302144,	// only listed in datasheet
	REG_TOUCH_TAG4_XY = 0x00302148, // only listed in datasheet
	REG_TOUCH_TAG4 = 0x0030214c,	// only listed in datasheet
	REG_TOUCH_TRANSFORM_A = 0x00302150,
	REG_TOUCH_TRANSFORM_B = 0x00302154,
	REG_TOUCH_TRANSFORM_C = 0x00302158,
	REG_TOUCH_TRANSFORM_D = 0x0030215c,
	REG_TOUCH_TRANSFORM_E = 0x00302160,
	REG_TOUCH_TRANSFORM_F = 0x00302164,
	REG_TOUCH_CONFIG = 0x00302168,
	REG_CTOUCH_TOUCH4_X = 0x0030216c,
	// RESERVED 0x00302170
	REG_BIST_EN = 0x00302174, // only listed in datasheet
	// RESERVED 0x00302178
	// RESERVED 0x0030217c
	REG_TRIM = 0x00302180,
	REG_ANA_COMP = 0x00302184,  // only listed in datasheet
	REG_SPI_WIDTH = 0x00302188, // listed with false offset in programmers guide V1.1
	REG_TOUCH_DIRECT_XY = 0x0030218c,
	REG_CTOUCH_TOUCH2_XY = REG_TOUCH_DIRECT_XY,
	REG_TOUCH_DIRECT_Z1Z2 = 0x00302190,
	REG_CTOUCH_TOUCH3_XY = REG_TOUCH_DIRECT_Z1Z2,
	// RESERVED 0x00302194 - 0x00302560
	REG_DATESTAMP0 = 0x00302564, // 128 bits (4 words) only listed in datasheet
	REG_DATESTAMP1 = 0x00302568,
	REG_DATESTAMP2 = 0x0030256C,
	REG_DATESTAMP3 = 0x00302570,
	REG_CMDB_SPACE = 0x00302574,
	REG_CMDB_WRITE = 0x00302578,
	// Co-processor registers - see programmers' guide
	REG_TRACKER = 0x00309000,
	REG_TRACKER_1 = 0x00309004,
	REG_TRACKER_2 = 0x00309008,
	REG_TRACKER_3 = 0x0030900c,
	REG_TRACKER_4 = 0x00309010,
	REG_MEDIAFIFO_READ = 0x00309014,
	REG_MEDIAFIFO_WRITE = 0x00309018,
};

/* Macros for static display list generation */

/**
 * @brief Define an inline function for emitting 32-bit DL command word with arguments
 */
#define INLINE_DL_COMMAND(function_name, ...) static inline constexpr uint32_t function_name(__VA_ARGS__)

INLINE_DL_COMMAND(BEGIN, GraphicsPrimitive prim)
{
	return MAKE_CMD_WORD(DL_BEGIN, prim);
}

INLINE_DL_COMMAND(END)
{
	return MAKE_CMD_WORD(DL_END, 0);
}

/**
 * @brief End the display list.
 */
INLINE_DL_COMMAND(DISPLAY)
{
	return MAKE_CMD_WORD(DL_DISPLAY, 0);
}

/**
 * @brief Set the alpha test function.
 */
INLINE_DL_COMMAND(ALPHA_FUNC, TestFunction func, uint8_t ref)
{
	const uint32_t funcv = (unsigned(func) & 7) << 8;
	return MAKE_CMD_WORD(DL_ALPHA_FUNC, funcv | ref);
}

/**
 * @brief Set the bitmap handle.
 */
INLINE_DL_COMMAND(BITMAP_HANDLE, uint8_t handle)
{
	return MAKE_CMD_WORD(DL_BITMAP_HANDLE, handle & 0x1F);
}

/**
 * @brief Set the source bitmap memory format and layout for the current handle.
 */
INLINE_DL_COMMAND(BITMAP_LAYOUT, BitmapFormat format, uint16_t linestride, uint16_t height)
{
	const uint32_t formatv = uint32_t(format & 0x1F) << 19;
	const uint32_t linestridev = uint32_t(linestride & 0x3FF) << 9;
	const uint32_t heightv = height & 0x1ff;
	return MAKE_CMD_WORD(DL_BITMAP_LAYOUT, formatv | linestridev | heightv);
}

/**
 * @brief Set the source bitmap memory format and layout for the current handle.
 */
INLINE_DL_COMMAND(BITMAP_SIZE, BitmapFilter filter, BitmapWrap wrapx, BitmapWrap wrapy, uint16_t width, uint16_t height)
{
	const uint32_t filterv = (unsigned(filter) & 0x01) << 20;
	const uint32_t wrapxv = (unsigned(wrapx) & 0x01) << 19;
	const uint32_t wrapyv = (unsigned(wrapy) & 0x01) << 18;
	const uint32_t widthv = (width & 0x01FF) << 9;
	const uint32_t heightv = height & 0x01FF;
	return MAKE_CMD_WORD(DL_BITMAP_SIZE, filterv | wrapxv | wrapyv | widthv | heightv);
}

/**
 * @brief Set the 2 most significant bits of the source bitmap memory format and layout for the current handle.
 * @param linestride 12-bit value specified to BITMAP_LAYOUT
 * @param height 11-bit value specified to BITMAP_LAYOUT
 * @note this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits
 */
INLINE_DL_COMMAND(BITMAP_LAYOUT_H, uint16_t linestride, uint16_t height)
{
	const uint32_t linestridev = ((linestride >> 10) & 0x03) << 2;
	const uint32_t heightv = (height >> 9) & 0x03;
	return MAKE_CMD_WORD(DL_BITMAP_LAYOUT_H, linestridev | heightv);
}

/**
 * @brief Set the 2 most significant bits of bitmaps dimension for the current handle.
 * @param linestride 11-bit value of bitmap width, the 2 most significant bits are used
 * @param height 11-bit value of bitmap width, the 2 most significant bits are used
 * @note this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits
 */
INLINE_DL_COMMAND(BITMAP_SIZE_H, uint16_t width, uint16_t height)
{
	const uint32_t widthv = ((width >> 9) & 0x03) << 2;
	const uint32_t heightv = (height >> 9) & 0x03;
	return MAKE_CMD_WORD(DL_BITMAP_SIZE_H, widthv | heightv);
}

/**
 * @brief Set the source address of bitmap data in RAM_G or flash memory.
 */
INLINE_DL_COMMAND(BITMAP_SOURCE, uint32_t addr)
{
	return MAKE_CMD_WORD(DL_BITMAP_SOURCE, addr & 0x3FFFFF);
}

/**
 * @brief Set the A coefficient of the bitmap transform matrix.
 */
INLINE_DL_COMMAND(BITMAP_TRANSFORM_A, uint32_t val)
{
	return MAKE_CMD_WORD(DL_BITMAP_TRANSFORM_A, val & 0x1FFFF);
}

/**
 * @brief Set the B coefficient of the bitmap transform matrix.
 */
INLINE_DL_COMMAND(BITMAP_TRANSFORM_B, uint32_t val)
{
	return MAKE_CMD_WORD(DL_BITMAP_TRANSFORM_B, val & 0x1FFFF);
}

/**
 * @brief Set the D coefficient of the bitmap transform matrix.
 */
INLINE_DL_COMMAND(BITMAP_TRANSFORM_D, uint32_t val)
{
	return MAKE_CMD_WORD(DL_BITMAP_TRANSFORM_D, val & 0x1FFFF);
}

/**
 * @brief Set he E coefficient of the bitmap transform matrix.
 */
INLINE_DL_COMMAND(BITMAP_TRANSFORM_E, uint32_t val)
{
	return MAKE_CMD_WORD(DL_BITMAP_TRANSFORM_E, val & 0x1FFFF);
}

/**
 * @brief Set the C coefficient of the bitmap transform matrix.
 */
INLINE_DL_COMMAND(BITMAP_TRANSFORM_C, uint32_t val)
{
	return MAKE_CMD_WORD(DL_BITMAP_TRANSFORM_C, val & 0x1FFFF);
}

/**
 * @brief Set the F coefficient of the bitmap transform matrix.
 */
INLINE_DL_COMMAND(BITMAP_TRANSFORM_F, uint32_t val)
{
	return MAKE_CMD_WORD(DL_BITMAP_TRANSFORM_F, val & 0x1FFFF);
}

/**
 * @brief Execute a sequence of commands at another location in the display list.
 */
INLINE_DL_COMMAND(BLEND_FUNC, BlendFunction src, BlendFunction dst)
{
	const uint32_t srcv = (unsigned(src) & 0x07) << 3;
	const uint32_t dstv = unsigned(dst) & 0x07;
	return MAKE_CMD_WORD(DL_BLEND_FUNC, srcv | dstv);
}

/**
 * @brief Execute a sequence of commands at another location in the display list.
 */
INLINE_DL_COMMAND(CALL, uint16_t dest)
{
	return MAKE_CMD_WORD(DL_CALL, dest);
}

/**
 * @brief Execute commands at another location in the display list.
 */
INLINE_DL_COMMAND(JUMP, uint16_t dest)
{
	return MAKE_CMD_WORD(DL_JUMP, dest);
}

/**
 * @brief Set the bitmap cell number for the VERTEX2F command.
 */
INLINE_DL_COMMAND(CELL, uint8_t cell)
{
	return MAKE_CMD_WORD(DL_CELL, cell & 0x7F);
}

/**
 * @brief Clear buffers to preset values.
 */
INLINE_DL_COMMAND(CLEAR, bool color, bool stencil, bool tag)
{
	const uint32_t colorv = (color & 0x01) << 2;
	const uint32_t stencilv = (stencil & 0x01) << 1;
	const uint32_t tagv = tag & 0x01;
	return MAKE_CMD_WORD(DL_CLEAR, colorv | stencilv | tagv);
}

/**
 * @brief Set clear value for the alpha channel.
 */
INLINE_DL_COMMAND(CLEAR_COLOR_A, uint8_t alpha)
{
	return MAKE_CMD_WORD(DL_CLEAR_COLOR_A, alpha);
}

/**
 * @brief Set clear values for red, green and blue channels.
 */
INLINE_DL_COMMAND(CLEAR_COLOR_RGB, uint8_t red, uint8_t green, uint8_t blue)
{
	const uint32_t redv = uint32_t(red) << 16;
	const uint32_t greenv = uint32_t(green) << 8;
	const uint32_t bluev = blue;
	return MAKE_CMD_WORD(DL_CLEAR_COLOR_RGB, redv | greenv | bluev);
}

/**
 * @brief Set clear value for the stencil buffer.
 */
INLINE_DL_COMMAND(CLEAR_STENCIL, uint8_t val)
{
	return MAKE_CMD_WORD(DL_CLEAR_STENCIL, val);
}

/**
 * @brief Set clear value for the tag buffer.
 */
INLINE_DL_COMMAND(CLEAR_TAG, uint8_t val)
{
	return MAKE_CMD_WORD(DL_CLEAR_TAG, val);
}

/**
 * @brief Set the current color alpha.
 */
INLINE_DL_COMMAND(COLOR_A, uint8_t alpha)
{
	return MAKE_CMD_WORD(DL_COLOR_A, alpha);
}

/**
 * @brief Enable or disable writing of color components.
 */
INLINE_DL_COMMAND(COLOR_MASK, bool red, bool green, bool blue, bool alpha)
{
	const uint32_t redv = (red & 0x01) << 3;
	const uint32_t greenv = (green & 0x01) << 2;
	const uint32_t bluev = (blue & 0x01) << 1;
	const uint32_t alphav = alpha & 0x01;
	return MAKE_CMD_WORD(DL_COLOR_MASK, redv | greenv | bluev | alphav);
}

/**
 * @brief Set the current color red, green and blue.
 */
INLINE_DL_COMMAND(COLOR_RGB, uint8_t red, uint8_t green, uint8_t blue)
{
	const uint32_t redv = uint32_t(red) << 16;
	const uint32_t greenv = uint32_t(green) << 8;
	const uint32_t bluev = blue;
	return MAKE_CMD_WORD(DL_COLOR_RGB, redv | greenv | bluev);
}

/**
 * @brief Set the width of lines to be drawn with primitive LINES in 1/16 pixel precision.
 */
INLINE_DL_COMMAND(LINE_WIDTH, uint16_t width)
{
	return MAKE_CMD_WORD(DL_LINE_WIDTH, width & 0x0FFF);
}

/**
 * @brief Execute a single command from a macro register.
 */
INLINE_DL_COMMAND(MACRO, uint8_t macro)
{
	return MAKE_CMD_WORD(DL_MACRO, macro & 0x01);
}

/**
 * @brief Set the base address of the palette.
 * @note 2-byte alignment is required if pixel format is PALETTE4444 or PALETTE565.
 */
INLINE_DL_COMMAND(PALETTE_SOURCE, uint32_t addr)
{
	return MAKE_CMD_WORD(DL_PALETTE_SOURCE, addr & 0x3FFFFF);
}

/**
 * @brief Set the radius of points to be drawn with primitive POINTS in 1/16 pixel precision.
 */
INLINE_DL_COMMAND(POINT_SIZE, uint16_t size)
{
	return MAKE_CMD_WORD(DL_POINT_SIZE, size & 0x1FFF);
}

/**
 * @brief Set the size of the scissor clip rectangle.
 * @note valid range for width and height is from zero to 2048
 */
INLINE_DL_COMMAND(SCISSOR_SIZE, uint16_t width, uint16_t height)
{
	const uint32_t widthv = uint32_t(width & 0x0FFF) << 12;
	const uint32_t heightv = height & 0x0FFF;
	return MAKE_CMD_WORD(DL_SCISSOR_SIZE, widthv | heightv);
}

/**
 * @brief Set the top left corner of the scissor clip rectangle.
 * @note valid range for width and height is from zero to 2047
 */
INLINE_DL_COMMAND(SCISSOR_XY, uint16_t xc0, uint16_t yc0)
{
	const uint32_t xc0v = uint32_t(xc0 & 0x7FF) << 11;
	const uint32_t yc0v = yc0 & 0x7FF;
	return MAKE_CMD_WORD(DL_SCISSOR_XY, xc0v | yc0v);
}

/**
 * @brief Set function and reference value for stencil testing.
 */
INLINE_DL_COMMAND(STENCIL_FUNC, TestFunction func, uint8_t ref, uint8_t mask)
{
	const uint32_t funcv = (uint32_t(func) & 0x07) << 16;
	const uint32_t refv = uint32_t(ref) << 8;
	const uint32_t maskv = mask;
	return MAKE_CMD_WORD(DL_STENCIL_FUNC, funcv | refv | maskv);
}

/**
 * @brief Control the writing of individual bits in the stencil planes.
 */
INLINE_DL_COMMAND(STENCIL_MASK, uint8_t mask)
{
	return MAKE_CMD_WORD(DL_STENCIL_MASK, mask);
}

/**
 * @brief Set stencil test actions.
 */
INLINE_DL_COMMAND(STENCIL_OP, StencilOp sfail, StencilOp spass)
{
	const uint32_t sfailv = (unsigned(sfail) & 0x07) << 3;
	const uint32_t spassv = unsigned(spass) & 0x07;
	return MAKE_CMD_WORD(DL_STENCIL_OP, sfailv | spassv);
}

/**
 * @brief Attach the tag value for the following graphics objects drawn on the screen.
 */
INLINE_DL_COMMAND(TAG, uint8_t tagval)
{
	return MAKE_CMD_WORD(DL_TAG, tagval);
}

/**
 * @brief Control the writing of the tag buffer.
 */
INLINE_DL_COMMAND(TAG_MASK, uint8_t mask)
{
	return MAKE_CMD_WORD(DL_TAG_MASK, mask & 0x01);
}

/**
 * @brief Set coordinates for graphics primitives.
 */
INLINE_DL_COMMAND(VERTEX2F, int16_t xc0, int16_t yc0)
{
	const uint32_t xc0v = uint32_t(xc0 & 0x7FFF) << 15;
	const uint32_t yc0v = yc0 & 0x7FFF;
	return MAKE_CMD_WORD(DL_VERTEX2F, xc0v | yc0v);
}

/**
 * @brief Set coordinates, bitmap-handle and cell-number for graphics primitves.
 */
INLINE_DL_COMMAND(VERTEX2II, uint16_t xc0, uint16_t yc0, uint8_t handle, uint8_t cell)
{
	const uint32_t xc0v = uint32_t(xc0 & 0x1FF) << 21;
	const uint32_t yc0v = uint32_t(yc0 & 0x1FF) << 12;
	const uint32_t handlev = uint32_t(handle & 0x1F) << 7;
	const uint32_t cellv = cell & 0x7F;
	return MAKE_CMD_WORD(DL_VERTEX2II, xc0v | yc0v | handlev | cellv);
}

/**
 * @brief Set the precision of VERTEX2F coordinates.
 */
INLINE_DL_COMMAND(VERTEX_FORMAT, uint8_t frac)
{
	return MAKE_CMD_WORD(DL_VERTEX_FORMAT, frac & 0x07);
}

/**
 * @brief Set the vertex transformations X translation component.
 */
INLINE_DL_COMMAND(VERTEX_TRANSLATE_X, int32_t xco)
{
	return MAKE_CMD_WORD(DL_VERTEX_TRANSLATE_X, xco & 0x1FFFF);
}

/**
 * @brief Set the vertex transformations Y translation component.
 */
INLINE_DL_COMMAND(VERTEX_TRANSLATE_Y, int32_t yco)
{
	return MAKE_CMD_WORD(DL_VERTEX_TRANSLATE_Y, yco & 0x1FFFF);
}

#undef INLINE_DL_COMMAND

} // namespace Graphics::EVE
