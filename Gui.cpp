#include "Gui.h"

#include "Convert.h"
#include "Core.h"
#include "Func.h"

FC_Font* Gui::GlobalFont;

bool Gui::PointOnInterface(const SDL_FPoint p) const
{
	if (!_root_element->_elements.empty()) {
		for (auto it = _root_element->_elements.begin(); it != _root_element->_elements.end();)
		{
			if ((*it)->_visible)
				if (Func::PointInGPU_Rect(p, (*it)->_dimensions)) {
					return true;
				}
			++it;
		}
	}
	return false;
}


Gui::Gui()
{
	_current_pallet = new Pallet();
	GlobalFont = Core::GetInstance()->_global_font;
	_root_element = new GuiElementTemplate();
	_root_element->_type = GuiElementTemplate::Type::ROOT_ELEMENT;
	_root_element->_pallet = _current_pallet;
}

void Gui::Clear() const
{
	if (!_root_element->_elements.empty()) {
		for (auto it = _root_element->_elements.begin(); it != _root_element->_elements.end();)
		{
			(*it)->_deleteElement();
			it = _root_element->_elements.erase(it);
		}
	}
}

void Gui::GuiElementTemplate::_deleteElement()
{
	if (_elements.empty()) {
		for (auto it = _elements.begin(); it != _elements.end();)
		{
			(*it)->_deleteElement();
			delete* it;
			it = _elements.erase(it);
		}
	}
}
void Gui::Render() const
{
	if (!_root_element->_elements.empty()) {
		for (const auto& elem : _root_element->_elements)
		{
			if (elem->_visible) {
				_render(elem);
			}
		}
	}
}
void Gui::_render(Gui::GuiElementTemplate* e)
{
	e->_focus = Func::PointInGPU_Rect(Core::GetInstance()->Mouse.XYf, e->_dimensions);
	e->Render();
	if (!e->_elements.empty()) {
		for (const auto& var : e->_elements)
		{
			if (var->_visible) {
				_render(var);
			}
		}
	}
}



void Gui::Events(SDL_Event* e) const
{
	if (!_root_element->_elements.empty()) {
		for (const auto& var : _root_element->_elements)
		{
			if ((e == nullptr) || (var->_enabled && var->_visible)) {
				_events(var, e);
			}
		}
	}

}

void Gui::_events(GuiElementTemplate* e, SDL_Event* se)
{
	e->ApplyStyle();
	if (!e->GetChildren().empty()) {
		for (const auto& var : e->_elements)
		{
			_events(var, se);
		}
	}
	if (se == nullptr) return;
	if (e->_enabled && e->_visible) {
		if (e->_focus) {
			if (se->button.button == SDL_BUTTON_LEFT) {
				if (Core::GetInstance()->Mouse.LeftPressed) {
					if (e->OnClick()) {
						if (e->_sound_onClick != nullptr) {
							// play audio(e->_sound_onClick);
						}
					}
					e->_focus = false;
				}
			}

		}
	}
}

void Gui::_updateView(GuiElementTemplate* e) {
	if (!e->_elements.empty()) {
		for (const auto& var : e->_elements)
		{
			_updateView(var);
		}
	}
	e->ApplyStyle();
}
void Gui::UpdateView()
{
	if (!_root_element->_elements.empty()) {
		for (const auto& var : _root_element->_elements)
		{
			_updateView(var);
		}
	}
}

Gui::GuiElementTemplate* Gui::AddElement(std::string tag, GuiElementTemplate* element) const
{
	if (tag.length() == 0) { Debug::WARNING("gui add element: tag size");  return nullptr; }
	if (element == nullptr) { Debug::WARNING("gui add element: element nullptr" );  return nullptr; }
	if (static_cast<Gui::GuiElementTemplate*> (element) == nullptr) { Debug::WARNING("gui add element: element not part of GuiElementTemplate");  return nullptr; }

	const std::vector<std::string> path = Func::Split(tag, '/');
	const size_t path_size = path.size();

	if (path_size == 0) { Debug::WARNING("gui add element: path error");  return nullptr; }

	if (path_size == 1) {
		_root_element->_elements.push_back(element);
		element->_tag = tag;
		element->_pallet = _root_element->_pallet;
		element->_parent = _root_element;
		element->ApplyStyle();
		return element;
	}
	else {
		Gui::GuiElementTemplate* target = _root_element;
		for (size_t i = 0; i < path_size - 1; i++) {
			for (Gui::GuiElementTemplate* elm : target->_elements) {
				if (elm->_tag == path[i]) {
					target = elm;
					break;
				}
			}
		}
		if (target == _root_element) { Debug::WARNING("gui add element: element not found");  return nullptr; }
		target->_elements.push_back(element);
		element->_tag = path.back();
		element->_pallet = target->_pallet;
		element->_parent = target;
		element->ApplyStyle();
		return element;
	}
}

Gui::GuiElementTemplate* Gui::Element(std::string tag) const
{
	if (tag.length() == 0) { Debug::WARNING("gui element: tag size");  return nullptr; }

	const std::vector<std::string> path = Func::Split(tag, '/');
	const size_t path_size = path.size();

	if (path_size == 0) { Debug::WARNING("gui element: path error");  return nullptr; }

	if (path_size == 1) {
		for (Gui::GuiElementTemplate* elm : _root_element->_elements) {
			if (elm->_tag == tag) return (elm);
		}
		return nullptr;
	}
	Gui::GuiElementTemplate* target = _root_element;
	for (size_t i = 0; i < path_size; i++) {
		for (Gui::GuiElementTemplate* elm : target->_elements) {
			if (elm->_tag == path[i]) {
				target = elm;
				break;
			}
			if (elm == target->_elements.back()) {
				Debug::WARNING("gui element: path error target not found");
				return nullptr;
			}
		}
	}
	if (target == _root_element) { Debug::WARNING("gui element: element not found");  return nullptr; }
	return (target);
}


void Gui::GuiElementTemplate::ApplyStyle()
{
	if (_parent != nullptr) {
		switch (_style)
		{
		case Gui::Style::ALIGN_LEFT:
		{
			this->_x = this->_create_x + _parent->GetX();
			this->_y = this->_create_y + _parent->GetY();
			this->_dimensions.x = (float)this->_create_x + _parent->GetX();
			this->_dimensions.y = (float)this->_y;
		}
		break;
		case Gui::Style::ALIGN_RIGHT:
		{
			this->_x = this->_create_x + _parent->GetX();
			this->_y = this->_create_y + _parent->GetY();
			this->_dimensions.x = (float)_parent->GetX() + _parent->GetDimensions().w - this->_dimensions.w - this->_create_x;
			this->_dimensions.y = (float)(this->_y);
		}
		break;
		case Gui::Style::ALIGN_CENTER:
		{
			this->_x = this->_create_x + _parent->GetX();
			this->_y = this->_create_y + _parent->GetY();
			this->_dimensions.x = (float)_parent->GetX() + _parent->GetDimensions().w * 0.5f - this->_dimensions.w * 0.5f;
			this->_dimensions.y = (float)(this->_y);
		}
		break;
		case Gui::Style::FILL_CENTER:
		{
			this->_x = this->_create_x + _parent->GetX();
			this->_y = this->_create_y + _parent->GetY();
			this->_dimensions.x = (float)(this->_x);
			this->_dimensions.y = (float)(this->_y);
			this->_dimensions.w = _parent->GetDimensions().w - _create_x * 2;
		}
		break;
		case Gui::Style::RELATIVE_PARENT:
		{
			this->_x += _parent->GetX();
			this->_y += _parent->GetY();
			this->_dimensions.x = (float)(this->_x);
			this->_dimensions.y = (float)(this->_y);
		}
		break;
		case Gui::Style::ABSOLUTE:
		{
			this->_dimensions.x = (float)(this->_x);
			this->_dimensions.y = (float)(this->_y);
		}
		break;
		default:
			break;
		}
	}
}

void Gui::GuiElementTemplate::Clear()
{
	if (!_elements.empty()) {
		for (auto it = _elements.begin(); it != _elements.end() /* not hoisted */; /* no increment */)
		{
			(*it)->_deleteElement();
			it = _elements.erase(it);
		}
	}
}

void Gui::GuiElementTemplate::SetEnabled(bool e)
{
	this->_enabled = e;
}

void Gui::GuiElementTemplate::SetVisible(bool v)
{
	this->_visible = v;
}

void Gui::GuiElementTemplate::SetSound(std::string sound)
{
	_sound_onClick = Core::GetInstance()->assetManager->GetSound(sound);
}

void Gui::GuiElementTemplate::SetStyle(Style s)
{
	_style = s;
	ApplyStyle();
}

void Gui::GuiElementTemplate::SetText(const std::string text, const FC_AlignEnum align, const FC_Scale scale)
{
	_text = text;
	_text_scale = scale;
	_text_align = align;
	_text_area = FC_GetBounds(_parent->_default_font, 0, 0, align, scale, text.c_str());
}


Gui::Pallet::Pallet(SDL_Color background, SDL_Color frame, SDL_Color active, SDL_Color font)
{
	this->background = background;
	this->background_disable = {
		(Uint8)(background.r * 0.2),
		(Uint8)(background.g * 0.2),
		(Uint8)(background.b * 0.2),
		background.a
	};
	this->frame = frame;
	this->active = active;
	this->font = font;
}

Gui::Pallet::Pallet(std::string& hash_background, std::string& hash_frame, std::string& hash_active, std::string& hash_font)
{

	this->background = Convert::Hex2Color(hash_background);
	this->background_disable = {
		(Uint8)(background.r * 0.2),
		(Uint8)(background.g * 0.2),
		(Uint8)(background.b * 0.2),
		background.a
	};
	this->frame = Convert::Hex2Color(hash_frame);
	this->active = Convert::Hex2Color(hash_active);
	this->font = Convert::Hex2Color(hash_font);

}
