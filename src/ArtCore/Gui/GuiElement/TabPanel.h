#pragma once
#include "ArtCore/Gui/Gui.h"

namespace GuiElement {
	class TabPanel final :
		public Gui::GuiElementTemplate
	{
	public:
		TabPanel();
		~TabPanel() override;

		void Render() override;

		TabPanel* CreateTab(const std::string& tab);
		TabPanel* SetActiveTab(const std::string& tab);
		[[nodiscard]] std::string GetTabTag(const std::string& tab) const;

	private:
		GuiElementTemplate* _current_tab;
	};
};