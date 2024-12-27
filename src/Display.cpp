#include "include/Graphics/EVE/Display.h"
#include "include/Graphics/EVE/Surface.h"
#include "include/Graphics/EVE/RomFont.h"
#include <Clock.h>
#include <Platform/Timers.h>

namespace Graphics
{
using namespace EVE;

bool EveDisplay::begin(HSPI::PinSet pinSet, uint8_t chipSelect, uint32_t spiClockSpeed, const Config& config)
{
	const uint32_t initClockSpeed{8'000'000};
	if(!MemoryDevice::begin(pinSet, chipSelect, std::min(initClockSpeed, spiClockSpeed))) {
		return false;
	}
	setBitOrder(MSBFIRST);
	setClockMode(HSPI::ClockMode::mode0);
	MemoryDevice::setIoMode(HSPI::IoMode::SPIHD);

	// Setup internal/external clock
	cmdWrite(config.extclk ? HostCommand::CLKEXT : HostCommand::CLKINT, 0);

	cmdWrite(HostCommand::RST_PULSE, 0);
	cmdWrite(HostCommand::ACTIVE, 0);

	// Set DISP, GPIO2, GPIO3 to output
	write16(REG_GPIOX_DIR, 0x80C0);

	// Wait for chip ready
	OneShotFastMs timer;
	timer.reset<400>();
	uint8_t b;
	while((b = read8(REG_ID)) != 0x7c) {
		if(b != 0 && b != 0xff) {
			debug_i("[EVE] GOT 0x%02x", b);
		}
		if(timer.expired()) {
			debug_e("[EVE] Timeout waiting for ready");
			return false;
		}
	}

	// Wait for reset complete
	timer.reset<50>();
	while((read8(REG_CPURESET) & 0x07) != 0) {
		if(timer.expired()) {
			return false;
		}
	}

	/* Initialize display parameters */
	write16(REG_HSIZE, config.hsize);
	write16(REG_HCYCLE, config.hcycle);
	write16(REG_HOFFSET, config.hoffset);
	write16(REG_HSYNC0, config.hsync0);
	write16(REG_HSYNC1, config.hsync1);
	write16(REG_VSIZE, config.vsize);
	write16(REG_VCYCLE, config.vcycle);
	write16(REG_VOFFSET, config.voffset);
	write16(REG_VSYNC0, config.vsync0);
	write16(REG_VSYNC1, config.vsync1);
	write8(REG_SWIZZLE, config.swizzle);
	write8(REG_PCLK_POL, config.pclkpol);
	write8(REG_CSPREAD, config.cspread);

	/* Configure Touch */
	write8(REG_TOUCH_MODE, EVE_TMODE_CONTINUOUS);
	write8(REG_TOUCH_OVERSAMPLE, 15);

	/* disable Audio for now */
	write8(REG_VOL_PB, 0);	// turn recorded audio volume down, reset-default is 0xff
	write8(REG_VOL_SOUND, 0); // turn synthesizer volume down, reset-default is 0xff
	write16(REG_SOUND, unsigned(Sound::MUTE));

	/* Create initial display list */
	const uint32_t dl[]{
		// As recommended in datasheet - these can be non-zero on boot
		BITMAP_LAYOUT_H(0, 0),
		BITMAP_SIZE_H(0, 0),
		// Clear display to black
		CLEAR_COLOR_RGB(0, 0, 0),
		CLEAR(true, true, true),
		DISPLAY(),
	};
	blockWrite(EVE_RAM_DL, dl, ARRAY_SIZE(dl));
	write32(REG_DLSWAP, EVE_DLSWAP_FRAME);

	/* Enable pixel clock */
	write8(REG_PCLK, config.pclk);

	/* Turn on backlight */
#if defined(EVE_BACKLIGHT_FREQ)
	write16(REG_PWM_HZ, EVE_BACKLIGHT_FREQ); /* set backlight frequency to configured value */
#else
	write16(REG_PWM_HZ, 10000);
#endif

#if defined(EVE_BACKLIGHT_PWM)
	write8(REG_PWM_DUTY, EVE_BACKLIGHT_PWM); /* set backlight pwm to user requested level */
#else
	write8(REG_PWM_DUTY, 0x20); // 25%
#endif

	// enable LCD DISP signal
	write8(REG_GPIO, 0x80);

	setClockSpeed(spiClockSpeed);

	nativeSize = Size{config.hsize, config.vsize};

	return true;
}

bool EveDisplay::setIoMode(HSPI::IoMode mode)
{
	auto oldMode = getIoMode();
	if(oldMode == mode) {
		return true;
	}

	uint8_t spiWidth;
	switch(mode) {
	case HSPI::IoMode::SDI:
		spiWidth = 0x01;
		break;
	case HSPI::IoMode::SQI:
		spiWidth = 0x04 | 0x02; // Add extra dummy byte for reads
		break;
	default:
		spiWidth = 0x00;
	}

	if(!isSupported(mode)) {
		debug_e("setIoMode(): Mode %u invalid", unsigned(mode));
		return false;
	}

	write8(REG_SPI_WIDTH, spiWidth);
	return MemoryDevice::setIoMode(mode);
}

void EveDisplay::cmdWrite(EVE::HostCommand cmd, uint8_t param)
{
	HSPI::Request req;
	req.setCommand8(uint8_t(cmd));
	req.out.set16(param);
	execute(req);
}

void EveDisplay::blockWrite(uint32_t addr, const uint32_t* values, unsigned count)
{
	assert(addrValid(addr) && addrValid(addr + count * 4 - 1));
	HSPI::Request req;
	req.setAddress24(0x800000 | (addr % EVE_MEMORY_SIZE));
	req.out.set(values, count * sizeof(uint32_t));
	execute(req);
}

/* Device */

String EveDisplay::getName() const
{
	return "EVE";
}

bool EveDisplay::setOrientation(Orientation orientation)
{
	/*
		bit0 = invert, bit2 = portrait, bit3 = mirrored.
		reset default value is 0x0 - not inverted, landscape, not mirrored.

		TODO: Touch is not affected by this transformation
	*/
	uint8_t value;
	switch(orientation) {
	case Orientation::deg0:
		value = 0;
		break;
	case Orientation::deg90:
		value = 3;
		break;
	case Orientation::deg180:
		value = 1;
		break;
	case Orientation::deg270:
		value = 2;
		break;
	}
	return false;

	write8(REG_ROTATE, value);
	this->orientation = orientation;
	return true;
}

Size EveDisplay::getNativeSize() const
{
	return nativeSize;
}

bool EveDisplay::setScrollMargins(uint16_t top, uint16_t bottom)
{
	// TODO
	return false;
}

bool EveDisplay::scroll(int16_t y)
{
	// TODO
	return false;
}

/* RenderTarget */

Size EveDisplay::getSize() const
{
	auto size = nativeSize;
	size.w = size.w / scale;
	size.h = size.h / scale;
	return size;
}

PixelFormat EveDisplay::getPixelFormat() const
{
	// Wide range of pixel formats supported, but start with this one
	return PixelFormat::RGB565;
}

Surface* EveDisplay::createSurface(size_t bufferSize)
{
	return new EveSurface(*this, bufferSize);
}

/* EveDisplay */

EveDisplay::BitmapSlot* EveDisplay::findBitmapSlot(AssetID id)
{
	for(auto& slot : bitmaps) {
		if(slot.id == id) {
			return &slot;
		}
	}

	return nullptr;
}

const EveDisplay::BitmapSlot* EveDisplay::getBitmapSlot(AssetID id) const
{
	auto slot = const_cast<EveDisplay*>(this)->findBitmapSlot(id);
	if(!slot) {
		debug_w("[EVE] Slot not found for id %u", id);
	}
	return slot;
}

EveDisplay::BitmapSlot* EveDisplay::getFreeSlot()
{
	auto slot = findBitmapSlot(0);
	if(!slot) {
		debug_w("[EVE] No free slots");
	}
	return slot;
}

const EveDisplay::BitmapSlot* EveDisplay::loadTypeface(const TypeFace& typeface, const GlyphOptions& options)
{
	auto slot = findBitmapSlot(typeface.id());
	if(slot) {
		// Already loaded
		return slot;
	}

	slot = getFreeSlot();
	if(slot == nullptr) {
		return nullptr;
	}

	auto romfont = static_cast<const FontMetrics*>(typeface.getDeviceData());
	if(romfont) {
		// A ROM font - doesn't require loading
		debug_i("[EVE] Loaded ROM font %u", typeface.id());
		*slot = BitmapSlot{
			.address = romfont->bitmap,
			.format = romfont->format(),
			.id = typeface.id(),
			.stride = romfont->stride,
			.width = romfont->width,
			.height = romfont->height,
		};
		return slot;
	}

	/*
	To conserve storage glyph bitmaps differ in size, which requires extra setup for each character.
	We can expand the glyphs to a consistent size for more efficient display list construction.
	*/
	auto loadAddress = nextRamAddress;
	// Determine minimum bounding rect for all glyphs
	uint8_t alpha{};
	uint8_t width{0};
	uint8_t height{0};
	unsigned maxPixels{0};
	for(unsigned blockIndex = 0;; ++blockIndex) {
		GlyphBlock block = typeface.getBlock(blockIndex);
		if(!block.length) {
			break;
		}
		uint16_t ch = block.codePoint;
		while(block.length--) {
			auto metrics = typeface.getMetrics(ch++);
			alpha = metrics.alpha;
			width = std::max(width, metrics.width);
			height = std::max(height, metrics.height);
			maxPixels = std::max(maxPixels, unsigned(metrics.width) * metrics.height);
		}
	}

	const uint8_t bitsPerPixel = 1 << alpha;
	const uint8_t pixelsPerByte = 8 / bitsPerPixel;
	const uint8_t stride = (width + pixelsPerByte - 1) / pixelsPerByte;

	const unsigned bufSize = stride * height;
	debug_i("bufSize %u, yadv %u, stride %u, size (%u, %u)", bufSize, typeface.height(), stride, width, height);

	auto buffer = std::make_unique<uint8_t[]>(bufSize);
	auto glyphDataSize = (maxPixels + pixelsPerByte - 1) / pixelsPerByte;
	auto glyphData = std::make_unique<uint8_t[]>(glyphDataSize);
	auto addr = loadAddress;
	for(unsigned blockIndex = 0;; ++blockIndex) {
		GlyphBlock block = typeface.getBlock(blockIndex);
		if(!block.length) {
			break;
		}
		auto ch = block.codePoint;
		for(; block.length--; ++ch, addr += bufSize) {
			auto glyph = typeface.getGlyph(ch, options);
			auto& metrics = glyph->getMetrics();
			memset(buffer.get(), 0, bufSize);
			glyph->readRaw(glyphData.get(), glyphDataSize);
			uint8_t* src = glyphData.get();
			auto dstrow = buffer.get();
			if(bitsPerPixel == 8) {
				for(unsigned y = 0; y < metrics.height; ++y, src += metrics.width, dstrow += stride) {
					memcpy(dstrow, src, metrics.width);
				}
			} else {
				uint8_t mask = (1 << bitsPerPixel) - 1;
				uint8_t srcbyte = 0;
				uint8_t srcshift = 0;
				for(unsigned y = 0; y < metrics.height; ++y, dstrow += stride) {
					auto dst = dstrow;
					uint8_t dstbyte{0};
					uint8_t dstshift = 8;
					for(unsigned x = 0; x < metrics.width; ++x) {
						if(srcshift == 0) {
							srcbyte = *src++;
							srcshift = 8;
						}
						srcshift -= bitsPerPixel;
						dstshift -= bitsPerPixel;
						dstbyte |= ((srcbyte >> srcshift) & mask) << dstshift;
						if(dstshift == 0) {
							*dst++ = dstbyte;
							dstbyte = 0;
							dstshift = 8;
						}
					}
					if(dstshift != 8) {
						*dst = dstbyte;
					}
				}
			}

			write(addr, buffer.get(), bufSize);
		}
	}

	nextRamAddress = addr;

	unsigned bmSize = addr - loadAddress;
	debug_i("Loaded face %u @ 0x%06x, %u bytes, bitsPerPixel %u", typeface.id(), loadAddress, bmSize, bitsPerPixel);

	const EVE::BitmapFormat formats[]{
		EVE::BMF_L1,
		EVE::BMF_L2,
		EVE::BMF_L4,
		EVE::BMF_L8,
	};

	*slot = BitmapSlot{
		.address = loadAddress,
		.format = formats[alpha],
		.id = typeface.id(),
		.stride = stride,
		.width = width,
		.height = height,
	};

	return slot;
}

const EveDisplay::BitmapSlot* EveDisplay::loadTypeface(const Font& font, uint8_t typefaceIndex,
													   const GlyphOptions& options)
{
	auto typeface = font.getFace(typefaceIndex);
	if(!typeface) {
		return nullptr;
	}
	return loadTypeface(*typeface, options);
}

} // namespace Graphics
