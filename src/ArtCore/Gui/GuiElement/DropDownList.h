#pragma once
#include "ArtCore/Functions/Func.h"
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

	std::string GetSelectedValue() const {return _selected_value;}

	[[nodiscard]] int GetSelectedIndex() const {return _selected_index;}
	void SetSelectedIndex(int index);
private:
	int _selected_index = -1;
	std::string _selected_value = "";
	bool _show_list = false;
	Func::str_vec _values;
	std::vector<bool> _child_state;
	int _temp_selected_index = -1;
};
};