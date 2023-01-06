#pragma once
#include <map>
#include <string>
#include <vector>

#include "ArtCore/Gui/Console.h"
#include "FC_Fontcache/SDL_FontCache.h"
#include "SDL2/IncludeAll.h"

class Scene;
class AssetManager;
class CodeExecutor;
class Core final
{
public:
	Core();
	~Core();
	static Core* GetInstance() { return &Core::_instance; }
	static bool Init(int argc, char* args[]);
	bool ProcessCoreKeys(Sint32 sym);
	static bool Run();
	static void Exit();
	static bool LoadData();

	static Scene* GetCurrentScene()
	{
		return _instance._current_scene;
	}
private:
	bool game_loop;
	bool use_bloom = false;
	Uint8 use_bloom_level = 0;
public:
	static void Pause() {
		_instance.game_loop = false;
	}
	static void Play() {
		_instance.game_loop = true;
	}


	bool ChangeScene(const std::string& name);

private:
	// graphic
	GPU_Target* _screenTarget;
public:
	static GPU_Target* GetScreenTarget() { return _instance._screenTarget; }
public:
	CodeExecutor* Executor;
private:
	// core data
	class graphic {
	public:
		graphic() {
			_window_width = 0;
			_window_height = 0;
			_window_fullscreen = 0;
			_window_framerate = 0;
			_window_vsync = 0;
		};
		void SetScreenResolution(const int w, const int h) {
			_window_width = w;
			_window_height = h;
		};
		void SetFullScreen(const bool value) {
			_window_fullscreen = value;
		};
		void SetFramerate(const int framerate) {
			_window_framerate = framerate;
		};
		void SetVSync(const bool vsync) {
			_window_vsync = vsync;
		};
		void Apply() const;
		int GetWindowWidth() const
		{
			return _window_width;
		};
		int GetWindowHeight() const
		{
			return _window_height;
		};
	private:
		int _window_width;
		int _window_height;
		bool _window_fullscreen;
		int _window_framerate;
		bool _window_vsync;
	};
public:
	static graphic Graphic;

private:
	class audio {
	public:
		audio() {
			_audio_master = 0.0f;
			_audio_music = 0.0f;
			_audio_sound = 0.0f;
		};
		void SetAudioMasterLevel(const float level) {
			_audio_master = level;
		};
		void SetAudioSoundLevel(const float level) {
			_audio_music = level;
		};
		void SetAudioMusicLevel(const float level) {
			_audio_sound = level;
		};
		void Apply();
	private:
		float _audio_master;
		float _audio_music;
		float _audio_sound;
	};
public:
	static audio Audio;

public:
	// fps
	static Uint32 FpsCounterCallback(Uint32 interval, void* parms);
	static inline double DeltaTime;
	int fps;
private:
	Uint64 NOW, LAST;
	int _frames;
	bool _show_fps;

public:
	// getters
	static SDL_Window* GetWindowHandle();

	static int GetScreenWidth() {
		return Graphic.GetWindowWidth();
	}

	static int GetScreenHeight() {
		return Graphic.GetWindowHeight();
	}

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
	};
	static MouseState Mouse;

	std::map<std::string, std::string> SettingsData;
	static int SD_GetInt(const std::string& field, int _default);
	static float SD_GetFloat(const std::string& field, float _default);
	static std::string SD_GetString(const std::string& field, std::string _default);

	// inner list
	FC_Font* _global_font;
	std::vector<std::string> ProgramArguments;
	AssetManager* assetManager;
	// scene
	Scene* _current_scene;
	std::string _primary_scene;
private:
	static Core _instance;
	SDL_Window* m_window;

#ifdef _DEBUG
	// all debug flags and variables
public:

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

