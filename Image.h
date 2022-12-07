#pragma once
#include "Gui.h"
#include "Sprite.h"

namespace GuiElement {
class Image final :
	public Gui::GuiElementTemplate
{
public:
	void Render() override;
	Image(int x, int y, Sprite* sprite, void(*call_back)(), Gui::ImageStyle style);
	virtual void ApplyStyle() override;
	~Image() override;
private:
	Sprite* _sprite;
	Gui::ImageStyle _iStyle;
};

};