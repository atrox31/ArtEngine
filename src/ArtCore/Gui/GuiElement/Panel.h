#pragma once
#include "ArtCore/Gui/Gui.h"

namespace GuiElement {
class Panel final :
	public Gui::GuiElementTemplate
{
public:
	void Render() override;
	Panel();
private:
};

};