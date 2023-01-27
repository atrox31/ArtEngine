#pragma once
#include <string>
#include <vector>

#include "ArtCore/_Debug/Debug.h"
#include "ArtCore/Structs/Rect.h"
#include "ArtCore/Enums/EnumExtend.h"

#include "SDL2/IncludeAll.h"
#include "FC_Fontcache/SDL_FontCache.h"

#include "nlohmann/json.hpp"
using nlohmann::json;


//
// Class gui contains User interface methods to draw and execute actions
// Every Gui element work for his own and his children
//
class Gui final
{
public:
	Gui();
	~Gui()
	{
		Clear();
	}
public:
	class GuiElementTemplate;
	static void SortAllElements(GuiElementTemplate*);
	[[nodiscard]] bool LoadFromJson(const json& data) const;
	bool SpawnElementFromJsonData(GuiElementTemplate* parrent, const nlohmann::basic_json<>& data) const;
	void Clear() const;
	void Render() const;
	[[nodiscard]] bool Events() const;
	void UpdateView() const;
	GuiElementTemplate* GetElementById(const std::string& tag, GuiElementTemplate* search_root = nullptr);
	[[nodiscard]] bool PointOnInterface(SDL_FPoint) const;

	GuiElementTemplate* AddElement(GuiElementTemplate* target, GuiElementTemplate* element) const;
	[[nodiscard]] GuiElementTemplate* Element(const std::string&) const;


	struct Pallet {
		SDL_Color Background{};
		SDL_Color BackgroundDisable{};
		SDL_Color Frame{};
		SDL_Color Active{};
		SDL_Color Font{};
		Pallet();

		Pallet(const Pallet& pattern);
		Pallet(SDL_Color background, SDL_Color frame, SDL_Color active, SDL_Color font);
		Pallet(const std::string& hash_background, const std::string& hash_frame, const std::string& hash_active, const std::string& hash_font);
	};
public:

	ENUM_WITH_STRING_CONVERSION(Style,
		(ALIGN_LEFT)
		(ALIGN_RIGHT)
		(ALIGN_CENTER)

		(FILL_CENTER)

		(RELATIVE_PARENT)
		(ABSOLUTE)
	)

	ENUM_WITH_STRING_CONVERSION(ImageStyle,
		(LEFT)
		(RIGHT)

		(CENTER)
		(CENTER_TOP)
		(CENTER_BOTTOM)

		(TOP)
		(BOTTOM)

		(FILL)
		(PURE)
		)
	

	class GuiElementTemplate {
	public:

		ENUM_WITH_STRING_CONVERSION(Type,
			(USER_DEFINED)
			(ROOT_ELEMENT)
			(PANEL)
			(TABPANEL)
			(BUTTON)
			(GRID)
			(LABEL)
			(IMAGE)
			(PROGRESS_BAR)
			(SLIDER)
			(CHECK_BUTTON)
			(DROP_DOWN_BUTTON)
		)

		GuiElementTemplate() {
			_x = 0;
			_y = 0;
			_create_x = 0;
			_create_y = 0;
			_tag = "undefined";
			_default_font = Gui::GlobalFont;
			_parent = nullptr;
			for(int i = EvCallbackInvalid + 1; i<EvCallbackEND; i++)
			{
				_callback_script[static_cast<EvCallback>(i)] = std::pair<const unsigned char*, Sint64>{ nullptr, 0 };
			}
			_sound_onClick = nullptr;
			_enabled = true;
			_focus = false;
			_visible = true;
			_enable_transparent = true;
			_mouse_hover = false;
			_hover_time = 0.0;
			_dimensions = { 0,0,0,0 };
			_text_area = { 0,0,0,0 };
			_text_scale = { 0,0 };
			_text_align = FC_AlignEnum::FC_ALIGN_LEFT;
			_style = Style::ALIGN_LEFT;
			_type = GuiElementTemplate::Type::USER_DEFINED;
			_enable_on_pause = false;
			_pallet = Pallet();
		}
	public:
		ENUM_WITH_STRING_CONVERSION(EvCallback, (EvOnClick)(EvOnHover))
		virtual ~GuiElementTemplate() = default;
		// pure virtual, every gui element have its own render function
		virtual void Render(){}
		virtual void ApplyStyle();
		virtual bool OnClick();

		virtual bool GetEnabled() final { return _enabled; }
		virtual bool GetVisible() final { return _visible; };
		virtual std::string GetTag() final { return _tag; }
		virtual std::vector<GuiElementTemplate*> GetChildren() final { return _elements; };

		virtual int GetX() final { return _x; }
		virtual int GetY() final { return _y; }
		virtual Rect GetDimensions() final { return _dimensions; }

		virtual void Clear();

		virtual GuiElementTemplate* SetFocus(const bool f) final { _focus = f; return this; }
		virtual GuiElementTemplate* SetEnabledOnPause(const bool e) final { this->_enable_on_pause = e; return this; }
		virtual GuiElementTemplate* SetTransparent(const bool e) final { this->_enable_transparent = e; return this;}
		virtual GuiElementTemplate* SetEnabled(bool e) final;
		virtual GuiElementTemplate* SetVisible(bool v) final;
		virtual GuiElementTemplate* SetCallback(const EvCallback ev, const std::pair<const unsigned char*, Sint64> code) final { _callback_script[ev] = code; return this;}
		virtual GuiElementTemplate* SetPosition(const int x, const int y) final { _x = x; _y = y;  return this;}
		virtual GuiElementTemplate* SetSound(Mix_Chunk* sound) final;
		virtual GuiElementTemplate* SetDefaultFont(FC_Font* font) final { _default_font = font; return this;}
		virtual GuiElementTemplate* SetPosition(const int x1, const int y1, const int x2, const int y2) final {
			_x = x1;
			_y = y1;
			_dimensions = { static_cast<float>(x1),static_cast<float>(y1),static_cast<float>(x2),static_cast<float>(y2) };
			ApplyStyle();
		return this;}
		virtual GuiElementTemplate* SetTag(const std::string& tag) final { _tag = tag; return this;}
		virtual GuiElementTemplate* SetPallet(Pallet* pallet) final { _pallet = *pallet; return this;}
		virtual GuiElementTemplate* SetParent(GuiElementTemplate* parrent) final { _parent = parrent; return this;}
		virtual GuiElementTemplate* SetStyle(Style s);
		virtual GuiElementTemplate* SetText(const std::string&, FC_AlignEnum = FC_AlignEnum::FC_ALIGN_LEFT, FC_Scale = { 1.0f, 1.0f });

		virtual void SetVariableFromString(const std::string& name, const std::string& value) final;
		// for root element, else must be override
		virtual void SetVariableFromStringEx(const std::string& name, const std::string& value);
	
	protected:
		int _x, _y;
		Rect _dimensions{};
		friend Gui;

		bool _enable_on_pause;
		std::string _tag;
		GuiElementTemplate::Type _type;

		std::string _text;
		FC_AlignEnum _text_align;
		FC_Scale _text_scale{};
		GPU_Rect _text_area{};

		GuiElementTemplate* _parent;
		bool _enabled;
		bool _visible;
		bool _focus;
		bool _enable_transparent;
		int _create_x, _create_y;
		Style _style;
		Pallet _pallet;
		std::map< EvCallback, std::pair<const unsigned char*, Sint64>> _callback_script;
		Mix_Chunk* _sound_onClick;
		bool _mouse_hover;
		double _hover_time;
		std::vector<GuiElementTemplate*> _elements;
		FC_Font* _default_font;
	private:
		void _deleteElement();
	};
public:
	inline static FC_Font* GlobalFont;
private:
	static void _render(GuiElementTemplate*);
	static bool _events(GuiElementTemplate*);
	static void _updateView(GuiElementTemplate*);
	GuiElementTemplate* _root_element;
};


