#pragma once
#include "ArtCore/Gui/Gui.h"
#include "ArtCore/Gui/Gui.h"

namespace GuiElement {
class DropDownList final :
	public Gui::GuiElementTemplate
{
public:
	void Render() override;
	bool OnClick() override;
	//void ApplyStyle() override;

	DropDownList();
	DropDownList* SetText(const std::string& text);

	void  SetVariableFromStringEx(const std::string& name, const std::string& value) override;

	int SelectedIndex = -1;
	std::string GetSelectedValue() {return _selected_value;}
private:
	std::string _selected_value;
	bool _show_list = false;
	std::vector<std::string> _values;
	std::vector<bool> _child_state;
	int _temp_selected_index = -1;
};
};