#pragma once

#include <HSPI/MemoryDevice.h>
#include "EVE.h"
#include <FlashString/Array.hpp>

namespace Graphics
{
class EveDisplay : public HSPI::MemoryDevice
{
public:
	struct Config {
		uint16_t hcycle;	 ///< Th Total length of line (visible and non-visible) (in PCLKs)
		uint16_t hoffset;	 ///< Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles)
		uint16_t hsize;		 ///< Thd Length of visible part of line (in PCLKs) - display width
		uint16_t hsync0;	 ///< Thf Horizontal Front Porch
		uint16_t hsync1;	 ///< Thf + Thp Horizontal Front Porch plus Hsync Pulse width
		uint16_t vcycle;	 ///< Tv Total number of lines (visible and non-visible) (in lines)
		uint16_t voffset;	 ///< Tvf + Tvp + Tvb Number of non-visible lines (in lines)
		uint16_t vsize;		 ///< Tvd Number of visible lines (in lines) - display height
		uint16_t vsync0;	 ///< Tvf Vertical Front Porch
		uint16_t vsync1;	 ///< Tvf + Tvp Vertical Front Porch plus Vsync Pulse width
		uint8_t swizzle : 4; ///< Output RGB signal swizzle
		uint8_t extclk : 1;	 ///< Enable external clock input, default is internal (0)
		uint8_t cspread : 1; ///< Output clock spreading enable
		uint8_t pclkpol : 1; ///< PCLK polarity; false=rising edge, true=rising edge
		uint8_t pclk;		 ///< PCLK frequency divider, 0=disable
	};

	using MemoryDevice::MemoryDevice;

	size_t getSize() const override
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
		req.dummyLen = 8 / getBitsPerClock();
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

private:
	void cmdWrite(EVE::HostCommand cmd, uint8_t param);
};

} // namespace Graphics
