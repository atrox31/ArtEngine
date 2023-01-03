#pragma once
#include "ArtCore/Gui/Gui.h"

namespace GuiElement {
class Button final :
	public Gui::GuiElementTemplate
{
public:
	void Render() override;
	Button();
	Button* SetText(const std::string& text);
	void  SetVariableFromStringEx(const std::string& name, const std::string& value) override;
private:
};
};