#include "Core.h"
#include "ArtCore/Graphic/Render.h"

Core::graphic::graphic() {
    _window_width = 0;
    _window_height = 0;
    _window_fullscreen = false;
    _window_frame_rate = 0;
    _window_v_sync = false;
}
void Core::graphic::SetScreenResolution(const int w, const int h) {
    _window_width = w;
    _window_height = h;
}
void Core::graphic::SetFullScreen(const bool value) {
    _window_fullscreen = value;
}
void Core::graphic::SetFrameRate(const int frame_rate) {
    _window_frame_rate = frame_rate;
}
void Core::graphic::SetVSync(const bool v_sync) {
    _window_v_sync = v_sync;
}
[[nodiscard]] int Core::graphic::GetWindowWidth() const
{
    return _window_width;
}
[[nodiscard]] int Core::graphic::GetWindowHeight() const
{
    return _window_height;
}
void Core::graphic::Apply() const
{
    GPU_SetFullscreen(_window_fullscreen, false);
    GPU_SetWindowResolution(static_cast<Uint16>(_window_width), static_cast<Uint16>(_window_height));
    if (!_window_fullscreen)
        SDL_SetWindowPosition(Core::GetWindowHandle(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    if (_window_v_sync == 0) {
        SDL_GL_SetSwapInterval(0);
    }
    else {
        SDL_GL_SetSwapInterval(1);
    }
    Render::CreateRender(_window_width, _window_height);

    SettingsData::SetValue("ACWindowMode", _window_fullscreen ? "1" : "0");
    SettingsData::SetValue("ACWindowResolutionX", std::to_string(_window_width));
    SettingsData::SetValue("ACWindowResolutionY", std::to_string(_window_height));
}








Core::audio::audio() {
    _audio_music_level = 0;
    _audio_sound_level = 0;
    _audio_master = true;
    _audio_music = true;
    _audio_sound = true;
}

// setters
void Core::audio::SetSoundLevel(const int level) {
    _audio_sound_level = std::clamp(level, 0, 100);
}
void Core::audio::SetMusicLevel(const int level) {
    _audio_music_level = std::clamp(level, 0, 100);
}
void Core::audio::SetMaster(const bool& enabled)
{
    _audio_master = enabled;
}
void Core::audio::SetSound(const bool& enabled)
{
    _audio_sound = enabled;
}
void Core::audio::SetMusic(const bool& enabled)
{
    _audio_music = enabled;
}
// getters
[[nodiscard]] int Core::audio::GetMusicLevel() const
{
	return (_audio_master && _audio_music) ? _audio_music_level : 0;
}
[[nodiscard]] int Core::audio::GetSoundLevel() const
{
	return (_audio_master && _audio_sound) ? _audio_sound_level : 0;
}
void Core::audio::Apply() const
{
    Mix_MasterVolume(
        ((_audio_master && _audio_sound) ? static_cast<int>(Func::LinearScale(
            static_cast<float>(_audio_sound_level),
            0.f,
            100.f,
            0.f,
            static_cast<float>(MIX_MAX_VOLUME)
        ))
            : 0));

    Mix_VolumeMusic(
        ((_audio_master && _audio_music) ? static_cast<int>(Func::LinearScale(
            static_cast<float>(_audio_music_level),
            0.f,
            100.f,
            0.f,
            static_cast<float>(MIX_MAX_VOLUME)
        ))
            : 0));


    SettingsData::SetValue("ACMusicLevel", std::to_string(_audio_music_level));
    SettingsData::SetValue("ACMusicLevel", std::to_string(_audio_music_level));
    SettingsData::SetValue("ACSoundEnabled", _audio_sound ? "1" : "0");
    SettingsData::SetValue("ACMusicEnabled", _audio_music ? "1" : "0");
}
