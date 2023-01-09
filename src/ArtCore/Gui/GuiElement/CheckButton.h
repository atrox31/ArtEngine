#pragma once
#include "ArtCore/Gui/Gui.h"

namespace GuiElement {
class CheckButton final :
	public Gui::GuiElementTemplate
{
public:
	void Render() override;
	bool OnClick() override;
	CheckButton* SetText(const std::string& text);
	CheckButton();
	void  SetVariableFromStringEx(const std::string& name, const std::string& value) override;

	[[nodiscard]] bool Checked() const
	{
		return _checked;
	}
	void SetState(const bool state)
	{
		_checked = state;
	}
private:
	bool _checked{};
};
};