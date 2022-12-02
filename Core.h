#pragma once

#include "SDL_version.h"
#include SDL2_INCLUDE_DIR
#include SDL2_IMAGE_INCLUDE_DIR
#include SDL2_MIXER_INCLUDE_DIR
#include SDL2_NET_INCLUDE_DIR
#include SDL2_TTF_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR

#include "CodeExecutor.h"
#include "Console.h"
#include "physfs-3.0.2/src/physfs.h"
#include "plf_colony-master/plf_colony.h"
#include "Debug.h"
#include "Scene.h"
#include "ColorDefinitions.h"
#include "assetManager.h"
#include "BackGroundRenderer.h"

class Scene;
class AssetManager;
class Core
{
public:
	Core();
	virtual ~Core();
	static Core* GetInstance() { return &Core::_instance; }
	static bool Init(int argc, char* args[]);
	static int Run();
	void Exit();
	static bool LoadData();

private:
	bool game_loop;
public:
	static void Pause() {
		_instance.game_loop = false;
	}
	static void Play() {
		_instance.game_loop = true;
	}


	bool ChangeScene(std::string name);

private:
	// graphic
	GPU_Target* _screenTarget;
public:
	static GPU_Target* GetScreenTarget() { return _instance._screenTarget; }
public:
	CodeExecutor Executor;
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
	bool _show_fps;

public:
	// getters
	SDL_Window* GetWindowHandle();
	int GetScreenWidth() {
		return Graphic.GetWindowWidth();
	}
	int GetScreenHeight() {
		return Graphic.GetWindowHeight();
	}

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
		SDL_FPoint XYf = { 0.f,0.f };
		int WHELL = 0;
	} gMouse;


	std::map<std::string, std::string> SettingsData;
	static int SD_GetInt(std::string field, int _default);
	static float SD_GetFloat(std::string field, float _default);
	static std::string SD_GetString(std::string field, std::string _default);

	// inner list
	FC_Font* _global_font;
	std::vector<std::string> ProgramArguments;
	AssetManager* assetManager;
	// scene
	Scene* _current_scene;
	std::vector<Scene> _scene_list;

	// members
	Console* Consola;
private:
	static Core _instance;
	SDL_Window* m_window;
};

