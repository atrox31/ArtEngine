#include "Label.h"

#include "ArtCore/Graphic/Render.h"


GuiElement::Label::Label()
{
	this->_type = GuiElementTemplate::Type::LABEL;
}

void GuiElement::Label::SetVariableFromStringEx(const std::string& name, const std::string& value)
{
	/*
	 new Variable(Variable.VariableType.VTypeString, "Text", "Label")
	 */
	if (name == "Text")
	{
		SetText(value); return;
	}
	Debug::WARNING("[GuiElement::Label::SetVariableFromStringEx]: '" + name + "' not found");
}

void GuiElement::Label::Render()
{
	Render::DrawText(_text, _default_font, { _dimensions.x, _dimensions.y }, _pallet.Font);
}
