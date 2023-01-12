#pragma once
#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "SettingsData.h"
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
	static bool Init(const Func::str_vec& args);
	static bool Run();
	static bool LoadData();
	static void Exit();

	bool ProcessEvents();
	void ProcessStep() const;
	void ProcessPhysics() const;
	void ProcessSceneRender() const;
	void ProcessPostProcessRender() const;
	void ProcessSystemRender() const;

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

private:
	// systems of handle game audio/video
	class graphic {
	public:
		graphic();
		// setters
		void SetScreenResolution(const int w, const int h);
		void SetFullScreen(const bool value);
		void SetFrameRate(const int frame_rate);
		void SetVSync(const bool v_sync);
		// getters
		[[nodiscard]] int GetWindowWidth() const;
		[[nodiscard]] int GetWindowHeight() const;
		Rect* GetScreenSpace();
		// apply
		void Apply();
	private:
		int _window_width;
		int _window_height;
		bool _window_fullscreen;
		int _window_frame_rate;
		bool _window_v_sync;
		Rect _screen_rect;
	};
public:
	inline static graphic Graphic;
private:
	class audio {
	public:
		audio();
		// setters
		void SetSoundLevel(const int level);
		void SetMusicLevel(const int level);
		void SetMaster(const bool& enabled);
		void SetSound(const bool& enabled);
		void SetMusic(const bool& enabled);
		// getters
		[[nodiscard]] int GetMusicLevel() const;
		[[nodiscard]] int GetSoundLevel() const;
		// apply
		void Apply() const;
	private:
		int _audio_music_level;
		int _audio_sound_level;
		bool _audio_master;
		bool _audio_music;
		bool _audio_sound;
	};
public:
	inline static audio Audio;

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
		
		// Current mouse coordinate int (x,y)
		SDL_Point XY = { 0,0 };
		// Current mouse coordinate float (x,y)
		SDL_FPoint XYf = { 0.f,0.f };
		// Current mouse wheel position
		int Wheel = 0;
		static void Reset();
	} inline static Mouse;

private:
	using program_argument = std::pair<const char*, const char*>;
	void PopulateArguments(const Func::str_vec&);
	program_argument GetProgramArgument(const std::string& argument);
	std::vector<program_argument> _program_arguments;

	static bool LoadSetupFile(const char*, const std::string&);

	FC_Font* _global_font;
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
		bool ProcessEvent(const SDL_Event* e);
		void Draw() const;
		void SetSpyLines(const int& lines)
		{
			_spy_line_max = lines;
			_spy_line_begin = 0;
			_spy_line_end = std::min(_spy_line_max,8);
		}

		void PerformanceTimeSecondPassed();

	private:
		bool _show_debug_options = false;
		int  _selected_debug_option = -1;
		bool _any_panel_shown = false;

		bool _show_performance = false;
	public:
		bool   _show_performance_times = false;
		double _performance_counter_all_rt{};
		double _performance_counter_step_rt{};
		double _performance_counter_psychics_rt{};
		double _performance_counter_post_process_rt{};
		double _performance_counter_render_rt{};
		double _performance_counter_gpu_flip_rt{};
	private:
		double _performance_counter_all{};
		double _performance_counter_other{};
		double _performance_counter_step{};
		double _performance_counter_psychics{};
		double _performance_counter_render{};
		double _performance_counter_post_process{};
		double _performance_counter_gpu_flip{};

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


