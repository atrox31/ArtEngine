#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "ColorDefinitions.h"
#include "SDL_version.h"
#include "SDL_FontCache.h"

#include SDL2_INCLUDE_DIR
#include SDL2_MIXER_INCLUDE_DIR

//
// Class gui contains User interface methods to draw and execute actions
// Every Gui element work for his own and his children
//
class Gui final
{
public:
	Gui();;
	virtual ~Gui()
	{
		Clear();
		delete _current_pallet;
	}
public:
	void Clear() const;
	void Render() const;
	void Events(SDL_Event* e) const;
	void UpdateView();
	bool PointOnInterface(SDL_FPoint) const;

	class GuiElementTemplate;
	GuiElementTemplate* AddElement(std::string, GuiElementTemplate*) const;
	[[nodiscard]] GuiElementTemplate* Element(std::string) const;


	struct Pallet {
		SDL_Color background{};
		SDL_Color background_disable{};
		SDL_Color frame{};
		SDL_Color active{};
		SDL_Color font{};
		Pallet() {
			this->background = C_DEFAULT;
			this->background_disable = C_DEFAULT;
			this->frame = C_DEFAULT;
			this->active = C_DEFAULT;
			this->font = C_DEFAULT;
		}
		Pallet(SDL_Color background, SDL_Color frame, SDL_Color active, SDL_Color font);
		Pallet(std::string& hash_background, std::string& hash_frame, std::string& hash_active, std::string& hash_font);
	};
	void SetPallet(const Pallet& pallet) const
	{
		*_current_pallet = pallet;
	}
private:
	Pallet* _current_pallet;
public:

	enum class Style
	{
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER,

		FILL_CENTER,

		RELATIVE_PARRENT,

		ABSOLUTE
	};

	enum class ImageStyle
	{
		LEFT,
		RIGHT,

		CENTER,
		CENTER_TOP,
		CENTER_BOTTOM,

		TOP,
		BOTTOM,

		FILL,
		ABSOLUTE
	};

	class GuiElementTemplate {
	public:

		enum class Type {
			USER_DEFINED,
			ROOT_ELEMENT,
			PANEL,
			TABPANEL,
			BUTTON,
			GRID,
			LABEL,
			PROGRESS_BAR
		};

		GuiElementTemplate() {
			_tag = "undefined";
			_default_font = Gui::GlobalFont;
			_parrent = nullptr;
			_call_back = nullptr;
			_sound_onClick = nullptr;
			_enabled = true;
			_focus = false;
			_visibled = true;
			_enable_transparent = true;
			_hover_time = .0f;
			_dimensions = { 0,0,0,0 };
			_text_area = { 0,0,0,0 };
			_text_scale = { 0,0 };
			_text_align = FC_AlignEnum::FC_ALIGN_LEFT;
			_style = Style::ALIGN_LEFT;
			_type = GuiElementTemplate::Type::USER_DEFINED;
			_enable_on_pause = false;
			_pallet = nullptr;
		}
		virtual ~GuiElementTemplate() = default;
		virtual void Render(){};
		virtual void ApplyStyle();
		virtual bool OnClick() { if (_call_back != nullptr) { _call_back(); return true; } return false; };

		virtual bool GetEnabled() final { return _enabled; }
		virtual bool GetVisibled() final { return _visibled; };
		virtual std::string GetTag() final { return _tag; }
		virtual std::vector<GuiElementTemplate*> GetChildren() final { return _elements; };

		virtual int GetX() final { return _x; }
		virtual int GetY() final { return _y; }
		virtual GPU_Rect GetDimensions() final { return _dimensions; }

		virtual void Clear();

		virtual void SetEnabledOnPause(bool e) final { this->_enable_on_pause = e; }
		virtual void SetTransparent(bool e) final { this->_enable_transparent = e; }
		virtual void SetEnabled(bool e) final;
		virtual void SetVisibled(bool v) final;
		virtual void SetCallback(void (*_call_back_)()) final { this->_call_back = _call_back_; }
		virtual void SetPostion(int x, int y) final { _x = x; _y = y;  };
		virtual void SetSound(std::string sound) final;
		virtual void SetDefaultFont(FC_Font* font) { _default_font = font; };
		virtual void SetPostion(int x1, int y1, int x2, int y2) final {
			_x = x1;
			_y = y1;
			_dimensions = { (float)(x1),(float)(y1),(float)(x2),(float)(y2) };
			ApplyStyle();
		};
		virtual void SetTag(const std::string& tag) final { _tag = tag; }
		virtual void SetPallet(Pallet* pallet) final { _pallet = pallet; }
		virtual void SetParrent(GuiElementTemplate* parrent) final { _parrent = parrent; }
		virtual void SetStyle(Style s);
		virtual void SetText(std::string, FC_AlignEnum = FC_AlignEnum::FC_ALIGN_LEFT, FC_Scale = { 1.0f, 1.0f });
		
	protected:
		int _x, _y;
		GPU_Rect _dimensions;
		friend Gui;

		bool _enable_on_pause;
		std::string _tag;
		GuiElementTemplate::Type _type;

		std::string _text;
		FC_AlignEnum _text_align;
		FC_Scale _text_scale;
		GPU_Rect _text_area;

		GuiElementTemplate* _parrent;
		bool _enabled;
		bool _visibled;
		bool _focus;
		bool _enable_transparent;
		int _create_x, _create_y;
		Style _style;
		Pallet* _pallet;
		void (*_call_back)();
		Mix_Chunk* _sound_onClick;
		float _hover_time;
		std::vector<GuiElementTemplate*> _elements;
		FC_Font* _default_font;
	private:
		void _deleteElement();
	};
public:
	static FC_Font* GlobalFont;
private:
	static void _render(GuiElementTemplate*);
	static void _events(GuiElementTemplate*, SDL_Event*);
	void _updateView(GuiElementTemplate*);
	GuiElementTemplate* _root_element;
};