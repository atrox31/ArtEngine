
#include "Gui.h"
#include "Image.h"

#include "Render.h"


void GuiElement::Image::Render()
{
	if (_sprite)
		Render::DrawSpriteBox(_sprite, _dimensions);
}

GuiElement::Image::Image(const int x, const int y, Sprite* sprite, void(*call_back)(), const Gui::ImageStyle style)
{
	_x = x;
	_y = y;
	if (sprite == nullptr) {
		_dimensions = { (float)x, (float)y, 0.0f, 0.0f };
	}
	else
	{
		_sprite = sprite;
	}

	this->_call_back = call_back;
	_iStyle = style;
	_sprite = nullptr;
}

void GuiElement::Image::ApplyStyle()
{
	if (_sprite == nullptr) return;
	switch (_iStyle)
	{
	case Gui::ImageStyle::LEFT:
		_dimensions = { (float)(_parent->GetX() + _x),(float)(_parent->GetY() + _y), (float)_sprite->GetWidth(), (float)_sprite->GetHeight() };
		break;
	case Gui::ImageStyle::RIGHT:
		_dimensions = { (float)(_parent->GetDimensions().w + _x - _sprite->GetWidth()),(float)(_parent->GetY() + _y), (float)_sprite->GetWidth(), (float)_sprite->GetHeight() };
		break;
	case Gui::ImageStyle::CENTER:
		_dimensions = { (float)(_parent->GetDimensions().w) * .5f, (float)(_parent->GetDimensions().h * .5f), (float)_sprite->GetWidth(), (float)_sprite->GetHeight() };
		break;
	case Gui::ImageStyle::CENTER_TOP:
		_dimensions = { (float)(_parent->GetDimensions().w) * .5f, (float)(_parent->GetY() + _y), (float)_sprite->GetWidth(), (float)_sprite->GetHeight() };
		break;
	case Gui::ImageStyle::CENTER_BOTTOM:
		_dimensions = { (float)(_parent->GetDimensions().w) * .5f, (float)(_parent->GetDimensions().h - _y - _sprite->GetHeight() * .5f), (float)_sprite->GetWidth(), (float)_sprite->GetHeight() };
		break;
	case Gui::ImageStyle::TOP:
		//TODO ImageStyle::TOP for image in GuiElementTemplate
		break;
	case Gui::ImageStyle::BOTTOM:
		//TODO ImageStyle::BOTTOM for image in GuiElementTemplate
		break;
	case Gui::ImageStyle::FILL:
		_dimensions = { (float)(_parent->GetDimensions().w) * .5f, (float)(_parent->GetDimensions().h * .5f), (float)_parent->GetDimensions().w, (float)_parent->GetDimensions().h };
		break;
	default:
		break;
	}
}

GuiElement::Image::~Image()
{
	delete _sprite;
}

