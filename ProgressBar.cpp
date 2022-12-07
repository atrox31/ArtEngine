
#include "Gui.h"
#include "ProgressBar.h"

#include "Render.h"


GuiElement::ProgressBar::ProgressBar(const int x, const int y, const int min, const int max, const ProgressBar::DrawingStyle drawing_style, const int height) {
	_x = x;
	_y = y;
	this->_create_x = _x;
	this->_create_y = _y;
	_min = min;
	_max = max;
	_value = (float)min;
	_dimensions.h = (float)height;
	_step = 1;
	_drawing_style = drawing_style;
	_hover_callback = nullptr;
	_complite_callback = nullptr;
	_complite_callback_args = "";
	_hover_callback_args = "";
	this->_type = GuiElementTemplate::Type::PROGRESS_BAR;
}
void GuiElement::ProgressBar::Render() {
	GPU_SetLineThickness(2.0f);

	if (_enable_transparent)GPU_SetShapeBlending(true);
	if (_enabled) {
		_pallet->background.a = 200;
		Render::DrawRectRoundedFilled(_dimensions, 6, _pallet->background);
		_pallet->background.a = 255;
	}
	else {
		_pallet->background.a = 200;
		Render::DrawRectRoundedFilled(_dimensions, 6, _pallet->background_disable);
		_pallet->background.a = 255;
	}

	GPU_SetLineThickness(1.0f);

	switch (_drawing_style)
	{
	case ProgressBar::DrawingStyle::STEP:
	case ProgressBar::DrawingStyle::STEP_BLEND:
	{
		const float stepW = _dimensions.w / (float)(_max - _min);
		for (int i = (int)_min; i < (int)_value; i += _step) {
			GPU_Rect block = {
				_dimensions.x + 1 + (i * stepW),
				_dimensions.y + 2,
				stepW - 2 ,
				_dimensions.h - 4
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
	Render::DrawRectRounded(_dimensions, 6.f, _pallet->frame);
	GPU_SetLineThickness(1.0f);
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