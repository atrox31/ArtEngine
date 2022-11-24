#include "Core.h"
#include "Func.h"
#include "ColorDefinitions.h"
#include "SplashScreen.h"
#include "Console.h"
#include "Render.h"
#include "Event.h"

Core Core::_instance = Core::Core();
Core::graphic Core::Graphic = Core::graphic();
Core::audio Core::Audio = Core::audio();

void Core::graphic::Apply()
{
    GPU_SetFullscreen(_window_fullscreen, false);
    GPU_SetWindowResolution((Uint16)_window_width, (Uint16)_window_height);
    if (!_window_fullscreen)
        SDL_SetWindowPosition(_instance.GetWindowHandle(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    if (_window_vsync == 0) {
        SDL_GL_SetSwapInterval(0);
    }
    else {
        SDL_GL_SetSwapInterval(1);
    }
    Render::CreateRender(_window_width, _window_height);
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
    _current_scene = nullptr;
    _show_fps = false;
    SettingsData = std::map<std::string, std::string>();
    _scene_list = std::vector<Scene*>();
    assetManager = new AssetManager();
}

Core::~Core()
{
    assetManager->ClearData();
    delete assetManager;
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

#define get_next (i+1<argc?args[++i]:"");continue;
#include "main.h"
bool Core::Init(int argc, char* args[])
{
    // flags
    const char* FL_game_dat_file = "game.dat";
    const char* FL_assets_file = "assets.pak";
    // args
    for (int i = 1; i < argc; i++) {
        if (args[i] == "-game_dat") FL_game_dat_file = get_next
        if (args[i] == "-assets") FL_assets_file = get_next

        if (args[i] == "-version") {
            std::cout << std::to_string(VERSION_MAIN) + '.' + std::to_string(VERSION_MINOR)+ '.' + std::to_string(VERSION_PATH) << std::endl;
        }
    }
    
    Debug::LOG("ArtCore v" + std::to_string(VERSION_MAIN) + '.' + std::to_string(VERSION_MINOR));
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        Debug::ERROR({ "sdl_error: ", SDL_GetError() });
        return false;
    }Debug::LOG("SDL_Init");

    if (!PHYSFS_init(NULL)) {
        Debug::ERROR({ "For some reason cant init archive reader libray... game not be able to run, sorry. Reason: " , PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) });
        return false;
    }Debug::LOG("PHYSFS_init");
    // primary game data
    if (!PHYSFS_mount(FL_game_dat_file, NULL, 0))
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
                        _instance.SettingsData.insert(std::make_pair(line[0], line[1]));
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

    _instance._screenTarget = GPU_Init(255, 255, GPU_INIT_DISABLE_VSYNC);
    if (_instance._screenTarget == NULL) {
        Debug::SDL_ERROR("GPU_Init: ");
        return false;
    }Debug::LOG("GPU_Init");

    SDL_SetWindowBordered(SDL_GetWindowFromID(_instance._screenTarget->context->windowID), SDL_FALSE);
    SDL_SetHint(SDL_HINT_RENDER_LINE_METHOD, "3");

    // splash screen for core loading
    GPU_Clear(_instance._screenTarget);
    GPU_Image* splash = GPU_CopyImageFromSurface(SDL_LoadBMP_RW(SDL_RWFromConstMem(eula, 8190), 1));
    if (splash == NULL)
    {
        Debug::SDL_ERROR("loading splah image:");
    }
    else
    {
        GPU_BlitRect(splash, NULL, _instance._screenTarget, NULL);
        GPU_Flip(_instance._screenTarget);
        GPU_Clear(_instance._screenTarget);
    }
    GPU_FreeImage(splash);
    splash = nullptr;
    Debug::LOG("Splash screen");

    if (!PHYSFS_mount(FL_assets_file, NULL, 0))
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
        _instance._global_font = FC_CreateFont();
        if (buf != nullptr)
            FC_LoadFont_RW(_instance._global_font, SDL_RWFromConstMem(buf, (int)len), 1, 24, C_BLACK, TTF_STYLE_NORMAL);
    }Debug::LOG("TitilliumWeb-Light.ttf");

    srand((unsigned int)time(NULL));

    _instance.DeltaTime = 0.0;
    _instance.NOW = SDL_GetTicks64();
    _instance.LAST = 0;
    _instance.frames = 0;
    _instance.Consola = new Console();

    //Render::CreateRender(SD_GetInt("DefaultResolution_x", 1920), SD_GetInt("DefaultResolution_y", 1080));
    Graphic.SetScreenResolution(SD_GetInt("DefaultResolution_x", 1920), SD_GetInt("DefaultResolution_y", 1080));
    Graphic.SetFramerate(SD_GetInt("DefaultFramerate", 60));
    Graphic.SetFullScreen(SD_GetInt("FullScreen", 0) == 1);
    //Graphic.Apply();

    Debug::LOG("rdy");
    return true;
}


SDL_Window* Core::GetWindowHandle()
{
    if (_instance._screenTarget) {
        return SDL_GetWindowFromID(_instance._screenTarget->context->windowID);
    }
    return nullptr;
}

int Core::SD_GetInt(std::string field, int _default)
{
    if (_instance.SettingsData.find(field) != _instance.SettingsData.end()) {
        return std::stoi(_instance.SettingsData[field]);
    }
    return _default;
}

float Core::SD_GetFloat(std::string field, float _default)
{
    if (_instance.SettingsData.find(field) != _instance.SettingsData.end()) {
        return std::stof(_instance.SettingsData[field]);
    }
    return _default;
}

std::string Core::SD_GetString(std::string field, std::string _default)
{
    if (_instance.SettingsData.find(field) != _instance.SettingsData.end()) {
        return _instance.SettingsData[field];
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
    SDL_TimerID my_timer_id = SDL_AddTimer((Uint32)1000, FpsCounterCallback, NULL);
    while (true) {
        //Art::Core::GetInstance()->Run_prepare();
        _instance.LAST = _instance.NOW;
        _instance.NOW = SDL_GetTicks64();
        _instance.DeltaTime = ((double)(_instance.NOW - _instance.LAST) / 1000.0);
        _instance.frames++;

        SDL_GetMouseState(&_instance.gMouse.X, &_instance.gMouse.Y);
        _instance.gMouse.XY = { _instance.gMouse.X, _instance.gMouse.Y };

        _instance.gMouse.LeftPressed = SDL_GetMouseState(NULL, NULL) == SDL_BUTTON(SDL_BUTTON_LEFT);
        _instance.gMouse.RightPressed = SDL_GetMouseState(NULL, NULL) == SDL_BUTTON(SDL_BUTTON_RIGHT);
        _instance.gMouse.WHELL = 0;
        _instance.gMouse.LeftEvent = Core::MouseState::ButtonState::NONE;
        _instance.gMouse.RightEvent = Core::MouseState::ButtonState::NONE;

        //if (Art::Core::GetInstance()->Run_events()) break;
        bool Ev_OnMouseInput = false;
        bool Ev_OnKeyboardInput = false;
        bool Ev_OnControllerInput = false;

        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
            case SDL_QUIT:
                Debug::LOG("exit request");
                return true;
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {

                }
                break;

            case SDL_CONTROLLERDEVICEADDED:

                break;

            case SDL_CONTROLLERDEVICEREMOVED:

                break;

            case SDL_RENDER_TARGETS_RESET:/**< The render targets have been reset and their contents need to be updated */
            case SDL_RENDER_DEVICE_RESET: /**< The device has been reset and all textures need to be recreated */

                break;

            case SDL_CONTROLLERAXISMOTION:
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                if (_instance.Consola->IsShown()) break;
                /*
                if (!_current_scene->_events[Art::Scene::Event::OnControllerInput].empty()) {
                    for (auto& ev : _current_scene->_events[Art::Scene::Event::OnControllerInput]) {
                        if(ev->Alive)
                        ev->Ev_OnControllerInput();
                    }
                }
                */
                break;

            case SDL_MOUSEWHEEL:
                if (_instance.Consola->IsShown()) break;
                /*
                if (!_current_scene->_events[Art::Scene::Event::OnMouseInput].empty()) {
                    for (auto& ev : _current_scene->_events[Art::Scene::Event::OnMouseInput]) {
                        gMouse.WHELL = e.wheel.y;
                        if (ev->Alive)
                        ev->Ev_OnMouseInput();
                    }
                }
                */
                break;

            case SDL_MOUSEBUTTONDOWN:
            {
                if (_instance.Consola->IsShown()) break;
                if (e.button.button == SDL_BUTTON_LEFT) {
                    _instance.gMouse.LeftEvent = Core::MouseState::ButtonState::PRESSED;
                }
                if (e.button.button == SDL_BUTTON_RIGHT) {
                    _instance.gMouse.RightEvent = Core::MouseState::ButtonState::PRESSED;
                }
                Ev_OnMouseInput = true;
            }
            break;
            case SDL_MOUSEBUTTONUP: {
                if (_instance.Consola->IsShown()) break;
                Ev_OnMouseInput = true;
                if (e.button.button == SDL_BUTTON_LEFT) {
                    _instance.gMouse.LeftEvent = Core::MouseState::ButtonState::RELASED;
                }
                if (e.button.button == SDL_BUTTON_RIGHT) {
                    _instance.gMouse.RightEvent = Core::MouseState::ButtonState::RELASED;
                }
                Ev_OnMouseInput = true;
            }
            break;

            case SDL_TEXTINPUT:
                if (_instance.Consola->IsShown()) {
                    _instance.Consola->ProcessTextInput(e.text.text);
                }
                break;

            case SDL_KEYDOWN:
                if (!_instance.Consola->ProcessKey((SDL_KeyCode)e.key.keysym.sym)) {
                    Ev_OnKeyboardInput = true;
                }
                break;
            case SDL_KEYUP:
                if (_instance.Consola->IsShown()) break;
                    Ev_OnKeyboardInput = true;
                break;
            }
        }
        //Art::Core::GetInstance()->Run_sceneStep();

        //Art::Core::GetInstance()->Run_sceneDraw();

        //Art::Render::RenderToTarget(Art::Core::GetScreenTarget());
        Render::RenderToTarget(Core::GetScreenTarget());
        
        //Art::Render::RenderClear();
        Render::RenderClear();
        
        // draw console
        if (_instance.Consola->IsShown()) {
            _instance.Consola->RenderConsole();
        }
        //Art::Core::GetInstance()->Draw_FPS();
        if (_instance._show_fps) {
            GPU_ActivateShaderProgram(0, NULL);

            GPU_Rect rect = FC_GetBounds(_instance._global_font, 2, 2, FC_ALIGN_LEFT, FC_Scale(1, 1), "%d", _instance.fps);

            GPU_RectangleFilled2(_instance._screenTarget, rect, C_BLACK);
            FC_DrawColor(_instance._global_font, _instance._screenTarget, 2, 2, C_RED, "%d", _instance.fps);
        }
        //Art::Core::GetInstance()->ShowScreen();
        GPU_Flip(_instance._screenTarget);
        GPU_ClearColor(_instance._screenTarget, C_LGRAY);
    }


    return EXIT_SUCCESS;
}
#include "CodeExecutor.h"
bool Core::LoadData()
{
    SDL_SetWindowBordered(SDL_GetWindowFromID(_instance._screenTarget->context->windowID), SDL_TRUE);
    Graphic.Apply();
    GPU_Clear(GetScreenTarget());
    GPU_Flip(GetScreenTarget());

    BackGroundRenderer bgr = BackGroundRenderer();
    bgr.Run();

    if (!_instance.Executor.LoadArtLib()) {
        bgr.Stop();
        return false;
    }
    bgr.SetProgress(10);
    
    if (!_instance.Executor.LoadObjectDefinitions()) {
        bgr.Stop();
        return false;
    }
    bgr.SetProgress(20);

    // load assets
    if (!Core::GetInstance()->assetManager->LoadData(&bgr, 20, 80)) {
        bgr.Stop();
        return false;
    }
    bgr.SetProgress(80);


    bgr.SetProgress(100);
    bgr.Stop();
    return true;
}

void Core::Exit()
{
    SDL_Event* sdlevent = new SDL_Event();
    sdlevent->type = SDL_QUIT;
    SDL_PushEvent(sdlevent);
}
