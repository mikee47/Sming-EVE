#include "EVE.h"
#include "Types.h"
#include <Graphics/Colors.h>

namespace Graphics::EVE
{
class DisplayList
{
public:
	DisplayList(unsigned bufferSize = EVE_RAM_DL_SIZE)
		: buffer(new uint8_t[bufferSize]), bufferSize(buffer ? bufferSize : 0)
	{
	}

	void reset()
	{
		size = 0;
	}

	void write(const void* data, unsigned len)
	{
		assert(size + len < bufferSize);
		::memcpy(&buffer[size], data, len);
		size += len;
	}

	void write(const String& s)
	{
		auto len = 1 + s.length();
		write(s.c_str(), len);
		align();
	}

	void align()
	{
		auto off = size % 4;
		if(off == 0) {
			return;
		}
		while(off++ < 4) {
			buffer[size++] = 0;
		}
	}

	const void* get() const
	{
		return buffer.get();
	}

	unsigned length() const
	{
		return size;
	}

	unsigned available() const
	{
		return bufferSize - size;
	}

	unsigned getBufferSize() const
	{
		return bufferSize;
	}

#include "dlcmd.h"

	void color(Color color)
	{
		color_rgb(getRed(color), getGreen(color), getBlue(color));
	}

	void clear_color(Color color)
	{
		clear_color_rgb(getRed(color), getGreen(color), getBlue(color));
	}

	void vertex2f(Point pt)
	{
		vertex2f(pt.x, pt.y);
	}

protected:
	std::unique_ptr<uint8_t[]> buffer;
	unsigned bufferSize;
	unsigned size{0};
};

} // namespace Graphics::EVE
