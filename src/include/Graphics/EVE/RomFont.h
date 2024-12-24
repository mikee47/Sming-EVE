/****
 * RomFont.h
 *
 ****/

#pragma once

#include "EVE.h"
#include <Graphics/Object.h>

namespace Graphics::EVE
{
/**
 * @brief Font description
 */
struct FontMetrics {
	uint8_t char_width[128]; ///< Width of each character glyph in pixels
	BitmapFormat format : 8; ///< Bitmap format of glyphs
	uint8_t stride;			 ///< Font line stride
	uint8_t width;			 ///< Font width in pixels
	uint8_t height;			 ///< Font height in pixels
	uint32_t bitmap;		 ///< Starting ROM address for bitmap
};

// As stored in ROM
struct RawFontMetrics {
	uint8_t char_width[128]; ///< Width of each character glyph in pixels
	BitmapFormat format;	 ///< Bitmap format of glyphs
	uint32_t stride;		 ///< Font line stride
	uint32_t width;			 ///< Font width in pixels
	uint32_t height;		 ///< Font height in pixels
	uint32_t bitmap;		 ///< Starting ROM address for bitmap
};

static_assert(sizeof(FontMetrics) == 128 + 4 + 4, "Bad FontMetrics size");

static constexpr unsigned ROM_FONT_MIN = 16;
static constexpr unsigned ROM_FONT_MAX = 34;

bool getRomFont(uint8_t index, FontMetrics& metrics);

class RomTypeFace : public TypeFace
{
public:
	RomTypeFace(uint8_t index);

	FontStyles getStyle() const override
	{
		return 0;
	}

	uint8_t height() const override
	{
		return metrics.height;
	}

	uint8_t descent() const override
	{
		return 1;
	}

	GlyphObject::Metrics getMetrics(char ch) const override;

	std::unique_ptr<GlyphObject> getGlyph(char, const GlyphObject::Options&) const override
	{
		return nullptr;
	}

private:
	FontMetrics metrics{};
};

class RomFont : public Font
{
public:
	RomFont(uint8_t index) : typeface(index)
	{
	}

	String name() const override
	{
		return F("romfont");
	}

	uint16_t height() const override
	{
		return typeface.height();
	}

	const TypeFace* getFace(FontStyles) const override
	{
		return &typeface;
	}

private:
	RomTypeFace typeface;
};

} // namespace Graphics::EVE
