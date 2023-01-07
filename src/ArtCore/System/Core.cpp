#include "Core.h"
#include "Core.h"

#include "ArtCore/Graphic/Render.h"
#include "ArtCore/System/AssetManager.h"
#include "ArtCore/_Debug/Debug.h"
#include "ArtCore/CodeExecutor/CodeExecutor.h"
#include "ArtCore/main.h" // for program version
#include "ArtCore/Graphic/BackGroundRenderer.h"
#include "ArtCore/Scene/Scene.h"
#include "ArtCore/Physics/Physics.h"

#include "ArtCore/predefined_headers/SplashScreen.h"
#include "ArtCore/Graphic/ColorDefinitions.h"

#include "physfs-3.0.2/src/physfs.h"

Core Core::_instance = Core();
Core::graphic Core::Graphic = graphic();
Core::audio Core::Audio = audio();
Core::MouseState Core::Mouse = MouseState();

void Core::graphic::Apply() const
{
    GPU_SetFullscreen(_window_fullscreen, false);
    GPU_SetWindowResolution((Uint16)_window_width, (Uint16)_window_height);
    if (!_window_fullscreen)
        SDL_SetWindowPosition(Core::GetWindowHandle(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
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
    _screenTarget = nullptr;
    DeltaTime = 0.0;
    LAST = 0;
    NOW = 0;
    _global_font = nullptr;
    fps = 0;
    _frames = 0;
    _current_scene = nullptr;
    _show_fps = false; 
    assetManager = new AssetManager();
    Executor = new CodeExecutor();
}

Core::~Core()
{
    Executor->Delete();
    Render::DestroyRender();
    assetManager->ClearData();
    delete assetManager;
    GPU_FreeTarget(_screenTarget);
    SettingsData.clear();
	if(_current_scene != nullptr)
	{
        _current_scene->Exit();
        delete _current_scene;
	}
    delete Executor;
    //Console::SaveToFile();
    Console::Exit();

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
bool Core::Init(int argc, char* args[])
{
    // flags
#ifdef _DEBUG
	#ifdef DEBUG_EDITOR
	    const char* fl_game_dat_file = "game.dat";
	    const char* fl_assets_file = "assets.pak";
	#else
		const char* fl_game_dat_file = "test\\game.dat";
		const char* fl_assets_file = "test\\assets.pak";
	#endif
#else //release
    const char* fl_game_dat_file = "game.dat";
    const char* fl_assets_file = "assets.pak";
#endif// _DEBUG
    // inicialize console to standard output
    Console::Create();
    // args
    for (int i = 1; i < argc; i++) {
        std::string argument(args[i]);
        if (argument == "-game_dat") {
            if (i + 1 < argc) {
                fl_game_dat_file = args[i+1];
                Console::WriteLine( "FL_game_dat_file set to: '" + std::string(fl_game_dat_file) + "'");
                ++i;
            }
            else {
                Console::WriteLine("FL_game_dat_file error, wrong arg");
            }
        }
        if (argument == "-assets") {
            if (i + 1 < argc) {
                fl_assets_file = args[i + 1];
                Console::WriteLine("FL_assets_file set to: '" + std::string(fl_assets_file) + "'");
                ++i;
            }
            else {
                Console::WriteLine("FL_assets_file error, wrong arg");
            }
        }

        if (argument == "-version") {
            std::cout << std::to_string(VERSION_MAIN) + '.' + std::to_string(VERSION_MINOR)+ '.' + std::to_string(VERSION_PATH) << std::endl;
        }
        if (argument == "-debug") {
            std::cout << "debug mode" << std::endl;
            Console::SetOutputFile("console.log");
        }
    }
    
    Console::WriteLine("ArtCore v" + std::to_string(VERSION_MAIN) + '.' + std::to_string(VERSION_MINOR));
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        Console::WriteLine({ "sdl_error: ", SDL_GetError() });
        return false;
    }Console::WriteLine("SDL_Init");

    if (!PHYSFS_init(args[0])) {
        Console::WriteLine({ "For some reason cant init archive reader libray... game not be able to run, sorry. Reason: " , PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) });
        return false;
    }Console::WriteLine("PHYSFS_init");
    // primary game data
    if (!PHYSFS_mount(fl_game_dat_file, nullptr, 0))
    {
        std::string err(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        Console::WriteLine( "Error when reading 'game.dat'. Reason: " + err + "\n" + fl_game_dat_file);
        return false;
    }Console::WriteLine("PHYSFS_mount game.dat");
    // read data from setup.ini
    {
        std::string string_data = std::string(Func::GetFileBuf("setup.ini", nullptr));

        for (std::string data : Func::Split(string_data, '\n')) {
            if (data.substr(0, 2) == "//") continue;
            std::vector<std::string> line = Func::Split(data, '=');
            if (!line.empty()) {
                if (line.size() == 2) {
                    if (line[0].substr(0, 2) == "SDL_GL_") {
                        bool error = false;
                        const SDL_GLattr attr = Func::GetSdlAttrFromString(line[0], &error);
                        if (!error) {
                            if (SDL_GL_SetAttribute(attr, Func::TryGetInt(line[1])) != 0) {
                                Console::WriteLine({ "sdl_error: -SDL_GL_SetAttribute-", SDL_GetError() });
                            }
                        }
                        else {
                            Console::WriteLine("unknown property '" + data + "'");
                        }
                    }
                    else if (line[0].substr(0, 2) == "SDL_") {
                        if (SDL_SetHint(line[0].c_str(), line[1].c_str()) == SDL_FALSE) {
                            Console::WriteLine({ "sdl_error: -SDL_SetHint-", SDL_GetError() });
                        }
                    }
                    else {
                        _instance.SettingsData.insert(std::make_pair(line[0], line[1]));
                    }
                }
                else {
                    Console::WriteLine("Cannot read property '" + data + "'");
                }
            }
        }
    }
    Console::WriteLine("reading setup.ini");
#ifdef _DEBUG
    GPU_SetDebugLevel(GPU_DEBUG_LEVEL_MAX);
#endif // _DEBUG

    _instance._screenTarget = GPU_Init(255, 255, GPU_INIT_DISABLE_VSYNC);
    if (_instance._screenTarget == nullptr) {
        Console::WriteLine("GPU_Init: " + std::string(SDL_GetError()));
        return false;
    }Console::WriteLine("GPU_Init");

    SDL_SetWindowBordered(SDL_GetWindowFromID(_instance._screenTarget->context->windowID), SDL_FALSE);
    SDL_SetHint(SDL_HINT_RENDER_LINE_METHOD, "3");

    // splash screen for core loading
    GPU_Clear(_instance._screenTarget);
    GPU_Image* splash = GPU_CopyImageFromSurface(SDL_LoadBMP_RW(SDL_RWFromConstMem(eula, 8190), 1));
    if (splash == nullptr)
    {
        Console::WriteLine("loading splash image:" + std::string(SDL_GetError()));
    }
    else
    {
        GPU_BlitRect(splash, nullptr, _instance._screenTarget, nullptr);
        GPU_Flip(_instance._screenTarget);
        GPU_Clear(_instance._screenTarget);
    }
    GPU_FreeImage(splash);
    splash = nullptr;
    Console::WriteLine("Splash screen");

    if (!PHYSFS_mount(fl_assets_file, nullptr, 0))
    {
        Console::WriteLine( "Error when reading 'assets.pak'. Reason: " + std::string(PHYSFS_getLastError()) );
        return false;
    }Console::WriteLine("PHYSFS_mount assets.pak");

    if (constexpr int img_flags = IMG_INIT_PNG | IMG_INIT_JPG; !(IMG_Init(img_flags) & img_flags))
    {
        Console::WriteLine("IMG_Init:" + std::string(SDL_GetError()));
        return false;
    } Console::WriteLine("IMG_Init");

    // hd - 44100 
    if (Mix_Init(MIX_InitFlags::MIX_INIT_MP3 | MIX_InitFlags::MIX_INIT_OGG) == 0) {
        Console::WriteLine("Mix_Init:" + std::string(SDL_GetError()));
        return false;
    } Console::WriteLine("Mix_Init");

    if (Mix_OpenAudio(Core::SD_GetInt("AUDIO_FREQ", 44100), AUDIO_S32LSB, 2, Core::SD_GetInt("AUDIO_CHUNKSIZE", 4096)) < 0)
    {
        Console::WriteLine("Mix_OpenAudio:" + std::string(SDL_GetError()));
        return false;
    }Console::WriteLine("Mix_OpenAudio");

    if (TTF_Init() == -1) {
        Console::WriteLine("TTF_Init:" + std::string(SDL_GetError()));
        return false;
    }Console::WriteLine("TTF_Init");

    if (SDLNet_Init() == -1) {
        Console::WriteLine("SDLNet_Init:" + std::string(SDL_GetError()));
        return false;
    }Console::WriteLine("SDLNet_Init");

    {
        Sint64 len = 0;
        const char* buf = Func::GetFileBuf("files/gamecontrollerdb.txt", &len);
        if (buf != nullptr)
            SDL_GameControllerAddMappingsFromRW(SDL_RWFromConstMem(buf, (int)len), 1);
    }Console::WriteLine("gamecontrollerdb.txt load");
    {
        Sint64 len = 0;
        const char* buf = Func::GetFileBuf("files/TitilliumWeb-Light.ttf", &len);
        _instance._global_font = FC_CreateFont();
        if (buf != nullptr)
            FC_LoadFont_RW(_instance._global_font, SDL_RWFromConstMem(buf, (int)len), 1, 24, C_BLACK, TTF_STYLE_NORMAL);
    }Console::WriteLine("TitilliumWeb-Light.ttf load");

    srand((unsigned int)time(nullptr));
    Console::Init();

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

    Console::WriteLine("rdy");
    return true;
}

bool Core::ProcessCoreKeys(Sint32 sym)
{
    if (sym == SDLK_INSERT) {
        _show_fps = !_show_fps;
        return true;
    }
	/*
    if (sym == SDLK_F9) {
        use_bloom = !use_bloom;
        return true;
    }
    
    if (sym == SDLK_F10) { // low
        Render::SetGaussianProperties(4, 4, 0.0205f);
        use_bloom_level = 1;
        return true;
    }
    if (sym == SDLK_F11) { // medium
        Render::SetGaussianProperties(8, 8, 0.0205f);
        use_bloom_level = 2;
        return true;
    }
    if (sym == SDLK_F8) { // high
        Render::SetGaussianProperties(16, 16, 0.0205f);
        use_bloom_level = 3;
        return true;
    }
    */
    return false;
}


SDL_Window* Core::GetWindowHandle()
{
    if (_instance._screenTarget) {
        return SDL_GetWindowFromID(_instance._screenTarget->context->windowID);
    }
    return nullptr;
}

void Core::MouseState::Reset()
{
    const Uint32 button_state = SDL_GetMouseState(&X, &Y);
    Mouse.XY = { X, Y };
    Mouse.XYf = { static_cast<float>(X),static_cast<float>(Y) };

    Mouse.LeftPressed = (button_state == SDL_BUTTON(SDL_BUTTON_LEFT));
    Mouse.RightPressed = (button_state == SDL_BUTTON(SDL_BUTTON_RIGHT));
    Mouse.Wheel = 0;
    Mouse.LeftEvent = ButtonState::NONE;
    Mouse.RightEvent = ButtonState::NONE;
}

int Core::SD_GetInt(const std::string& field, const int _default)
{
    if (_instance.SettingsData.contains(field)) {
        return Func::TryGetInt(_instance.SettingsData[field]);
    }
    return _default;
}

float Core::SD_GetFloat(const std::string& field, const float _default)
{
    if (_instance.SettingsData.contains(field)) {
        return Func::TryGetFloat(_instance.SettingsData[field]);
    }
    return _default;
}

std::string Core::SD_GetString(const std::string& field, std::string _default)
{
    if (_instance.SettingsData.contains(field)) {
        return _instance.SettingsData[field];
    }
    return _default;
}

Uint32 Core::FpsCounterCallback(Uint32 interval, void*)
{
    Core::GetInstance()->fps = Core::GetInstance()->_frames;
    Core::GetInstance()->_frames = 0;

    return interval;
}

bool Core::Run()
{
    _instance.game_loop = true;
    SDL_TimerID my_timer_id = SDL_AddTimer((Uint32)1000, FpsCounterCallback, nullptr);
    while (true) {
        if (_instance._current_scene == nullptr) return EXIT_FAILURE;
        // FPS measurement
        _instance.LAST = _instance.NOW;
        _instance.NOW = SDL_GetTicks64();
        DeltaTime = ((double)(_instance.NOW - _instance.LAST) / 1000.0);
        _instance._frames++;
        // Set global mouse state
        Mouse.Reset();
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
            if(e.type == SDL_QUIT)
            {
                Console::WriteLine("exit request");
                return true;
            }
#ifdef _DEBUG
			// core shortcuts, only on debug mode
            if (e.type == SDL_KEYDOWN) { 
                if (_instance.CoreDebug.ProcessEvent(&e)) break;
            }
#endif
            // console input
            if ((e.type == SDL_TEXTINPUT) || (e.type == SDL_KEYDOWN)) {
                if (Console::ProcessEvent(&e)) break;
            }

            switch (e.type) {
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
                if (e.button.button == SDL_BUTTON_LEFT) {
                    Mouse.LeftEvent = Core::MouseState::ButtonState::PRESSED;
                }
                if (e.button.button == SDL_BUTTON_RIGHT) {
                    Mouse.RightEvent = Core::MouseState::ButtonState::PRESSED;
                }
                Ev_OnMouseInputDown = true;
				// interface (gui) events
                _instance._current_scene->GuiSystem.Events(&e);
            }
            break;
            case SDL_MOUSEBUTTONUP: {
                Ev_Input = true;
                if (e.button.button == SDL_BUTTON_LEFT) {
                    Mouse.LeftEvent = Core::MouseState::ButtonState::RELEASED;
                }
                if (e.button.button == SDL_BUTTON_RIGHT) {
                    Mouse.RightEvent = Core::MouseState::ButtonState::RELEASED;
                }
                Ev_OnMouseInputUp = true;
                // interface (gui) events
                _instance._current_scene->GuiSystem.Events(&e);
            }
            break;
            
            case SDL_KEYDOWN:
                if (_instance.ProcessCoreKeys(e.key.keysym.sym)) break;
                
                    Ev_Input = true;
                    Ev_OnKeyboardInputDown = true;
                break;
            case SDL_KEYUP:
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
	                if (Instance* c_instance = (*it); c_instance->Alive) {
                        // step
                        _instance.Executor->ExecuteScript(c_instance, Event::EvStep);
                        const event_bit c_flag = c_instance->EventFlag;

                        // in view
                        if (c_instance->Alive) {
                            //TODO: implement camera system
                            Rect screen{ 0,0, Core::GetScreenWidth(), Core::GetScreenHeight() };
                            SDL_FPoint pos{ c_instance->PosX, c_instance->PosY };
                            const bool oldInView = c_instance->InView;
                            c_instance->InView = screen.PointInRect(pos);
                            if (c_instance->InView != oldInView) {
                                if (EVENT_BIT_TEST(event_bit::HAVE_VIEW_CHANGE, c_flag)) {
                                    if (oldInView == true) { // be inside, now exit view
                                        _instance.Executor->ExecuteScript(c_instance, Event::EvOnViewLeave);
                                    }
                                    else {
                                        _instance.Executor->ExecuteScript(c_instance, Event::EvOnViewEnter);
                                    }
                                }
                            }
                        }

                        // collision
                        if (EVENT_BIT_TEST(event_bit::HAVE_COLLISION, c_flag)) {
                            for (Instance* instance : _instance._current_scene->InstanceColony) {
                                if (Physics::CollisionTest(c_instance, instance)) {
                                    _instance._current_scene->CurrentCollisionInstance = instance;
                                    _instance._current_scene->CurrentCollisionInstanceId = instance->GetId();
                                    _instance.Executor->ExecuteScript(c_instance, Event::EvOnCollision);
                                    _instance._current_scene->CurrentCollisionInstance = nullptr;
                                    _instance._current_scene->CurrentCollisionInstanceId = -1;
                                }
                            }
                        }
                        
                        // input
                        if (Ev_Input && c_instance->Alive) {
                            if (EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT, c_flag)) {
                                if (EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT_UP, c_flag) && Ev_OnMouseInputUp) {
                                    _instance.Executor->ExecuteScript(c_instance, Event::EvOnMouseUp);
                                }
                                if (EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT_DOWN, c_flag) && Ev_OnMouseInputDown) {
                                    _instance.Executor->ExecuteScript(c_instance, Event::EvOnMouseDown);
                                }
                                if (!Ev_ClickedDone && EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT_CLICK, c_flag) && Ev_OnMouseInputDown) {
                                    if (c_instance->CheckMaskClick(Mouse.XYf)) {
                                        _instance.Executor->ExecuteScript(c_instance, Event::EvClicked);
                                        Ev_ClickedDone = true;
                                    }
                                }
                            }

                        }
                        ++it;
                    }
                    else {
                        _instance.Executor->ExecuteScript(c_instance, Event::EvOnDestroy);
                        delete (*it);
                        it = _instance._current_scene->DeleteInstance(it);
                    }
                } // step loop
            } // is any instance
        // execute all suspended code
        CodeExecutor::SuspendedCodeExecute();
        } // if game_loop

		// render scene background
        if (_instance._current_scene->BackGround.Texture != nullptr) {
            Render::DrawTextureBox(_instance._current_scene->BackGround.Texture, nullptr, nullptr);
        }else
        {
            Render::RenderClear(_instance._current_scene->BackGround.Color);
        }
        // draw all instances if in view (defined in step event)
        if (_instance._current_scene->IsAnyInstances()) {
            for (Instance* instance : _instance._current_scene->InstanceColony) {
                if (instance->InView) {
                    _instance.Executor->ExecuteScript(instance, Event::EvDraw);

                }
            }
        }
        
        // post process
        if(_instance.use_bloom)Render::ProcessImageWithGaussian();
        // draw interface
        _instance._current_scene->GuiSystem.Render();

        if (_instance._show_fps) {
            GPU_DeactivateShaderProgram();
            const std::string text = "FPS: " + std::to_string(_instance.fps);
            GPU_Rect info_rect = FC_GetBounds(_instance._global_font, 0, 0, FC_ALIGN_LEFT, FC_Scale{ 1.2f, 1.2f }, text.c_str());
            constexpr float info_rect_move = 8.f;
        	info_rect.x += info_rect_move;
            info_rect.y += info_rect_move;
            info_rect.w += info_rect_move;
            info_rect.h += info_rect_move;
        	Render::DrawRectFilled(info_rect, C_BLACK);
            Render::DrawRect(info_rect, C_DGREEN);
            Render::DrawTextAlign(text, _instance._global_font, { info_rect.x+4.f, info_rect.y }, C_DGREEN, FC_ALIGN_LEFT);
        }

        // draw all instances with post process
    	Render::RenderToTarget(_instance._screenTarget);

// DEBUG DRAW
#ifdef _DEBUG
        _instance.CoreDebug.Draw();
#endif // _DEBUG
        //Art::Render::RenderClear();
        Render::RenderClear();
        
        // draw console
        Console::RenderConsole(_instance._screenTarget);
        //Art::Core::GetInstance()->ShowScreen();
        GPU_Flip(_instance._screenTarget);
    }


    return false;
}

bool Core::LoadData()
{
    SDL_SetWindowBordered(SDL_GetWindowFromID(_instance._screenTarget->context->windowID), SDL_TRUE);
    Graphic.Apply();
    GPU_Clear(GetScreenTarget());
    GPU_Flip(GetScreenTarget());

    BackGroundRenderer bgr = BackGroundRenderer();
    bgr.Run();

    if (!_instance.Executor->LoadArtLib()) {
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

    if (!_instance.Executor->LoadObjectDefinitions(&bgr, 60, 90)) {
        bgr.Stop();
        return false;
    }
    bgr.SetProgress(90);

    // set starting scene
    _instance._primary_scene = Func::GetFileText("scene/StartingScene.txt", nullptr, false)[0];

    // if error try set first scene
    if (!_instance.ChangeScene(_instance._primary_scene)) {
        Console::WriteLine("starting scene '" + _instance._primary_scene + "' not exists!");
        bgr.Stop();
        return false;
    }
    Render::LoadShaders();
    bgr.SetProgress(100);

    bgr.Stop();
    return true;
}

void Core::Exit()
{
    SDL_Event* sdl_event = new SDL_Event();
    sdl_event->type = SDL_QUIT;
    SDL_PushEvent(sdl_event);
}

bool Core::ChangeScene(const std::string& name)
{
    if (_instance._current_scene != nullptr) {
        // exit scene
        _current_scene->Exit();
    }
    CodeExecutor::SuspendedCodeStop();

    Scene* new_scene = new Scene();
    if(new_scene->Load(name))
    {
        _current_scene = new_scene;
        if (_current_scene->Start()) {
            return true;
        }
        else
        {
            delete new_scene;
            Console::WriteLine("[Core::ChangeScene] Error while starting new scene.");
        }
    }else
    {
        delete new_scene;
        Console::WriteLine("[Core::ChangeScene] scene '" + name + "' not exists! Try to load primary scene");
        Scene* primary_scene = new Scene();
        if (primary_scene->Load(_primary_scene))
        {
            _current_scene = primary_scene;
            if (_current_scene->Start()) {
                return true;
            }
            else
            {
                delete primary_scene;
                Console::WriteLine("[Core::ChangeScene] Error while starting primary scene.");
            }
        }else
        {
            delete primary_scene;
            Console::WriteLine("[Core::ChangeScene] Error while load primary scene.");
        }
    }
    return false;
}
#ifdef _DEBUG
Core::CoreDebug::CoreDebug()
{
    Options.emplace_back( "Performance", &_show_performance, SDLK_F2);
    Options.emplace_back( "Draw Instance info", &_show_instance_info, SDLK_F3);
    Options.emplace_back( "Draw Collider`s", &_show_collider, SDLK_F4);
    Options.emplace_back( "Draw Direction`s", &_show_directions, SDLK_F5);
    Options.emplace_back( "Show spy window", &_show_spy_window, SDLK_F6);
}

bool Core::CoreDebug::ProcessEvent(SDL_Event* e)
{
    switch (e->type)
    {
    case SDL_MOUSEWHEEL:
	    {
		    
	    }
        break;
    case SDL_MOUSEBUTTONDOWN:
	    {

	    }
	    break;
    case SDL_KEYDOWN:
    {
        if (e->key.keysym.sym == SDLK_F1) {
            _show_debug_options = !_show_debug_options;
            return true;
        }
        for (option& option : Options)
        {
            if (e->key.keysym.sym == option.Key)
            {
                *option.Anchor = !*option.Anchor;
                return true;
            }
        }
        if (e->key.keysym.sym == SDLK_PAGEUP)
        {
            if (_spy_line_begin > 0) {
                _spy_line_begin -= 1;
                _spy_line_end -= 1;
            }
            return true;
        }
        if (e->key.keysym.sym == SDLK_PAGEDOWN)
        {
            if (_spy_line_end < _spy_line_max) {
                _spy_line_begin += 1;
                _spy_line_end += 1;
            }
            return true;
        }

    }
    break;
    }
    return false;
}

void Core::CoreDebug::Draw()
{
    if (_show_instance_info || _show_collider || _show_directions)
    {
        if (_instance._current_scene->IsAnyInstances()) {
            for (const Instance* instance : _instance._current_scene->InstanceColony) {
                GPU_Circle(_instance._screenTarget, instance->PosX, instance->PosY, 4, C_BLACK);
                // always draw origins point
            	if (instance->SelfSprite) {
                    GPU_Circle(_instance._screenTarget, instance->PosX + instance->SelfSprite->GetCenterXRel(), instance->PosY + instance->SelfSprite->GetCenterYRel(), 6, C_GOLD);
                }
                else {
                    GPU_Circle(_instance._screenTarget, instance->PosX, instance->PosY, 6, C_GOLD);
                }

                if(_show_directions)
                {
                    const float prv = GPU_SetLineThickness(4.f);
                    vec2f line_begin{ instance->PosX, instance->PosY };
                    vec2f line_end = Func::GetDirectionVector(instance->Direction);
                    line_end *= 42.f;
                    line_end += line_begin;
                    GPU_Line(_instance._screenTarget, line_begin.x, line_begin.y, line_end.x, line_end.y, C_DYELLOW);
                    GPU_SetLineThickness(prv);
                }

                if (_show_collider) {
                    if (instance->Body.Type == Instance::BodyType::Circle) {
                        const float radius_scale = instance->Body.Value * ((instance->SpriteScaleX + instance->SpriteScaleY) / 2.f);
                        GPU_Circle(_instance._screenTarget, instance->PosX, instance->PosY, radius_scale, C_BLUE);
                    }
                    if (instance->Body.Type == Instance::BodyType::Rect) {
                        GPU_Rectangle2(_instance._screenTarget, instance->GetBodyMask().ToGPU_Rect_wh(), C_BLUE);
                    }
                }

                if (_show_instance_info) {
                    const std::string text = instance->Name + "#" + std::to_string(instance->GetId()) + "[" + std::to_string((int)instance->PosX) + "," + std::to_string((int)instance->PosY) + "]";
                    GPU_Rect draw_surface = FC_GetBounds(_instance._global_font, 0.f, 0.f, FC_ALIGN_LEFT, { 1.f, 1.f }, text.c_str());
                    draw_surface.x = std::clamp(instance->PosX, 0.f, static_cast<float>(GetScreenWidth()) - draw_surface.w);
                    draw_surface.y = std::clamp(instance->PosY, 0.f, static_cast<float>(GetScreenHeight()) - draw_surface.h);
                    FC_DrawColor(_instance._global_font, _instance._screenTarget, draw_surface.x, draw_surface.y, C_RED, text.c_str());
                }
            }
        }
    }

    float draw_height = 0.f;

    if(_show_debug_options)
    {
        std::string output_text;
        for (auto& option : Options)
        {
            output_text += "[" + std::string((*option.Anchor) ? "X" : "  ") + "] " + option.Text + " | " + SDL_GetKeyName(option.Key)+ '\n';
        }
        GPU_Rect draw_surface = FC_GetBounds(_instance._global_font, 4.f, 14.f, FC_ALIGN_LEFT, { 1.f, 1.f }, output_text.c_str());

        draw_surface.y = 30.f;
        draw_surface.h += 30.f;

        const float moving = static_cast<float>(GetScreenWidth() - 20);
        draw_surface.x = moving - draw_surface.w;
        draw_surface.w += 18.f;

        GPU_RectangleFilled2(_instance._screenTarget, draw_surface, C_BLACK);
        GPU_Rectangle2(_instance._screenTarget, draw_surface, C_DGREEN);
        FC_DrawColor(_instance._global_font, _instance._screenTarget, draw_surface.x + 4.f, draw_surface.y + 10.f, C_DGREEN, output_text.c_str());

        GPU_Rect dev_text_frame = FC_GetBounds(_instance._global_font, 4.f, 4.f, FC_ALIGN_LEFT, { 1.2f, 1.2f }, "%s", "DEBUG MENU");
        dev_text_frame.x = draw_surface.x + 12.f;
        dev_text_frame.y = draw_surface.y - 28.f;
        GPU_RectangleFilled2(_instance._screenTarget, dev_text_frame, C_BLACK);
        GPU_Rectangle2(_instance._screenTarget, dev_text_frame, C_DGREEN);
        FC_DrawColor(_instance._global_font, _instance._screenTarget, dev_text_frame.x + 4.f, dev_text_frame.y + 4.f, C_DGREEN, "%s", "DEBUG MENU");
        draw_height += draw_surface.h + 8.f;
    }

    if(_show_performance)
    {
        const std::string text =
            "instance count[colony size]: " + std::to_string(_instance._current_scene->GetInstancesCount()) + '[' + std::to_string(_instance._current_scene->InstanceColony.size()) + ']' + '\n' +
            "delta time: " + std::to_string(_instance.DeltaTime) + '\n' +
            "Executor global stack size[capacity]: " + std::to_string(CodeExecutor::GetGlobalStackSize()) + '[' + std::to_string(CodeExecutor::GetGlobalStackSize()) + ']' + '\n' +
            "Executor if-test stack size: " + std::to_string(Core::GetInstance()->Executor->DebugGetIfTestResultStackSize()) + ']' + '\n' +
            "bloom draw: " + (_instance.use_bloom ? "enabled (" + std::to_string(_instance.use_bloom_level) + ")" : "disabled");

    	GPU_Rect info_rect = FC_GetBounds(_instance._global_font, 0.f, 0.f, FC_ALIGN_LEFT, FC_Scale{ 1.f, 1.f }, text.c_str());

        const float moving = static_cast<float>(GetScreenWidth() - 20);
        info_rect.x = moving - info_rect.w;
        info_rect.w += 18.f;

        info_rect.y += draw_height + 28.f;
        info_rect.h += 8.f;

        GPU_RectangleFilled2(_instance._screenTarget, info_rect, C_BLACK);
        GPU_Rectangle2(_instance._screenTarget, info_rect, C_DGREEN);
        FC_DrawColor(_instance._global_font, _instance._screenTarget, info_rect.x + 4, info_rect.y + 4, C_DGREEN, "%s", text.c_str());
        draw_height += info_rect.h + 8.f;
    }

    if(_show_spy_window)
    {
        const std::vector<std::string> text = Func::Split(_instance.Executor->DebugGetTrackInfo(), '\n');
        std::string sliced_text;

        for(int i= _spy_line_begin; i< _spy_line_end; i++)
        {
            sliced_text += text[i] + '\n';
        }
       
        GPU_Rect info_rect = FC_GetBounds(_instance._global_font, 0.f, 0.f, FC_ALIGN_LEFT, FC_Scale{ 1.f, 1.f }, sliced_text.c_str());

    	const float moving = static_cast<float>(GetScreenWidth() - 20);
        info_rect.x = moving - info_rect.w;
        info_rect.w += 18.f;

        info_rect.y += draw_height + 28.f;
        info_rect.h += 8.f;

		GPU_RectangleFilled2(_instance._screenTarget, info_rect, C_BLACK);
        GPU_Rectangle2(_instance._screenTarget, info_rect, C_DGREEN);
        FC_DrawColor(_instance._global_font, _instance._screenTarget,  info_rect.x + 4, info_rect.y + 4 , C_DGREEN, "%s", sliced_text.c_str());
    }
}
#endif