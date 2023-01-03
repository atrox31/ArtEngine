#include "Panel.h"
#include "ArtCore/Graphic/Render.h"

GuiElement::Panel::Panel()
{
	this->_type = GuiElementTemplate::Type::PANEL;
}
void GuiElement::Panel::Render()
{
	GPU_SetLineThickness(2.0f);

	if (_enable_transparent)GPU_SetShapeBlending(true);
	if (_enabled) {
		Gui::Pallet shadow_copy_background = _pallet;
		shadow_copy_background.Background.a = 200;
		Render::DrawRectRoundedFilled(_dimensions.ToGPU_Rect_wh(), 6.0f, shadow_copy_background.Background);
	}
	else {
		Render::DrawRectRoundedFilled(_dimensions.ToGPU_Rect_wh(), 6.0f, _pallet.BackgroundDisable);
	}

	if (_enable_transparent)GPU_SetShapeBlending(false);
	Render::DrawRectRounded(_dimensions.ToGPU_Rect_wh(), 6.0f, _pallet.Frame);
	GPU_SetLineThickness(1.0f);

}