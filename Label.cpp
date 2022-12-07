
#include "Gui.h"
#include "Label.h"

#include <utility>

#include "Render.h"

GuiElement::Label::Label(int x, int y, std::string text)
{
	this->_type = GuiElementTemplate::Type::LABEL;
	this->_x = x;
	this->_y = y;
	this->_create_x = x;
	this->_create_y = y;
	GuiElementTemplate::SetText(std::move(text));
	this->_dimensions = this->_text_area;
}
void GuiElement::Label::Render()
{
	Render::DrawText(_text, _default_font, { _dimensions.x, _dimensions.y }, _pallet->font);
}