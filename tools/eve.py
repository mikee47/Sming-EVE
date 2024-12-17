'''Python tools for EVE display controllers.

Defines command structures which can be used in python script to communicate with hardware
e.g. via USB or serial.

Types are defined so they can be used to generate definitions for other languages.

'''

from dataclasses import dataclass
import struct
from enum import IntEnum
import enum

'''Memory'''
EVE_RAM_G = 0x000000
EVE_RAM_CHIPID = 0x0C0000
EVE_ROM_BASE = 0x200000
EVE_ROM_FONT_ADDR = 0x2FFFFC
EVE_RAM_DL = 0x300000
EVE_RAM_REG = 0x302000
EVE_RAM_CMD = 0x308000

'''Memory buffer sizes'''
EVE_RAM_G_SIZE = 1024 * 1024
EVE_CMDFIFO_SIZE = 4 * 1024
EVE_RAM_DL_SIZE = 8 * 1024
EVE_MEMORY_SIZE = 0x00400000


'''Registers'''
class REG(IntEnum):
	ID = 0x00302000
	FRAMES = 0x00302004
	CLOCK = 0x00302008
	FREQUENCY = 0x0030200c
	RENDERMODE = 0x00302010 # only listed in datasheet
	SNAPY = 0x00302014		# only listed in datasheet
	SNAPSHOT = 0x00302018   # only listed in datasheet
	SNAPFORMAT = 0x0030201c # only listed in datasheet
	CPURESET = 0x00302020
	TAP_CRC = 0x00302024    # only listed in datasheet
	TAP_MASK = 0x00302028   # only listed in datasheet
	HCYCLE = 0x0030202c
	HOFFSET = 0x00302030
	HSIZE = 0x00302034
	HSYNC0 = 0x00302038
	HSYNC1 = 0x0030203c
	VCYCLE = 0x00302040
	VOFFSET = 0x00302044
	VSIZE = 0x00302048
	VSYNC0 = 0x0030204c
	VSYNC1 = 0x00302050
	DLSWAP = 0x00302054
	ROTATE = 0x00302058
	OUTBITS = 0x0030205c
	DITHER = 0x00302060
	SWIZZLE = 0x00302064
	CSPREAD = 0x00302068
	PCLK_POL = 0x0030206c
	PCLK = 0x00302070
	TAG_X = 0x00302074
	TAG_Y = 0x00302078
	TAG = 0x0030207c
	VOL_PB = 0x00302080
	VOL_SOUND = 0x00302084
	SOUND = 0x00302088
	PLAY = 0x0030208c
	GPIO_DIR = 0x00302090
	GPIO = 0x00302094
	GPIOX_DIR = 0x00302098
	GPIOX = 0x0030209c
	# RESERVED 0x003020a
	# RESERVED 0x003020a
	INT_FLAGS = 0x003020a8
	INT_EN = 0x003020ac
	INT_MASK = 0x003020b0
	PLAYBACK_START = 0x003020b4
	PLAYBACK_LENGTH = 0x003020b8
	PLAYBACK_READPTR = 0x003020bc
	PLAYBACK_FREQ = 0x003020c0
	PLAYBACK_FORMAT = 0x003020c4
	PLAYBACK_LOOP = 0x003020c8
	PLAYBACK_PLAY = 0x003020cc
	PWM_HZ = 0x003020d0
	PWM_DUTY = 0x003020d4
	MACRO_0 = 0x003020d8
	MACRO_1 = 0x003020dc
	# RESERVED 0x003020e
	# RESERVED 0x003020e
	# RESERVED 0x003020e
	# RESERVED 0x003020e
	# RESERVED 0x003020f
	# RESERVED 0x003020f
	CMD_READ = 0x003020f8
	CMD_WRITE = 0x003020fc
	CMD_DL = 0x00302100
	TOUCH_MODE = 0x00302104
	TOUCH_ADC_MODE = 0x00302108
	CTOUCH_EXTENDED = 0x00302108 # TOUCH_ADC_MODE
	TOUCH_CHARGE = 0x0030210c
	TOUCH_SETTLE = 0x00302110
	TOUCH_OVERSAMPLE = 0x00302114
	TOUCH_RZTHRESH = 0x00302118
	TOUCH_RAW_XY = 0x0030211c
	CTOUCH_TOUCH1_XY = 0x0030211c # TOUCH_RAW_XY
	TOUCH_RZ = 0x00302120
	CTOUCH_TOUCH4_Y = 0x00302120 # TOUCH_RZ
	TOUCH_SCREEN_XY = 0x00302124
	CTOUCH_TOUCH_XY = 0x00302124 # TOUCH_SCREEN_XY # only listed in datasheet
	TOUCH_TAG_XY = 0x00302128
	TOUCH_TAG = 0x0030212c
	TOUCH_TAG1_XY = 0x00302130      # only listed in datasheet
	TOUCH_TAG1 = 0x00302134         # only listed in datasheet
	TOUCH_TAG2_XY = 0x00302138      # only listed in datasheet
	TOUCH_TAG2 = 0x0030213c         # only listed in datasheet
	TOUCH_TAG3_XY = 0x00302140      # only listed in datasheet
	TOUCH_TAG3 = 0x00302144         # only listed in datasheet
	TOUCH_TAG4_XY = 0x00302148      # only listed in datasheet
	TOUCH_TAG4 = 0x0030214c         # only listed in datasheet
	TOUCH_TRANSFORM_A = 0x00302150
	TOUCH_TRANSFORM_B = 0x00302154
	TOUCH_TRANSFORM_C = 0x00302158
	TOUCH_TRANSFORM_D = 0x0030215c
	TOUCH_TRANSFORM_E = 0x00302160
	TOUCH_TRANSFORM_F = 0x00302164
	TOUCH_CONFIG = 0x00302168
	CTOUCH_TOUCH4_X = 0x0030216c
	# RESERVED 0x0030217
	BIST_EN = 0x00302174            # only listed in datasheet
	# RESERVED 0x0030217
	# RESERVED 0x0030217
	TRIM = 0x00302180
	ANA_COMP = 0x00302184           # only listed in datasheet
	SPI_WIDTH = 0x00302188          # listed with false offset in programmers guide V1.
	TOUCH_DIRECT_XY = 0x0030218c
	CTOUCH_TOUCH2_XY = 0x0030218c # TOUCH_DIRECT_XY
	TOUCH_DIRECT_Z1Z2 = 0x00302190
	CTOUCH_TOUCH3_XY = 0x00302190 # TOUCH_DIRECT_Z1Z2
	# RESERVED 0x00302194 - 0x0030256
	DATESTAMP0 = 0x00302564         # 128 bits (4 words) only listed in datasheet
	DATESTAMP1 = 0x00302568
	DATESTAMP2 = 0x0030256C
	DATESTAMP3 = 0x00302570
	CMDB_SPACE = 0x00302574
	CMDB_WRITE = 0x00302578
	# co-processor registers - see programmers' guide
	TRACKER = 0x00309000
	TRACKER_1 = 0x00309004
	TRACKER_2 = 0x00309008
	TRACKER_3 = 0x0030900c
	TRACKER_4 = 0x00309010
	MEDIAFIFO_READ = 0x00309014
	MEDIAFIFO_WRITE = 0x00309018


'''Graphics primitives (BEGIN)'''
class GraphicsPrimitive(IntEnum):
	BITMAPS = 1
	POINTS = 2
	LINES = 3
	LINE_STRIP = 4
	EDGE_STRIP_R = 5
	EDGE_STRIP_L = 6
	EDGE_STRIP_A = 7
	EDGE_STRIP_B = 8
	RECTS = 9
	INT_G8 = 18
	INT_L8C = 12
	INT_VGA = 13
	PALETTED565 = 14
	PALETTED4444 = 15
	PALETTED8 = 16
	L2 = 17

GP = GraphicsPrimitive

'''Bitmap formats'''
class BitmapFormat(IntEnum):
    ARGB1555 = 0
    L1 = 1
    L4 = 2
    L8 = 3
    RGB332 = 4
    ARGB2 = 5
    ARGB4 = 6
    RGB565 = 7
    PALETTED = 8
    TEXT8X8 = 9
    TEXTVGA = 10
    BARGRAPH = 11


'''BITMAP_SIZE filter types'''
class BitmapFilter(IntEnum):
	NEAREST = 0
	BILINEAR = 1


'''DL_BITMAP_SIZE wrap types'''
class BitmapWrap(IntEnum):
	BORDER = 0
	REPEAT = 1


@dataclass
class FontMetrics:
    char_widths: list[int]
    format: int
    stride: int
    width: int
    height: int
    bitmap: int


'''BLEND_FUNC types'''
class BlendFunction(IntEnum):
    ZERO = 0
    ONE = 1
    SRC_ALPHA = 2
    DST_ALPHA = 3
    ONE_MINUS_SRC_ALPHA = 4
    ONE_MINUS_DST_ALPHA = 5


@dataclass
class Type:
    name: str
    bitcount: int
    signed: bool = False

    @property
    def is_int(self):
        return 'int' in self.name


@dataclass
class Param:
    name: str
    typedef: Type
    desc: str = None
    stored_bits: int = None # Takes precedence over type definition
    offset: int = None # Calculated when this unit is initialised

    @property
    def bitcount(self):
        return self.stored_bits or self.typedef.bitcount

    @property
    def size(self):
        return align(self.bitcount, 16) // 8

    @property
    def basetype(self):
        sign = '' if self.typedef.signed else 'u'
        return f'{sign}int32_t' if self.size == 4 else f'{sign}int16_t'

    def pack(self, arg) -> bytes:
        if self.typedef is CString:
            arg = arg.encode() + b'\0'
            return arg.ljust(align(len(arg)), b'\0')
        if self.typedef in [Fixed15_8, Fixed16_16]:
            arg = int(arg * 65536)
        elif self.typedef is Fixed8_8:
            arg = int(arg * 256)
        if self.size == 2:
            return struct.pack('<H', arg)
        if self.typedef.signed:
            return struct.pack('<l', arg)
        return struct.pack('<L', arg)

    def unpack(self, data: bytes) -> int:
        if self.size == 2:
            return struct.unpack('<H', data[:2])[0]
        if self.typedef.signed:
            value = struct.unpack('<l', data[:4])[0]
            if self.typedef in [Fixed15_8, Fixed16_16]:
                return value / 65536
            if self.typedef is Fixed8_8:
                return value / 256
            return value
        return struct.unpack('<L', data[:4])[0]



@dataclass
class CommandDef:
    code: int   # MSB of command word
    name: str
    description: str = None
    params: list[Param] = None

    def get_cmdword(self, param: int):
        assert param >= 0 and param <= 0x00ffffff
        return (self.code << 24) | param

    @property
    def total_bits(self):
        return sum(param.bitcount for param in self.params or [])



@dataclass
class Arg:
    param: Param
    value: any


class DlCmd(CommandDef):
    '''Display List Command Definition'''

    def unpack(self, data: bytes) -> tuple[list[Arg], int]:
        cmdword, = struct.unpack('<L', data[:4])
        args = []
        off = 0
        # print('>', self.name, hex(cmdword))
        for param in self.params or []:
            bitcount = param.bitcount
            mask = (1 << bitcount) - 1
            value = (cmdword >> off) & mask
            args.append(Arg(param, value))
            off += bitcount
        return args, 4

    def pack(self, *args):
        assert len(self.params or []) == len(args)
        value = 0
        for param, arg in zip(reversed(self.params or []), args):
            value <<= param.bitcount
            mask = (1 << param.bitcount) - 1
            if arg > mask:
                raise ValueError(f'{param.cmd} out of range: {arg}')
            value |= int(arg) & mask
        value |= self.code << 24
        return struct.pack('<L', value)


Address = Type('Address', 22)
ColorChannel = Type('ColorChannel', 8)
Tag = Type('Tag', 8)
Handle = Type('Handle', 5)
Cell = Type('Cell', 7)
Boolean = Type('bool', 1)
TestFunction = Type('TestFunction', 4)
StencilOp = Type('StencilOp', 3)
Fixed8_8 = Type('Fixed8', 17, signed=True)
Fixed15_8 = Type('Fixed8', 24, signed=True)
DisplayListOffset = Type('DisplayListOffset', 16)

# Used with co-processor commands
RGB = Type('RGB', 24)
ARGB = Type('ARGB', 32)
UInt8 = Type('uint8_t', 8)
Int16 = Type('int16_t', 16, signed=True)
UInt16 = Type('uint16_t', 16)
Int32 = Type('int32_t', 32, signed=True)
UInt32 = Type('uint32_t', 32)
CString = Type('CString', 0) # Variable length, NUL terminated
Fixed16_16 = Type('Fixed16', 32, signed=True)
Angle = Type('Angle', 32) # Units of 1/65536 of a circle
Options = Type('Options', 16)

class DataBlock(Type):
    '''Variable length'''
    def __init__(self, length_param: str):
        super().__init__('DataBlock', 0)
        self.length_param = length_param


@dataclass
class EnumDef:
    name: str
    value: int
    desc: str
    commands: list[str]

OptionDefs: list[EnumDef] = [
    EnumDef('3D', 0, '3D effect', ['BUTTON', 'CLOCK', 'KEYS', 'GAUGE', 'SLIDER', 'DIAL', 'TOGGLE', 'PROGRESS', 'SCROLLBAR']),
    EnumDef('RGB565', 0, 'Decode the source image to RGB565 format', ['LOADIMAGE']),
    EnumDef('MONO', 1, 'Decode the source JPEG image to L8 format, i.e., monochrome', ['LOADIMAGE']),
    EnumDef('NODL', 2, 'No display list commands generated', ['LOADIMAGE']),
    EnumDef('FLAT', 256, 'No 3D effect', ['BUTTON', 'CLOCK', 'KEYS', 'GAUGE', 'SLIDER', 'DIAL', 'TOGGLE', 'PROGRESS', 'SCROLLBAR']),
    EnumDef('SIGNED', 256, 'The number is treated as a 32-bit signed integer', ['NUMBER']),
    EnumDef('CENTERX', 512, 'Horizontally-centered style', ['KEYS', 'TEXT', 'NUMBER']),
    EnumDef('CENTERY', 1024, 'Vertically centered style', ['KEYS', 'TEXT', 'NUMBER']),
    EnumDef('CENTER', 1536, 'horizontally and vertically centered style', ['KEYS', 'TEXT', 'NUMBER']),
    EnumDef('RIGHTX', 2048, 'Right justified style', ['KEYS', 'TEXT', 'NUMBER']),
    EnumDef('NOBACK', 4096, 'No background drawn', ['CLOCK', 'GAUGE']),
    EnumDef('NOTICKS', 8192, 'No Ticks', ['CLOCK', 'GAUGE']),
    EnumDef('NOHM', 16384, 'No hour and minute hands', ['CLOCK']),
    EnumDef('NOPOINTER', 16384, 'No pointer', ['GAUGE']),
    EnumDef('NOSECS', 32768, 'No second hands', ['CLOCK']),
    EnumDef('NOHANDS', 49152, 'No hands', ['CLOCK']),
    EnumDef('NOTEAR', 4, 'Synchronize video updates to the display blanking interval, avoiding horizontal “tearing” artefacts.', ['PLAYVIDEO']),
    EnumDef('FULLSCREEN', 8, 'Zoom the video so that it fills as much of the screen as possible.', ['PLAYVIDEO']),
    EnumDef('MEDIAFIFO', 16, 'source video/image/compressed(zlib) data from the defined media FIFO', ['PLAYVIDEO', 'VIDEOFRAME', 'LOADIMAGE']),
    EnumDef('SOUND', 32, 'Decode the audio data of MJPEG stream', ['PLAYVIDEO']),
]


DisplayListCommands: list[DlCmd] = [
	DlCmd(0x00, 'DISPLAY', 'End the display list. All the commands following this command will be ignored.'),
	DlCmd(0x01, 'BITMAP_SOURCE', 'Specify the source address of bitmap data in RAM_G.', [
        Param('addr', Address, '''Bitmap address in RAM_G, aligned with respect to the bitmap format.
For example, if the bitmap format is RGB565/ARGB4/ARGB1555, the bitmap source shall be aligned to 2 bytes.''')
    ]),
	DlCmd(0x02, 'CLEAR_COLOR_RGB', 'Specify clear values for red, green and blue channels', [
        Param('blue', ColorChannel),
        Param('green', ColorChannel),
        Param('red', ColorChannel),
    ]),
	DlCmd(0x03, 'TAG', '''Attach the tag value for the following graphics objects drawn on the screen.
The initial tag buffer value is 255.''', [
        Param('tag', Tag, '''Tag value. Valid value range is from 1 to 255.''')
    ]),
	DlCmd(0x04, 'COLOR_RGB', 'Set the current color red, green and blue.', [
        Param('blue', ColorChannel),
        Param('green', ColorChannel),
        Param('red', ColorChannel),
    ]),
	DlCmd(0x05, 'BITMAP_HANDLE', 'Specify the bitmap handle', [
        Param('handle', Handle)
    ]),
	DlCmd(0x06, 'CELL', 'Specify the bitmap cell number for the VERTEX2F command.', [
        Param('cell', Cell, 'Bitmap cell number. The initial value is 0.')
    ]),
	DlCmd(0x07, 'BITMAP_LAYOUT', 'Specify the source bitmap memory format and layout for the current handle.', [
        Param('height', UInt32, stored_bits=9),
        Param('linestride', UInt32, stored_bits=10),
        Param('format', Type('BitmapFormat', 5))
    ]),
	DlCmd(0x08, 'BITMAP_SIZE', 'Specify the screen drawing of bitmaps for the current handle', [
        Param('height', UInt32, stored_bits=9),
        Param('width', UInt32, stored_bits=9),
        Param('wrapy', Type('BitmapWrap', 1)),
        Param('wrapx', Type('BitmapWrap', 1)),
        Param('filter', Type('BitmapFilter', 1))
    ]),
	DlCmd(0x09, 'ALPHA_FUNC', 'Specify the alpha test function', [
        Param('ref', UInt8),
        Param('func', TestFunction)
    ]),
	DlCmd(0x0A, 'STENCIL_FUNC', 'Set function and reference value for stencil testing.', [
        Param('mask', UInt8),
        Param('ref', UInt8),
        Param('func', TestFunction)
    ]),
	DlCmd(0x0B, 'BLEND_FUNC', 'Specify pixel arithmetic', [
        Param('dst', Type('BlendFunction', 3)),
        Param('src', Type('BlendFunction', 3))
    ]),
	DlCmd(0x0C, 'STENCIL_OP', 'Set stencil test actions', [
        Param('spass', StencilOp),
        Param('sfail', StencilOp)
    ]),
	DlCmd(0x0D, 'POINT_SIZE', 'Specify the radius of points', [
        Param('size', UInt32, stored_bits=13)
    ]),
	DlCmd(0x0E, 'LINE_WIDTH', 'Specify the width of lines to be drawn with primitive LINES in 1/16 pixel precision.', [
        Param('width', UInt32, stored_bits=12)
    ]),
	DlCmd(0x0F, 'CLEAR_COLOR_A', 'Specify clear value for the alpha channel', [
        Param('alpha', ColorChannel)
    ]),
	DlCmd(0x10, 'COLOR_A', 'Set the current color alpha', [
        Param('alpha', ColorChannel)
    ]),
	DlCmd(0x11, 'CLEAR_STENCIL', 'Specify clear value for the stencil buffer', [
        Param('s', ColorChannel)
    ]),
	DlCmd(0x12, 'CLEAR_TAG', 'Specify clear value for the tag buffer', [
        Param('tag', Tag)
    ]),
	DlCmd(0x13, 'STENCIL_MASK', 'Control the writing of individual bits in the stencil planes', [
        Param('mask', UInt8)
    ]),
	DlCmd(0x14, 'TAG_MASK', 'Control the writing of the tag buffer', [
        Param('mask', Boolean, '''Allow updates to the tag buffer.
The initial value is one and it means the tag buffer is updated with the value given by the TAG command.
Therefore, the following graphics objects will be attached to the tag value given by the TAG command.
The value zero means the tag buffer is set as the default value, rather than the value given by TAG command in the display list.''')
    ]),
	DlCmd(0x15, 'BITMAP_TRANSFORM_A', 'Specify the 𝐴 coefficient of the bitmap transform matrix', [
        Param('a', Fixed8_8)
    ]),
	DlCmd(0x16, 'BITMAP_TRANSFORM_B',  'Specify the B coefficient of the bitmap transform matrix', [
        Param('b', Fixed8_8)
    ]),
	DlCmd(0x17, 'BITMAP_TRANSFORM_C',  'Specify the C coefficient of the bitmap transform matrix', [
        Param('c', Fixed15_8)
    ]),
	DlCmd(0x18, 'BITMAP_TRANSFORM_D',  'Specify the D coefficient of the bitmap transform matrix', [
        Param('d', Fixed8_8)
    ]),
	DlCmd(0x19, 'BITMAP_TRANSFORM_E',  'Specify the E coefficient of the bitmap transform matrix', [
        Param('e', Fixed8_8)
    ]),
	DlCmd(0x1A, 'BITMAP_TRANSFORM_F',  'Specify the F coefficient of the bitmap transform matrix', [
        Param('f', Fixed15_8)
    ]),
	DlCmd(0x1B, 'SCISSOR_XY', 'Specify the top left corner of the scissor clip rectangle', [
        Param('y', UInt32, stored_bits=11),
        Param('x', UInt32, stored_bits=11),
    ]),
	DlCmd(0x1C, 'SCISSOR_SIZE', 'Specify the size of the scissor clip rectangle', [
        Param('height', UInt32, stored_bits=12),
        Param('width', UInt32, stored_bits=12),
    ]),
	DlCmd(0x1D, 'CALL', 'Execute a sequence of commands at another location in the display list', [
        Param('dest', DisplayListOffset)
    ]),
	DlCmd(0x1E, 'JUMP', 'Execute commands at another location in the display list', [
        Param('dest', DisplayListOffset)
    ]),
	DlCmd(0x1F, 'BEGIN', 'Begin drawing a graphics primitive', [
        Param('prim', Type('GraphicsPrimitive', 4))
    ]),
	DlCmd(0x20, 'COLOR_MASK', 'Enable or disable writing of color components', [
        Param('a', Boolean),
        Param('b', Boolean),
        Param('g', Boolean),
        Param('r', Boolean),
    ]),
	DlCmd(0x21, 'END', '''End drawing a graphics primitive.
It is recommended to have an END for each BEGIN. However, advanced users may avoid the usage of END in order to save space for extra graphics instructions in RAM_DL.'''),
	DlCmd(0x22, 'SAVE_CONTEXT', 'Push the current graphics context on the context stack'),
	DlCmd(0x23, 'RESTORE_CONTEXT', 'Restore the current graphics context from the context stack.'),
	DlCmd(0x24, 'RETURN', 'Return from a previous CALL command'),
	DlCmd(0x25, 'MACRO', 'Execute a single command from a macro register', [
        Param('m', UInt32, stored_bits=1)
    ]),
	DlCmd(0x26, 'CLEAR', 'Clear buffers to preset values', [
        Param('t', Boolean, 'Clear tag buffer'),
        Param('s', Boolean, 'Clear stencil buffer'),
        Param('c', Boolean, 'Clear color buffer'),
    ]),
	DlCmd(0x27, 'VERTEX_FORMAT', 'Set the precision of VERTEX2F coordinates', [
        Param('frac', UInt32, stored_bits=3)
    ]),
	DlCmd(0x28, 'BITMAP_LAYOUT_H', 'Specify the 2 most significant bits of the source bitmap memory format and layout for the current handle.', [
        Param('height', UInt32, stored_bits=2),
        Param('linestride', UInt32, stored_bits=2),
    ]),
	DlCmd(0x29, 'BITMAP_SIZE_H', 'Specify the 2 most significant bits of bitmaps dimension for the current handle.', [
        Param('height', UInt32, stored_bits=2),
        Param('width', UInt32, stored_bits=2),
    ]),
	DlCmd(0x2A, 'PALETTE_SOURCE', 'Specify the base address of the palette', [
        Param('addr', Address)
    ]),
	DlCmd(0x2B, 'VERTEX_TRANSLATE_X', 'Specify the vertex transformations X translation component', [
        Param('x', UInt32, stored_bits=17)
    ]),
	DlCmd(0x2C, 'VERTEX_TRANSLATE_Y', 'Specify the vertex transformation’s Y translation component', [
        Param('y', UInt32, stored_bits=17)
    ]),
	DlCmd(0x2D, 'NOP', 'No operation'),
	# Top 2 bits reserved for these commands
    DlCmd(0x40, 'VERTEX2F', 'Start the operation of graphics primitives at the specified screen coordinate, in the pixel precision defined by VERTEX_FORMAT.', [
        Param('y', UInt32, stored_bits=15),
        Param('x', UInt32, stored_bits=15),
    ]),
	DlCmd(0x80, 'VERTEX2II', 'Start the operation of graphics primitive at the specified coordinates in pixel precision', [
        Param('cell', Cell),
        Param('handle', Handle),
        Param('y', UInt32, stored_bits=9),
        Param('x', UInt32, stored_bits=9),
    ]),
]


class CpCmd(CommandDef):
    '''Co-processor command definition'''

    def unpack(self, data: bytes) -> tuple[list[Arg], int]:
        args = []
        size = 4
        for param in self.params or []:
            off = param.offset
            param_size = param.size
            if isinstance(param.typedef, DataBlock):
                length_param = param.typedef.length_param
                if length_param:
                    length_arg = next(x for x in args if x.param.name == length_param)
                    print('Datablock', length_param, length_arg.value)
                    param_size = length_arg.value
                    value = data[off:off+param_size]
            elif param.typedef is CString:
                param_size = data[off:].index(b'\0')
                print('CString', param_size)
                value = data[off:off+param_size].decode()
                param_size += 1 # NUL
            else:
                value = param.unpack(data[off:])
            args.append(Arg(param, value))
            size = align(off + param_size)
        return args, size

    def pack(self, *args):
        result = struct.pack('<L', 0xffffff00 | self.code)
        for param, arg in zip(self.params or [], args):
            result = result.ljust(param.offset, b'\0')
            if isinstance(param.typedef, DataBlock):
                length_param = param.typedef.length_param
                length_arg = next(x for x in args if x.param.name == length_param)
                assert length_arg.value == len(arg)
                result += param.pack(arg)
            else:
                result += param.pack(arg)
        return result.ljust(align(len(result)), b'\0')

    @property
    def size(self):
        if not self.params:
            return 4
        last_param = self.params[-1]
        return align(last_param.offset + last_param.size)


class Widget(CpCmd):
    '''Widget definition'''


'''Co-processor commands
These form the last byte (least significant) of any command word, as 0xffffffxx
Each command parameter can be a fixed 16 or 32 bit value, naturally aligned.
'''
CoprocessorCommands: list[CpCmd] = [
	CpCmd(0x00, 'DLSTART', '''This command starts a new display list.
When the coprocessor engine executes this command, it waits until the current display list is ready for writing, and then sets REG_CMD_DL to zero.'''),
	CpCmd(0x01, 'SWAP', '''This command is used to swap the current display list.
When the coprocessor engine executes this command, it requests a display list swap immediately after the current display list is scanned out.
Internally, the coprocessor engine implements this command by writing to REG_DLSWAP with 0x02.
This coprocessor engine command will not generate any display list command into display list memory RAM_DL. It is expected to be used with CMD_DLSTART in pair.'''),
	CpCmd(0x02, 'INTERRUPT', '''This command is used to trigger Interrupt CMDFLAG.
When the coprocessor engine executes this command, it triggers interrupt, which will set the bit field CMDFLAG of REG_INT_FLAGS, unless the corresponding bit in REG_INT_MASK is zero.''', [
        Param('ms', UInt32)
    ]),
	CpCmd(0x09, 'BGCOLOR', 'Set the background color', [
        Param('color', RGB)
    ]),
	CpCmd(0x0A, 'FGCOLOR', 'Set the foreground color', [
        Param('color', RGB)
    ]),
	Widget(0x0B, 'GRADIENT', '', [
        Param('x0', Int16),
        Param('y0', Int16),
        Param('rgb0', RGB),
        Param('x1', Int16),
        Param('y1', Int16),
        Param('rgb1', RGB),
    ]),
	Widget(0x0C, 'TEXT', '', [
        Param('x', Int16),
        Param('y', Int16),
        Param('font', Handle, stored_bits=16),
        Param('options', Options),
        Param('s', CString)
    ]),
	Widget(0x0D, 'BUTTON', '', [
        Param('x', Int16),
        Param('y', Int16),
        Param('w', UInt16),
        Param('h', UInt16),
        Param('font', Handle),
        Param('options', Options),
        Param('s', CString)
    ]),
	Widget(0x0E, 'KEYS', '', [
        Param('x', Int16),
        Param('y', Int16),
        Param('w', UInt16),
        Param('h', UInt16),
        Param('font', Handle),
        Param('options', Options),
        Param('s', CString)
    ]),
	Widget(0x0F, 'PROGRESS', '', [
        Param('x', Int16),
        Param('y', Int16),
        Param('w', UInt16),
        Param('h', UInt16),
        Param('options', Options),
        Param('value', UInt16),
        Param('range', UInt16),
    ]),
	Widget(0x10, 'SLIDER', '', [
        Param('x', Int16),
        Param('y', Int16),
        Param('w', UInt16),
        Param('h', UInt16),
        Param('options', Options),
        Param('value', UInt16),
        Param('range', UInt16),
    ]),
	Widget(0x11, 'SCROLLBAR', '', [
        Param('x', Int16),
        Param('y', Int16),
        Param('w', UInt16),
        Param('h', UInt16),
        Param('options', Options),
        Param('value', UInt16),
        Param('size', UInt16),
        Param('range', UInt16),
    ]),
	Widget(0x12, 'TOGGLE', '', [
        Param('x', Int16),
        Param('y', Int16),
        Param('w', UInt16),
        Param('font', Handle),
        Param('options', Options),
        Param('state', UInt16),
        Param('s', CString),
    ]),
	Widget(0x13, 'GAUGE', '', [
        Param('x', Int16),
        Param('y', Int16),
        Param('r', UInt16),
        Param('options', Options),
        Param('major', UInt16),
        Param('minor', UInt16),
        Param('value', UInt16),
        Param('range', UInt16),
    ]),
	Widget(0x14, 'CLOCK', '', [
        Param('x', Int16),
        Param('y', Int16),
        Param('r', UInt16),
        Param('options', Options),
        Param('h', UInt16),
        Param('m', UInt16),
        Param('s', UInt16),
        Param('ms', UInt16),
    ]),
	CpCmd(0x15, 'CALIBRATE', '', [
        Param('result', UInt32) # OUT
    ]),
	CpCmd(0x16, 'SPINNER', '', [
        Param('x', Int16),
        Param('y', Int16),
        Param('style', UInt16),
        Param('scale', UInt16),
    ]),
	CpCmd(0x17, 'STOP', 'Stop periodic operation (SKETCH, SPINNER, SCREENSAVER)'),
	CpCmd(0x18, 'MEMCRC', 'Compute CRC32 for block of memory', [
        Param('ptr', Address),
        Param('num', UInt32),
        Param('result', UInt32), # OUT
    ]),
	CpCmd(0x19, 'REGREAD', 'Read a register value', [
        Param('ptr', Address),
        Param('result', UInt32), # OUT
    ]),
	CpCmd(0x1A, 'MEMWRITE', 'Write data into memory or registers', [
        Param('ptr', Address),
        Param('num', UInt32),
        Param('data', DataBlock('num')),
    ]),
	CpCmd(0x1B, 'MEMSET', '', [
        Param('ptr', Address),
        Param('value', UInt8),
        Param('num', UInt32),
    ]),
	CpCmd(0x1C, 'MEMZERO', '', [
        Param('ptr', Address),
        Param('num', UInt32),
    ]),
	CpCmd(0x1D, 'MEMCPY', '', [
        Param('dest', Address),
        Param('src', Address),
        Param('num', UInt32),
    ]),
	CpCmd(0x1E, 'APPEND', 'Append commands from RAM_G', [
        Param('ptr', Address),
        Param('num', UInt32),
    ]),
	CpCmd(0x1F, 'SNAPSHOT', 'Take screen snapshot as ARGB4 bitmap', [
        Param('ptr', Address),
    ]),
	CpCmd(0x22, 'INFLATE', 'Decompress ZLIB data into RAM_G', [
        Param('ptr', Address),
        Param('data', DataBlock(None))
    ]),
	CpCmd(0x23, 'GETPTR', 'Get end memory address from last INFLATE command', [
        Param('result', Int32) # OUT
    ]),
	CpCmd(0x24, 'LOADIMAGE', 'Load a JPEG or PNG image', [
        Param('ptr', Address),
        Param('options', Options),
    ]),
	CpCmd(0x25, 'GETPROPS', 'Get source address and bitmap size from previous LOADIMAGE', [
        Param('ptr', Address),
        Param('width', UInt32),   # OUT
        Param('height', UInt32),  # OUT
    ]),
	CpCmd(0x26, 'LOADIDENTITY', 'Reset transform to identity matrix'),
	CpCmd(0x27, 'TRANSLATE', 'Apply transformation to current matrix', [
        Param('tx', Fixed16_16),
        Param('ty', Fixed16_16)
    ]),
	CpCmd(0x28, 'SCALE', 'Apply scale to current matrix', [
        Param('sx', Fixed16_16),
        Param('sy', Fixed16_16)
    ]),
	CpCmd(0x29, 'ROTATE', 'Apply a rotation to the current matrix', [
        Param('a', Angle)
    ]),
	CpCmd(0x2A, 'SETMATRIX', 'Set current matrix as bitmap transform'),
	CpCmd(0x2B, 'SETFONT', 'Register custom-defined bitmap font', [
        Param('font', Handle),
        Param('ptr', Address, 'Must be word-aligned')
    ]),
	CpCmd(0x2C, 'TRACK', 'Setup tracking for graphical object', [
        Param('x', Int16),
        Param('y', Int16),
        Param('w', UInt16),
        Param('h', UInt16),
        Param('tag', UInt8),
    ]),
	Widget(0x2D, 'DIAL', '', [
        Param('x', Int16),
        Param('y', Int16),
        Param('r', UInt16),
        Param('options', Options),
        Param('value', UInt16),
    ]),
	Widget(0x2E, 'NUMBER', '', [
        Param('x', Int16),
        Param('y', Int16),
        Param('font', Handle),
        Param('options', Options),
        Param('n', Int32),
    ]),
	CpCmd(0x2F, 'SCREENSAVER', ''),
	CpCmd(0x30, 'SKETCH', ''),
	CpCmd(0x31, 'LOGO', ''),
	CpCmd(0x32, 'COLDSTART', 'Reset coprocessor to default state'),
	CpCmd(0x33, 'GETMATRIX', 'Get current matrix', [
        Param('a', Fixed8_8),
        Param('b', Fixed8_8),
        Param('c', Fixed8_8),
        Param('d', Fixed8_8),
        Param('e', Fixed8_8),
        Param('f', Fixed8_8),
    ]),
	CpCmd(0x34, 'GRADCOLOR', 'Set 3D button highlight color', [
        Param('color', RGB)
    ]),
	CpCmd(0x36, 'SETROTATE', 'Rotate the screen', [
        Param('r', UInt8)
    ]),
	CpCmd(0x37, 'SNAPSHOT2', 'Take partial screen snapshot', [
        Param('fmt', UInt8),
        Param('ptr', Address),
        Param('x', Int16),
        Param('y', Int16),
        Param('w', UInt16),
        Param('h', UInt16),
    ]),
	CpCmd(0x38, 'SETBASE', 'Set base for NUMBER output', [
        Param('b', UInt8)
    ]),
	CpCmd(0x39, 'MEDIAFIFO', 'Setup a streaming media FIFO', [
        Param('ptr', Address),
        Param('size', UInt32)
    ]),
	CpCmd(0x3A, 'PLAYVIDEO', 'Play MJPEG-encoded AVI video', [
        Param('options', Options)
    ]),
	CpCmd(0x3B, 'SETFONT2', 'Setup a custom font', [
        Param('font', Handle),
        Param('ptr', Address),
        Param('firstchar', UInt8),
    ]),
	CpCmd(0x3C, 'SETSCRATCH', 'Designate scratch bitmap for widgets to use', [
        Param('handle', Handle, 'Default is 15')
    ]),
	CpCmd(0x3F, 'ROMFONT', 'Load a ROM font into a bitmap handle', [
        Param('font', Handle, stored_bits=32),
        Param('romslot', UInt8)
    ]),
	CpCmd(0x40, 'VIDEOSTART', 'Initialise video frame decoder'),
	CpCmd(0x41, 'VIDEOFRAME', 'Load next frame of video', [
        Param('dst', Address),
        Param('ptr', Address)
    ]),
	CpCmd(0x43, 'SETBITMAP', 'Generate bitmap information DL commands', [
        Param('source', Address),
        Param('fmt', UInt8),
        Param('width', UInt16),
        Param('height', UInt16),
    ]),
	Widget(0x57, 'GRADIENTA', 'BT88x only', [
        Param('x0', Int16),
        Param('y0', Int16),
        Param('argb0', ARGB),
        Param('x1', Int16),
        Param('y1', Int16),
        Param('argb1', ARGB),
    ]),
]


def get_command(word: int):
    if word >> 8 == 0x00ffffff:
        code = word & 0xff
        cmd_defs = CoprocessorCommands
    else:
        code = word >> 24
        if code & 0xC0:
            code &= 0xC0
        cmd_defs = DisplayListCommands
    return next(cmd for cmd in cmd_defs if cmd.code == code)


def align(offset: int, size: int = 4) -> int:
    e = offset % size
    return offset + size - e if e else offset


# General command enumerations
DL = enum.IntEnum('DL', dict(
    (cmd.name, cmd.code) for cmd in DisplayListCommands
))
CMD = enum.IntEnum('CMD', dict(
    (cmd.name, cmd.code) for cmd in CoprocessorCommands
))
OPT = enum.IntEnum('OPT', dict(
    (opt.name, opt.value) for opt in OptionDefs
))


def initialise():
    # Calculate parameter offsets
    for cmd in CoprocessorCommands:
        assert isinstance(cmd.description, str), cmd.name
        offset = 4
        for param in cmd.params or []:
            if param.size:
                offset = align(offset, param.size)
            param.offset = offset
            offset += param.size


initialise()
