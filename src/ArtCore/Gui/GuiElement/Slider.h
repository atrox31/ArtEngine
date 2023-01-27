#pragma once
#include "ArtCore/Gui/Gui.h"

namespace GuiElement {
class Slider final :
	public Gui::GuiElementTemplate
{
public:
	void Render() override;
	bool OnClick() override;
	Slider();
	void  SetVariableFromStringEx(const std::string& name, const std::string& value) override;

	void SetValue(const int value)
	{
		_value = SDL_clamp(value, _value_min, _value_max);
	}
	[[nodiscard]] int GetValue() const 
	{
		return _value;
	}
	void SetValueMin(const int value) { _value_min = value; }
	void SetValueMax(const int value) { _value_max = value; }
	void SetValueStep(const int step) { _step = step; }
	void SetShowValue(const bool show) { _show_value = show; }
private:
	int _value{};
	int _value_min{};
	int _value_max{};
	int _step{};
	bool _show_value{};
	bool _slider_move;
};
};