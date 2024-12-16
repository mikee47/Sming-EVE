#include "EVE.h"
#include <Graphics/Colors.h>

namespace Graphics::EVE
{
using Address = uint32_t;
using ColorChannel = uint8_t;
using Tag = uint8_t;
using Handle = uint8_t;
using Cell = uint8_t;
using DisplayListOffset = uint16_t;

// Used with co-processor commands
using RGB = Color;  //uint32_t;
using ARGB = Color; //uint32_t;
using Options = uint16_t;

struct Angle {
	uint16_t value;

	constexpr Angle(uint16_t angle = 0) : value(angle)
	{
	}

	explicit operator uint32_t() const
	{
		return value;
	}
};

struct Degrees : public Angle {
	constexpr Degrees() : Angle(0)
	{
	}

	constexpr Degrees(float angle) : Angle(angle * 65536 / 360)
	{
	}
};

struct Fixed8_8 {
	int32_t value;

	constexpr Fixed8_8(double dbl) : value(dbl * 0x100)
	{
	}

	explicit operator int32_t() const
	{
		return value;
	}
};

struct Fixed15_8 {
	int32_t value;

	constexpr Fixed15_8(double dbl) : value(dbl * 0x100)
	{
	}

	explicit operator int32_t() const
	{
		return value;
	}
};

struct Fixed16_16 {
	int32_t value;

	constexpr Fixed16_16(double dbl) : value(dbl * 0x10000)
	{
	}

	explicit operator int32_t() const
	{
		return value;
	}
};

class CommandBuffer
{
public:
	void reset()
	{
		size = 0;
	}

	void write(const void* data, unsigned len)
	{
		assert(size + len < sizeof(buffer));
		::memcpy(&buffer[size], data, len);
		size += len;
	}

	void write(const String& s)
	{
		auto len = 1 + s.length();
		write(s.c_str(), len);
		align();
	}

	void align()
	{
		auto off = size % 4;
		if(off == 0) {
			return;
		}
		while(off++ < 4) {
			buffer[size++] = 0;
		}
	}

	const void* get() const
	{
		return buffer;
	}

	unsigned length() const
	{
		return size;
	}

#include "command_defs.h"

	void color(Color color)
	{
		color_rgb(getRed(color), getGreen(color), getBlue(color));
	}

	void clear_color(Color color)
	{
		clear_color_rgb(getRed(color), getGreen(color), getBlue(color));
	}

	void vertex2f(Point pt)
	{
		vertex2f(pt.x, pt.y);
	}

private:
	uint8_t buffer[EVE_CMDFIFO_SIZE];
	unsigned size{0};
};

} // namespace Graphics::EVE
