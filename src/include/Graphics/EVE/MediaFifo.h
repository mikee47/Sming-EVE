#include "Display.h"
#include <Stream.h>
#include <SimpleTimer.h>

namespace Graphics::EVE
{
/*
 * @brief Class to manage streaming media FIFO.
*/
class MediaFifo
{
public:
	/**
     * @brief Construct a streaming media FIFO instance
     * @param address CMD_PLAYVIDEO uses RAM_G for bitmap (2 * 800 * 480) and sound (32K)
     *                so first safe address is 0xc3800
     * @param size
     *
     * Both address and size must be uint32 aligned.
     */
	MediaFifo(EveDisplay& display, uint32_t address, uint32_t size)
		: display(display), fifoAddress(address), fifoSize(size)
	{
	}

	bool begin(Stream& source);
	void end();

private:
	void fillBuffer(uint32_t readpos);
	static bool update(HSPI::Request& req);

	EveDisplay& display;
	uint32_t fifoAddress;
	uint32_t fifoSize;
	Stream* source{};

	SimpleTimer timer;
	uint32_t writepos;
	struct Buffer {
		static const size_t size{4092}; // SPI_MAX_DMA_LEN
		uint8_t data[size];
		HSPI::Request request;
	};
	Buffer buffers[2];
	HSPI::Request readRequest;
	HSPI::Request writeRequest;
	uint8_t bufferIndex{0};
};

} // namespace Graphics::EVE
