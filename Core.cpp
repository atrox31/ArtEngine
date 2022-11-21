#include "Core.h"
#include "Func.h"
#include "ColorDefinitions.h"
#include "SplashScreen.h"
#include "Console.h"

Core* Core::_instance = nullptr;
Core::graphic Core::Graphic = Core::graphic();
Core::audio Core::Audio = Core::audio();

void Core::graphic::Apply()
{
    GPU_SetFullscreen(_window_fullscreen, false);
    GPU_SetWindowResolution((Uint16)_window_width, (Uint16)_window_height);
    if (!_window_fullscreen)
        SDL_SetWindowPosition(_instance->GetWindowHandle(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    if (_window_vsync == 0) {
        SDL_GL_SetSwapInterval(0);
    }
    else {
        SDL_GL_SetSwapInterval(1);
    }

}

void Core::audio::Apply()
{

}

Core::Core()
{
    _screenTarget = nullptr;
    DeltaTime = 0.0;
    LAST = 0;
    NOW = 0;
    _global_font = nullptr;
    Consola = nullptr;
    fps = 0;
    frames = 0;
    //_current_scene = nullptr;
    SettingsData = std::map<std::string, std::string>();
}

Core::~Core()
{
    GPU_FreeTarget(_screenTarget);
    delete Consola;
    SettingsData.clear();
    
    GPU_Quit();
    IMG_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    SDLNet_Quit();
    SDL_Quit();
}

bool Core::Init()
{
    _instance = new Core();
    Debug::LOG("ArtCore v0.3");
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        Debug::ERROR({ "sdl_error: ", SDL_GetError() });
        return false;
    }Debug::LOG("SDL_Init");

    if (!PHYSFS_init(NULL)) {
        Debug::ERROR({ "For some reason cant init archive reader libray... game not be able to run, sorry. Reason: " , PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) });
        return false;
    }Debug::LOG("PHYSFS_init");
    // primary game data
    if (!PHYSFS_mount("game.dat", NULL, 0))
    {
        Debug::ERROR({ "Error when reading 'game.dat'. Reason: " , PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) });
        return false;
    }Debug::LOG("PHYSFS_mount game.dat");
    // read data from setup.ini
    {
        std::string string_data = std::string(Func::GetFileBuf("setup.ini", nullptr));

        for (std::string data : Func::Split(string_data, '\n')) {
            if (data.starts_with("//")) continue;
            std::vector<std::string> line = Func::Split(data, '=');
            if (line.size() > 0) {
                if (line.size() == 2) {
                    if (line[0].starts_with("SDL_GL_")) {
                        bool error = false;
                        SDL_GLattr attr = Func::get_sdl_attr_from_string(line[0], &error);
                        if (!error) {
                            if (SDL_GL_SetAttribute(attr, std::stoi(line[1])) != 0) {
                                Debug::WARNING({ "sdl_error: -SDL_GL_SetAttribute-", SDL_GetError() });
                            }
                        }
                        else {
                            Debug::WARNING("unknown property '" + data + "'");
                        }
                    }
                    else if (line[0].starts_with("SDL_")) {
                        if (SDL_SetHint(line[0].c_str(), line[1].c_str()) == SDL_FALSE) {
                            Debug::WARNING({ "sdl_error: -SDL_SetHint-", SDL_GetError() });
                        }
                    }
                    else {
                        _instance->SettingsData.insert(std::make_pair(line[0], line[1]));
                    }
                }
                else {
                    Debug::WARNING("Cannot read property '" + data + "'");
                }
            }
        }
    }
    Debug::LOG("reading setup.ini");
#ifdef _DEBUG
    GPU_SetDebugLevel(GPU_DEBUG_LEVEL_MAX);
#endif // _DEBUG

    _instance->_screenTarget = GPU_Init(255, 255, GPU_INIT_DISABLE_VSYNC);
    if (_instance->_screenTarget == NULL) {
        Debug::SDL_ERROR("GPU_Init: ");
        return false;
    }Debug::LOG("GPU_Init");

    SDL_SetWindowBordered(SDL_GetWindowFromID(_instance->_screenTarget->context->windowID), SDL_FALSE);
    SDL_SetHint(SDL_HINT_RENDER_LINE_METHOD, "3");

    // splash screen for core loading
    GPU_Clear(_instance->_screenTarget);
    GPU_Image* splash = GPU_CopyImageFromSurface(SDL_LoadBMP_RW(SDL_RWFromConstMem(eula, 8190), 1));
    if (splash == NULL)
    {
        Debug::SDL_ERROR("loading splah image:");
    }
    else
    {
        GPU_BlitRect(splash, NULL, _instance->_screenTarget, NULL);
        GPU_Flip(_instance->_screenTarget);
        GPU_Clear(_instance->_screenTarget);
    }
    GPU_FreeImage(splash);
    splash = nullptr;
    Debug::LOG("Splash screen");

    if (!PHYSFS_mount("assets.pak", NULL, 0))
    {
        Debug::ERROR({ "Error when reading 'assets.pak'. Reason: " , PHYSFS_getLastError() });
        return false;
    }Debug::LOG("PHYSFS_mount assets.pak");

    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        Debug::SDL_ERROR("IMG_Init:");
        return false;
    } Debug::LOG("IMG_Init");

    // hd - 44100 
    if (Mix_Init(MIX_InitFlags::MIX_INIT_MP3 | MIX_InitFlags::MIX_INIT_OGG) == 0) {
        Debug::SDL_ERROR("Mix_Init:");
        return false;
    } Debug::LOG("Mix_Init");

    if (Mix_OpenAudio(Core::SD_GetInt("AUDIO_FREQ", 44100), AUDIO_S32LSB, 2, Core::SD_GetInt("AUDIO_CHUNKSIZE", 4096)) < 0)
    {
        Debug::SDL_ERROR("Mix_OpenAudio:");
        return false;
    }Debug::LOG("Mix_OpenAudio");

    if (TTF_Init() == -1) {
        Debug::SDL_ERROR("TTF_Init:");
        return false;
    }Debug::LOG("TTF_Init");

    if (SDLNet_Init() == -1) {
        Debug::SDL_ERROR("SDLNet_Init:");
        return false;
    }Debug::LOG("SDLNet_Init");

    {
        Sint64 len = 0;
        const char* buf = Func::GetFileBuf("files/gamecontrollerdb.txt", &len);
        if (buf != nullptr)
            SDL_GameControllerAddMappingsFromRW(SDL_RWFromConstMem(buf, (int)len), 1);
    }Debug::LOG("gamecontrollerdb.txt");
    {
        Sint64 len = 0;
        const char* buf = Func::GetFileBuf("files/TitilliumWeb-Light.ttf", &len);
        _instance->_global_font = FC_CreateFont();
        if (buf != nullptr)
            FC_LoadFont_RW(_instance->_global_font, SDL_RWFromConstMem(buf, (int)len), 1, 24, C_BLACK, TTF_STYLE_NORMAL);
    }Debug::LOG("TitilliumWeb-Light.ttf");

    srand((unsigned int)time(NULL));

    _instance->DeltaTime = 0.0;
    _instance->NOW = SDL_GetTicks64();
    _instance->LAST = 0;
    _instance->frames = 0;
    _instance->Consola = new Console();
    Debug::LOG("rdy");
    return true;
}


SDL_Window* Core::GetWindowHandle()
{
    if (_instance) {
        return SDL_GetWindowFromID(_instance->_screenTarget->context->windowID);
    }
    return nullptr;
}

int Core::SD_GetInt(std::string field, int _default)
{
    if (_instance->SettingsData.find(field) != _instance->SettingsData.end()) {
        return std::stoi(_instance->SettingsData[field]);
    }
    return _default;
}

float Core::SD_GetFloat(std::string field, float _default)
{
    if (_instance->SettingsData.find(field) != _instance->SettingsData.end()) {
        return std::stof(_instance->SettingsData[field]);
    }
    return _default;
}

std::string Core::SD_GetString(std::string field, std::string _default)
{
    if (_instance->SettingsData.find(field) != _instance->SettingsData.end()) {
        return _instance->SettingsData[field];
    }
    return _default;
}

Uint32 Core::FpsCounterCallback(Uint32 interval, void* parms)
{
    Core::GetInstance()->fps = Core::GetInstance()->frames;
    Core::GetInstance()->frames = 0;

    return interval;
}

int Core::Run()
{
    SDL_Delay(5000);
    return 0;
}

void Core::Exit()
{
    SDL_Event* sdlevent = new SDL_Event();
    sdlevent->type = SDL_QUIT;
    SDL_PushEvent(sdlevent);
}