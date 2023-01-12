#include "Gui.h"

#include "ArtCore/Functions/Convert.h"
#include "ArtCore/Functions/Func.h"
#include "ArtCore/System/Core.h"
#include "ArtCore/Graphic/ColorDefinitions.h"
#include "ArtCore/CodeExecutor/CodeExecutor.h"
#include "ArtCore/Scene/Scene.h"
#include "ArtCore/System/AssetManager.h"

#include "GuiElement/Button.h"
#include "GuiElement/CheckButton.h"
#include "GuiElement/DropDownList.h"
#include "GuiElement/Grid.h"
#include "GuiElement/Image.h"
#include "GuiElement/Label.h"
#include "GuiElement/Panel.h"
#include "GuiElement/ProgressBar.h"
#include "GuiElement/Slider.h"
#include "GuiElement/TabPanel.h"

// All systems is from previous version of ArtCore writed many years ago.
// Some code is rewrited, mostly drawing and events but rest have old style
// returns of self.
// This is from times when ArtCore do not have editor, everything must
// be writed and compiled at once.
// This works but I can write this better.
// TODO rewrite this to newest standards, clean and comment this code.

bool Gui::PointOnInterface(const SDL_FPoint p) const
{
	if (!_root_element->_elements.empty()) {
		for (auto it = _root_element->_elements.begin(); it != _root_element->_elements.end();)
		{
			if ((*it)->_visible) {
				if ((*it)->_dimensions.PointInRect(p)) {
					return true;
				}
			}
			++it;
		}
	}
	return false;
}


Gui::Gui()
{
	GlobalFont = Core::GetGlobalFont();
	_root_element = new GuiElementTemplate();
	_root_element->_type = GuiElementTemplate::Type::ROOT_ELEMENT;
}

bool Gui::LoadFromJson(const json& data) const
{
	if (!data.is_object()) {
		Console::WriteLine("[Gui::LoadFromJson] error, wrong data structure, object expect");
		return false;
	}
	if (data["Name"].get<std::string>() != "root")
	{
		Console::WriteLine("[Gui::LoadFromJson] error, first node must be root");
		return false;
	}

	for (const auto& variables : data["_elementVariables"])
	{
		const std::string v_name = variables["Name"].get<std::string>();
		const std::string v_value = variables["Default"].get<std::string>();
		_root_element->SetVariableFromString(v_name, v_value);
	}
	// spawn all elements in tree
	for (const auto& object : data["_children"])
	{
		if (!SpawnElementFromJsonData(_root_element, object)) return false;
	}

	SortAllElements(_root_element);

	return true;
}
void Gui::SortAllElements(GuiElementTemplate* root)
{
	for (GuiElementTemplate* element : root->_elements)
	{
		SortAllElements(element);
	}

	std::ranges::sort(root->_elements, [](GuiElementTemplate* a, GuiElementTemplate* b) {
		return a->GetY() < b->GetY();
		});

}

bool Gui::SpawnElementFromJsonData(GuiElementTemplate* parrent, const nlohmann::basic_json<>& data) const
{
	const std::string type = data["Name"].get<std::string>();
	GuiElementTemplate* new_element = nullptr;
	if(type == "Panel")
	{
		new_element = new GuiElement::Panel();
	}else if (type == "Button")
	{
		new_element = new GuiElement::Button();
	}
	else if(type == "Grid")
	{
		new_element = new GuiElement::Grid();
	}
	else if(type == "Image")
	{
		new_element = new GuiElement::Image();
	}
	else if(type == "Label")
	{
		new_element = new GuiElement::Label();
	}
	else if(type == "ProgressBar")
	{
		new_element = new GuiElement::ProgressBar();
	}
	else if(type == "TabPanel")
	{
		new_element = new GuiElement::TabPanel();
	}
	else if(type == "DropDownList")
	{
		new_element = new GuiElement::DropDownList();
	}
	else if(type == "Slider")
	{
		new_element = new GuiElement::Slider();
	}
	else if(type == "CheckButton")
	{
		new_element = new GuiElement::CheckButton();
	}
	else if(type == "root")
	{
		new_element = _root_element;
	}else
	{
		Console::WriteLine("[Gui::SpawnElementFromJsonData]: unknown element type '" + type + "'");
		return false;
	}
	new_element->SetParent(parrent);
	for (const auto& variables : data["_elementVariables"])
	{
		const std::string v_name =  variables["Name"].get<std::string>();
		const std::string v_value = variables["Default"].get<std::string>();
		new_element->SetVariableFromString(v_name, v_value);
	}
	AddElement(parrent, new_element);

	for (const auto& object : data["_children"])
	{
		if (!SpawnElementFromJsonData(new_element, object)) return false;
	}
	return true;
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
	const bool point_on_element = e->_dimensions.PointInRectWh(Core::Mouse.XYf);
	e->_mouse_hover = point_on_element;
	if(e->_mouse_hover)
	{
		e->_hover_time += Core::DeltaTime;
	}else
	{
		e->_hover_time = 0.0;
	}

	if (Core::Mouse.LeftEvent == Core::MouseState::ButtonState::PRESSED) {
		e->_focus = point_on_element;
	}

	if (e->_focus)
	{
		e->_focus = point_on_element;
	}

	e->Render();
	if (!e->_elements.empty()) {
		for (std::vector<GuiElementTemplate*>::reverse_iterator i = e->_elements.rbegin();
			i != e->_elements.rend(); ++i)
			//for (int i = 0; i < e->_elements.size(); ++i)
		{
			if ((*i)->_visible) {
				_render((*i));
			}
		}

	}
}



bool Gui::Events() const
{
	if (!_root_element->_elements.empty()) {
		for (const auto& var : _root_element->_elements)
		{
			if ((var->_enabled && var->_visible)) {
				if(_events(var))
				{
					// some of gui element handle event so return
					return true;
				}
			}
		}
	}
	return false;
}

bool Gui::_events(GuiElementTemplate* e)
{
	//e->ApplyStyle();
	if (!e->GetChildren().empty()) {
		//for (int i = 0; i < e->_elements.size(); ++i)
		for (const auto & element : e->_elements)
		{
			if (_events(element))
			{
				return true;
			}
		}
	}
	if (e->_enabled && e->_visible) {
		if (e->_focus) {
			if (Core::Mouse.LeftEvent == Core::MouseState::ButtonState::RELEASED) {
				if (e->OnClick()) {
					if (e->_sound_onClick != nullptr) {
						// play audio(e->_sound_onClick);
					}
					return true;
				}
				e->_focus = false;
			}
		}
	}
	return false;
}

void Gui::GuiElementTemplate::SetVariableFromString(const std::string& name, const std::string& value)
{
	/*
			new Variable(Variable.VariableType.VTypeString, "Tag", "GuiElement_"),
            new Variable(Variable.VariableType.VTypeInt, "Position_x", "0"),
            new Variable(Variable.VariableType.VTypeInt, "Position_y", "0"),
            new Variable(Variable.VariableType.VTypeInt, "Width", "100"),
            new Variable(Variable.VariableType.VTypeInt, "Height", "100"),
            new Variable(Variable.VariableType.VTypeBool, "Enabled", "true"),
            new Variable(Variable.VariableType.VTypeBool, "Visible", "true"),
            new Variable(Variable.VariableType.VTypeEnum, "Style", "ALIGN_LEFT", new List<string>()
	 */
	if(name == "Tag")
	{
		SetTag(value); return;
	}
	if(name == "Position_x")
	{
		_create_x = std::stoi(value); return;
	}
	if(name == "Position_y")
	{
		_create_y = std::stoi(value); return;
	}
	if(name == "Width")
	{
		_dimensions.W = std::stof(value); return;
	}
	if(name == "Height")
	{
		_dimensions.H = std::stof(value); return;
	}
	if(name == "Enabled")
	{
		_enabled = Convert::Str2Bool(value); return;
	}
	if(name == "Visible")
	{
		_visible = Convert::Str2Bool(value); return;
	}
	if(name == "Style")
	{
		_style = Gui::Style_fromString(value); return;
	}
	// not universal variable
	SetVariableFromStringEx(name, value);
}

void Gui::GuiElementTemplate::SetVariableFromStringEx(const std::string& name, const std::string& value)
{
	if (name == "Pallet_background")
	{
		_pallet.Background = Convert::Hex2Color(value); return;
	}
	if (name == "Pallet_background_disable")
	{
		_pallet.BackgroundDisable = Convert::Hex2Color(value); return;
	}
	if (name == "Pallet_frame")
	{
		_pallet.Frame = Convert::Hex2Color(value); return;
	}
	if (name == "Pallet_active")
	{
		_pallet.Active = Convert::Hex2Color(value); return;
	}
	if (name == "Pallet_font")
	{
		_pallet.Font = Convert::Hex2Color(value); return;
	}
	Console::WriteLine("[GuiElement::GuiElementTemplate::SetVariableFromStringEx]: '" + name + "' not found");
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
void Gui::UpdateView() const
{
	if (!_root_element->_elements.empty()) {
		for (const auto& var : _root_element->_elements)
		{
			_updateView(var);
		}
	}
}

Gui::GuiElementTemplate* Gui::GetElementById(const std::string& tag, GuiElementTemplate* search_root)
{
	if (search_root == nullptr) {
		if (_root_element->_elements.empty()) return nullptr;
		return GetElementById(tag, _root_element);
	}else
	{
		if (search_root->_tag == tag) return search_root;
		GuiElementTemplate* _return = nullptr;
		for (GuiElementTemplate* element : search_root->_elements)
		{
			_return = GetElementById(tag, element);
			if (_return != nullptr) return _return;
		}
		return _return;
	}
}

Gui::GuiElementTemplate* Gui::AddElement(GuiElementTemplate* target, GuiElementTemplate* element) const
{
	// add element on front, draw functions use reverse order to draw
	target->_elements.insert(target->_elements.begin(), element);
	element->_pallet = _root_element->_pallet;
	if (element->_parent == nullptr) {
		element->_parent = target;
	}
	element->ApplyStyle();
	return element;
}

Gui::GuiElementTemplate* Gui::Element(const std::string& tag) const
{
	if (tag.length() == 0) { Console::WriteLine("gui element: tag size");  return nullptr; }

	const str_vec path = Func::Split(tag, '/');
	const size_t path_size = path.size();

	if (path_size == 0) { Console::WriteLine("gui element: path error");  return nullptr; }

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
				Console::WriteLine("gui element: path error target not found");
				return nullptr;
			}
		}
	}
	if (target == _root_element) { Console::WriteLine("gui element: element not found");  return nullptr; }
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
			this->_dimensions.X = static_cast<float>(this->_create_x) + _parent->GetX();
			this->_dimensions.Y = static_cast<float>(this->_y);
		}
		break;
		case Gui::Style::ALIGN_RIGHT:
		{
			this->_x = this->_create_x + _parent->GetX();
			this->_y = this->_create_y + _parent->GetY();
			this->_dimensions.X = static_cast<float>(_parent->GetX()) + _parent->GetDimensions().W - this->_dimensions.W - this->_create_x;
			this->_dimensions.Y = static_cast<float>(this->_y);
		}
		break;
		case Gui::Style::ALIGN_CENTER:
		{
			this->_x = this->_create_x + _parent->GetX();
			this->_y = this->_create_y + _parent->GetY();
			this->_dimensions.X = static_cast<float>(_parent->GetX()) + _parent->GetDimensions().W * 0.5f - this->_dimensions.W * 0.5f;
			this->_dimensions.Y = static_cast<float>(this->_y);
		}
		break;
		case Gui::Style::FILL_CENTER:
		{
			this->_x = this->_create_x + _parent->GetX();
			this->_y = this->_create_y + _parent->GetY();
			this->_dimensions.X = static_cast<float>(this->_x);
			this->_dimensions.Y = static_cast<float>(this->_y);
			this->_dimensions.W = _parent->GetDimensions().W - _create_x * 2;
		}
		break;
		case Gui::Style::RELATIVE_PARENT:
		{
			this->_x = this->_create_x + _parent->GetX();
			this->_y = this->_create_y + _parent->GetY();
			this->_dimensions.X = static_cast<float>(this->_x);
			this->_dimensions.Y = static_cast<float>(this->_y);
		}
		break;
		case Gui::Style::ABSOLUTE:
		{
			this->_x = _create_x;
			this->_y = _create_y;
			this->_dimensions.X = static_cast<float>(this->_x);
			this->_dimensions.Y = static_cast<float>(this->_y);
		}
		break;
		default:
			break;
		}
	}
}

bool Gui::GuiElementTemplate::OnClick()
{
	if(_callback_script[EvCallback::EvOnClick].first != nullptr)
	{
		Core::Executor()->ExecuteCode(
			Core::GetCurrentScene()->GetVariableHolder(),
			&_callback_script[EvCallback::EvOnClick]);
		return true;
	}
	return false;
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

Gui::GuiElementTemplate* Gui::GuiElementTemplate::SetEnabled(bool e)
{
	this->_enabled = e;
	return this;
}

Gui::GuiElementTemplate* Gui::GuiElementTemplate::SetVisible(bool v)
{
	this->_visible = v;
	return this;
}

Gui::GuiElementTemplate* Gui::GuiElementTemplate::SetSound(const std::string& sound)
{
	_sound_onClick = Core::GetAssetManager()->GetSound(sound);
	return this;
}

Gui::GuiElementTemplate* Gui::GuiElementTemplate::SetStyle(Style s)
{
	_style = s;
	ApplyStyle();
	return this;
}

Gui::GuiElementTemplate* Gui::GuiElementTemplate::SetText(const std::string& text, const FC_AlignEnum align, const FC_Scale scale)
{
	_text = text;
	_text_scale = scale;
	_text_align = align;
	_text_area = FC_GetBounds(_parent->_default_font, 0, 0, align, scale, text.c_str());
	return this;
}


Gui::Pallet::Pallet()
{
	this->Background = C_DEFAULT;
	this->BackgroundDisable = C_DEFAULT;
	this->Frame = C_DEFAULT;
	this->Active = C_DEFAULT;
	this->Font = C_DEFAULT;
}

Gui::Pallet::Pallet(const Pallet& pattern)
{
	this->Background = pattern.Background;
	this->BackgroundDisable = pattern.BackgroundDisable;
	this->Frame = pattern.Frame;
	this->Active = pattern.Active;
	this->Font = pattern.Font;
}

Gui::Pallet::Pallet(const SDL_Color background, const SDL_Color frame, const SDL_Color active, const SDL_Color font)
{
	this->Background = background;
	this->BackgroundDisable = {
		static_cast<Uint8>(background.r * 0.2),
		static_cast<Uint8>(background.g * 0.2),
		static_cast<Uint8>(background.b * 0.2),
		background.a
	};
	this->Frame = frame;
	this->Active = active;
	this->Font = font;
}

Gui::Pallet::Pallet(const std::string& hash_background, const  std::string& hash_frame, const  std::string& hash_active, const  std::string& hash_font)
{

	this->Background = Convert::Hex2Color(hash_background);
	this->BackgroundDisable = {
		static_cast<Uint8>(Background.r * 0.2),
		static_cast<Uint8>(Background.g * 0.2),
		static_cast<Uint8>(Background.b * 0.2),
		Background.a
	};
	this->Frame = Convert::Hex2Color(hash_frame);
	this->Active = Convert::Hex2Color(hash_active);
	this->Font = Convert::Hex2Color(hash_font);

}
