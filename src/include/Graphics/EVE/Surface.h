#include <Graphics/Surface.h>
#include <Graphics/AddressWindow.h>
#include "Display.h"
#include "CommandBuffer.h"

namespace Graphics
{
class EveSurface : public Surface
{
public:
	EveSurface(EveDisplay& display, size_t bufferSize) : display(display)
	{
		reset();
	}

	/* Meta */

	void write(MetaWriter&) const override
	{
	}

	/* Surface */

	Type getType() const override;
	Stat stat() const override;
	Size getSize() const override;
	PixelFormat getPixelFormat() const override;
	bool setAddrWindow(const Rect& rect) override;
	uint8_t* getBuffer(uint16_t minBytes, uint16_t& available) override;
	void commit(uint16_t length) override;
	bool blockFill(const void* data, uint16_t length, uint32_t repeat) override;
	bool writeDataBuffer(SharedBuffer& buffer, size_t offset, uint16_t length) override;
	bool setPixel(PackedColor color, Point pt) override;
	bool writePixels(const void* data, uint16_t length) override;
	int readDataBuffer(ReadBuffer& buffer, ReadStatus* status = nullptr, ReadCallback callback = nullptr,
					   void* param = nullptr) override;
	using Surface::render;
	bool render(const Object& object, const Rect& location, std::unique_ptr<Renderer>& renderer) override;
	void reset() override;
	bool present(PresentCallback callback = nullptr, void* param = nullptr) override;
	bool fillRect(PackedColor color, const Rect& rect) override;

private:
	struct Context {
		EVE::GraphicsPrimitive prim = EVE::GP_NONE;
		Color color{Color::White};
		uint16_t pointSize{16};
	};

	void setColor(PackedColor color)
	{
		setColor(unpack(color, PixelFormat::RGB565));
	}

	void setColor(Color color);

	void setPointSize(uint16_t size)
	{
		if(context.pointSize != size) {
			cmd.point_size(size);
			context.pointSize = size;
		}
	}

	void vertex(Point pt)
	{
		auto scale = display.getScale();
		pt.x = pt.x * scale;
		pt.y = pt.y * scale;
		cmd.vertex2f(pt);
	}

	void begin(EVE::GraphicsPrimitive prim)
	{
		if(context.prim != prim) {
			cmd.begin(prim);
			context.prim = prim;
		}
	}

	void end()
	{
		if(context.prim != EVE::GP_NONE) {
			cmd.end();
			context.prim = EVE::GP_NONE;
		}
	}

	EveDisplay& display;
	EVE::CommandBuffer cmd;
	AddressWindow addrWindow;

	Context context{};
};

} // namespace Graphics
