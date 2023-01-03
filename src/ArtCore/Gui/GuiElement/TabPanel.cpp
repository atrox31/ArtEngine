#include "TabPanel.h"

#include "Panel.h"
#include "ArtCore/Graphic/Render.h"

void GuiElement::TabPanel::Render() {
	GPU_SetLineThickness(2.0f);

	if (_enable_transparent)GPU_SetShapeBlending(true);
	if (_enabled) {
		_pallet.Background.a = 200;
		Render::DrawRectRoundedFilled(_dimensions.ToGPU_Rect_wh(), 6.f, _pallet.Background);
		_pallet.Background.a = 255;
	}
	else {
		_pallet.Background.a = 200;
		Render::DrawRectRoundedFilled(_dimensions.ToGPU_Rect_wh(), 6.f, _pallet.BackgroundDisable);
		_pallet.Background.a = 255;
	}

	if (_enable_transparent)GPU_SetShapeBlending(false);
	Render::DrawRectRounded(_dimensions.ToGPU_Rect_wh(), 6.f, _pallet.Frame);
	GPU_SetLineThickness(1.0f);
}

GuiElement::TabPanel* GuiElement::TabPanel::CreateTab(const std::string& tab)
{
	const auto element = new Panel();
	element->SetPosition((int)_dimensions.x, (int)_dimensions.y, (int)_dimensions.w, (int)_dimensions.h);
	element->SetTag(tab);
	element->SetPallet(&_pallet);
	element->SetParent(this);
	element->SetVisible(false);
	_elements.push_back(element);
	return this;
}

GuiElement::TabPanel* GuiElement::TabPanel::SetActiveTab(const std::string& tab)
{
	for (auto& element : _elements) {
		if (element->GetTag() == tab) {
			if (_current_tab != nullptr) {
				_current_tab->SetVisible(false);
			}
			_current_tab = element;
			_current_tab->SetVisible(true);
			return this;
		}
	}
	Debug::WARNING("GuiElementTemplate::tabPanel : setActiveTab - tab '" + tab + "' not found");
	return this;
}
std::string GuiElement::TabPanel::GetTabTag(const std::string& tab) const
{
	return _tag + "_tab/" + tab;
}

GuiElement::TabPanel::TabPanel() {
	this->_type = Gui::GuiElementTemplate::Type::TABPANEL;
	_current_tab = nullptr;
}

GuiElement::TabPanel::~TabPanel()
{
	//TODO delete childs
}
