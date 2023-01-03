#pragma once
#include "ArtCore/Gui/Gui.h"

namespace GuiElement {
class Label final :
	public Gui::GuiElementTemplate
{
public:
	void Render() override;
	Label();
	void  SetVariableFromStringEx(const std::string& name, const std::string& value) override;
private:
};
};