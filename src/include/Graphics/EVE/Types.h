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

constexpr Handle invalidHandle{255};

using PointSize = uint16_t; ///< In sixteenth pixels
using LineWidth = uint16_t; ///< In sixteenth pixels

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

	explicit operator double() const
	{
		return double(value) / scalar;
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

/*
 * @brief Details for loaded bitmap instance
 */
struct BitmapSlot {
	uint32_t address : 24;		  ///< Location of asset in ROM/RAMG
	EVE::BitmapFormat format : 8; // :5
	uint32_t metrics : 24;		  ///< Location of loaded font metrics for soft fonts
	uint16_t stride;			  // :12
	uint16_t width;				  // :11
	uint16_t height;			  // :11
	AssetID id;					  ///< Associated asset, 0 is 'unassigned'
};

/**
 * @brief Processed multi-touch values
 */
struct TouchValues {
	uint8_t tag[5];
	Point pt[5];
};

struct RawTouchData {
	struct alignas(4) TouchXY {
		int16_t y;
		int16_t x;
	};
	TouchXY touch1_xy;
	int16_t touch4_y;
	TouchXY touch_xy;
	TouchXY touch_tag_xy;
	uint8_t touch_tag;
	TouchXY touch_tag1_xy;
	uint8_t touch_tag1;
	TouchXY touch_tag2_xy;
	uint8_t touch_tag2;
	TouchXY touch_tag3_xy;
	uint8_t touch_tag3;
	TouchXY touch_tag4_xy;
	uint8_t touch_tag4;
	uint32_t transform[6];
	uint32_t touch_config;
	int16_t touch4_x;
	uint32_t padding8[7];
	TouchXY touch2_xy;
	TouchXY touch3_xy;

	TouchValues getValues() const
	{
		return {{
					touch_tag,
					touch_tag1,
					touch_tag2,
					touch_tag3,
					touch_tag4,
				},
				{
					{touch_xy.x, touch_xy.y},
					{touch_tag1_xy.x, touch_tag1_xy.y},
					{touch_tag2_xy.x, touch_tag2_xy.y},
					{touch_tag3_xy.x, touch_tag3_xy.y},
					{touch_tag4_xy.x, touch_tag4_xy.y},
				}};
	}
};
static_assert(sizeof(RawTouchData) == 30 * 4);

struct RawTrackerData {
	static constexpr unsigned size{5};
	struct alignas(uint32_t) Data {
		uint8_t tag;
		uint8_t reserved;
		uint16_t value;
	};
	Data data[size];
};
static_assert(sizeof(RawTrackerData) == 4 * 5);

} // namespace Graphics::EVE
