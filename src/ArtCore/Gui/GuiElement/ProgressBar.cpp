#include "ProgressBar.h"

#include "ArtCore/Graphic/Render.h"
#include "ArtCore/Functions/Func.h"
#include "ArtCore/Functions/SDL_Color_extend.h"
#include "ArtCore/Gui/Console.h"


GuiElement::ProgressBar::ProgressBar() {
	_min = 0;
	_max = 0;
	_value = 0.f;
	_step = 1;
	_drawing_style = (DrawingStyle)1;
	_hover_callback = nullptr;
	_complite_callback = nullptr;
	_complite_callback_args = "";
	_hover_callback_args = "";
	this->_type = GuiElementTemplate::Type::PROGRESS_BAR;
}
void GuiElement::ProgressBar::Render() {
	const float line_thickness = GPU_SetLineThickness(2.0f);

	if (_enable_transparent)GPU_SetShapeBlending(true);
	if (_enabled) {
		_pallet.Background.a = 200;
		Render::DrawRectRoundedFilled(_dimensions.ToGPU_Rect_wh(), 6, _pallet.Background);
		_pallet.Background.a = 255;
	}
	else {
		_pallet.Background.a = 200;
		Render::DrawRectRoundedFilled(_dimensions.ToGPU_Rect_wh(), 6, _pallet.BackgroundDisable);
		_pallet.Background.a = 255;
	}

	GPU_SetLineThickness(1.0f);

	switch (_drawing_style)
	{
	case ProgressBar::DrawingStyle::STEP:
	case ProgressBar::DrawingStyle::STEP_BLEND:
	{
		const float stepW = _dimensions.W / (float)(_max - _min);
		for (int i = (int)_min; i < (int)_value; i += _step) {
			GPU_Rect block = {
				_dimensions.X + 1 + (i * stepW),
				_dimensions.Y + 2,
				stepW - 2 ,
				_dimensions.H - 4
			};
			const float current_percent = (float)i / (float)_max;
			SDL_Color color;
			if (_drawing_style == ProgressBar::DrawingStyle::STEP) {
				if (current_percent < .33f) color = start_value_color;
				else if (current_percent < .66f) color = middle_value_color;
				else color = end_value_color;
			}
			else {
				// blend
				color = {
					(Uint8)(((float)start_value_color.r * (1.0f - current_percent) + (float)end_value_color.r * current_percent) * 0.5),
					(Uint8)(((float)start_value_color.g * (1.0f - current_percent) + (float)end_value_color.g * current_percent) * 0.5),
					(Uint8)(((float)start_value_color.b * (1.0f - current_percent) + (float)end_value_color.b * current_percent) * 0.5),
					(Uint8)0xFF
				};
			}
			Render::DrawRectFilled(block, color);
		}
	}
	break;
	case ProgressBar::DrawingStyle::SOLID:
	case ProgressBar::DrawingStyle::SOLID_BLEND:

		break;
	default:
		break;
	}
	GPU_SetLineThickness(2.0f);
	if (_enable_transparent)GPU_SetShapeBlending(false);
	Render::DrawRectRounded(_dimensions.ToGPU_Rect_wh(), 6.f, _pallet.Frame);
	GPU_SetLineThickness(line_thickness);
}

GuiElement::ProgressBar* GuiElement::ProgressBar::SetValue(const float value)
{
	_value = std::ceilf(std::min((float)_min, std::max((float)_max, value)));
	if (_value >= _max) {
		_value = (float)_max;
		if (_complite_callback != nullptr) {
			_complite_callback(_complite_callback_args);
		}
	}
	return this;
}

GuiElement::ProgressBar* GuiElement::ProgressBar::SetDrawingColor(const SDL_Color start, const SDL_Color middle, const SDL_Color end) {
	start_value_color = start;
	middle_value_color = middle;
	end_value_color = end;
	return this;
}

GuiElement::ProgressBar* GuiElement::ProgressBar::SetDrawingColor(const SDL_Color start, const SDL_Color end) {
	start_value_color = start;
	middle_value_color = start * end; // mix two colors
	end_value_color = end;
	return this;
}

GuiElement::ProgressBar* GuiElement::ProgressBar::SetDrawingColor(const SDL_Color color) {
	start_value_color = color;
	middle_value_color = color;
	end_value_color = color;
	return this;
}

void GuiElement::ProgressBar::SetVariableFromStringEx(const std::string& name, const std::string& value)
{
	/*
					new Variable(Variable.VariableType.VTypeInt, "Minimum", "0"),
                    new Variable(Variable.VariableType.VTypeInt, "Maximum", "100"),
                    new Variable(Variable.VariableType.VTypeInt, "Step", "1"),
                    new Variable(Variable.VariableType.VTypeInt, "height", "32"),
                    new Variable(Variable.VariableType.VTypeEnum, "DrawingStyle", "STEP", new List<string>()
	 */
	if (name == "Minimum")
	{
		_min = std::stoi(value); return;
	}
	if (name == "Maximum")
	{
		_max = std::stoi(value); return;
	}
	if (name == "Step")
	{
		_step = std::stoi(value); return;
	}
	if (name == "height")
	{
		_dimensions.H = std::stof(value); return;
	}
	if (name == "DrawingStyle")
	{
		_drawing_style = DrawingStyle_fromString(value); return;
	}
	Console::WriteLine("[GuiElement::Label::SetVariableFromStringEx]: '" + name + "' not found");
}
