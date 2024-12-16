#pragma once

#include <Graphics/Types.h>

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
	int32_t value{0};

	constexpr Fixed8_8() = default;

	constexpr Fixed8_8(double dbl) : value(dbl * 0x100)
	{
	}

	explicit operator int32_t() const
	{
		return value;
	}
};

struct Fixed15_8 {
	int32_t value{0};

	constexpr Fixed15_8() = default;

	constexpr Fixed15_8(double dbl) : value(dbl * 0x100)
	{
	}

	explicit operator int32_t() const
	{
		return value;
	}
};

struct Fixed16_16 {
	int32_t value{0};

	constexpr Fixed16_16() = default;

	constexpr Fixed16_16(double dbl) : value(dbl * 0x10000)
	{
	}

	explicit operator int32_t() const
	{
		return value;
	}
};

} // namespace Graphics::EVE
