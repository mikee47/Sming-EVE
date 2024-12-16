#pragma once

#include <HSPI/MemoryDevice.h>
#include <Graphics/AbstractDisplay.h>
#include "EVE.h"
#include <FlashString/Array.hpp>

namespace Graphics
{
/**
 * @brief Manage instance of an attached FTDI EVE display controller
 *
 * Streaming
 * ---------
 * 
 * There can be multiple active streams:
 * - Media FIFO (x1)
 * - Audio stream (x1)
 * - General transfer(s) to/from graphics RAM
 *
 * Streaming is required where the transfer has to be split into chunks,
 * typically when transferring to/from bulk storage (SD card, WiFi, etc.).
 *
 * SPI transfer completion is signalled in interrupt context, at which point a task
 * callback must be queued to prepare further data for streaming.
 * All streaming operations are handled internally via single task callback.
 *
 * Servicing display interrupts
 * ----------------------------
 *
 * When an interrupt is received the status register must be read before further action
 * can be decided upon. It *may* be possible to queue this request directly from the ISR,
 * with further work to the HSPI driver. Issues to be resolved:
 * - Display interrupt priority MUST be <= SPI interrupt priority, so that when servicing the
 * 	 display interrupt we can be sure it's not interrupting the SPI ISR itself.
 * 	 Since these are all routed CPU interrupts it looks like they're at the same priority.
 * - Implement `HSPI::Controller::queueFromISR`
 * - Only asynchronous requests will be supported
 * - SPI interrupts are enabled if required
 * - Requests must be pre-configured, so they're just queued from ISR.
 *   Methods such as `prepareRead` may block. 
 * - Request setup methods should be in IRAM or force inlined.
 *
 * Status flags can also be checked within the SPI completion ISR callback.
 * Specifically, changes to TOUCH or CONVCOMPLETE can be handled by issuing appropriate
 * pre-prepared requests.
 *
 * Application is responsible for allocating GRAM so MediaFifo class requires position/size parameters.
 * Same for AudioFIFO class.
 * Both of these could use a Stream to obtain data, or maybe just a callback. Or either.
 *
 * Driver manages enabling/disabling interrupts as required.
 * If application doesn't register a callback for, say, touch responses, then
 * that interrupt remains disabled, and the driver ISR ignores the relevant status flags.
 *
 * 
 *
 */
class EveDisplay : public HSPI::MemoryDevice, public AbstractDisplay
{
public:
	struct Config {
		uint16_t hcycle;	 ///< Th Total length of line (visible and non-visible) (in PCLKs)
		uint16_t hoffset;	///< Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles)
		uint16_t hsize;		 ///< Thd Length of visible part of line (in PCLKs) - display width
		uint16_t hsync0;	 ///< Thf Horizontal Front Porch
		uint16_t hsync1;	 ///< Thf + Thp Horizontal Front Porch plus Hsync Pulse width
		uint16_t vcycle;	 ///< Tv Total number of lines (visible and non-visible) (in lines)
		uint16_t voffset;	///< Tvf + Tvp + Tvb Number of non-visible lines (in lines)
		uint16_t vsize;		 ///< Tvd Number of visible lines (in lines) - display height
		uint16_t vsync0;	 ///< Tvf Vertical Front Porch
		uint16_t vsync1;	 ///< Tvf + Tvp Vertical Front Porch plus Vsync Pulse width
		uint8_t swizzle : 4; ///< Output RGB signal swizzle
		uint8_t extclk : 1;  ///< Enable external clock input, default is internal (0)
		uint8_t cspread : 1; ///< Output clock spreading enable
		uint8_t pclkpol : 1; ///< PCLK polarity; false=rising edge, true=rising edge
		uint8_t pclk;		 ///< PCLK frequency divider, 0=disable
	};

	using MemoryDevice::MemoryDevice;

	size_t getMemorySize() const override
	{
		return EVE::EVE_MEMORY_SIZE;
	}

	HSPI::IoModes getSupportedIoModes() const
	{
		using namespace HSPI;
		return IoMode::SPIHD | IoMode::SDI | IoMode::SQI;
	}

	bool begin(HSPI::PinSet pinSet, uint8_t chipSelect, uint32_t spiClockSpeed, const Config& config);

	bool setIoMode(HSPI::IoMode mode) override;

	void prepareWrite(HSPI::Request& req, uint32_t address) override
	{
		using namespace EVE;
		wait(req);
		assert(addrValid(address));
		req.setAddress24(0x800000 | (address % EVE_MEMORY_SIZE));
		req.dummyLen = 0;
	}

	void prepareRead(HSPI::Request& req, uint32_t address) override
	{
		using namespace EVE;
		wait(req);
		assert(addrValid(address));
		req.setAddress24(address % EVE_MEMORY_SIZE);
		if(getIoMode() == HSPI::IoMode::SPIHD) {
			req.dummyLen = 8;
		} else {
			// SDI gets one dummy byte, but for SQI we use two
			req.dummyLen = 4;
		}
	}

	static constexpr bool addrValid(uint32_t addr)
	{
		return addr < EVE::EVE_MEMORY_SIZE;
	}

	void blockWrite(uint32_t addr, const uint32_t* values, unsigned count);

	void blockWrite(uint32_t addr, const FSTR::Array<uint32_t>& values)
	{
		blockWrite(addr, values.data(), values.length());
	}

	/* Device */

	String getName() const override;
	bool setOrientation(Orientation orientation) override;
	Size getNativeSize() const override;
	bool setScrollMargins(uint16_t top, uint16_t bottom) override;
	bool scroll(int16_t y) override;

	/* RenderTarget */

	Size getSize() const override;
	PixelFormat getPixelFormat() const override;
	Surface* createSurface(size_t bufferSize = 0) override;

private:
	void cmdWrite(EVE::HostCommand cmd, uint8_t param);

	Size nativeSize{};
	Orientation orientation{};
};

} // namespace Graphics
