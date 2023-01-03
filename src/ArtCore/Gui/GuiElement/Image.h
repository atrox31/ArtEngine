#pragma once
#include "ArtCore/Graphic/Sprite.h"
#include "ArtCore/Gui/Gui.h"

namespace GuiElement {
class Image final :
	public Gui::GuiElementTemplate
{
public:
	void Render() override;
	Image();
	virtual void ApplyStyle() override;
	~Image() override;
	void SetVariableFromStringEx(const std::string& name, const std::string& value) override;
private:
	Sprite* _sprite;
	Gui::ImageStyle _iStyle;
};

};