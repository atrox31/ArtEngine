#include "CheckButton.h"

#include "ArtCore/Graphic/Render.h"
#include "ArtCore/System/Core.h"

GuiElement::CheckButton::CheckButton()
{
	this->_type = GuiElementTemplate::Type::CHECK_BUTTON;
}

bool GuiElement::CheckButton::OnClick()
{
	_checked = !_checked;
	GuiElementTemplate::OnClick();
	return true;
}
GuiElement::CheckButton* GuiElement::CheckButton::SetText(const std::string& text)
{
	this->_text = text;
	this->_dimensions = FC_GetBounds(Gui::GlobalFont, 0, 0, FC_ALIGN_LEFT, FC_MakeScale(1.0f, 1.0f), text.c_str());
	this->_dimensions.W += 12;
	this->_dimensions.H += 2;
	return this;
}

void GuiElement::CheckButton::SetVariableFromStringEx(const std::string& name, const std::string& value)
{
	/*
	 new Variable(Variable.VariableType.VTypeString, "Text", "check button")
	 */
	if (name == "Text")
	{
		SetText(value); return;
	}
	Console::WriteLine("[GuiElement::CheckButton::SetVariableFromStringEx]: '" + name + "' not found");
}

void GuiElement::CheckButton::Render()
{
	const float line_thickness = GPU_SetLineThickness(2.0f);
	if (_enabled) {
		if (_mouse_hover) {
			Render::DrawRectRoundedFilled(_dimensions.ToGPU_Rect(), 2.0f, _pallet.Active);
			if (Core::Mouse.LeftPressed) {
				GPU_SetShapeBlending(true);
				Render::DrawRectRoundedFilled(_dimensions.ToGPU_Rect(), 2.0f, { 0,0,0,100 });
				const GPU_Rect frame_border = (_dimensions / 2).ToGPU_Rect();
				Render::DrawRectRounded(frame_border, 2.0f, { 0,0,0,100 });
				GPU_SetShapeBlending(false);
			}
		}
		else {
			Render::DrawRectRoundedFilled(_dimensions.ToGPU_Rect(), 2.0f, _pallet.Background);
		}
	}
	else {
		Render::DrawRectRoundedFilled(_dimensions.ToGPU_Rect(), 2.0f, _pallet.BackgroundDisable);
	}
	Render::DrawRectRounded(_dimensions.ToGPU_Rect(), 2.0f, _pallet.Frame);
	if (_mouse_hover) {
		if (Core::Mouse.LeftPressed) {
			GPU_SetShapeBlending(true);
			GPU_SetLineThickness(4.0f);
			Render::DrawRectRoundedFilled(_dimensions.ToGPU_Rect(), 2.0f, { 0,0,0,100 });
			GPU_SetLineThickness(2.0f);
			GPU_SetShapeBlending(false);
		}
	}
	const GPU_Rect temp_dimensions = {
				_dimensions.X ,
				_dimensions.Y + (_mouse_hover ? 1.0f : 0.0f),
				_dimensions.W ,
				_dimensions.H };
	constexpr float box_dimensions = 24.f;
	Rect check_box_rect = {
		_dimensions.X + _dimensions.W - box_dimensions - 8.f,
		_dimensions.Y + _dimensions.H - box_dimensions - 8.f,
		box_dimensions,box_dimensions
	};

	Render::DrawRectRounded(check_box_rect.ToGPU_Rect(), 2.0f, _mouse_hover ? _pallet.Frame : _pallet.Active);
	check_box_rect /= 1.f;
	Render::DrawRectRounded(check_box_rect.ToGPU_Rect(), 2.0f, _mouse_hover ? _pallet.Active : _pallet.Frame);
	if(_checked)
	{
		// outer
		Render::DrawLine(
			check_box_rect.A_wh(), check_box_rect.C_wh(),
			6.f, _mouse_hover ? _pallet.Active : _pallet.Frame
		);
		Render::DrawLine(
			check_box_rect.B_wh(), check_box_rect.D_wh(),
			6.f, _mouse_hover ? _pallet.Active : _pallet.Frame
		);
		// inner
		Render::DrawLine(
			check_box_rect.A_wh(), check_box_rect.C_wh(),
			3.f, _mouse_hover ? _pallet.Frame : _pallet.Active
		);
		Render::DrawLine(
			check_box_rect.B_wh(), check_box_rect.D_wh(),
			3.f, _mouse_hover ?_pallet.Frame : _pallet.Active 
		);

	}


	Render::DrawTextBox(" " + _text, _default_font, temp_dimensions, _pallet.Font, FC_ALIGN_LEFT);
	GPU_SetLineThickness(line_thickness);
}
