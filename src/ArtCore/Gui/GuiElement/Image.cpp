#include "Image.h"

#include "ArtCore/Graphic/Render.h"
#include "ArtCore/System/Core.h"
#include "ArtCore/System/AssetManager.h"


void GuiElement::Image::Render()
{
	if (_sprite)
		Render::DrawSpriteBox(_sprite, _dimensions.ToGPU_Rect_wh());
}

GuiElement::Image::Image()
{
	_iStyle = (Gui::ImageStyle)1;
	_sprite = nullptr;
	_type = Type::IMAGE;
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
		_dimensions = { (float)(_parent->GetDimensions().W + _x - _sprite->GetWidth()),(float)(_parent->GetY() + _y), (float)_sprite->GetWidth(), (float)_sprite->GetHeight() };
		break;
	case Gui::ImageStyle::CENTER:
		_dimensions = { (float)(_parent->GetDimensions().W) * .5f, (float)(_parent->GetDimensions().H * .5f), (float)_sprite->GetWidth(), (float)_sprite->GetHeight() };
		break;
	case Gui::ImageStyle::CENTER_TOP:
		_dimensions = { (float)(_parent->GetDimensions().W) * .5f, (float)(_parent->GetY() + _y), (float)_sprite->GetWidth(), (float)_sprite->GetHeight() };
		break;
	case Gui::ImageStyle::CENTER_BOTTOM:
		_dimensions = { (float)(_parent->GetDimensions().W) * .5f, (float)(_parent->GetDimensions().H - _y - _sprite->GetHeight() * .5f), (float)_sprite->GetWidth(), (float)_sprite->GetHeight() };
		break;
	case Gui::ImageStyle::TOP:
		//TODO ImageStyle::TOP for image in GuiElementTemplate
		break;
	case Gui::ImageStyle::BOTTOM:
		//TODO ImageStyle::BOTTOM for image in GuiElementTemplate
		break;
	case Gui::ImageStyle::FILL:
		_dimensions = { (float)(_parent->GetDimensions().W) * .5f, (float)(_parent->GetDimensions().H * .5f), (float)_parent->GetDimensions().W, (float)_parent->GetDimensions().H };
		break;
	default:
		break;
	}
}

GuiElement::Image::~Image()
{
	delete _sprite;
}

void GuiElement::Image::SetVariableFromStringEx(const std::string& name, const std::string& value)
{
	/*
	  new Variable(Variable.VariableType.VTypeSprite, "Sprite", "null")
	 
	 */
	if (name == "Sprite")
	{
		_sprite = Core::GetAssetManager()->GetSprite(value);
	}
	Console::WriteLine("[GuiElement::Image::SetVariableFromStringEx]: '" + name + "' not found");
}

