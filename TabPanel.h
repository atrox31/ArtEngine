#pragma once
#include "Gui.h"
namespace GuiElement {
	class TabPanel final :
		public Gui::GuiElementTemplate
	{
	public:
		TabPanel(int x1, int y1, int x2, int y2);
		~TabPanel() override;

		void Render() override;

		TabPanel* CreateTab(const std::string& tab);
		TabPanel* SetActiveTab(const std::string& tab);
		std::string GetTabTag(const std::string& tab) const;

	private:
		GuiElementTemplate* _current_tab;
	};
};