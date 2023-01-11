#include "DropDownList.h"

#include "ArtCore/Functions/Func.h"
#include "ArtCore/Graphic/Render.h"
#include "ArtCore/Scene/Scene.h"
#include "ArtCore/System/Core.h"

GuiElement::DropDownList::DropDownList()
{
	this->_type = GuiElementTemplate::Type::DROP_DOWN_BUTTON;
}

GuiElement::DropDownList* GuiElement::DropDownList::SetText(const std::string& text)
{
	this->_text = text;
	this->_dimensions = FC_GetBounds(Gui::GlobalFont, 0, 0, FC_ALIGN_LEFT, FC_MakeScale(1.0f, 1.0f), text.c_str());
	this->_dimensions.W += 12;
	this->_dimensions.H += 2;
	return this;
}
bool GuiElement::DropDownList::OnClick()
{
	_show_list = !_show_list;
	for (const auto parrent_children_list = _parent->GetChildren(); const auto i : parrent_children_list)
	{
		if (i == this) continue;
		if(_show_list)
		{
			_child_state.emplace_back(i->GetEnabled());
			i->SetEnabled(false);
		}else
		{
			i->SetEnabled(_child_state.back());
			i->SetFocus(false);
			_child_state.pop_back();
		}
	}
	if(!_show_list)
	{
		_selected_index = _temp_selected_index;
		if (_selected_index >= 0) {
			_selected_value = _values[_temp_selected_index];
		}
		_child_state.clear();
		GuiElementTemplate::OnClick();
	}else
	{
		std::reverse(_child_state.begin(), _child_state.end());
	}
	return true;
}

void GuiElement::DropDownList::SetVariableFromStringEx(const std::string& name, const std::string& value)
{
	if (name == "Text")
	{
		SetText(value); return;
	}
	if (name == "Values")
	{
		Func::str_vec sliced_values = Func::Split(value, '|');
		_values.insert(_values.begin(), sliced_values.begin(), sliced_values.end());
		return;
	}
	Console::WriteLine("[GuiElement::DropDownList::SetVariableFromStringEx]: '" + name + "' = '"+ value+"' not found");
}

void GuiElement::DropDownList::SetSelectedIndex(const int index)
{
	_selected_index = std::clamp(index, 0, static_cast<int>(_values.size()-1));
	if (_selected_index >= 0) {
		_selected_value = _values[_selected_index];
	}
}

void GuiElement::DropDownList::Render()
{
	const float line_thickness = GPU_SetLineThickness(2.0f);
	if (_enabled) {
		if (_mouse_hover || _show_list) {
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
	if (_mouse_hover && _enabled) {
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
	
	Render::DrawTextBox(_text, _default_font, temp_dimensions, _pallet.Font, FC_ALIGN_CENTER);

	Rect alter_dimensions = _dimensions;
	alter_dimensions.Y += _dimensions.H + 4.f;

	if (_show_list) {
		int tmp_i = -1;
		for (const auto& value : _values)
		{
			tmp_i++;
			const bool element_mouse_hover = alter_dimensions.PointInRectWh(Core::Mouse.XYf);
			if(element_mouse_hover)
			{
				_temp_selected_index = tmp_i;
				_mouse_hover = true;
				_focus = true;
			}
			Render::DrawRectRoundedFilled(alter_dimensions.ToGPU_Rect(), 2.0f,
				(element_mouse_hover ?
					_pallet.Active : _pallet.Background)
			);
			Render::DrawRectRounded(alter_dimensions.ToGPU_Rect(), 2.0f, _pallet.Frame);
			alter_dimensions.Y += Render::DrawTextBox(value, _default_font, alter_dimensions.ToGPU_Rect(), _pallet.Font, FC_ALIGN_CENTER).h + 2.f;
		}
	}else
	{
		Render::DrawRectRoundedFilled(alter_dimensions.ToGPU_Rect(), 2.0f, _pallet.BackgroundDisable);
		Render::DrawTextBox(_selected_value, _default_font, alter_dimensions.ToGPU_Rect(), _pallet.Font, FC_ALIGN_CENTER);
	}

	GPU_SetLineThickness(line_thickness);
}
