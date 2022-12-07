#pragma once
#include "Gui.h"
namespace GuiElement {
class Panel final :
	public Gui::GuiElementTemplate
{
public:
	void Render() override;
	Panel(int x1, int y1, int x2, int y2);
private:
};

};