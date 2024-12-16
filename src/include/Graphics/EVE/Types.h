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

template <uint8_t precision> struct FixedTemplate {
	static constexpr unsigned scalar = 1U << precision;
	int32_t value{0};

	constexpr FixedTemplate() = default;

	constexpr FixedTemplate(double dbl) : value(dbl * scalar)
	{
	}

	template <uint8_t otherPrecision>
	constexpr FixedTemplate(FixedTemplate<otherPrecision> other)
		: value((precision < otherPrecision) ? (other.value >> (otherPrecision - precision))
											 : (other.value << (precision - otherPrecision)))
	{
	}

	explicit operator int32_t() const
	{
		return value;
	}

	int operator*(int num) const
	{
		return (num * value + scalar / 2) / scalar;
	}
};

using Fixed8 = FixedTemplate<8>;
using Fixed16 = FixedTemplate<16>;

template <uint8_t precision> int operator*(int value, FixedTemplate<precision> div)
{
	return div * value;
}

template <uint8_t precision> int operator/(int value, FixedTemplate<precision> div)
{
	return (value * div.scalar + div.value / 2) / div.value;
}

} // namespace Graphics::EVE
