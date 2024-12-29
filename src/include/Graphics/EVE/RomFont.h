/****
 * RomFont.h
 *
 ****/

#pragma once

#include "EVE.h"
#include "Types.h"
#include <Graphics/Object.h>

namespace Graphics::EVE
{
/**
 * @brief Font description
 */
struct FontMetrics {
	uint8_t char_width[128]; ///< Width of each character glyph in pixels
	uint8_t alpha : 2;		 ///< log2(Bits per pixel)
	uint8_t stride;			 ///< Font line stride
	uint8_t width;			 ///< Font width in pixels
	uint8_t height;			 ///< Font height in pixels
	uint8_t descent;		 ///< Distance to baseline from bottom of glyph
	uint8_t firstchar;
	uint8_t numchars;
	uint8_t reserved;
	uint32_t bitmap; ///< Starting ROM address for bitmap

	BitmapFormat format() const
	{
		switch(alpha) {
		case 0:
			return BMF_L1;
		case 1:
			return BMF_L2;
		case 2:
			return BMF_L4;
		case 3:
			return BMF_L8;
		}
	}
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

static constexpr unsigned ROM_FONT_MIN = 16;
static constexpr unsigned ROM_FONT_MAX = 34;

bool getRomFont(uint8_t index, FontMetrics& metrics);

class RomTypeFace : public TypeFace
{
public:
	RomTypeFace(uint8_t index);

	const void* getDeviceData() const override
	{
		return &slot;
	}

	GlyphBlock getBlock(unsigned index) const;

	FontStyles getStyle() const override
	{
		return 0;
	}

	uint8_t height() const override
	{
		return slot.height;
	}

	uint8_t descent() const override
	{
		return mdescent;
	}

	GlyphObject::Metrics getMetrics(uint16_t ch) const override;

	std::unique_ptr<GlyphObject> getGlyph(uint16_t, const GlyphObject::Options&) const override
	{
		return nullptr;
	}

private:
	BitmapSlot slot{};
	uint8_t index:6;
	uint8_t alpha: 2;
	uint8_t firstchar{};
	uint8_t numchars{};
	uint8_t mdescent;
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
