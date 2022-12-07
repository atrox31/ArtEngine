#pragma once
#include "Gui.h"
namespace GuiElement {
class Label final :
	public Gui::GuiElementTemplate
{
public:
	void Render() override;
	Label(int x, int y, std::string text);
private:
};
};