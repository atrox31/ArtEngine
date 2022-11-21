#pragma once

#pragma warning(push)
#include "SDL_version.h"
#include SDL2_INCLUDE_DIR
#include SDL2_IMAGE_INCLUDE_DIR
#include SDL2_MIXER_INCLUDE_DIR
#include SDL2_NET_INCLUDE_DIR
#include SDL2_TTF_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR
#pragma warning(pop)

#include "Console.h"
#include "physfs-3.0.2/src/physfs.h"
#include "plf_colony-master/plf_colony.h"
#include "Debug.h"

class Core
{
public:
	Core();
	virtual ~Core();
	static Core* GetInstance() { return Core::_instance; }
	static bool Init();
	static int Run();
	void Exit();

private:
	// graphic
	GPU_Target* _screenTarget;
public:
	static GPU_Target* GetScreenTarget() { return _instance->_screenTarget; }

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
		void SetScreenResolution(int w, int h) {
			_window_width = w;
			_window_height = h;
		};
		void SetFullScreen(bool value) {
			_window_fullscreen = value;
		};
		void SetFramerate(int framerate) {
			_window_framerate = framerate;
		};
		void SetVSync(bool vsync) {
			_window_vsync = vsync;
		};
		void Apply();
		int GetWindowWidth() {
			return _window_width;
		};
		int GetWindowHeight() {
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
		void SetAudioMasterLevel(float level) {
			_audio_master = level;
		};
		void SetAudioSoundLevel(float level) {
			_audio_music = level;
		};
		void SetAudioMusicLevel(float level) {
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
	double DeltaTime;
	int fps;
private:
	Uint64 NOW, LAST;
	int frames;

public:
	// getters
	SDL_Window* GetWindowHandle();
	int GetScreenWidth() {
		return Graphic.GetWindowWidth();
	}
	int GetScreenHeight() {
		return Graphic.GetWindowHeight();
	}

	enum class Event {
		EV_ONCREATE,

		EV_ONKEY_DOWN,
		EV_ONKEY_UP,

		EV_ONMOUSE_MOTION,
		EV_ONMOUSE_WHEEL,
		EV_ONMOUSE_DOWN,
		EV_ONMOUSE_UP,

		EV_ONCOLLISION,

		EV_ONVIEW_ENTER,
		EV_ONVIEW_LEAVE,

		EV_CLICKED,
		EV_TRIGGER,

		EV_STEP,
		EV_DRAW,
	};

	struct MouseState {
		enum class ButtonState {
			PRESSED, RELASED, NONE
		};
		ButtonState LeftEvent = ButtonState::NONE;
		ButtonState RightEvent = ButtonState::NONE;
		bool LeftPressed = false;
		bool RightPressed = false;
		int X = 0;
		int Y = 0;
		SDL_Point XY = { 0,0 };
		int WHELL = 0;
	} gMouse;


	std::map<std::string, std::string> SettingsData;
	static int SD_GetInt(std::string field, int _default);
	static float SD_GetFloat(std::string field, float _default);
	static std::string SD_GetString(std::string field, std::string _default);

	// font
	FC_Font* _global_font;
	std::vector<std::string> ProgramArguments;

	// members
	Console* Consola;
private:
	static Core* _instance;
	SDL_Window* m_window;
};

