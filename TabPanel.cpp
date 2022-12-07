#include "TabPanel.h"

#include "Debug.h"
#include "Panel.h"
#include "Render.h"

void GuiElement::TabPanel::Render() {
	GPU_SetLineThickness(2.0f);

	if (_enable_transparent)GPU_SetShapeBlending(true);
	if (_enabled) {
		_pallet->background.a = 200;
		Render::DrawRectRoundedFilled(_dimensions, 6.f, _pallet->background);
		_pallet->background.a = 255;
	}
	else {
		_pallet->background.a = 200;
		Render::DrawRectRoundedFilled(_dimensions, 6.f, _pallet->background_disable);
		_pallet->background.a = 255;
	}

	if (_enable_transparent)GPU_SetShapeBlending(false);
	Render::DrawRectRounded(_dimensions, 6.f, _pallet->frame);
	GPU_SetLineThickness(1.0f);
}

GuiElement::TabPanel* GuiElement::TabPanel::CreateTab(const std::string& tab)
{
	auto element = new Panel((int)_dimensions.x, (int)_dimensions.y, (int)_dimensions.w, (int)_dimensions.h);
	element->SetTag(tab);
	element->SetPallet(this->_pallet);
	element->SetParrent(this);
	element->SetVisibled(false);
	_elements.push_back(element);
	return this;
}

GuiElement::TabPanel* GuiElement::TabPanel::SetActiveTab(const std::string& tab)
{
	for (auto& element : _elements) {
		if (element->GetTag() == tab) {
			if (_current_tab != nullptr) {
				_current_tab->SetVisibled(false);
			}
			_current_tab = element;
			_current_tab->SetVisibled(true);
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

GuiElement::TabPanel::TabPanel(const int x1, const int y1, const int x2, const int y2) {
	this->_type = Gui::GuiElementTemplate::Type::TABPANEL;
	this->_x = x1;
	this->_y = y1;
	this->_create_x = _x;
	this->_create_y = _y;
	this->_dimensions = { float(x1),float(y1),float(x2),float(y2) };

	_current_tab = nullptr;
}

GuiElement::TabPanel::~TabPanel()
{
	//TODO delete childs
}
