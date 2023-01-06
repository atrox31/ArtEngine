#include "Grid.h"

#include "ArtCore/Functions/Func.h"
#include "ArtCore/Graphic/ColorDefinitions.h"
#include "ArtCore/Graphic/Render.h"
#include "ArtCore/System/Core.h"
#include "ArtCore/Functions/SDL_Point_extend.h"

GuiElement::Grid::Grid() {
	this->_type = GuiElementTemplate::Type::GRID;
	_focus_element = { -1, -1 };
	_clicked_element = { -1, -1 };
	this->_focus_xy = { .0f,.0f };
	_grid_elements = std::vector< gh_button* >();
}

GuiElement::Grid::~Grid() {
	if (!_grid_elements.empty()) {
		for (auto it = _grid_elements.begin(); it != _grid_elements.end() /* not hoisted */; /* no increment */)
		{
			delete (*it);
			it = _grid_elements.erase(it);
		}
	}
}
void GuiElement::Grid::Render() {
	_focus_element = _null_focus;
	_clicked_element = _null_focus;
	for (int y = 0; y < _rows; y++) {
		for (int x = 0; x < _columns; x++) {
			int v = x + (y * _columns);
			const SDL_Point grid_selected_point = { x, y };

			GPU_Rect grid_box_dimensions = {
				_dimensions.x + ((_spacing * (x + 1)) + (_element_size.x * x)),
				_dimensions.y + ((_spacing * (y + 1)) + (_element_size.y * y)),
				(float)_element_size.x,(float)_element_size.y
			};
			if (_grid_elements[v] == nullptr) {
				Render::DrawRectRoundedFilled(grid_box_dimensions, 2.f, _pallet.BackgroundDisable);
			}
			else {
				if (Func::PointInGPU_Rect(Core::GetInstance()->Mouse.XYf, grid_box_dimensions)) {
					if (_focus_element == _null_focus) {
						_focus_element = grid_selected_point;
					}
					if (Core::GetInstance()->Mouse.LeftPressed) {
						_clicked_element = grid_selected_point;
					}
				}

				Render::DrawRectRoundedFilled(grid_box_dimensions, 2.f, _pallet.Background);
				if (_grid_elements[v]->image != nullptr) {
					Render::DrawSpriteBox(_grid_elements[v]->image, grid_box_dimensions, 0, 0.f, 1.f);
					

					if (*_grid_elements[v]->enabled == 0) {
						GPU_SetShapeBlending(true);
						SDL_Color temp_shadow_color = C_BLACK;
						temp_shadow_color.a = 180;
						Render::DrawRectRoundedFilled(grid_box_dimensions, 2.f, temp_shadow_color);
						GPU_SetShapeBlending(false);

					}
					else {

						if (_clicked_element == grid_selected_point) {
							GPU_SetShapeBlending(true);
							SDL_Color temp_shadow_color = C_WHITE;
							temp_shadow_color.a = 20;
							Render::DrawRectRoundedFilled(grid_box_dimensions, 2.f, temp_shadow_color);
							GPU_SetShapeBlending(false);

						}
					}
					if (_focus_element == grid_selected_point) {
						GPU_SetShapeBlending(true);
						GPU_SetLineThickness(4);
						SDL_Color t = C_WHITE;
						t.a = 60;
						Render::DrawRectRounded(grid_box_dimensions, 2.f, t);
						GPU_SetShapeBlending(false);
						GPU_SetLineThickness(1);

						this->_focus_xy = { grid_box_dimensions.x, grid_box_dimensions.y - grid_box_dimensions.h };
						_grid_elements[v]->_hover_time += (float)Core::GetInstance()->DeltaTime;
					}
					else {
						_grid_elements[v]->_hover_time = 0.0f;
					}
					Render::DrawRectRounded(grid_box_dimensions, 2.f, _pallet.Frame);
				}
			}
		}
	}

	if (_focus_element != _null_focus) {
		gh_button* t = GetButton(_focus_element.x, _focus_element.y);
		if (t->call_back_hover == nullptr) return;
		t->call_back_hover(t->arg, t->_hover_time, _focus_xy);
	}
}
bool GuiElement::Grid::OnClick()
{
	const gh_button* button = GetButton(_clicked_element.x, _clicked_element.y);
	if (button != nullptr) {
		if (button->enabled) {
			if (this->_sound_onClick != nullptr) {
				//TODO play sound AC->GetCurrentScene()->Audio->PlaySound(this->_sound_onClick);
			}
			button->call_back(button->arg);
			return true;
		}
	}
	return false;
}

GuiElement::Grid::gh_button* GuiElement::Grid::GetButton(const int x, const int y) const
{
	if (x < 0) return nullptr;
	if (y < 0) return nullptr;
	if (x > _columns) return nullptr;
	if (y > _rows) return nullptr;

	const int index = x + (y * _columns);

	return _grid_elements[index];
}

GuiElement::Grid::gh_button* GuiElement::Grid::AddButton(const int x, const int y, gh_button* button) {
	if (x < 0) return nullptr;
	if (y < 0) return nullptr;
	if (x > _columns) return nullptr;
	if (y > _rows) return nullptr;

	const int index = x + (y * _columns);
	if (!_grid_elements.empty() && _grid_elements[index] != nullptr) {
		delete _grid_elements[index];
	}
	_grid_elements[index] = button;
	return _grid_elements[index];
}
void GuiElement::Grid::AddButtons(const std::initializer_list<gh_button*> buttons)
{
	int x = 0;
	int y = 0;
	for (const auto button : buttons) {
		AddButton(x, y, button);
		x++;
		if (x == _columns) {
			x = 0;
			y++;
			if (y == _rows) {
				return;
			}
		}
	}
}

GuiElement::Grid::gh_button* GuiElement::Grid::GetButton(const SDL_Point xy) const
{
	return (xy == _null_focus ? nullptr : GetButton(xy.x, xy.y));
}

void GuiElement::Grid::SetElementSpacing(const int index)
{
	if (index < 0) {
		Debug::WARNING("grid_holder::SetElementSpacing error index=" + std::to_string(index));
		return;
	}
	_spacing = index;
	_dimensions = {
		(float)_x,(float)_y,
		(float)(_element_size.x * _columns + (_columns + 2) * _spacing),
		(float)(_element_size.y * _rows + (_rows + 2) * _spacing)
	};
}

void GuiElement::Grid::Clear()
{
	if (!_grid_elements.empty()) {
		for (auto it = _grid_elements.begin(); it != _grid_elements.end() /* not hoisted */; /* no increment */)
		{
			it = _grid_elements.erase(it);
		}
	}
	for (int y = 0; y < _rows; y++) {
		for (int x = 0; x < _columns; x++) {
			_grid_elements.push_back(nullptr);
		}
	}
}

void GuiElement::Grid::SetVariableFromStringEx(const std::string& name, const std::string& value)
{
	/*
					new Variable(Variable.VariableType.VTypeInt, "Columns", "3"),
                    new Variable(Variable.VariableType.VTypeInt, "Rows", "3"),
                    new Variable(Variable.VariableType.VTypeInt, "ElementWidth", "64"),
                    new Variable(Variable.VariableType.VTypeInt, "ElementHeight", "64"),
                    new Variable(Variable.VariableType.VTypeInt, "ElementSpacing", "8")
                    */
	if (name == "Columns")
	{
		_columns = std::stoi(value);
		Clear(); // create new elements array
		return;
	}
	if (name == "Rows")
	{
		_rows = std::stoi(value);
		Clear(); // create new elements array
		return;
	}
	if (name == "ElementWidth")
	{
		_element_size.x = std::stoi(value); return;
	}
	if (name == "ElementHeight")
	{
		_element_size.y = std::stoi(value); return;
	}
	if (name == "ElementSpacing")
	{
		_spacing = std::stoi(value); return;
	}
	Debug::WARNING("[GuiElement::Grid::SetVariableFromStringEx]: '" + name + "' not found");
}

void GuiElement::Grid::SetElementSize(const int w, const int h)
{
	if (w < 1 || h < 1) {
		Debug::WARNING("grid_holder::SetElementSize error w=" + std::to_string(w) + ", h=" + std::to_string(h));
		return;
	}
	_element_size = { w, h };
	_dimensions = {
		(float)_x,(float)_y,
		(float)(_element_size.x * _columns + (_columns + 2) * _spacing),
		(float)(_element_size.y * _rows + (_rows + 2) * _spacing)
	};
}
