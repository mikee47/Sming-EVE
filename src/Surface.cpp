#include "include/Graphics/EVE/Surface.h"
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
		.used = cmd.length(),
		.available = EVE_CMDFIFO_SIZE - cmd.length(),
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
		begin(GP_POINTS);
		vertex(pt);
		return true;
	}

	case Object::Kind::Rect: {
		auto& obj = static_cast<const RectObject&>(object);
		color = obj.pen.getColor();
		setColor(color);
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
		color = obj.brush.getColor();
		setColor(color);
		begin(EVE::GP_RECTS);
		vertex(obj.rect.topLeft());
		vertex(obj.rect.bottomRight());
		return true;
	}

	case Object::Kind::Line: {
		auto& obj = static_cast<const LineObject&>(object);
		color = obj.pen.getColor();
		setColor(color);
		begin(EVE::GP_LINES);
		vertex(obj.pt1);
		vertex(obj.pt2);
		return true;
	}

	case Object::Kind::Polyline: {
		auto& obj = static_cast<const PolylineObject&>(object);
		color = obj.pen.getColor();
		setColor(color);
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
		color = obj.brush.getColor();
		setColor(color);
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
	cmd.reset();
	cmd.dlstart();
	cmd.vertex_format(0);
}

bool EveSurface::present(PresentCallback callback, void* param)
{
	/* TODO
    Should transfer any buffered RAMG data, followed by command buffer.
    When transfer has completed we should release RAMG buffer.
    */
	cmd.display();
	cmd.swap();
	debug_i("%s(%u)", __FUNCTION__, cmd.length());

	display.write(EVE::REG_CMDB_WRITE, cmd.get(), cmd.length());
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
	if(context.color == color) {
		return;
	}

	// debug_i("setColor(0x%08x): %u %u %u / %u", color, getRed(color), getGreen(color), getBlue(color), getAlpha(color));

	PixelBuffer pbnew{color};
	PixelBuffer pbcur{context.color};
	if(pbnew.packed.alpha != pbcur.packed.alpha) {
		cmd.color_a(pbnew.packed.alpha);
	}
	if(pbnew.packed.value != pbcur.packed.value) {
		cmd.color(color);
	}
	context.color = color;
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
			cmd.romfont(fontHandle, font->typeface.id());
			auto h = font->typeface.height();
			auto fontScaleX = scale * options.scale.scaleX();
			auto fontScaleY = scale * options.scale.scaleY();
			// debug_i("fontScale (%d, %d), scale 0x%08x", fontScaleX, fontScaleY, scale.value);
			cmd.bitmap_size(EVE::BitmapFilter::NEAREST, EVE::BitmapWrap::BORDER, EVE::BitmapWrap::BORDER,
							fontScaleX * h, fontScaleY * h);
			cmd.loadidentity();
			cmd.scale(fontScaleX, fontScaleY);
			cmd.setmatrix();
			break;
		}
		case TextObject::Element::Kind::Color: {
			auto& elem = static_cast<const TextObject::ColorElement&>(element);
			options.fore = elem.fore;
			options.back = elem.back;
			cmd.color(options.fore.getColor());
			break;
		}
		case TextObject::Element::Kind::Run: {
			auto& run = static_cast<const TextObject::RunElement&>(element);
			auto ymax = location.bottom();

			// debug_i("RUN location (%s), pos (%s), run.pos (%s)", location.toString().c_str(), pos.toString().c_str(),
			// 		run.pos.toString().c_str());

			int16_t x = pos.x + run.pos.x;
			int16_t y = pos.y + run.pos.y;

			// Skip any runs which fall outside the destination area
			if(y >= ymax) {
				break;
			}

			cmd.begin(EVE::GP_BITMAPS);

			uint8_t advdiff{0};
			for(uint16_t charIndex = 0; y < ymax && charIndex < run.length; ++charIndex) {
				char ch = text->read(run.offset + charIndex);

				auto charMetrics = font->typeface.getMetrics(ch);

				x -= advdiff;
				if(x + charMetrics.xOffset < 0) {
					debug_e("[[FONT X2]] %u, %d", x, charMetrics.xOffset);
					x = -charMetrics.xOffset;
				}

				cmd.cell(ch);
				vertex({x, y});

				auto x1 = x + charMetrics.advance * options.scale.scaleX();
				auto x2 = x + (charMetrics.xOffset + charMetrics.width) * options.scale.scaleX();
				if(x1 >= x2) {
					advdiff = 0;
					x = x1;
				} else {
					advdiff = x2 - x1;
					x = x2;
				}
			}

			// Decorate run

			auto line = [&](int8_t line) {
				// Typeface may not  have room for this
				if(line >= font->typeface.height()) {
					// return;
				}
				// TODO
				// memset(&data[x + size.w * line], 0xff, charMetrics.advance);
				cmd.begin(EVE::GP_LINES);
				int16_t x1 = pos.x + run.pos.x;
				int16_t y = pos.y + run.pos.y + line;
				int16_t x2 = x1 + run.width;
				vertex({x1, y});
				vertex({x2, y});
			};

			auto baseline = font->typeface.baseline();
			if(font->style[FontStyle::Underscore]) {
				line(baseline + 1);
			}
			if(font->style[FontStyle::DoubleUnderscore]) {
				line(baseline + 1);
				line(baseline + 3);
			}
			if(font->style[FontStyle::Overscore]) {
				line(1);
			}
			if(font->style[FontStyle::DoubleOverscore]) {
				line(1);
				line(3);
			}
			if(font->style[FontStyle::Strikeout]) {
				line(font->typeface.height() / 2);
			}
			if(font->style[FontStyle::DoubleStrikeout]) {
				uint8_t c = font->typeface.height() / 2;
				line(c - 1);
				line(c + 2);
			}

			break;
		}
		}
	}
}

}; // namespace Graphics
