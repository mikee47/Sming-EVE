#include "Display.h"
#include <Stream.h>
#include <SimpleTimer.h>

namespace Graphics::EVE
{
class AudioFifo
{
public:
	AudioFifo(EveDisplay& display, uint32_t address, uint32_t size)
		: display(display), fifoAddress(address), fifoSize(size)
	{
	}

	void play(Stream& source, EVE::SampleFormat format, unsigned frequency);

	void stop();

private:
	EveDisplay& display;
	uint32_t fifoAddress;
	uint32_t fifoSize;
	static constexpr uint32_t bufferSize{4092};
	Stream* source{};
	uint8_t buffer[bufferSize];
	HSPI::Request requests[2];
	HSPI::Request readPosRequest;
	uint32_t writepos;
	SimpleTimer timer;

	void fillBuffer(uint32_t readpos);
	static bool IRAM_ATTR readPosComplete(HSPI::Request& req);
};

} // namespace Graphics::EVE
