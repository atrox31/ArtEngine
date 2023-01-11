#include "Slider.h"

#include "ArtCore/Functions/Convert.h"
#include "ArtCore/Functions/Func.h"
#include "ArtCore/Graphic/Render.h"
#include "ArtCore/System/Core.h"

GuiElement::Slider::Slider()
{
	this->_type = GuiElementTemplate::Type::SLIDER;
	_slider_move = false;
}

bool GuiElement::Slider::OnClick()
{
	GuiElementTemplate::OnClick();
	return _slider_move;
}

void GuiElement::Slider::SetVariableFromStringEx(const std::string& name, const std::string& value)
{
	/*
					new Variable(Variable.VariableType.VTypeString, "Text", "slider"),
                    new Variable(Variable.VariableType.VTypeInt, "Minimum", "0"),
                    new Variable(Variable.VariableType.VTypeInt, "Maximum", "100"),
                    new Variable(Variable.VariableType.VTypeInt, "Step", "10"),
                    new Variable(Variable.VariableType.VTypeBool, "ShowValue", "true"),
	 */
	if (name == "Text")
	{
		SetText(value); return;
	}
	if (name == "Minimum")
	{
		_value_min = Func::TryGetInt(value); return;
	}
	if (name == "Maximum")
	{
		_value_max = Func::TryGetInt(value); return;
	}
	if (name == "Step")
	{
		_step = Func::TryGetInt(value); return;
	}
	if (name == "ShowValue")
	{
		_show_value = Convert::Str2Bool(value); return;
	}
	Console::WriteLine("[GuiElement::Slider::SetVariableFromStringEx]: '" + name + "' not found");
}

void GuiElement::Slider::Render()
{
	constexpr float slider_width = 24.f;
	// slider action must be in render so can refresh every frame
	if(_slider_move)
	{
		if(Core::Mouse.LeftEvent == Core::MouseState::ButtonState::RELEASED)
		{
			_slider_move = false;
		}else
		{
			if(Core::Mouse.X < static_cast<int>(_dimensions.X + 16.f))
			{
				_value = _value_min;
			}else
			if(Core::Mouse.X > static_cast<int>(_dimensions.X + _dimensions.W - 16.f))
			{
				_value = _value_max;
			}else
			{
				_value = std::clamp(
					(static_cast<int>(Func::LinearScale(
						static_cast<float>(Core::Mouse.X),
						_dimensions.X + 16.f,
						(_dimensions.X + _dimensions.W - 16.f - slider_width),
						static_cast<float>(_value_min), static_cast<float>(_value_max)
					)) / _step) * _step,
					_value_min,
					_value_max
				);
			}
			
		}
	}

	// draw frame
	const float line_thickness = GPU_SetLineThickness(2.0f);
	Render::DrawRectRoundedFilled(_dimensions.ToGPU_Rect(), 2.0f,_enabled ? _pallet.Background : _pallet.BackgroundDisable);
	Render::DrawRectRounded(_dimensions.ToGPU_Rect(), 2.0f, _pallet.Frame);

	// draw slider itself
	const Rect slider_bar{
		_dimensions.X + 16.f,
		_dimensions.Y + (_dimensions.H - 24.f),
		_dimensions.W - 32.f,
		12.f
	};
	const Rect slider_point{
		_dimensions.X + Func::LinearScale(
			static_cast<float>(_value),
			static_cast<float>(_value_min),
			static_cast<float>(_value_max),
			(16.f),
			(_dimensions.W - 16.f - slider_width)) - slider_width/2.f,
		_dimensions.Y + 36.f,
		slider_width,
		36.f
	};
	const bool slider_point_mouse_hover = ( ( slider_point.PointInRectWh(Core::Mouse.XYf) || _slider_move) || slider_bar.PointInRectWh(Core::Mouse.XYf));
	SDL_Color slider_point_color = _pallet.Active;
	if (Core::Mouse.LeftPressed && slider_point_mouse_hover)
	{
		slider_point_color.a = 200;
		GPU_SetShapeBlending(true);
	}
	if (Core::Mouse.LeftEvent == Core::MouseState::ButtonState::PRESSED && slider_point_mouse_hover)
	{
		_slider_move = true;
	}

	Render::DrawRectRoundedFilled(slider_bar.ToGPU_Rect(), 16.0f,
		(_enabled && slider_point_mouse_hover ? (
			Core::Mouse.LeftPressed ? slider_point_color : _pallet.Background )
			: _pallet.BackgroundDisable));
	Render::DrawRectRounded(slider_bar.ToGPU_Rect(), 16.0f, _pallet.Frame);

	GPU_SetLineThickness(4.0f);
	Render::DrawRectRoundedFilled(slider_point.ToGPU_Rect(), 16.0f, (slider_point_mouse_hover ? slider_point_color : _pallet.Background ));
	Render::DrawRectRounded(slider_point.ToGPU_Rect(), 16.0f, _pallet.Frame);
	GPU_SetLineThickness(2.0f);
	GPU_SetShapeBlending(false);
	
	Render::DrawTextBox(_text, _default_font, _dimensions.ToGPU_Rect(), _pallet.Font, FC_ALIGN_CENTER);
	if (_show_value) {
		Render::DrawTextBox(std::to_string(_value) + " ", _default_font, _dimensions.ToGPU_Rect(), _pallet.Font, FC_ALIGN_RIGHT);
	}
	GPU_SetLineThickness(line_thickness);
}
