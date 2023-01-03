#pragma once
#include <string>
#include <vector>

#include "ArtCore/_Debug/Debug.h"
#include "ArtCore/Structs/Rect.h"
#include "ArtCore/Enums/enumstring.h"

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
		Debug::NOTE_DEATH("[Gui::~Gui]");
		Clear();
	}
public:
	class GuiElementTemplate;
	bool LoadFromJson(const json& data) const;
	bool SpawnElementFromJsonData(GuiElementTemplate* parrent, const nlohmann::basic_json<>& data) const;
	void Clear() const;
	void Render() const;
	void Events(SDL_Event* e) const;
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
			_hover_time = .0f;
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
		virtual void Render(){};
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

		virtual void SetEnabledOnPause(const bool e) final { this->_enable_on_pause = e; }
		virtual void SetTransparent(const bool e) final { this->_enable_transparent = e; }
		virtual void SetEnabled(bool e) final;
		virtual void SetVisible(bool v) final;
		virtual void SetCallback(const EvCallback ev, const std::pair<const unsigned char*, Sint64> code) final { _callback_script[ev] = code; }
		virtual void SetPosition(const int x, const int y) final { _x = x; _y = y;  };
		virtual void SetSound(std::string sound) final;
		virtual void SetDefaultFont(FC_Font* font) { _default_font = font; };
		virtual void SetPosition(const int x1, const int y1, const int x2, const int y2) final {
			_x = x1;
			_y = y1;
			_dimensions = { (float)(x1),(float)(y1),(float)(x2),(float)(y2) };
			ApplyStyle();
		};
		virtual void SetTag(const std::string& tag) final { _tag = tag; }
		virtual void SetPallet(Pallet* pallet) final { _pallet = *pallet; }
		virtual void SetParent(GuiElementTemplate* parrent) final { _parent = parrent; }
		virtual void SetStyle(Style s);
		virtual void SetText(std::string, FC_AlignEnum = FC_AlignEnum::FC_ALIGN_LEFT, FC_Scale = { 1.0f, 1.0f });

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
	static void _updateView(GuiElementTemplate*);
	GuiElementTemplate* _root_element;
};

