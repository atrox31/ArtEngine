#pragma once
#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "ArtCore/Functions/Func.h"
#include "ArtCore/Gui/Console.h"
#include "ArtCore/Structs/Rect.h"
#include "FC_Fontcache/SDL_FontCache.h"
#include "SDL2/IncludeAll.h"

class Scene;
class AssetManager;
class CodeExecutor;
class Core final
{
	private:
	Core();
	~Core();
public:
	// Core step execute
	static bool Init(int argc, char* args[]);
	static bool Run();
	static bool LoadData();
	static void Exit();

	bool ProcessEvents();
	void ProcessStep();
	void ProcessPhysics();
	void ProcessRender();

	// getters
	static Core* GetInstance() { return &Core::_instance; }
	static Scene* GetCurrentScene();
	static GPU_Target* GetScreenTarget() { return _instance._screenTarget; }
	static SDL_Window* GetWindowHandle();
	static int GetScreenWidth() {	return Graphic.GetWindowWidth();	}
	static int GetScreenHeight() {	return Graphic.GetWindowHeight();	}
	static FC_Font* GetGlobalFont() { return _instance._global_font;  }
	static AssetManager* GetAssetManager() { return _instance._asset_manager; }


	// setters
	static void Pause() {_instance.game_loop = false;}
	static void Play() {_instance.game_loop = true;	}
	static CodeExecutor* Executor() {return _instance._executor;}
	bool ChangeScene(const std::string& name);

	static inline double DeltaTime;
private:
	bool ProcessCoreKeys(Sint32 sym);
	bool game_loop;
	// graphic
	bool use_bloom = false;
	Uint8 use_bloom_level = 0;
	GPU_Target* _screenTarget;
	CodeExecutor* _executor;

	// fps
	static Uint32 FpsCounterCallback(Uint32 interval, void* parms);
	int fps;
	Uint64 NOW, LAST;
	int _frames;
	bool _show_fps;

public:
	// systems of handle game audio/video
	class graphic {
	public:
		graphic() {
			_window_width = 0;
			_window_height = 0;
			_window_fullscreen = false;
			_window_frame_rate = 0;
			_window_v_sync = false;
			_screen_rect = {};
		}
		void SetScreenResolution(const int w, const int h) {
			_window_width = w;
			_window_height = h;
		}
		void SetFullScreen(const bool value) {
			_window_fullscreen = value;
		}
		void SetFrameRate(const int frame_rate) {
			_window_frame_rate = frame_rate;
		}
		void SetVSync(const bool v_sync) {
			_window_v_sync = v_sync;
		}
		void Apply();
		[[nodiscard]] int GetWindowWidth() const
		{
			return _window_width;
		}

		[[nodiscard]] int GetWindowHeight() const
		{
			return _window_height;
		}

		Rect* GetScreenSpace()
		{
			return &_screen_rect;
		}

	private:
		int _window_width;
		int _window_height;
		bool _window_fullscreen;
		int _window_frame_rate;
		bool _window_v_sync;
		Rect _screen_rect;
	} inline static Graphic;
	class audio {
	public:
		audio() {
			_audio_music_level = 0.0f;
			_audio_sound_level = 0.0f;
			_audio_master = true;
			_audio_music = true;
			_audio_sound = true;
		}
		// setters
		void SetSoundLevel(const float level) {
			_audio_sound_level = std::clamp(level, 0.f, 100.f);
			Apply();
		}
		void SetMusicLevel(const float level) {
			_audio_music_level = std::clamp(level, 0.f, 100.f);
			Apply();
		}
		void SetMaster(const bool& enabled) { _audio_master = enabled; Apply();	}
		void SetSound(const bool& enabled) { _audio_sound = enabled; Apply();	}
		void SetMusic(const bool& enabled) { _audio_music = enabled; Apply();	}
		// getters
		[[nodiscard]] float GetMusicLevel() const { return (_audio_master && _audio_music) ? _audio_music_level : 0.f; }
		[[nodiscard]] float GetSoundLevel() const { return (_audio_master && _audio_sound) ? _audio_sound_level : 0.f; }

		void Apply() const;
	private:
		float _audio_music_level;
		float _audio_sound_level;

		bool _audio_master;
		bool _audio_music;
		bool _audio_sound;
	} inline static  Audio;
	
	struct MouseState {
		enum class ButtonState {
			PRESSED, RELEASED, NONE
		};
		// Current event of mouse buttons
		ButtonState LeftEvent = ButtonState::NONE;
		// Current event of mouse buttons
		ButtonState RightEvent = ButtonState::NONE;
		// current state of buttons
		bool LeftPressed = false;
		bool RightPressed = false;
		// Current mouse coordinate X
		int X = 0;
		// Current mouse coordinate X
		int Y = 0;
		// Current mouse coordinate int (x,y)
		SDL_Point XY = { 0,0 };
		// Current mouse coordinate float (x,y)
		SDL_FPoint XYf = { 0.f,0.f };
		// Current mouse wheel position
		int Wheel = 0;
		void Reset();
	} inline static  Mouse;
private:
	// settings data -> global settings
	std::map<std::string, std::string> SettingsData;
public:
	static int SD_GetInt(const std::string& field, int _default);
	static float SD_GetFloat(const std::string& field, float _default);
	static std::string SD_GetString(const std::string& field, std::string _default);

private:
	// inner list
	FC_Font* _global_font;
	std::vector<std::string> _program_arguments;
	AssetManager* _asset_manager;

	Scene* _current_scene;
	std::string _primary_scene;

	static Core _instance;
	SDL_Window* _window;
#ifdef _DEBUG
	// all debug flags and variables
private:
	
	class CoreDebug
	{
	public:
		CoreDebug();
		bool ProcessEvent(SDL_Event* e);
		void Draw();
		void SetSpyLines(const int& lines)
		{
			_spy_line_max = lines;
			_spy_line_begin = 0;
			_spy_line_end = std::min(_spy_line_max,8);
		}
	private:
		bool _show_debug_options = false;
		int  _selected_debug_option = -1;
		bool _any_panel_shown = false;

		bool _show_performance = false;
		bool _show_collider = false;
		bool _show_instance_info = false;
		bool _show_directions = false;
		bool _show_spy_window = false;
		// spy window
		int _spy_line_begin = 0;
		int _spy_line_end = 2;
		int _spy_line_max = 2;
		struct option
		{
			std::string Text;
			bool* Anchor;
			SDL_Keycode Key;
			option(const std::string& text, bool* anchor, const SDL_Keycode key)
			{
				Text = text;
				Anchor = anchor;
				Key = key;
			}
		};
		std::vector < option > Options;
	};
public:
	CoreDebug CoreDebug;
#endif
};

