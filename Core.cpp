#include "Core.h"
#include "Func.h"
#include "ColorDefinitions.h"
#include "SplashScreen.h"
#include "Console.h"
#include "Render.h"
#include "Event.h"
#include "CodeExecutor.h"
#include "Panel.h"

Core Core::_instance = Core::Core();
Core::graphic Core::Graphic = Core::graphic();
Core::audio Core::Audio = Core::audio();

void Core::graphic::Apply() const
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
    game_loop = false;
    m_window = nullptr;
    Consola = nullptr;
    _screenTarget = nullptr;
    DeltaTime = 0.0;
    LAST = 0;
    NOW = 0;
    _global_font = nullptr;
    _instance.Consola = new Console();
    fps = 0;
    _frames = 0;
    _current_scene = nullptr;
    _show_fps = false; 
    assetManager = new AssetManager();
}

Core::~Core()
{
    Executor.Delete();
    Render::DestroyRender();
    assetManager->ClearData();
    delete assetManager;
    GPU_FreeTarget(_screenTarget);
    SettingsData.clear();
    for(const auto scene : _scene_list)
    {
        delete scene;
    }
    _scene_list.clear();
    /// <summary>
    /// After this point not alllow DEBUG::
    /// </summary>
    delete Consola;

    if(_global_font!=nullptr)
    FC_FreeFont(_global_font);
    
    if (SDL_WasInit(0) != 0) {
        GPU_Quit();
        IMG_Quit();
        TTF_Quit();
        Mix_CloseAudio();
        SDLNet_Quit();
        SDL_Quit();
    }
}
#include "main.h"
bool Core::Init(int argc, char* args[])
{
    Debug::LOG("start");
    // flags
#ifdef _DEBUG
    const char* FL_game_dat_file = "test\\game.dat";
    const char* FL_assets_file = "test\\assets.pak";
#else //release
    const char* FL_game_dat_file = "game.dat";
    const char* FL_assets_file = "assets.pak";
#endif// _DEBUG

    // args
    for (int i = 1; i < argc; i++) {
        std::string argument(args[i]);
        if (argument == "-game_dat") {
            if (i + 1 < argc) {
                FL_game_dat_file = args[i+1];
                Debug::LOG( "FL_game_dat_file set to: '" + std::string(FL_game_dat_file) + "'");
                ++i;
            }
            else {
                Debug::WARNING("FL_game_dat_file error, wrong arg");
            }
        }
        if (argument == "-assets") {
            if (i + 1 < argc) {
                FL_assets_file = args[i + 1];
                Debug::LOG("FL_assets_file set to: '" + std::string(FL_assets_file) + "'");
                ++i;
            }
            else {
                Debug::WARNING("FL_assets_file error, wrong arg");
            }
        }

        if (argument == "-version") {
            std::cout << std::to_string(VERSION_MAIN) + '.' + std::to_string(VERSION_MINOR)+ '.' + std::to_string(VERSION_PATH) << std::endl;
        }
        if (argument == "-debug") {
            std::cout << "debug mode" << std::endl;
            Debug::SetOutputFile("console.log");
        }
    }
    
    Debug::LOG("ArtCore v" + std::to_string(VERSION_MAIN) + '.' + std::to_string(VERSION_MINOR));
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        Debug::ERROR({ "sdl_error: ", SDL_GetError() });
        return false;
    }Debug::LOG("SDL_Init");

    if (!PHYSFS_init(args[0])) {
        Debug::ERROR({ "For some reason cant init archive reader libray... game not be able to run, sorry. Reason: " , PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) });
        return false;
    }Debug::LOG("PHYSFS_init");
    // primary game data
    if (!PHYSFS_mount(FL_game_dat_file, NULL, 0))
    {
        std::string err(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        Debug::ERROR( "Error when reading 'game.dat'. Reason: " + err + "\n" + FL_game_dat_file);
        return false;
    }Debug::LOG("PHYSFS_mount game.dat");
    // read data from setup.ini
    {
        std::string string_data = std::string(Func::GetFileBuf("setup.ini", nullptr));

        for (std::string data : Func::Split(string_data, '\n')) {
            if (data.substr(0, 2) == "//") continue;
            std::vector<std::string> line = Func::Split(data, '=');
            if (line.size() > 0) {
                if (line.size() == 2) {
                    if (line[0].substr(0, 2) == "SDL_GL_") {
                        bool error = false;
                        SDL_GLattr attr = Func::get_sdl_attr_from_string(line[0], &error);
                        if (!error) {
                            if (SDL_GL_SetAttribute(attr, Func::TryGetInt(line[1])) != 0) {
                                Debug::WARNING({ "sdl_error: -SDL_GL_SetAttribute-", SDL_GetError() });
                            }
                        }
                        else {
                            Debug::WARNING("unknown property '" + data + "'");
                        }
                    }
                    else if (line[0].substr(0, 2) == "SDL_") {
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
        Debug::ERROR( "Error when reading 'assets.pak'. Reason: " + std::string(PHYSFS_getLastError()) );
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
    }Debug::LOG("gamecontrollerdb.txt load");
    {
        Sint64 len = 0;
        const char* buf = Func::GetFileBuf("files/TitilliumWeb-Light.ttf", &len);
        _instance._global_font = FC_CreateFont();
        if (buf != nullptr)
            FC_LoadFont_RW(_instance._global_font, SDL_RWFromConstMem(buf, (int)len), 1, 24, C_BLACK, TTF_STYLE_NORMAL);
    }Debug::LOG("TitilliumWeb-Light.ttf load");

    srand((unsigned int)time(NULL));

    _instance.Consola->Init();

    _instance.DeltaTime = 0.0;
    _instance.NOW = SDL_GetTicks64();
    _instance.LAST = 0;
    _instance._frames = 0;
    FC_SetDefaultColor(_instance._global_font, C_BLACK);
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

int Core::SD_GetInt(const std::string field, const int _default)
{
    if (_instance.SettingsData.find(field) != _instance.SettingsData.end()) {
        return Func::TryGetInt(_instance.SettingsData[field]);
    }
    return _default;
}

float Core::SD_GetFloat(const std::string field, const float _default)
{
    if (_instance.SettingsData.find(field) != _instance.SettingsData.end()) {
        return Func::TryGetFloat(_instance.SettingsData[field]);
    }
    return _default;
}

std::string Core::SD_GetString(const std::string field, std::string _default)
{
    if (_instance.SettingsData.find(field) != _instance.SettingsData.end()) {
        return _instance.SettingsData[field];
    }
    return _default;
}

Uint32 Core::FpsCounterCallback(Uint32 interval, void* parms)
{
    Core::GetInstance()->fps = Core::GetInstance()->_frames;
    Core::GetInstance()->_frames = 0;

    return interval;
}

int Core::Run()
{
#ifdef _DEBUG
    bool Debug_ShowInfo = false;
    bool Debug_ShowStats = false;
#endif // _DEBUG
    Render::LoadShaders();
    bool use_bloom = false;
    _instance.game_loop = true;
    SDL_TimerID my_timer_id = SDL_AddTimer((Uint32)1000, FpsCounterCallback, NULL);
    while (true) {
        if (_instance._current_scene == nullptr) return EXIT_FAILURE;
        //Art::Core::GetInstance()->Run_prepare();
        _instance.LAST = _instance.NOW;
        _instance.NOW = SDL_GetTicks64();
        _instance.DeltaTime = ((double)(_instance.NOW - _instance.LAST) / 1000.0);
        _instance._frames++;

        const Uint32 ButtonState = SDL_GetMouseState(&_instance.Mouse.X, &_instance.Mouse.Y);
        _instance.Mouse.XY = { _instance.Mouse.X, _instance.Mouse.Y };
        _instance.Mouse.XYf = { (float)_instance.Mouse.X,(float)_instance.Mouse.Y };

        _instance.Mouse.LeftPressed = (ButtonState == SDL_BUTTON(SDL_BUTTON_LEFT));
        _instance.Mouse.RightPressed = (ButtonState == SDL_BUTTON(SDL_BUTTON_RIGHT));
        _instance.Mouse.WHELL = 0;
        _instance.Mouse.LeftEvent = Core::MouseState::ButtonState::NONE;
        _instance.Mouse.RightEvent = Core::MouseState::ButtonState::NONE;

        //if (Art::Core::GetInstance()->Run_events()) break;
        bool Ev_Input = false;
        bool Ev_OnMouseInputDown = false;
        bool Ev_OnMouseInputUp = false;
        bool Ev_OnKeyboardInputDown = false;
        bool Ev_OnKeyboardInputUp = false;
        bool Ev_OnControllerInput = false;
        bool Ev_ClickedDone = false;

        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
            case SDL_QUIT:
                Debug::LOG("exit request");
                return EXIT_SUCCESS;
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
                        Mouse.WHELL = e.wheel.y;
                        if (ev->Alive)
                        ev->Ev_OnMouseInput();
                    }
                }
                */
                break;

            case SDL_MOUSEBUTTONDOWN:
            {
                Ev_Input = true;
                if (_instance.Consola->IsShown()) break;
                if (e.button.button == SDL_BUTTON_LEFT) {
                    _instance.Mouse.LeftEvent = Core::MouseState::ButtonState::PRESSED;
                }
                if (e.button.button == SDL_BUTTON_RIGHT) {
                    _instance.Mouse.RightEvent = Core::MouseState::ButtonState::PRESSED;
                }
                Ev_OnMouseInputDown = true;
				// interface (gui) events
                _instance._current_scene->GuiSystem.Events(&e);
            }
            break;
            case SDL_MOUSEBUTTONUP: {
                Ev_Input = true;
                if (_instance.Consola->IsShown()) break;
                if (e.button.button == SDL_BUTTON_LEFT) {
                    _instance.Mouse.LeftEvent = Core::MouseState::ButtonState::RELASED;
                }
                if (e.button.button == SDL_BUTTON_RIGHT) {
                    _instance.Mouse.RightEvent = Core::MouseState::ButtonState::RELASED;
                }
                Ev_OnMouseInputUp = true;
                // interface (gui) events
                _instance._current_scene->GuiSystem.Events(&e);
            }
            break;

            case SDL_TEXTINPUT:
                if (_instance.Consola->IsShown()) {
                    _instance.Consola->ProcessTextInput(e.text.text);
                }
                break;

            case SDL_KEYDOWN:
#ifdef _DEBUG
                if (e.key.keysym.sym == SDLK_F1) {
                    Debug_ShowInfo = !Debug_ShowInfo;
                }
                if (e.key.keysym.sym == SDLK_F2) {
                    Debug_ShowStats = !Debug_ShowStats;
                }
                if (e.key.keysym.sym == SDLK_F12) {
                    _instance._current_scene->Start();
                }
#endif // _DEBUG
                if (e.key.keysym.sym == SDLK_F5) {
                    use_bloom = !use_bloom;
                }
                if (e.key.keysym.sym == SDLK_F6) { // low
                    Render::SetGaussianProperties(4, 4, 0.0205f);
                }
                if (e.key.keysym.sym == SDLK_F7) { // medium
                    Render::SetGaussianProperties(8, 8, 0.0205f);
                }
                if (e.key.keysym.sym == SDLK_F8) { // high
                    Render::SetGaussianProperties(16, 16, 0.0205f);
                }
                if (e.key.keysym.sym == SDLK_F4) {
                    _instance._show_fps = !_instance._show_fps;
                    break;
                }

                if (!_instance.Consola->ProcessKey((SDL_KeyCode)e.key.keysym.sym)) {
                    Ev_Input = true;
                    Ev_OnKeyboardInputDown = true;
                }
                break;
            case SDL_KEYUP:
                if (_instance.Consola->IsShown()) break;
                    Ev_Input = true;
                    Ev_OnKeyboardInputUp = true;
                break;
            }
        }
        //Art::Core::GetInstance()->Run_sceneStep();
        if (_instance.game_loop) {
            _instance._current_scene->SpawnAll();
            if (_instance._current_scene->IsAnyInstances()) {
                for (plf::colony<Instance*>::iterator it = _instance._current_scene->InstanceColony.begin(); it != _instance._current_scene->InstanceColony.end();) {
                    Instance* cInstance = (*it);
                    if (cInstance->Alive) {
                        // step
                        _instance.Executor.ExecuteScript(cInstance, Event::EvStep);
                        const EventBit c_flag = cInstance->EventFlag;

                        // inview
                        if (cInstance->Alive) {
                            //TODO: implement camera system
                            Rect screen{ 0,0,_instance.GetScreenWidth(), _instance.GetScreenHeight() };
                            SDL_FPoint pos{ cInstance->PosX, cInstance->PosY };
                            const bool oldInView = cInstance->InView;
                            cInstance->InView = screen.PointInRect(pos);
                            if (cInstance->InView != oldInView) {
                                if (EventBitTest(EventBit::HAVE_VIEWCHANGE, c_flag)) {
                                    if (oldInView == true) { // be inside, now exit view
                                        _instance.Executor.ExecuteScript(cInstance, Event::EvOnViewLeave);
                                    }
                                    else {
                                        _instance.Executor.ExecuteScript(cInstance, Event::EvOnViewEnter);
                                    }
                                }
                            }
                        }

                        // collision
                        if (EventBitTest(EventBit::HAVE_COLLISION, c_flag)) {
                            for (Instance* instance : _instance._current_scene->InstanceColony) {
                                if (instance == cInstance) continue; // self
                                if (instance->Body.Type == Instance::BodyType::None) continue; // no collision mask
                                if (instance->CollideTest(cInstance)) {
                                    _instance._current_scene->CurrentCollisionInstance = instance;
                                    _instance._current_scene->CurrentCollisionInstanceId = instance->GetId();
                                    _instance.Executor.ExecuteScript(cInstance, Event::EvOnCollision);
                                    _instance._current_scene->CurrentCollisionInstance = nullptr;
                                    _instance._current_scene->CurrentCollisionInstanceId = -1;
                                }
                            }
                        }

                        // input
                        if (Ev_Input && cInstance->Alive) {
                            if (EventBitTest(EventBit::HAVE_MOUSE_EVENT, c_flag)) {
                                if (EventBitTest(EventBit::HAVE_MOUSE_EVENT_UP, c_flag) && Ev_OnMouseInputUp) {
                                    _instance.Executor.ExecuteScript(cInstance, Event::EvOnMouseUp);
                                }
                                if (EventBitTest(EventBit::HAVE_MOUSE_EVENT_DOWN, c_flag) && Ev_OnMouseInputDown) {
                                    _instance.Executor.ExecuteScript(cInstance, Event::EvOnMouseDown);
                                }
                                if (!Ev_ClickedDone && EventBitTest(EventBit::HAVE_MOUSE_EVENT_CLICK, c_flag) && Ev_OnMouseInputDown) {
                                    if (cInstance->CheckMaskClick(_instance.Mouse.XYf)) {
                                        _instance.Executor.ExecuteScript(cInstance, Event::EvClicked);
                                        Ev_ClickedDone = true;
                                    }
                                }
                            }

                        }
                        ++it;
                    }
                    else {
                        _instance.Executor.ExecuteScript(cInstance, Event::EvOnDestroy);
                        it = _instance._current_scene->DeleteInstance(it);
                    }
                } // step loop
            } // is any instance
        } // if game_loop
           


		// render scene background
        if (_instance._current_scene->BackGround.texture != nullptr) {
            Render::DrawTextureBox(_instance._current_scene->BackGround.texture, nullptr, nullptr);
        }else
        {
            GPU_ClearColor(_instance._screenTarget, _instance._current_scene->BackGround.color);
        }
        // draw all instances if in view (defined in step event)
        if (_instance._current_scene->IsAnyInstances()) {
            for (Instance* instance : _instance._current_scene->InstanceColony) {
                if (instance->InView) {
                    _instance.Executor.ExecuteScript(instance, Event::EvDraw);

                }
            }
        }
        
        // post process
        if(use_bloom)Render::ProcessImageWithGaussian();
        // draw interface
        _instance._current_scene->GuiSystem.Render();

// DEBUG DRAW
#ifdef _DEBUG
        if (Debug_ShowInfo) {
            if (_instance._current_scene->IsAnyInstances()) {
                for (Instance* instance : _instance._current_scene->InstanceColony) {

                    Render::DrawCircle({ instance->PosX, instance->PosY }, 4, C_BLACK);

                    if (instance->SelfSprite) {
                        Render::DrawCircle({ instance->PosX + instance->SelfSprite->GetCenterXRel(), instance->PosY + instance->SelfSprite->GetCenterYRel() }, 6, C_GOLD);
                    }
                    else {
                        Render::DrawCircle({ instance->PosX, instance->PosY }, 6, C_GOLD);
                    }

                    instance->DebugDrawMask();
                    instance->DebugDrawCollision();

                    Render::DrawText(
                        instance->Name + "#" + std::to_string(instance->GetId()) + "[" + std::to_string((int)instance->PosX) + "," + std::to_string((int)instance->PosY) + "]",
                        _instance._global_font,
                        { instance->PosX, instance->PosY },
                        C_RED
                    );
                }
            }
        }
        if (Debug_ShowStats) {
            Render::DrawTextAlign(
                "instance count: " + std::to_string(_instance._current_scene->GetInstancesCount()) + '\n' +
                "delta time: " + std::to_string(_instance.DeltaTime) + '\n' +
                "global stack size[capacity]: " + std::to_string(_instance.Executor.GetGlobalStackSize()) + '[' + std::to_string(_instance.Executor.GetGlobalStackSize()) + ']' + '\n'

                ,
                _instance._global_font,
                { 12.f, 40.f },
                C_RED,
                FC_ALIGN_LEFT
            );
        }
#endif // _DEBUG

        // draw all instances with post process
    	Render::RenderToTarget(_instance._screenTarget);

        //Art::Render::RenderClear();
        Render::RenderClear();
        
        // draw console
        if (_instance.Consola->IsShown()) {
            _instance.Consola->RenderConsole();
        }
        //Art::Core::GetInstance()->Draw_FPS();
        if (_instance._show_fps) {
            GPU_DeactivateShaderProgram();
            const GPU_Rect rect = FC_GetBounds(_instance._global_font, 2, 2, FC_ALIGN_LEFT, FC_Scale({ 1, 1 }), "%d", _instance.fps);
            GPU_RectangleFilled2(_instance._screenTarget, rect, C_BLACK);
            FC_DrawColor(_instance._global_font, _instance._screenTarget, 2, 2, C_RED, "%d", _instance.fps);
        }
        //Art::Core::GetInstance()->ShowScreen();
        GPU_Flip(_instance._screenTarget);
    }


    return EXIT_SUCCESS;
}

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
    bgr.SetProgress(5);
    
    // load assets
    if (!_instance.assetManager->LoadData(&bgr, 5, 60)) {
        bgr.Stop();
        return false;
    }
    bgr.SetProgress(60);

    // load scenes
    std::vector<std::string> scene_list(Func::GetFileText("scene/list.txt", nullptr, false));
    if (scene_list.size() == 0) return false;
    for (std::string scene : scene_list) {
        std::string fname = "scene/" + scene + "/" + scene + ".asd";
            if (PHYSFS_exists(fname.c_str())) {
                Scene* tmp_scene = new Scene();
                if (tmp_scene->Load(fname)) {
                    // copy scene
                    _instance._scene_list.push_back(tmp_scene);
                }
                else {
                    return false;
                }
            }
            else {
                Debug::WARNING("scene '" + fname + "' not exists!");
                return false;
            }
    }
    bgr.SetProgress(69);
    // if no scenes exit
    if (_instance._scene_list.size() == 0) {
        Debug::WARNING("there is no scenes!");
        return false;
    }
    bgr.SetProgress(70);


    if (!_instance.Executor.LoadObjectDefinitions()) {
        bgr.Stop();
        return false;
    }
    bgr.SetProgress(98);

    // set starting scene
    std::string start_scene = Func::GetFileText("scene/StartingScene.txt", nullptr, false)[0];
    for (auto& scene : _instance._scene_list) {
        if (scene->GetName() == start_scene) {
            if (!_instance.ChangeScene(start_scene)) {
                Debug::WARNING("starting scene '" + start_scene + "' not exists!");
            }
            break;
        }
    }

    bgr.SetProgress(99);
    // if error try set first scene
    if (_instance._current_scene == nullptr) {  
        if (!_instance.ChangeScene(_instance._scene_list[0]->GetName())) {
            Debug::WARNING("starting scene '" + start_scene + "' not exists!");
            return false;
        }
    }

    bgr.SetProgress(100);
    //SDL_Delay(1000);
    bgr.Stop();
    return true;
}

void Core::Exit()
{
    SDL_Event* sdlevent = new SDL_Event();
    sdlevent->type = SDL_QUIT;
    SDL_PushEvent(sdlevent);
}

bool Core::ChangeScene(std::string name)
{
    if (_instance._current_scene != nullptr) {
        // exit scene
        _current_scene->Exit();
    }
    if (_instance._scene_list.size() == 0) {
        Debug::WARNING("there is no scenes!");
        return false;
    }
    for (auto& scene : _instance._scene_list) {
        if (scene->GetName() == name) {
            
            _current_scene = scene;
            _current_scene->Start();

            return true;
        }
    }
    Debug::WARNING("starting scene '" + name + "' not exists!");
    return false;
}