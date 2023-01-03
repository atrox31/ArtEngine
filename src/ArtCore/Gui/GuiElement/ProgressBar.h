#pragma once
#include "ArtCore/Gui/Gui.h"

namespace GuiElement {
class ProgressBar final :
	public Gui::GuiElementTemplate
{
public:
	ENUM_WITH_STRING_CONVERSION(DrawingStyle,
		(STEP)(SOLID)(STEP_BLEND)(SOLID_BLEND)
	)

	ProgressBar();
	void Render() override;

	ProgressBar* SetMin(int min) { _min = min; return this; }
	ProgressBar* SetMax(int max) { _max = max; return this; }
	ProgressBar* SetValue(float value);
	ProgressBar* SetStep(int value) { _step = value; return this; }
	ProgressBar* SetWidth(int value) { _dimensions.w = (float)value; return this; }
	ProgressBar* SetHeight(int value) { _dimensions.h = (float)value; return this; }

	ProgressBar* SetDrawingColor(SDL_Color start, SDL_Color middle, SDL_Color end);
	ProgressBar* SetDrawingColor(SDL_Color start, SDL_Color end);
	ProgressBar* SetDrawingColor(SDL_Color color);

	ProgressBar* BindCallback_OnComplite(void (*complite_callback)(std::string arg), std::string args) { this->_complite_callback = complite_callback; this->_complite_callback_args = args; return this; }
	ProgressBar* BindCallback_OnHover(void (*hover_callback)(std::string arg), std::string args) { this->_hover_callback = hover_callback; this->_hover_callback_args = args; return this; }

	void  SetVariableFromStringEx(const std::string& name, const std::string& value) override;
private:
	SDL_Color start_value_color{};
	SDL_Color middle_value_color{};
	SDL_Color end_value_color{};

	float _value;
	int _min;
	int _max;
	int _step;

	void (*_complite_callback)(std::string args);
	std::string _complite_callback_args;
	void (*_hover_callback)(std::string args);
	std::string _hover_callback_args;

	DrawingStyle _drawing_style;
};
};