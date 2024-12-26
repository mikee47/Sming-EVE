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
    Inst inst{ref, uint32_t(func), 0, 0x09};
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
    Inst inst{uint32_t(prim), 0, 0x1f};
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
    Inst inst{uint32_t(handle), 0, 0x05};
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
    Inst inst{height, linestride, uint32_t(format), 0x07};
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
    Inst inst{height, width, uint32_t(wrapy), uint32_t(wrapx), uint32_t(filter), 0, 0x08};
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
    Inst inst{uint32_t(addr), 0, 0x01};
    write(&inst, sizeof(inst));
}

void bitmap_transform_a(Fixed8 a)
{
    struct Inst {
        int32_t a: 17;
        uint32_t unused: 7;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{int32_t(a), 0, 0x15};
    write(&inst, sizeof(inst));
}

void bitmap_transform_b(Fixed8 b)
{
    struct Inst {
        int32_t b: 17;
        uint32_t unused: 7;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{int32_t(b), 0, 0x16};
    write(&inst, sizeof(inst));
}

void bitmap_transform_c(Fixed8 c)
{
    struct Inst {
        int32_t c: 24;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{int32_t(c), 0x17};
    write(&inst, sizeof(inst));
}

void bitmap_transform_d(Fixed8 d)
{
    struct Inst {
        int32_t d: 17;
        uint32_t unused: 7;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{int32_t(d), 0, 0x18};
    write(&inst, sizeof(inst));
}

void bitmap_transform_e(Fixed8 e)
{
    struct Inst {
        int32_t e: 17;
        uint32_t unused: 7;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{int32_t(e), 0, 0x19};
    write(&inst, sizeof(inst));
}

void bitmap_transform_f(Fixed8 f)
{
    struct Inst {
        int32_t f: 24;
        uint8_t code: 8;
    };
    static_assert(sizeof(Inst) == 4, "Bad Inst");
    Inst inst{int32_t(f), 0x1a};
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
    Inst inst{uint32_t(dst), uint32_t(src), 0, 0x0b};
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
    Inst inst{uint32_t(dest), 0, 0x1d};
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
    Inst inst{uint32_t(cell), 0, 0x06};
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
    Inst inst{uint32_t(t), uint32_t(s), uint32_t(c), 0, 0x26};
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
    Inst inst{uint32_t(alpha), 0, 0x0f};
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
    Inst inst{uint32_t(blue), uint32_t(green), uint32_t(red), 0x02};
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
    Inst inst{uint32_t(s), 0, 0x11};
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
    Inst inst{uint32_t(tag), 0, 0x12};
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
    Inst inst{uint32_t(alpha), 0, 0x10};
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
    Inst inst{uint32_t(a), uint32_t(b), uint32_t(g), uint32_t(r), 0, 0x20};
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
    Inst inst{uint32_t(blue), uint32_t(green), uint32_t(red), 0x04};
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
    Inst inst{uint32_t(dest), 0, 0x1e};
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
    Inst inst{uint32_t(addr), 0, 0x2a};
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
    Inst inst{mask, ref, uint32_t(func), 0, 0x0a};
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
    Inst inst{uint32_t(spass), uint32_t(sfail), 0, 0x0c};
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
    Inst inst{uint32_t(tag), 0, 0x03};
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
    Inst inst{uint32_t(mask), 0, 0x14};
    write(&inst, sizeof(inst));
}

void vertex2f(int32_t x, int32_t y)
{
    struct Inst {
        int32_t y: 15;
        int32_t x: 15;
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
    Inst inst{uint32_t(cell), uint32_t(handle), y, x, 0x02};
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

