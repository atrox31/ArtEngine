
#include "Gui.h"
#include "Panel.h"

#include "Render.h"

GuiElement::Panel::Panel(int x1, int y1, int x2, int y2)
{
	this->_type = GuiElementTemplate::Type::PANEL;
	this->_x = x1;
	this->_y = y1;
	this->_create_x = _x;
	this->_create_y = _y;
	this->_dimensions = { float(x1),float(y1),float(x2),float(y2) };
}
void GuiElement::Panel::Render()
{
	GPU_SetLineThickness(2.0f);

	if (_enable_transparent)GPU_SetShapeBlending(true);
	if (_enabled) {
		Gui::Pallet shadow_copy_background = *_pallet;
		shadow_copy_background.background.a = 200;
		Render::DrawRectRoundedFilled(_dimensions, 6.0f, shadow_copy_background.background);
	}
	else {
		Render::DrawRectRoundedFilled(_dimensions, 6.0f, _pallet->background_disable);
	}

	if (_enable_transparent)GPU_SetShapeBlending(false);
	Render::DrawRectRounded(_dimensions, 6.0f, _pallet->frame);
	GPU_SetLineThickness(1.0f);

}