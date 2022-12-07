
#include "Gui.h"
#include "Button.h"

#include "Core.h"
#include "Render.h"

GuiElement::Button::Button(const int x, const int y, const std::string text, void(*call_back)(), Gui::Style style)
{
	this->_type = GuiElementTemplate::Type::BUTTON;
	this->_create_x = x;
	this->_x = x;
	this->_create_y = y;
	this->_y = y;
	this->_text = text;
	this->_call_back = call_back;
	this->_style = style;

	this->_dimensions = FC_GetBounds(Gui::GlobalFont, 0, 0, FC_ALIGN_LEFT, FC_MakeScale(1.0f, 1.0f), text.c_str());
	this->_dimensions.w += 12;
	this->_dimensions.h += 2;

}
void GuiElement::Button::Render()
{
	GPU_SetLineThickness(2.0f);
	if (_enabled) {
		if (_focus) {
			Render::DrawRectRoundedFilled(_dimensions, 2.0f, _pallet->active);
			if (Core::GetInstance()->Mouse.LeftPressed) {
				GPU_SetShapeBlending(true);
				Render::DrawRectRoundedFilled(_dimensions, 2.0f, { 0,0,0,100 });
				const GPU_Rect frame_border = {
				_dimensions.x + 2.0f,
				_dimensions.y + 2.0f,
				_dimensions.w - 4.0f,
				_dimensions.h - 4.0f
				};
				Render::DrawRectRounded(frame_border, 2.0f, { 0,0,0,100 });
				GPU_SetShapeBlending(false);
			}
		}
		else {
			Render::DrawRectRoundedFilled(_dimensions, 2.0f, _pallet->background);
		}
	}
	else {
		Render::DrawRectRoundedFilled(_dimensions, 2.0f, _pallet->background_disable);
	}
	Render::DrawRectRounded(_dimensions, 2.0f, _pallet->frame);
	if (_focus) {
		if (Core::GetInstance()->Mouse.LeftPressed) {
			GPU_SetShapeBlending(true);
			GPU_SetLineThickness(4.0f);
			Render::DrawRectRoundedFilled(_dimensions, 2.0f, { 0,0,0,100 });
			GPU_SetLineThickness(2.0f);
			GPU_SetShapeBlending(false);
		}
	}
	GPU_Rect temp_dimensions = {
				_dimensions.x ,
				_dimensions.y + (_focus ? 1.0f : 0.0f),
				_dimensions.w ,
				_dimensions.h };
	Render::DrawTextBox(_text, _default_font, temp_dimensions, _pallet->font, FC_ALIGN_CENTER);
	GPU_SetLineThickness(1.0f);
}