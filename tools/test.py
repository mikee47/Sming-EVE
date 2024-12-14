import eve
from eve import align
from fonts import fonts
import parse
import struct
import binascii
import math
from websockets.sync.client import connect
from datetime import datetime
from dataclasses import dataclass
from time import sleep
from PIL import Image
from io import BytesIO
import os

def make_cmd(cmd: eve.CMD | eve.DL, *args):
    if isinstance(cmd, eve.CMD):
        cmdlist = eve.CoprocessorCommands
    else:
        cmdlist = eve.DisplayListCommands
    cmd = next(x for x in cmdlist if x.code == cmd)
    return cmd.pack(*args)


NOW = datetime.now()

TEST_DISPLAYLIST = [
    make_cmd(eve.CMD.DLSTART),
    # make_cmd(eve.DL.CLEAR_COLOR_RGB, 10, 10, 10),
    make_cmd(eve.DL.CLEAR, 1, 1, 1),
    # make_cmd(eve.DL.COLOR_RGB, 100, 100, 100),
    # make_cmd(eve.CMD.FGCOLOR, 0x003870),
    # make_cmd(eve.CMD.BGCOLOR, 0x002040),
    # make_cmd(eve.CMD.GRADCOLOR, 0xFFFFFF),

    make_cmd(eve.DL.TAG, 1),
    make_cmd(eve.CMD.CLOCK, 100, 100, 80, 0, NOW.hour, NOW.minute, NOW.second, 0),
    #
    make_cmd(eve.DL.TAG, 2),
    make_cmd(eve.CMD.CLOCK, 350, 100, 80, 0, NOW.hour, NOW.minute, NOW.second, 0),
    #
    make_cmd(eve.DL.TAG, 3),
    make_cmd(eve.CMD.DIAL, 550, 100, 50, 0, 0),
    #
    make_cmd(eve.DL.TAG, 4),
    make_cmd(eve.CMD.SLIDER, 100, 220, 400, 30, 0, 50, 100),
    make_cmd(eve.CMD.TRACK, 100, 220, 400, 30, 4),
    #
    make_cmd(eve.DL.TAG, 5),
    make_cmd(eve.CMD.SLIDER, 100, 320, 400, 30, 0, 50, 100),
    make_cmd(eve.CMD.TRACK, 100, 320, 400, 30, 5),
    #
    make_cmd(eve.DL.TAG, 6),
    make_cmd(eve.CMD.TEXT, 50, 400, 30, 0, 'testing, testing, one two three'),

    # Vertical gradients
    make_cmd(eve.DL.SCISSOR_SIZE, 50, 470),
    # make_cmd(eve.DL.SCISSOR_XY, 650, 0),
    # make_cmd(eve.DL.TAG, 10),
    # make_cmd(eve.CMD.GRADIENT, 0, 0, 0x000000, 0, 400, 0xff0000),
    # make_cmd(eve.DL.SCISSOR_XY, 700, 0),
    # make_cmd(eve.DL.TAG, 11),
    # make_cmd(eve.CMD.GRADIENT, 0, 0, 0x000000, 0, 400, 0x00ff00),
    # make_cmd(eve.DL.SCISSOR_XY, 750, 0),
    # make_cmd(eve.DL.TAG, 12),
    # make_cmd(eve.CMD.GRADIENT, 0, 0, 0x000000, 0, 400, 0x0000ff),

    # Solid colour blocks
    make_cmd(eve.DL.CLEAR_COLOR_RGB, 255, 0, 0),
    make_cmd(eve.DL.SCISSOR_XY, 645, 5),
    make_cmd(eve.DL.CLEAR, 1, 0, 0),
    make_cmd(eve.DL.CLEAR_COLOR_RGB, 0, 255, 0),
    make_cmd(eve.DL.SCISSOR_XY, 695, 5),
    make_cmd(eve.DL.CLEAR, 1, 0, 0),
    make_cmd(eve.DL.CLEAR_COLOR_RGB, 0, 0, 255),
    make_cmd(eve.DL.SCISSOR_XY, 745, 5),
    make_cmd(eve.DL.CLEAR, 1, 0, 0),

    # # Horizontal gradients
    # make_cmd(eve.DL.SCISSOR_SIZE, 800, 100),
    # make_cmd(eve.DL.SCISSOR_XY, 0, 0),
    # make_cmd(eve.CMD.GRADIENT, 0, 0, 0x000000, 800, 0, 0xff0000),
    # make_cmd(eve.DL.SCISSOR_XY, 0, 100),
    # make_cmd(eve.CMD.GRADIENT, 0, 0, 0x000000, 800, 0, 0x00ff00),
    # make_cmd(eve.DL.SCISSOR_XY, 0, 200),
    # make_cmd(eve.CMD.GRADIENT, 0, 0, 0x000000, 800, 0, 0x0000ff),

    # Box
    make_cmd(eve.DL.SCISSOR_XY, 0, 0),
    make_cmd(eve.DL.SCISSOR_SIZE, 1024, 1024),
    make_cmd(eve.DL.LINE_WIDTH, 16),
    make_cmd(eve.DL.COLOR_RGB, 255, 255, 255),
    make_cmd(eve.DL.BEGIN, eve.GP.LINE_STRIP),
    make_cmd(eve.DL.VERTEX2F, 0, 0),
    make_cmd(eve.DL.VERTEX2F, 799 * 16, 0),
    make_cmd(eve.DL.VERTEX2F, 799 * 16, 479 * 16),
    make_cmd(eve.DL.VERTEX2F, 0, 479 * 16),
    make_cmd(eve.DL.VERTEX2F, 0, 0),
    # make_cmd(eve.DL.END),

    make_cmd(eve.DL.DISPLAY),
    make_cmd(eve.CMD.SWAP),
]

TEST_DISPLAYLIST2 = [
    make_cmd(eve.CMD.DLSTART),
    make_cmd(eve.DL.CLEAR, 1, 1, 1),

    make_cmd(eve.DL.BITMAP_HANDLE, 0),
    make_cmd(eve.CMD.SETBITMAP, eve.EVE_RAM_G + 0x40000, eve.BitmapFormat.RGB565, 800, 480),

    make_cmd(eve.DL.BEGIN, eve.GP.BITMAPS),
    make_cmd(eve.DL.VERTEX2II, 0, 0, 0, 0),
    make_cmd(eve.DL.END),

    make_cmd(eve.DL.DISPLAY),
    make_cmd(eve.CMD.SWAP),
]


@dataclass
class Signal:
    freq: int


class Sine(Signal):
    def __call__(self, t):
        return math.sin(self.freq * t)


class Cosine(Signal):
    def __call__(self, t):
        return math.cos(self.freq * t)


@dataclass
class Scope:
    timescale: float # Time per divisions
    divs: int
    width: int
    step: int


@dataclass
class Trace:
    scope: Scope
    signal: Signal
    xorig: int
    yorig: int
    height: int
    trigger_offset: float = 0

    def get_table(self):
        width = self.scope.width
        xscale = self.scope.timescale * self.scope.divs
        yoff =  self.yorig + self.height // 2
        yscale = self.height / 2
        def get_sample(x: int) -> int:
            t = self.trigger_offset + (x * xscale / width)
            return yoff - round(self.signal(t) * yscale)
        return [(self.xorig + x, get_sample(x)) for x in range(0, self.scope.width, self.scope.step)]


class Socket:
    MAGIC_WRITE = 0xA7B8FE30
    MAGIC_READ = 0xA7B8FE31

    def connect(self, url: str):
        self.websocket = connect(url)

    def read(self, addr: int, size: int) -> bytes:
        res = b''
        while size:
            len = min(size, 1024)
            self.websocket.send(struct.pack('<LLL', Socket.MAGIC_READ, addr, len))
            res += self.websocket.recv()
            addr += len
            size -= len
        return res

    def read32(self, addr: int) -> int:
        data = self.read(addr, 4)
        return struct.unpack('<L', data)[0]

    def send(self, addr: int, data: bytes):
        print(f'SEND addr 0x{addr:x}, len {len(data)}')
        CHUNK_SIZE = 8192 #4096 #1024
        for i in range(0, len(data), CHUNK_SIZE):
            chunk_size = min(len(data) - i, CHUNK_SIZE)
            self.websocket.send(struct.pack('<LLL', Socket.MAGIC_WRITE, addr, chunk_size) + data[i:i+chunk_size])
            addr += chunk_size
            rsp = self.websocket.recv()
            if rsp != 'OK':
                print(f'send(0x{addr:x}, {len(data)}) failed: {rsp}')
                assert False
                return False
        return True

    def send_cmdlist(self, cmdlist: list):
        if isinstance(cmdlist, list):
            cmdlist = b''.join(cmdlist)
        self.send(eve.REG.CMDB_WRITE, cmdlist)

    def write32(self, addr: int, value: int):
        self.send(addr, struct.pack('<L', value))


def prepare_image(filename: str) -> bytes:
    with Image.open(filename) as im:
        im = im.convert('RGB')
        if im.height / im.width > 1.1:
            im = im.rotate(-90, expand=True)
        sw = 800 / im.width
        sh = 480 / im.height
        if sw > sh:
            h = 480
            w = im.width * h // im.height
        else:
            w = 800
            h = im.height * w // im.width
        print(f'Resize "{filename}" from ({im.width}, {im.height}) -> ({w}, {h})')
        im = im.resize((w, h), Image.Resampling.BICUBIC)
        im = im.crop((0, 0, 800, 480))
        f = BytesIO()
        im.save(f, 'jpeg', optimize=True, progressive=False)
        return f.getvalue()


def load_image(socket, data: bytes):
    cmdlist = [
        make_cmd(eve.CMD.LOADIMAGE, 0x40000, 0)
        # make_cmd(eve.CMD.LOADIMAGE, 0x40000, eve.OPT.NODL)
    ]
    socket.send_cmdlist(b''.join(cmdlist) + data.ljust(align(len(data)), b'\0'))


def slideshow(socket: Socket, path: str):
    while True:
        for e in os.scandir(path, ):
            if not e.is_file:
                continue
            filename = os.path.join(path, e.name)
            try:
                imgdata = prepare_image(filename)
            except Exception as e:
                print(e)
                continue
            show_image(socket, imgdata)
            sleep(2)


def show_image(socket: Socket, imgdata: bytes):
    dl1 = [
        make_cmd(eve.CMD.DLSTART),
        make_cmd(eve.DL.CLEAR, 1, 1, 1),
        make_cmd(eve.DL.DISPLAY),
        make_cmd(eve.CMD.SWAP),
        make_cmd(eve.CMD.DLSTART),
        make_cmd(eve.DL.CLEAR, 1, 1, 1),
    ]
    dl2 = [
        make_cmd(eve.DL.BEGIN, eve.GP.BITMAPS),
        make_cmd(eve.DL.VERTEX2II, 0, 0, 0, 0),
        make_cmd(eve.DL.DISPLAY),
        make_cmd(eve.CMD.SWAP),
    ]
    socket.send_cmdlist(dl1)
    load_image(socket, imgdata)
    socket.send_cmdlist(dl2)


def show_image_file(socket: Socket, filename: str):
    imgdata = prepare_image(filename)
    show_image(socket, imgdata)


def copro_reset(socket: Socket):
    socket.write_reg(eve.REG.CPURESET, 1)
    socket.write_reg(eve.REG.CMD_READ, 0)
    socket.write_reg(eve.REG.CMD_WRITE, 0)
    socket.write_reg(eve.REG.CMD_DL, 0)
    # socket.write_reg(eve.REG.MEDIAFIFO_READ, 0)
    # socket.write_reg(eve.REG.MEDIAFIFO_WRITE, 0)
    socket.write_reg(eve.REG.CPURESET, 0)


def stream_video_file1(socket: Socket, filename: str):
    # copro_reset(socket)
    FIFO_ADDR = 0x080000
    FIFO_SIZE = 0x080000
    dl1 = [
        make_cmd(eve.CMD.DLSTART),
        make_cmd(eve.DL.CLEAR, 1, 1, 1),
        make_cmd(eve.DL.DISPLAY),
        make_cmd(eve.CMD.SWAP),
        # make_cmd(eve.CMD.MEMZERO, eve.REG.MEDIAFIFO_READ, 4),
        # make_cmd(eve.CMD.MEMZERO, eve.REG.MEDIAFIFO_WRITE, 4),
        make_cmd(eve.CMD.MEDIAFIFO, FIFO_ADDR, FIFO_SIZE),
    ]
    dl2 = [
        make_cmd(eve.CMD.PLAYVIDEO, eve.OPT.FULLSCREEN | eve.OPT.MEDIAFIFO),
    ]
    parse.parse(b''.join(dl1))
    socket.send_cmdlist(dl1)
    sleep(0.5)
    data = socket.recv(eve.REG.MEDIAFIFO_READ, 8)
    readpos, writepos = struct.unpack('<LL', data)
    print(f'readpos 0x{readpos:x}, writepos 0x{writepos:x}')
    CHUNK_SIZE = 8192
    writepos = readpos
    running = False
    with open(filename, 'rb') as f:
        while True:
            data = f.read(CHUNK_SIZE)
            socket.send(FIFO_ADDR + (writepos % FIFO_SIZE), data)
            socket.write32(eve.REG.MEDIAFIFO_WRITE, (writepos + CHUNK_SIZE) % FIFO_SIZE)
            writepos += CHUNK_SIZE
            if not running:
                if writepos >= FIFO_SIZE // 2:
                    parse.parse(b''.join(dl2))
                    socket.send_cmdlist(dl2)
                    running = True
                    break
            if running:
                sleep(0.4)


def stream_video_file(socket: Socket, filename: str):
    # copro_reset(socket)
    dl = [
        make_cmd(eve.CMD.DLSTART),
        make_cmd(eve.DL.CLEAR, 1, 1, 1),
        make_cmd(eve.DL.DISPLAY),
        make_cmd(eve.CMD.SWAP),
        make_cmd(eve.CMD.MEDIAFIFO, 0, 0),
        make_cmd(eve.CMD.PLAYVIDEO, eve.OPT.FULLSCREEN | eve.OPT.SOUND),
    ]
    parse.parse(b''.join(dl))
    socket.send_cmdlist(dl)
    CHUNK_SIZE = 2048
    with open(filename, 'rb') as f:
        while True:
            data = f.read(CHUNK_SIZE)
            socket.send(eve.REG.CMDB_WRITE, data)


def font_test(socket):
    xo, yo = 400, 240
    text = 'Now is the time for all good bits to come to the aid of the parity . . . .'
    spacing = 0
    start_angle = 65536 * 90 // 360

    handle = 31
    font_slot = 31
    # font_slot = 25
    # font_slot = 23
    # font_slot = 20

    font = fonts[font_slot - 16]
    tx, ty = font.width/2, font.height/2
    print(font)

    dlist = [
        make_cmd(eve.CMD.DLSTART),
        make_cmd(eve.DL.CLEAR, 1, 1, 1),
        # make_cmd(eve.DL.COLOR_A, 150),

        make_cmd(eve.CMD.ROMFONT, handle, font_slot),
        make_cmd(eve.DL.VERTEX_TRANSLATE_X, xo*16),
        make_cmd(eve.DL.VERTEX_TRANSLATE_Y, yo*16),
        make_cmd(eve.DL.VERTEX_FORMAT, 0),
        make_cmd(eve.DL.BITMAP_HANDLE, handle),
    ]


    words = iter(text.split())
    for angle in range(0, 65536, 4096):
        a = (angle + start_angle) & 0xffff
        dlist += [
            make_cmd(eve.CMD.LOADIDENTITY),
            make_cmd(eve.CMD.TRANSLATE, tx, ty),
            make_cmd(eve.CMD.SCALE, 0.8, 0.8),
            make_cmd(eve.CMD.ROTATE, a),
            make_cmd(eve.CMD.TRANSLATE, -tx, -ty),
            make_cmd(eve.CMD.SETMATRIX),
            make_cmd(eve.DL.COLOR_RGB, angle*200//65536, 0, 200 - (angle * 200 // 65536)),
            make_cmd(eve.DL.BEGIN, eve.GP.BITMAPS),
        ]

        base = font.height * 2
        off = 0
        ar = 2 * math.pi * a / 65536
        ax, ay = math.cos(ar), math.sin(ar)
        xb = round(base * ax * 1.8)
        yb = round(base * ay * 1.2)
        for c in next(words):
            char_width = font.char_widths[ord(c)]
            x = round(xb + off * ax)
            y = round(yb + off * ay)
            dlist += [
                make_cmd(eve.DL.CELL, ord(c)),
                make_cmd(eve.DL.VERTEX2F, x, y)
            ]
            off += char_width + spacing

        dlist += [
            make_cmd(eve.DL.END)
        ]

    dlist += [
        make_cmd(eve.DL.VERTEX_TRANSLATE_X, 0),
        make_cmd(eve.DL.VERTEX_TRANSLATE_Y, 0),
        make_cmd(eve.CMD.LOADIDENTITY),
        make_cmd(eve.CMD.SETMATRIX),
        make_cmd(eve.CMD.TEXT, 10, 10, 31, 0, 'Hello there!'),
        make_cmd(eve.DL.DISPLAY),
        make_cmd(eve.CMD.SWAP),
    ]
    parse.parse(b''.join(dlist))
    print(f'dlist length {len(dlist)}')
    socket.send_cmdlist(dlist)


def dump_fonts(socket):
    rom_font_addr = socket.read32(eve.EVE_ROM_FONT_ADDR)
    print(f'rom_font_addr = 0x{rom_font_addr:06x}')
    METRICS_SIZE = 148
    FONT_COUNT = 19
    metrics = socket.read(rom_font_addr, METRICS_SIZE * FONT_COUNT)
    for i in range(FONT_COUNT):
        off = i * METRICS_SIZE
        fields = struct.unpack('<128s5L', metrics[off:off+METRICS_SIZE])
        font = eve.FontMetrics(list(fields[0]), *fields[1:])
        print(f'ROM font #{i+16} @ 0x{rom_font_addr + off:06x}:')
        print(font)
    # parse.hexdump(metrics)


def dump_rom(socket):
    addr = eve.EVE_ROM_FONT
    size = 0x300000 - addr
    # addr, size = 0x200000, 0x100000
    print(f'@{addr:06x} -> {addr+size-1:06x}:')
    data = socket.read(addr, size)
    parse.hexdump(data, addr)


def button_test(socket):
    BMPSRC_BUTTON = 0x20004c
    BMPSRC_GRADIENT = 0x200065

    if False:
        print('BMP BUTTON')
        # LAYOUT = L8, 1, 25
        bmp_button = socket.read(BMPSRC_BUTTON, 25)
        parse.hexdump(bmp_button)

        print('BMP GRADIENT')
        # LAYOUT = L8, 512, 1
        bmp_gradient = socket.read(BMPSRC_GRADIENT, 512)
        parse.hexdump(bmp_gradient)

    dlist = [
        make_cmd(eve.CMD.DLSTART),
        make_cmd(eve.DL.CLEAR, 1, 1, 1),
        make_cmd(eve.DL.VERTEX_FORMAT, 0),

        make_cmd(eve.DL.BITMAP_HANDLE, 15),
        make_cmd(eve.DL.CELL, 0),
        make_cmd(eve.DL.BITMAP_SOURCE, BMPSRC_BUTTON),
        make_cmd(eve.DL.BITMAP_LAYOUT_H, 0, 0),
        make_cmd(eve.DL.BITMAP_LAYOUT, eve.BitmapFormat.L8, 1, 250),
        make_cmd(eve.DL.BITMAP_SIZE_H, 0, 0),
        make_cmd(eve.DL.BITMAP_SIZE, eve.BitmapFilter.NEAREST, eve.BitmapWrap.REPEAT, eve.BitmapWrap.BORDER, 400, 200),

        # make_cmd(eve.DL.COLOR_MASK, 0, 0, 0, 1),
        # make_cmd(eve.DL.BLEND_FUNC, eve.BlendFunction.ZERO, eve.BlendFunction.ZERO),
        # make_cmd(eve.DL.BEGIN, eve.GP.BITMAPS),
        # make_cmd(eve.DL.VERTEX2F, 0, 0),

        make_cmd(eve.DL.COLOR_MASK, 1, 1, 1, 1),
        make_cmd(eve.DL.BLEND_FUNC, eve.BlendFunction.SRC_ALPHA, eve.BlendFunction.ONE_MINUS_SRC_ALPHA),
        make_cmd(eve.DL.LINE_WIDTH, 3.75 * 16),
        make_cmd(eve.DL.BEGIN, eve.GP.RECTS),
        make_cmd(eve.DL.COLOR_RGB, 0, 56, 112),
        make_cmd(eve.DL.VERTEX2F, 0, 0),
        make_cmd(eve.DL.VERTEX2F, 400, 200),

        make_cmd(eve.DL.COLOR_MASK, 0, 0, 0, 1),
        make_cmd(eve.DL.BLEND_FUNC, eve.BlendFunction.DST_ALPHA, eve.BlendFunction.ZERO),
        make_cmd(eve.DL.BEGIN, eve.GP.BITMAPS),
        make_cmd(eve.DL.VERTEX2F, 0, 0),

        make_cmd(eve.DL.COLOR_MASK, 1, 1, 1, 0),
        make_cmd(eve.DL.BLEND_FUNC, eve.BlendFunction.DST_ALPHA, eve.BlendFunction.ONE_MINUS_DST_ALPHA),
        make_cmd(eve.DL.COLOR_RGB, 255, 255, 255),
        make_cmd(eve.DL.BEGIN, eve.GP.BITMAPS),
        make_cmd(eve.DL.VERTEX2F, 0, 0),

        make_cmd(eve.DL.DISPLAY),
        make_cmd(eve.CMD.SWAP),
    ]
    socket.send_cmdlist(dlist)


def custom_font_test(socket):
    xo, yo = 400, 240
    text = 'Now is the time for all good bits to come to the aid of the parity . . . .'
    spacing = 0
    start_angle = 0

    handle = 0
    font_slot = 30
    # font_slot = 25
    # font_slot = 23
    # font_slot = 20

    font = fonts[font_slot - 16]

    REPS = 4

    rects = []
    for i in range(REPS):
        rects += [
            make_cmd(eve.DL.COLOR_RGB, 50*(i&1), 20, 100*(i&1)),
            make_cmd(eve.DL.BEGIN, eve.GP.RECTS),
            make_cmd(eve.DL.VERTEX2F, 0, i*font.height),
            make_cmd(eve.DL.VERTEX2F, font.width-1, i*font.height + font.height-1),
        ]

    '''Rotated font requires an enlarged, centred glyph.
    Worst case is at 45 degrees where box width and height = sqrt(w*w + h*h).
    Font 30 is 28x36, diag is 46.
    96 characters (32-127) normally require (14*36*96) = 48384 bytes.
    Rotated font requires (23*46*96) = 101568 bytes.

    Font 34 is 78x108, diag is 134.
    Normal size (39*108*96) = 404352 bytes.
    Rotated font (67*134*96) = 861888 bytes.

    So if this becomes necessary it's likely going to be more efficient building the actual
    string text as a bitmap and rotating that.

    '''
    diag = math.ceil(math.sqrt(font.width * font.width + font.height * font.height))
    print(f'Font {font.width} x {font.height}, diag {diag}')

    h = font.height*REPS
    tx, ty = font.width / 2, h / 2
    dlist = [
        make_cmd(eve.CMD.DLSTART),
        make_cmd(eve.DL.CLEAR, 1, 1, 1),
        # make_cmd(eve.CMD.ROMFONT, handle, font_slot),
        make_cmd(eve.DL.VERTEX_TRANSLATE_X, 400*16),
        make_cmd(eve.DL.VERTEX_TRANSLATE_Y, 10*16),
        make_cmd(eve.CMD.LOADIDENTITY),
        make_cmd(eve.CMD.TRANSLATE, tx, ty),
        make_cmd(eve.CMD.ROTATE, -45 * 65536 // 360),
        make_cmd(eve.CMD.TRANSLATE, -tx, -ty),
        make_cmd(eve.CMD.SETMATRIX),
        make_cmd(eve.DL.VERTEX_FORMAT, 0),
        make_cmd(eve.DL.BITMAP_HANDLE, handle),
        make_cmd(eve.DL.BITMAP_SOURCE, font.bitmap),
        make_cmd(eve.DL.BITMAP_LAYOUT_H, 0, h >> 9),
        make_cmd(eve.DL.BITMAP_LAYOUT, font.format, font.stride, h & 0x1ff),
        make_cmd(eve.DL.BITMAP_SIZE_H, 0, h >> 9),
        make_cmd(eve.DL.BITMAP_SIZE, eve.BitmapFilter.NEAREST, eve.BitmapWrap.BORDER, eve.BitmapWrap.BORDER, font.width, h & 0x1ff),
        *rects,
        make_cmd(eve.DL.COLOR_RGB, 255, 255, 255),
        make_cmd(eve.DL.BEGIN, eve.GP.BITMAPS),
        make_cmd(eve.DL.CELL, ord('A')//REPS),
        make_cmd(eve.DL.VERTEX2F, 0, 0),

        make_cmd(eve.DL.DISPLAY),
        make_cmd(eve.CMD.SWAP),
    ]
    # parse.parse(b''.join(dlist))
    print(f'dlist length {len(dlist)}')
    socket.send_cmdlist(dlist)


def main():
    # data = b''.join(TEST_DISPLAYLIST_1)
    # parse.hexdump(data)

    socket = Socket()
    socket.connect('ws://192.168.1.175/ws')

    # dump_fonts(socket)
    # dump_rom(socket)
    # button_test(socket)
    font_test(socket)
    # custom_font_test(socket)
    return

    if True:
        scope = Scope(100e-6, 10, 16*780, 32)
        trace1 = Trace(scope, Sine(10000), 16*10, 16*10, 16*200)
        trace2 = Trace(scope, Cosine(20000), 16*10, 16*10, 16*200)

        frame_count = 0
        trace2.trigger_offset = -50e-6
        while True:
            frame_count += 1
            dlist = [
                make_cmd(eve.CMD.DLSTART),
                make_cmd(eve.DL.CLEAR, 1, 1, 1),

                # make_cmd(eve.DL.LINE_WIDTH, 8),
                make_cmd(eve.DL.POINT_SIZE, 16),

                make_cmd(eve.DL.BEGIN, eve.GP.LINE_STRIP),
                # make_cmd(eve.DL.BEGIN, eve.GP.POINTS),
                make_cmd(eve.DL.COLOR_RGB, 0, 0, 255),
                *(make_cmd(eve.DL.VERTEX2F, x, y) for x, y in trace1.get_table()),
                make_cmd(eve.DL.END),
                make_cmd(eve.DL.BEGIN, eve.GP.LINE_STRIP),
                # make_cmd(eve.DL.BEGIN, eve.GP.POINTS),
                make_cmd(eve.DL.COLOR_RGB, 255, 0, 0),
                *(make_cmd(eve.DL.VERTEX2F, x, y) for x, y in trace2.get_table()),

                make_cmd(eve.DL.COLOR_RGB, 80, 80, 80),
                make_cmd(eve.CMD.NUMBER, 10, 450, 30, 0, frame_count),

                make_cmd(eve.CMD.BUTTON, 400, 440, 90, 40, 25, 0, 'line'),
                make_cmd(eve.CMD.BUTTON, 500, 440, 90, 40, 25, eve.OPT.FLAT, 'point'),

                make_cmd(eve.DL.DISPLAY),
                make_cmd(eve.CMD.SWAP),
            ]
            socket.send_cmdlist(dlist)
            trace1.trigger_offset += 1e-6
            trace2.signal.freq += 100


    # send(eve.REG.PCLK_POL, struct.pack('<L', 1))
    # send(eve.REG.CSPREAD, struct.pack('<L', 1))

    if False:
        socket.send(eve.REG.HCYCLE, struct.pack('<10L',
            928,	    # HCYCLE
            128,	    # HOFFSET
            800,	    # HSIZE
            40,	        # HSYNC0
            88,	        # HSYNC1
            525,	    # VCYCLE
            32 + 13,    # VOFFSET
            480,	    # VSIZE
            0,	        # VSYNC0
            3,	        # VSYNC1
        ))

    # send(eve.REG_DITHER, struct.pack('<L', 1))

    # send(eve.REG.HSYNC0, struct.pack('<L', 39))
    # send(eve.REG.HSYNC1, struct.pack('<L', 88))
    # send(eve.REG.HOFFSET, struct.pack('<L', 127))

    # send(eve.REG.VSYNC0, struct.pack('<L', 5))
    # send(eve.REG.VSYNC1, struct.pack('<L', 15))
    # send(eve.REG.VOFFSET, struct.pack('<L', 42))

    # send(eve.REG.CPURESET, struct.pack('<L', 1))
    # send(eve.REG.CMD_READ, struct.pack('<LLL', 0, 0, 0))
    # send(eve.REG.CPURESET, struct.pack('<L', 0))

    if False:
        data = b''.join(TEST_DISPLAYLIST)
        parse.hexdump(data)
        parse.parse(data)
        socket.send_cmdlist(data)


if __name__ == '__main__':
    main()
