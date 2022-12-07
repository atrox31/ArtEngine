#pragma once
#include "Gui.h"
namespace GuiElement {
class Button final :
	public Gui::GuiElementTemplate
{
public:
	void Render() override;
	Button(int x, int y, std::string text, void(*_call_back)(), Gui::Style style);
private:
};
};