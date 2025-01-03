#include <Graphics/Surface.h>
#include <Graphics/AddressWindow.h>
#include "Display.h"
#include "CommandBuffer.h"

namespace Graphics
{
class EveSurface : public Surface
{
public:
	EveSurface(EveDisplay& display, size_t) : display(display)
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
	void setColor(PackedColor color)
	{
		setColor(unpack(color, PixelFormat::RGB565));
	}

	void setColor(Color color);

	void setPointSize(EVE::PointSize size);
	void setLineWidth(EVE::LineWidth width);

	void setHandle(EVE::Handle handle)
	{
		auto& context = display.context;
		if(handle == context.handle) {
			return;
		}
		context.handle = handle;
		dl.bitmap_handle(handle);
	}

	void vertex(Point pt);
	void vertex(Point pt, EVE::Handle handle, EVE::Cell cell);

	void begin(EVE::GraphicsPrimitive prim)
	{
		auto& context = display.context;
		if(context.prim != prim) {
			dl.begin(prim);
			context.prim = prim;
		}
	}

	void end()
	{
		auto& context = display.context;
		if(context.prim != EVE::GP_NONE) {
			dl.end();
			context.prim = EVE::GP_NONE;
		}
	}

	void drawImage(const Rect& location, const ImageObject& object);
	void drawText(const Rect& location, const TextObject& object);

	EveDisplay& display;
	EVE::DisplayList dl;
	AddressWindow addrWindow;
};

} // namespace Graphics
