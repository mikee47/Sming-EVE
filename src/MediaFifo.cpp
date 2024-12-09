#include "include/Graphics/EVE/MediaFifo.h"

namespace Graphics::EVE
{
void MediaFifo::fillBuffer(uint32_t readpos)
{
	size_t space = readpos - writepos - 4;
	if(readpos <= writepos) {
		space += fifoSize;
	}
	size_t bytesWritten{0};
	while(space >= Buffer::size) {
		// debug_i("FIFO read 0x%08x, write 0x%08x", readpos, writepos);
		auto bytesToRead = std::min(Buffer::size, space);
		if(writepos + bytesToRead > fifoSize) {
			bytesToRead = fifoSize - writepos;
		}
		// debug_i("FILE READ %u", space);
		auto& buffer = buffers[bufferIndex];
		auto bytesToWrite = source->readBytes(buffer.data, bytesToRead);
		if(bytesToWrite <= 0) {
			debug_i("file read %d", bytesToWrite);
			timer.stop();
			break;
		}

		// debug_i("@FF read 0x%08x, write 0x%08x, count 0x%08x", readpos, writepos, bytesToWrite);

		display.write(buffer.request, fifoAddress + writepos, buffer.data, bytesToWrite);
		space -= bytesToWrite;
		bytesWritten += bytesToWrite;
		writepos += bytesToWrite;
		if(writepos >= fifoSize) {
			writepos -= fifoSize;
		}
		bufferIndex = 1 - bufferIndex;

		if(size_t(bytesToWrite) < bytesToRead) {
			break;
		}
	}

	// debug_i("FIFO read 0x%08x, write 0x%08x, count 0x%08x", readpos, writestart, bytesWritten);
	if(bytesWritten) {
		display.write32(writeRequest, EVE::REG_MEDIAFIFO_WRITE, writepos);
	}
}

bool IRAM_ATTR MediaFifo::update(HSPI::Request& req)
{
	System.queueCallback(
		[](void* param) {
			auto self = static_cast<MediaFifo*>(param);
			self->fillBuffer(self->readRequest.in.data32);
		},
		req.param);
	return true;
}

bool MediaFifo::begin(Stream& source)
{
	end();
	this->source = &source;

	const uint32_t cmdlist1[]{
		MAKE_COPROC_CMD_WORD(EVE::CMD_MEDIAFIFO), fifoAddress, fifoSize, //
	};
	display.write(EVE::REG_CMDB_WRITE, cmdlist1, sizeof(cmdlist1));
	while(display.read32(EVE::REG_CMDB_SPACE) < 0xffc) {
		//
	}
	writepos = 0;
	fillBuffer(0);

	// Get FIFO read position on a timer to kick off feeding the FIFO
	display.prepareRead(readRequest, EVE::REG_MEDIAFIFO_READ);
	readRequest.out.clear();
	readRequest.in.set32(0);
	readRequest.setAsync(update, this);
	timer.initializeMs<50>(
		[](void* param) {
			auto self = static_cast<MediaFifo*>(param);
			if(!self->readRequest.busy) {
				self->display.execute(self->readRequest);
			}
		},
		this);
	timer.start();
	return true;
}

void MediaFifo::end()
{
	timer.stop();
}

} // namespace Graphics::EVE
