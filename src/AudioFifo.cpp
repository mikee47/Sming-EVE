#include "include/Graphics/EVE/AudioFifo.h"

namespace Graphics::EVE
{
uint32_t align8(uint32_t value)
{
	return value & ~0x07;
}

void AudioFifo::fillBuffer(uint32_t readpos)
{
	size_t chunkSize1;
	size_t chunkSize2;
	writepos %= fifoSize;
	if(readpos < writepos) {
		// wrapped around end of buffer
		chunkSize1 = fifoSize - writepos;
		chunkSize2 = align8(readpos);
	} else {
		// write behind read position
		chunkSize1 = align8(readpos - writepos);
		chunkSize2 = 0;
	}

	if(chunkSize1 == 0) {
		timer.stop();
		display.write8(EVE::REG_PLAYBACK_LOOP, 0);
		return;
	}

	if(chunkSize1 > bufferSize) {
		chunkSize1 = bufferSize;
		chunkSize2 = 0;
	} else if(chunkSize1 + chunkSize2 > bufferSize) {
		chunkSize2 = bufferSize - chunkSize1;
	}

	auto len = source->readBytes(buffer, chunkSize1 + chunkSize2);
	if(size_t(len) < chunkSize1 + chunkSize2) {
		memset(&buffer[len], 0, chunkSize1 + chunkSize2 - size_t(len));
	}
	debug_i("Write(0x%04x, %d), readpos %d...", writepos, len, int(readpos));
	display.write(requests[0], fifoAddress + writepos, buffer, chunkSize1);
	if(chunkSize2) {
		display.write(requests[1], fifoAddress, &buffer[chunkSize1], chunkSize2);
	}

	writepos += len;
}

bool IRAM_ATTR AudioFifo::readPosComplete(HSPI::Request& req)
{
	System.queueCallback(
		[](void* param) {
			auto self = static_cast<AudioFifo*>(param);
			self->fillBuffer(self->readPosRequest.in.data32 - self->fifoAddress);
		},
		req.param);
	return true;
}

void AudioFifo::play(Stream& source, EVE::SampleFormat format, unsigned frequency)
{
	stop();
	this->source = &source;

	writepos = 0;
	fillBuffer(0);

	uint32_t setup[]{
		fifoAddress,	  // START
		fifoSize,		  // LENGTH
		0,				  // READPTR
		frequency,		  // FREQ
		unsigned(format), // FORMAT
		1,				  // LOOP
		1,				  // PLAY
	};
	display.write(EVE::REG_PLAYBACK_START, setup, sizeof(setup));

	display.prepareRead(readPosRequest, EVE::REG_PLAYBACK_READPTR);
	readPosRequest.in.set32(0);
	readPosRequest.setAsync(readPosComplete, this);
	// Timer interval depends on data rate (frequency, format)
	unsigned interval = 1000 * bufferSize / frequency;
	if(format == EVE::SampleFormat::ADPCM) {
		interval *= 2;
	}
	// Refill buffer at around 25% full
	interval = 3 * interval / 4;
	timer.initializeMs(
		interval,
		[](void* param) {
			auto self = static_cast<AudioFifo*>(param);
			self->display.execute(self->readPosRequest);
		},
		this);
	timer.start();
}

void AudioFifo::stop()
{
	timer.stop();
	display.write32(EVE::REG_PLAYBACK_LENGTH, 0);
	display.write8(EVE::REG_PLAYBACK_PLAY, 1);
}

} // namespace Graphics::EVE
