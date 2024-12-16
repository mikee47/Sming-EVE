/*
 *
 * This file is auto-generated.
 *
 */

void alpha_func(TestFunction func, uint8_t ref)
{
    struct Inst {
        uint32_t ref: 8;
        uint32_t func: 4;
        uint32_t unused: 12;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{ref, uint32_t(uint16_t(func)), 0, 0x09};
    write(&inst, sizeof(inst));
}

void begin(GraphicsPrimitive prim)
{
    struct Inst {
        uint32_t prim: 4;
        uint32_t unused: 20;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(prim)), 0, 0x1f};
    write(&inst, sizeof(inst));
}

void bitmap_handle(Handle handle)
{
    struct Inst {
        uint32_t handle: 5;
        uint32_t unused: 19;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(handle)), 0, 0x05};
    write(&inst, sizeof(inst));
}

void bitmap_layout(BitmapFormat format, uint32_t linestride, uint32_t height)
{
    struct Inst {
        uint32_t height: 9;
        uint32_t linestride: 10;
        uint32_t format: 5;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{height, linestride, uint32_t(uint16_t(format)), 0x07};
    write(&inst, sizeof(inst));
}

void bitmap_layout_h(uint32_t linestride, uint32_t height)
{
    struct Inst {
        uint32_t height: 2;
        uint32_t linestride: 2;
        uint32_t unused: 20;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{height, linestride, 0, 0x28};
    write(&inst, sizeof(inst));
}

void bitmap_size(BitmapFilter filter, BitmapWrap wrapx, BitmapWrap wrapy, uint32_t width, uint32_t height)
{
    struct Inst {
        uint32_t height: 9;
        uint32_t width: 9;
        uint32_t wrapy: 1;
        uint32_t wrapx: 1;
        uint32_t filter: 1;
        uint32_t unused: 3;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{height, width, uint32_t(uint16_t(wrapy)), uint32_t(uint16_t(wrapx)), uint32_t(uint16_t(filter)), 0, 0x08};
    write(&inst, sizeof(inst));
}

void bitmap_size_h(uint32_t width, uint32_t height)
{
    struct Inst {
        uint32_t height: 2;
        uint32_t width: 2;
        uint32_t unused: 20;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{height, width, 0, 0x29};
    write(&inst, sizeof(inst));
}

void bitmap_source(Address addr)
{
    struct Inst {
        uint32_t addr: 22;
        uint32_t unused: 2;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint32_t(addr)), 0, 0x01};
    write(&inst, sizeof(inst));
}

void bitmap_transform_a(Fixed8 a)
{
    struct Inst {
        uint32_t a: 17;
        uint32_t unused: 7;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(int32_t(a)), 0, 0x15};
    write(&inst, sizeof(inst));
}

void bitmap_transform_b(Fixed8 b)
{
    struct Inst {
        uint32_t b: 17;
        uint32_t unused: 7;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(int32_t(b)), 0, 0x16};
    write(&inst, sizeof(inst));
}

void bitmap_transform_c(Fixed8 c)
{
    struct Inst {
        uint32_t c: 24;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(int32_t(c)), 0x17};
    write(&inst, sizeof(inst));
}

void bitmap_transform_d(Fixed8 d)
{
    struct Inst {
        uint32_t d: 17;
        uint32_t unused: 7;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(int32_t(d)), 0, 0x18};
    write(&inst, sizeof(inst));
}

void bitmap_transform_e(Fixed8 e)
{
    struct Inst {
        uint32_t e: 17;
        uint32_t unused: 7;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(int32_t(e)), 0, 0x19};
    write(&inst, sizeof(inst));
}

void bitmap_transform_f(Fixed8 f)
{
    struct Inst {
        uint32_t f: 24;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(int32_t(f)), 0x1a};
    write(&inst, sizeof(inst));
}

void blend_func(BlendFunction src, BlendFunction dst)
{
    struct Inst {
        uint32_t dst: 3;
        uint32_t src: 3;
        uint32_t unused: 18;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(dst)), uint32_t(uint16_t(src)), 0, 0x0b};
    write(&inst, sizeof(inst));
}

void call(DisplayListOffset dest)
{
    struct Inst {
        uint32_t dest: 16;
        uint32_t unused: 8;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(dest)), 0, 0x1d};
    write(&inst, sizeof(inst));
}

void cell(Cell cell)
{
    struct Inst {
        uint32_t cell: 7;
        uint32_t unused: 17;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(cell)), 0, 0x06};
    write(&inst, sizeof(inst));
}

void clear(bool c, bool s, bool t)
{
    struct Inst {
        uint32_t t: 1;
        uint32_t s: 1;
        uint32_t c: 1;
        uint32_t unused: 21;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(t)), uint32_t(uint16_t(s)), uint32_t(uint16_t(c)), 0, 0x26};
    write(&inst, sizeof(inst));
}

void clear_color_a(ColorChannel alpha)
{
    struct Inst {
        uint32_t alpha: 8;
        uint32_t unused: 16;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(alpha)), 0, 0x0f};
    write(&inst, sizeof(inst));
}

void clear_color_rgb(ColorChannel red, ColorChannel green, ColorChannel blue)
{
    struct Inst {
        uint32_t blue: 8;
        uint32_t green: 8;
        uint32_t red: 8;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(blue)), uint32_t(uint16_t(green)), uint32_t(uint16_t(red)), 0x02};
    write(&inst, sizeof(inst));
}

void clear_stencil(ColorChannel s)
{
    struct Inst {
        uint32_t s: 8;
        uint32_t unused: 16;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(s)), 0, 0x11};
    write(&inst, sizeof(inst));
}

void clear_tag(Tag tag)
{
    struct Inst {
        uint32_t tag: 8;
        uint32_t unused: 16;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(tag)), 0, 0x12};
    write(&inst, sizeof(inst));
}

void color_a(ColorChannel alpha)
{
    struct Inst {
        uint32_t alpha: 8;
        uint32_t unused: 16;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(alpha)), 0, 0x10};
    write(&inst, sizeof(inst));
}

void color_mask(bool r, bool g, bool b, bool a)
{
    struct Inst {
        uint32_t a: 1;
        uint32_t b: 1;
        uint32_t g: 1;
        uint32_t r: 1;
        uint32_t unused: 20;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(a)), uint32_t(uint16_t(b)), uint32_t(uint16_t(g)), uint32_t(uint16_t(r)), 0, 0x20};
    write(&inst, sizeof(inst));
}

void color_rgb(ColorChannel red, ColorChannel green, ColorChannel blue)
{
    struct Inst {
        uint32_t blue: 8;
        uint32_t green: 8;
        uint32_t red: 8;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(blue)), uint32_t(uint16_t(green)), uint32_t(uint16_t(red)), 0x04};
    write(&inst, sizeof(inst));
}

void display()
{
    struct Inst {
        uint32_t unused: 24;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{0, 0x00};
    write(&inst, sizeof(inst));
}

void end()
{
    struct Inst {
        uint32_t unused: 24;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{0, 0x21};
    write(&inst, sizeof(inst));
}

void jump(DisplayListOffset dest)
{
    struct Inst {
        uint32_t dest: 16;
        uint32_t unused: 8;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(dest)), 0, 0x1e};
    write(&inst, sizeof(inst));
}

void line_width(uint32_t width)
{
    struct Inst {
        uint32_t width: 12;
        uint32_t unused: 12;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{width, 0, 0x0e};
    write(&inst, sizeof(inst));
}

void macro(uint32_t m)
{
    struct Inst {
        uint32_t m: 1;
        uint32_t unused: 23;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{m, 0, 0x25};
    write(&inst, sizeof(inst));
}

void nop()
{
    struct Inst {
        uint32_t unused: 24;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{0, 0x2d};
    write(&inst, sizeof(inst));
}

void palette_source(Address addr)
{
    struct Inst {
        uint32_t addr: 22;
        uint32_t unused: 2;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint32_t(addr)), 0, 0x2a};
    write(&inst, sizeof(inst));
}

void point_size(uint32_t size)
{
    struct Inst {
        uint32_t size: 13;
        uint32_t unused: 11;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{size, 0, 0x0d};
    write(&inst, sizeof(inst));
}

void restore_context()
{
    struct Inst {
        uint32_t unused: 24;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{0, 0x23};
    write(&inst, sizeof(inst));
}

void return_()
{
    struct Inst {
        uint32_t unused: 24;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{0, 0x24};
    write(&inst, sizeof(inst));
}

void save_context()
{
    struct Inst {
        uint32_t unused: 24;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{0, 0x22};
    write(&inst, sizeof(inst));
}

void scissor_size(uint32_t width, uint32_t height)
{
    struct Inst {
        uint32_t height: 12;
        uint32_t width: 12;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{height, width, 0x1c};
    write(&inst, sizeof(inst));
}

void scissor_xy(uint32_t x, uint32_t y)
{
    struct Inst {
        uint32_t y: 11;
        uint32_t x: 11;
        uint32_t unused: 2;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{y, x, 0, 0x1b};
    write(&inst, sizeof(inst));
}

void stencil_func(TestFunction func, uint8_t ref, uint8_t mask)
{
    struct Inst {
        uint32_t mask: 8;
        uint32_t ref: 8;
        uint32_t func: 4;
        uint32_t unused: 4;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{mask, ref, uint32_t(uint16_t(func)), 0, 0x0a};
    write(&inst, sizeof(inst));
}

void stencil_mask(uint8_t mask)
{
    struct Inst {
        uint32_t mask: 8;
        uint32_t unused: 16;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{mask, 0, 0x13};
    write(&inst, sizeof(inst));
}

void stencil_op(StencilOp sfail, StencilOp spass)
{
    struct Inst {
        uint32_t spass: 3;
        uint32_t sfail: 3;
        uint32_t unused: 18;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(spass)), uint32_t(uint16_t(sfail)), 0, 0x0c};
    write(&inst, sizeof(inst));
}

void tag(Tag tag)
{
    struct Inst {
        uint32_t tag: 8;
        uint32_t unused: 16;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(tag)), 0, 0x03};
    write(&inst, sizeof(inst));
}

void tag_mask(bool mask)
{
    struct Inst {
        uint32_t mask: 1;
        uint32_t unused: 23;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(mask)), 0, 0x14};
    write(&inst, sizeof(inst));
}

void vertex2f(uint32_t x, uint32_t y)
{
    struct Inst {
        uint32_t y: 15;
        uint32_t x: 15;
        uint8_t code: 2;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{y, x, 0x01};
    write(&inst, sizeof(inst));
}

void vertex2ii(uint32_t x, uint32_t y, Handle handle, Cell cell)
{
    struct Inst {
        uint32_t cell: 7;
        uint32_t handle: 5;
        uint32_t y: 9;
        uint32_t x: 9;
        uint8_t code: 2;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{uint32_t(uint16_t(cell)), uint32_t(uint16_t(handle)), y, x, 0x02};
    write(&inst, sizeof(inst));
}

void vertex_format(uint32_t frac)
{
    struct Inst {
        uint32_t frac: 3;
        uint32_t unused: 21;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{frac, 0, 0x27};
    write(&inst, sizeof(inst));
}

void vertex_translate_x(uint32_t x)
{
    struct Inst {
        uint32_t x: 17;
        uint32_t unused: 7;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{x, 0, 0x2b};
    write(&inst, sizeof(inst));
}

void vertex_translate_y(uint32_t y)
{
    struct Inst {
        uint32_t y: 17;
        uint32_t unused: 7;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{y, 0, 0x2c};
    write(&inst, sizeof(inst));
}


void append(Address ptr, uint32_t num)
{
    struct Inst {
        uint32_t code;
        uint32_t ptr;
        uint32_t num;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff1e, uint32_t(ptr), num};
    write(&inst, sizeof(inst));
}

void bgcolor(RGB color)
{
    struct Inst {
        uint32_t code;
        uint32_t color;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff09, uint32_t(color)};
    write(&inst, sizeof(inst));
}

void button(int16_t x, int16_t y, uint16_t w, uint16_t h, Handle font, Options options, const String& s)
{
    struct Inst {
        uint32_t code;
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t h;
        uint16_t font;
        uint16_t options;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff0d, x, y, w, h, uint16_t(font), uint16_t(options)};
    write(&inst, sizeof(inst));
    write(s);
}

void calibrate(uint32_t result)
{
    struct Inst {
        uint32_t code;
        uint32_t result;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff15, result};
    write(&inst, sizeof(inst));
}

void clock(int16_t x, int16_t y, uint16_t r, Options options, uint16_t h, uint16_t m, uint16_t s, uint16_t ms)
{
    struct Inst {
        uint32_t code;
        int16_t x;
        int16_t y;
        uint16_t r;
        uint16_t options;
        uint16_t h;
        uint16_t m;
        uint16_t s;
        uint16_t ms;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff14, x, y, r, uint16_t(options), h, m, s, ms};
    write(&inst, sizeof(inst));
}

void coldstart()
{
    struct Inst {
        uint32_t code;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff32};
    write(&inst, sizeof(inst));
}

void dial(int16_t x, int16_t y, uint16_t r, Options options, uint16_t value)
{
    struct Inst {
        uint32_t code;
        int16_t x;
        int16_t y;
        uint16_t r;
        uint16_t options;
        uint16_t value;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff2d, x, y, r, uint16_t(options), value};
    write(&inst, sizeof(inst));
}

void dlstart()
{
    struct Inst {
        uint32_t code;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff00};
    write(&inst, sizeof(inst));
}

void fgcolor(RGB color)
{
    struct Inst {
        uint32_t code;
        uint32_t color;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff0a, uint32_t(color)};
    write(&inst, sizeof(inst));
}

void gauge(int16_t x, int16_t y, uint16_t r, Options options, uint16_t major, uint16_t minor, uint16_t value, uint16_t range)
{
    struct Inst {
        uint32_t code;
        int16_t x;
        int16_t y;
        uint16_t r;
        uint16_t options;
        uint16_t major;
        uint16_t minor;
        uint16_t value;
        uint16_t range;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff13, x, y, r, uint16_t(options), major, minor, value, range};
    write(&inst, sizeof(inst));
}

void getmatrix(Fixed8 a, Fixed8 b, Fixed8 c, Fixed8 d, Fixed8 e, Fixed8 f)
{
    struct Inst {
        uint32_t code;
        int32_t a;
        int32_t b;
        int32_t c;
        int32_t d;
        int32_t e;
        int32_t f;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff33, int32_t(a), int32_t(b), int32_t(c), int32_t(d), int32_t(e), int32_t(f)};
    write(&inst, sizeof(inst));
}

void getprops(Address ptr, uint32_t width, uint32_t height)
{
    struct Inst {
        uint32_t code;
        uint32_t ptr;
        uint32_t width;
        uint32_t height;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff25, uint32_t(ptr), width, height};
    write(&inst, sizeof(inst));
}

void getptr(int32_t result)
{
    struct Inst {
        uint32_t code;
        int32_t result;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff23, result};
    write(&inst, sizeof(inst));
}

void gradcolor(RGB color)
{
    struct Inst {
        uint32_t code;
        uint32_t color;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff34, uint32_t(color)};
    write(&inst, sizeof(inst));
}

void gradient(int16_t x0, int16_t y0, RGB rgb0, int16_t x1, int16_t y1, RGB rgb1)
{
    struct Inst {
        uint32_t code;
        int16_t x0;
        int16_t y0;
        uint32_t rgb0;
        int16_t x1;
        int16_t y1;
        uint32_t rgb1;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff0b, x0, y0, uint32_t(rgb0), x1, y1, uint32_t(rgb1)};
    write(&inst, sizeof(inst));
}

void gradienta(int16_t x0, int16_t y0, ARGB argb0, int16_t x1, int16_t y1, ARGB argb1)
{
    struct Inst {
        uint32_t code;
        int16_t x0;
        int16_t y0;
        uint32_t argb0;
        int16_t x1;
        int16_t y1;
        uint32_t argb1;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff57, x0, y0, uint32_t(argb0), x1, y1, uint32_t(argb1)};
    write(&inst, sizeof(inst));
}

void inflate(Address ptr, const void* data, unsigned data_length)
{
    struct Inst {
        uint32_t code;
        uint32_t ptr;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff22, uint32_t(ptr)};
    write(&inst, sizeof(inst));
    write(data, data_length);
    align();
}

void interrupt(uint32_t ms)
{
    struct Inst {
        uint32_t code;
        uint32_t ms;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff02, ms};
    write(&inst, sizeof(inst));
}

void keys(int16_t x, int16_t y, uint16_t w, uint16_t h, Handle font, Options options, const String& s)
{
    struct Inst {
        uint32_t code;
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t h;
        uint16_t font;
        uint16_t options;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff0e, x, y, w, h, uint16_t(font), uint16_t(options)};
    write(&inst, sizeof(inst));
    write(s);
}

void loadidentity()
{
    struct Inst {
        uint32_t code;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff26};
    write(&inst, sizeof(inst));
}

void loadimage(Address ptr, Options options)
{
    struct Inst {
        uint32_t code;
        uint32_t ptr;
        uint16_t options;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff24, uint32_t(ptr), uint16_t(options)};
    write(&inst, sizeof(inst));
}

void logo()
{
    struct Inst {
        uint32_t code;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff31};
    write(&inst, sizeof(inst));
}

void mediafifo(Address ptr, uint32_t size)
{
    struct Inst {
        uint32_t code;
        uint32_t ptr;
        uint32_t size;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff39, uint32_t(ptr), size};
    write(&inst, sizeof(inst));
}

void memcpy(Address dest, Address src, uint32_t num)
{
    struct Inst {
        uint32_t code;
        uint32_t dest;
        uint32_t src;
        uint32_t num;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff1d, uint32_t(dest), uint32_t(src), num};
    write(&inst, sizeof(inst));
}

void memcrc(Address ptr, uint32_t num, uint32_t result)
{
    struct Inst {
        uint32_t code;
        uint32_t ptr;
        uint32_t num;
        uint32_t result;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff18, uint32_t(ptr), num, result};
    write(&inst, sizeof(inst));
}

void memset(Address ptr, uint8_t value, uint32_t num)
{
    struct Inst {
        uint32_t code;
        uint32_t ptr;
        uint16_t value;
        uint32_t num;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff1b, uint32_t(ptr), uint16_t(value), num};
    write(&inst, sizeof(inst));
}

void memwrite(Address ptr, uint32_t num, const void* data)
{
    struct Inst {
        uint32_t code;
        uint32_t ptr;
        uint32_t num;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff1a, uint32_t(ptr), num};
    write(&inst, sizeof(inst));
    write(data, num);
    align();
}

void memzero(Address ptr, uint32_t num)
{
    struct Inst {
        uint32_t code;
        uint32_t ptr;
        uint32_t num;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff1c, uint32_t(ptr), num};
    write(&inst, sizeof(inst));
}

void number(int16_t x, int16_t y, Handle font, Options options, int32_t n)
{
    struct Inst {
        uint32_t code;
        int16_t x;
        int16_t y;
        uint16_t font;
        uint16_t options;
        int32_t n;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff2e, x, y, uint16_t(font), uint16_t(options), n};
    write(&inst, sizeof(inst));
}

void playvideo(Options options)
{
    struct Inst {
        uint32_t code;
        uint16_t options;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff3a, uint16_t(options)};
    write(&inst, sizeof(inst));
}

void progress(int16_t x, int16_t y, uint16_t w, uint16_t h, Options options, uint16_t value, uint16_t range)
{
    struct Inst {
        uint32_t code;
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t h;
        uint16_t options;
        uint16_t value;
        uint16_t range;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff0f, x, y, w, h, uint16_t(options), value, range};
    write(&inst, sizeof(inst));
}

void regread(Address ptr, uint32_t result)
{
    struct Inst {
        uint32_t code;
        uint32_t ptr;
        uint32_t result;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff19, uint32_t(ptr), result};
    write(&inst, sizeof(inst));
}

void romfont(Handle font, uint8_t romslot)
{
    struct Inst {
        uint32_t code;
        uint32_t font;
        uint16_t romslot;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff3f, uint32_t(font), uint16_t(romslot)};
    write(&inst, sizeof(inst));
}

void rotate(Angle a)
{
    struct Inst {
        uint32_t code;
        uint32_t a;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff29, uint32_t(a)};
    write(&inst, sizeof(inst));
}

void scale(Fixed16 sx, Fixed16 sy)
{
    struct Inst {
        uint32_t code;
        int32_t sx;
        int32_t sy;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff28, int32_t(sx), int32_t(sy)};
    write(&inst, sizeof(inst));
}

void screensaver()
{
    struct Inst {
        uint32_t code;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff2f};
    write(&inst, sizeof(inst));
}

void scrollbar(int16_t x, int16_t y, uint16_t w, uint16_t h, Options options, uint16_t value, uint16_t size, uint16_t range)
{
    struct Inst {
        uint32_t code;
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t h;
        uint16_t options;
        uint16_t value;
        uint16_t size;
        uint16_t range;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff11, x, y, w, h, uint16_t(options), value, size, range};
    write(&inst, sizeof(inst));
}

void setbase(uint8_t b)
{
    struct Inst {
        uint32_t code;
        uint16_t b;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff38, uint16_t(b)};
    write(&inst, sizeof(inst));
}

void setbitmap(Address source, uint8_t fmt, uint16_t width, uint16_t height)
{
    struct Inst {
        uint32_t code;
        uint32_t source;
        uint16_t fmt;
        uint16_t width;
        uint16_t height;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff43, uint32_t(source), uint16_t(fmt), width, height};
    write(&inst, sizeof(inst));
}

void setfont(Handle font, Address ptr)
{
    struct Inst {
        uint32_t code;
        uint16_t font;
        uint32_t ptr;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff2b, uint16_t(font), uint32_t(ptr)};
    write(&inst, sizeof(inst));
}

void setfont2(Handle font, Address ptr, uint8_t firstchar)
{
    struct Inst {
        uint32_t code;
        uint16_t font;
        uint32_t ptr;
        uint16_t firstchar;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff3b, uint16_t(font), uint32_t(ptr), uint16_t(firstchar)};
    write(&inst, sizeof(inst));
}

void setmatrix()
{
    struct Inst {
        uint32_t code;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff2a};
    write(&inst, sizeof(inst));
}

void setrotate(uint8_t r)
{
    struct Inst {
        uint32_t code;
        uint16_t r;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff36, uint16_t(r)};
    write(&inst, sizeof(inst));
}

void setscratch(Handle handle)
{
    struct Inst {
        uint32_t code;
        uint16_t handle;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff3c, uint16_t(handle)};
    write(&inst, sizeof(inst));
}

void sketch()
{
    struct Inst {
        uint32_t code;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff30};
    write(&inst, sizeof(inst));
}

void slider(int16_t x, int16_t y, uint16_t w, uint16_t h, Options options, uint16_t value, uint16_t range)
{
    struct Inst {
        uint32_t code;
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t h;
        uint16_t options;
        uint16_t value;
        uint16_t range;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff10, x, y, w, h, uint16_t(options), value, range};
    write(&inst, sizeof(inst));
}

void snapshot(Address ptr)
{
    struct Inst {
        uint32_t code;
        uint32_t ptr;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff1f, uint32_t(ptr)};
    write(&inst, sizeof(inst));
}

void snapshot2(uint8_t fmt, Address ptr, int16_t x, int16_t y, uint16_t w, uint16_t h)
{
    struct Inst {
        uint32_t code;
        uint16_t fmt;
        uint32_t ptr;
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t h;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff37, uint16_t(fmt), uint32_t(ptr), x, y, w, h};
    write(&inst, sizeof(inst));
}

void spinner(int16_t x, int16_t y, uint16_t style, uint16_t scale)
{
    struct Inst {
        uint32_t code;
        int16_t x;
        int16_t y;
        uint16_t style;
        uint16_t scale;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff16, x, y, style, scale};
    write(&inst, sizeof(inst));
}

void stop()
{
    struct Inst {
        uint32_t code;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff17};
    write(&inst, sizeof(inst));
}

void swap()
{
    struct Inst {
        uint32_t code;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff01};
    write(&inst, sizeof(inst));
}

void text(int16_t x, int16_t y, Handle font, Options options, const String& s)
{
    struct Inst {
        uint32_t code;
        int16_t x;
        int16_t y;
        uint16_t font;
        uint16_t options;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff0c, x, y, uint16_t(font), uint16_t(options)};
    write(&inst, sizeof(inst));
    write(s);
}

void toggle(int16_t x, int16_t y, uint16_t w, Handle font, Options options, uint16_t state, const String& s)
{
    struct Inst {
        uint32_t code;
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t font;
        uint16_t options;
        uint16_t state;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff12, x, y, w, uint16_t(font), uint16_t(options), state};
    write(&inst, sizeof(inst));
    write(s);
}

void track(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t tag)
{
    struct Inst {
        uint32_t code;
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t h;
        uint16_t tag;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff2c, x, y, w, h, uint16_t(tag)};
    write(&inst, sizeof(inst));
}

void translate(Fixed16 tx, Fixed16 ty)
{
    struct Inst {
        uint32_t code;
        int32_t tx;
        int32_t ty;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff27, int32_t(tx), int32_t(ty)};
    write(&inst, sizeof(inst));
}

void videoframe(Address dst, Address ptr)
{
    struct Inst {
        uint32_t code;
        uint32_t dst;
        uint32_t ptr;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff41, uint32_t(dst), uint32_t(ptr)};
    write(&inst, sizeof(inst));
}

void videostart()
{
    struct Inst {
        uint32_t code;
    };
    static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");
    Inst inst{0xffffff40};
    write(&inst, sizeof(inst));
}

