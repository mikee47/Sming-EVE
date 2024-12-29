#include "include/Graphics/EVE/Surface.h"
#include "include/Graphics/EVE/RomFont.h"
#include <Platform/System.h>

namespace Graphics
{
using namespace EVE;

/* Surface */

Surface::Type EveSurface::getType() const
{
	return Type::Device;
}

Surface::Stat EveSurface::stat() const
{
	return Stat{
		.used = dl.length(),
		.available = dl.available(),
	};
}

Size EveSurface::getSize() const
{
	return display.getSize();
}

PixelFormat EveSurface::getPixelFormat() const
{
	return display.getPixelFormat();
}

bool EveSurface::setAddrWindow(const Rect& rect)
{
	/* TODO
    We're about to update an area of screen with bitmap data (RGB565).
    We should reserve an area of RAMG for the bitmap, probably generate
    the rendering command for it as well.
    */
	addrWindow = rect;
	return true;
}

uint8_t* EveSurface::getBuffer(uint16_t minBytes, uint16_t& available)
{
	/* TODO
    We need a buffer for pixel data.
    Small bitmaps could go in the command buffer but as the data
    is destined for RAMG we'd should probably buffer this separately.
    */
	available = 0;
	return nullptr;
}

void EveSurface::commit(uint16_t length)
{
	//
}

bool EveSurface::blockFill(const void* data, uint16_t length, uint32_t repeat)
{
	return false;
}

bool EveSurface::writeDataBuffer(SharedBuffer& buffer, size_t offset, uint16_t length)
{
	// This data needs to go into RAMG
	return false;
}

bool EveSurface::setPixel(PackedColor color, Point pt)
{
	setColor(color);
	begin(GP_POINTS);
	vertex(pt);
	return true;
}

bool EveSurface::writePixels(const void* data, uint16_t length)
{
	/*
    Standard implementation is fine.
    */
	return Surface::writePixels(data, length);
}

int EveSurface::readDataBuffer(ReadBuffer& buffer, ReadStatus* status, ReadCallback callback, void* param)
{
	/*
    We likely don't need this as it's mainly for blending effects which EVE can do for us.
    */
	return 0;
}

bool EveSurface::render(const Object& object, const Rect& location, std::unique_ptr<Renderer>& renderer)
{
	Color color{Color::White};

	switch(object.kind()) {
	case Object::Kind::Custom:
		break;

	case Object::Kind::Point: {
		auto& obj = static_cast<const PointObject&>(object);
		Point pt = obj.point + location.topLeft();
		if(obj.brush.isSolid()) {
			color = obj.brush.getColor();
		} else {
			Brush brush(obj.brush);
			brush.setPixelFormat(PixelFormat::BGRA32);
			brush.writePixel(Location{location, {}, obj.point}, &color);
		}
		setColor(color);
		setPointSize(16);
		begin(GP_POINTS);
		vertex(pt);
		return true;
	}

	case Object::Kind::Rect: {
		auto& obj = static_cast<const RectObject&>(object);
		setColor(obj.pen.getColor());
		setLineWidth(16 * obj.pen.width);
		begin(EVE::GP_LINE_STRIP);
		auto r = obj.rect + location.topLeft();
		vertex(r.topLeft());
		vertex(r.topRight());
		vertex(r.bottomRight());
		vertex(r.bottomLeft());
		vertex(r.topLeft());
		end();
		return true;
	}

	case Object::Kind::FilledRect: {
		auto& obj = static_cast<const FilledRectObject&>(object);
		// if(obj.blender || obj.radius != 0 || obj.brush.isTransparent()) {
		// 	break;
		// }
		// if(!obj.brush.isSolid() && !isSmall(obj.rect)) {
		// 	break;
		// }
		// return fillSmallRect(obj.brush, location, obj.rect);
		setColor(obj.brush.getColor());
		begin(EVE::GP_RECTS);
		vertex(obj.rect.topLeft());
		vertex(obj.rect.bottomRight());
		return true;
	}

	case Object::Kind::Line: {
		auto& obj = static_cast<const LineObject&>(object);
		setColor(obj.pen.getColor());
		setLineWidth(16 * obj.pen.width);
		begin(EVE::GP_LINES);
		vertex(obj.pt1);
		vertex(obj.pt2);
		return true;
	}

	case Object::Kind::Polyline: {
		auto& obj = static_cast<const PolylineObject&>(object);
		setColor(obj.pen.getColor());
		setLineWidth(16 * obj.pen.width);
		begin(obj.connected ? EVE::GP_LINE_STRIP : EVE::GP_LINES);
		for(unsigned i = 0; i < obj.numPoints; ++i) {
			vertex(obj[i]);
		}
		end();
		return true;
	}

	case Object::Kind::Circle:
		break;

	case Object::Kind::FilledCircle: {
		auto& obj = static_cast<const FilledCircleObject&>(object);
		setColor(obj.brush.getColor());
		begin(EVE::GP_POINTS);
		setPointSize(16 * obj.radius);
		vertex(obj.centre);
		return true;
	}

	case Object::Kind::Ellipse:
		break;

	case Object::Kind::FilledEllipse:
		break;

	case Object::Kind::Arc:
		break;

	case Object::Kind::FilledArc:
		break;

	case Object::Kind::Drawing:
		break;

	case Object::Kind::Image:
		break;

	case Object::Kind::Glyph:
		break;

	case Object::Kind::Text: {
		auto& obj = static_cast<const TextObject&>(object);
		renderText(location, obj);
		return true;
	}

	case Object::Kind::Scene:
		break;

	case Object::Kind::Reference:
		break;

	case Object::Kind::Surface:
		break;

	case Object::Kind::Copy:
		break;

	case Object::Kind::Scroll:
		break;
	}

	return Surface::render(object, location, renderer);
}

void EveSurface::reset()
{
	dl.reset();
	dl.vertex_format(0);
}

bool EveSurface::present(PresentCallback callback, void* param)
{
	/* TODO
    Should transfer any buffered RAMG data, followed by command buffer.
    When transfer has completed we should release RAMG buffer.
    */
	dl.display();
	debug_i("%s(%u)", __FUNCTION__, dl.length());

	display.write(EVE::EVE_RAM_DL, dl.get(), dl.length());
	display.write8(EVE::REG_DLSWAP, EVE::EVE_DLSWAP_FRAME);
	if(callback) {
		System.queueCallback(callback, param);
	}
	return true;
}

bool EveSurface::fillRect(PackedColor color, const Rect& rect)
{
	setColor(color);
	begin(EVE::GP_RECTS);
	vertex(rect.topLeft());
	vertex(rect.bottomRight());
	return true;
}

void EveSurface::setColor(Color color)
{
	auto& context = display.context;
	if(context.color == color) {
		return;
	}

	// debug_i("setColor(0x%08x): %u %u %u / %u", color, getRed(color), getGreen(color), getBlue(color), getAlpha(color));

	PixelBuffer pbnew{color};
	PixelBuffer pbcur{context.color};
	if(pbnew.packed.alpha != pbcur.packed.alpha) {
		dl.color_a(pbnew.packed.alpha);
	}
	if(pbnew.packed.value != pbcur.packed.value) {
		dl.color(color);
	}
	context.color = color;
}

void EveSurface::setPointSize(PointSize size)
{
	auto& context = display.context;
	if(context.pointSize == size) {
		return;
	}
	context.pointSize = size;
	dl.point_size(size * display.getScale());
}

void EveSurface::setLineWidth(LineWidth width)
{
	auto& context = display.context;
	if(context.lineWidth == width) {
		return;
	}
	context.lineWidth = width;
	dl.line_width(width * display.getScale());
}

void EveSurface::vertex(Point pt, EVE::Handle handle, EVE::Cell cell)
{
	auto scale = display.getScale();
	pt.x = pt.x * scale;
	pt.y = pt.y * scale;
	if(unsigned(pt.x) <= 511 && unsigned(pt.y) <= 511) {
		dl.vertex2ii(pt.x, pt.y, handle, cell);
	} else {
		dl.cell(cell);
		dl.vertex2f(pt);
	}
}

void EveSurface::renderText(const Rect& location, const TextObject& object)
{
	Point pos = location.topLeft() + object.bounds.topLeft();
	auto scale = display.getScale();

	const TextObject::FontElement* font{};
	const TextAsset* text{};
	GlyphObject::Options options;
	const EVE::Handle fontHandle{0};

	for(auto& element : object.elements) {
		switch(element.kind) {
		case TextObject::Element::Kind::Text: {
			auto& elem = static_cast<const TextObject::TextElement&>(element);
			text = &elem.text;
			continue;
		}
		case TextObject::Element::Kind::Font: {
			font = static_cast<const TextObject::FontElement*>(&element);
			options.scale = font->scale;
			options.style = font->style;
			if(options.scale.scaleX() <= 1) {
				options.style -= FontStyle::DotMatrix | FontStyle::VLine;
			}
			if(options.scale.scaleY() <= 1) {
				options.style -= FontStyle::DotMatrix | FontStyle::HLine;
			}

			auto slot = display.loadTypeface(font->typeface, options);
			if(!slot) {
				break;
			}
			dl.bitmap_handle(fontHandle);
			dl.bitmap_source(slot->address);
			dl.bitmap_layout(slot->format, slot->stride, slot->height);
			auto fontScaleX = scale * options.scale.scaleX();
			auto fontScaleY = scale * options.scale.scaleY();
			dl.bitmap_size(EVE::BitmapFilter::NEAREST, EVE::BitmapWrap::BORDER, EVE::BitmapWrap::BORDER,
						   fontScaleX * slot->width, fontScaleY * slot->height);
			dl.bitmap_transform_a(1.0 / fontScaleX);
			dl.bitmap_transform_e(1.0 / fontScaleY);
			break;
		}
		case TextObject::Element::Kind::Color: {
			auto& elem = static_cast<const TextObject::ColorElement&>(element);
			options.fore = elem.fore;
			options.back = elem.back;
			setColor(options.fore.getColor());
			break;
		}
		case TextObject::Element::Kind::Run: {
			if(!font) {
				break;
			}

			auto& run = static_cast<const TextObject::RunElement&>(element);
			auto ymax = location.bottom();

			int16_t xo = pos.x + run.pos.x;
			const int16_t yo = pos.y + run.pos.y;

			// Skip any runs which fall outside the destination area
			if(yo >= ymax) {
				break;
			}

			begin(EVE::GP_BITMAPS);

			char textbuf[run.length];
			text->read(run.offset, textbuf, run.length);
			for(uint8_t ch : textbuf) {
				auto charMetrics = font->typeface.getMetrics(ch);
				int16_t x = xo + options.scale.scaleX(charMetrics.xOffset);
				int16_t y = yo + options.scale.scaleY(font->typeface.baseline() + charMetrics.yOffset);
				uint8_t cell = font->typeface.getCharIndex(ch);
				vertex({x, y}, fontHandle, cell);
				xo += options.scale.scaleX(charMetrics.advance);
			}

			// Decorate run

			auto line = [&](int16_t line) {
				setLineWidth(8);
				begin(EVE::GP_LINES);
				int16_t x = pos.x + run.pos.x;
				int16_t y = yo + line;
				vertex({x, y});
				vertex(Point(x + run.width, y));
			};

			auto baseline = options.scale.scaleY(font->typeface.baseline());
			if(font->style[FontStyle::Underscore]) {
				line(baseline + 2);
			} else if(font->style[FontStyle::DoubleUnderscore]) {
				line(baseline + 2);
				line(baseline + 4);
			}
			if(font->style[FontStyle::Overscore]) {
				line(0);
			} else if(font->style[FontStyle::DoubleOverscore]) {
				line(0);
				line(-2);
			}
			uint8_t c = options.scale.scaleY(font->typeface.height()) / 2;
			if(font->style[FontStyle::Strikeout]) {
				line(c);
			} else if(font->style[FontStyle::DoubleStrikeout]) {
				line(c - 1);
				line(c + 2);
			}

			break;
		}
		}
	}
}

}; // namespace Graphics
