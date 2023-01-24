// ReSharper disable CppUseAuto
#include "Core.h"

#include "ArtCore/Graphic/Render.h"
#include "ArtCore/System/AssetManager.h"
#include "ArtCore/CodeExecutor/CodeExecutor.h"
#include "ArtCore/main.h" // for program version
#include "ArtCore/Graphic/BackGroundRenderer.h"
#include "ArtCore/Scene/Scene.h"
#include "ArtCore/System/SettingsData.h"

#include "ArtCore/predefined_headers/SplashScreen.h"
#include "ArtCore/Graphic/ColorDefinitions.h"
#include "ArtCore/_Debug/Time.h"

#include "physfs-release-3.2.0/src/physfs.h"

#ifdef AC_ENABLE_DEBUG_MODE
// time of core events
#include "ArtCore/_Debug/Time.h"
#endif

// static core structure.
Core Core::_instance = Core();

// Core create
Core::Core()
{
    game_loop = false;
    _window = nullptr;
    _screenTarget = nullptr;
    DeltaTime = 0.0;
    LAST = 0;
    NOW = 0;
    _global_font = nullptr;
    fps = 0;
    _frames = 0;
    _show_fps = false; 
    _asset_manager = nullptr;
    _executor = nullptr;
}

// Core destroy
Core::~Core()
{
    if(_executor != nullptr)
		_executor->Delete();
    delete _executor;

    if(_asset_manager != nullptr)
		_asset_manager->ClearData();
    delete _asset_manager;

    Scene::Delete();

    Render::DestroyRender();

    if(_screenTarget != nullptr)
		GPU_FreeTarget(_screenTarget);
    //delete _screenTarget; heap error ?
    
    Console::Exit();

    PHYSFS_deinit();

    if (_global_font != nullptr)
        FC_FreeFont(_global_font);

    if (_window) {
        GPU_Quit();
    }
    IMG_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    SDLNet_Quit();

    SDL_QuitSubSystem(SDL_INIT_TIMER);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
    SDL_QuitSubSystem(SDL_INIT_HAPTIC);
    SDL_QuitSubSystem(SDL_INIT_EVENTS);

    SDL_Quit();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ArtCore normal work algorithm
//
// 1. Init(args) <- initialize all core systems and all needed libs. 
// 2. LoadData() <- load all asset, objects (instances) and try to load and run first scene
// 3. Run()      <- This is main program loop. Steps:
//  1. Fill mouse event data, count fps, prepare new frame
//  2. Process all system events
//  3. Step -> Update all instances
//      1. Create all new instances
//      2. Process ExStep event for all
//      3. Check if instance is in view
//      4. Execute all instance events if have any (like mouse click, keyboard, death)
//  4. Process Physics step like moving, collisions
//  5. Render scene to buffer
//  6. Apply post process if enabled
//  7. System render, normal only gui in debug all console or debug view
//  8. GPU_Flip <- show all on screen
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// macro to help load data in init
////////////////////////////////////////////////////////////////////////////////////
// try to initialize critic game system, break loading when error
#define TRY_TO_INIT_CRITIC(init, failure_msg, component)        \
 if (!(init)) {                                                 \
     Console::WriteLine(std::string( (component) )+": error."); \
     Console::WriteLine((failure_msg));                         \
     return false;                                              \
 } Console::WriteLine(std::string( (component) )+": success."); 
// try to initialize game system, continue after error
#define TRY_TO_INIT(init, failure_msg, component)                \
 if (!(init)) {                                                  \
     Console::WriteLine(std::string( (component) )+": warning. " + (failure_msg) );\
 } else {Console::WriteLine(std::string( (component) )+": success.");}
////////////////////////////////////////////////////////////////////////////////////

// init all game data and libs
bool Core::Init(const str_vec& args)
{
#ifdef AC_ENABLE_DEBUG_MODE
    // debug timer to time all events in game
    Time timer;
    timer.StartTest();
#endif

    // default assets path and name
#ifdef AC_ENABLE_DEBUG_MODE
    const char* fl_game_dat_file = "test\\game.dat";
    const char* fl_assets_file = "test\\assets.pak";
    const char* fl_platform_file = "test\\Platform.dat";
#else //release
    const char* fl_game_dat_file = "game.dat";
    const char* fl_assets_file = "assets.pak";
    const char* fl_platform_file = "Platform.dat";
#endif

    // get command line arguments
    _instance.PopulateArguments(args);
    // first output then exit application
    if (const program_argument argument = _instance.GetProgramArgument("-version"); argument.first != nullptr)
    {
        SDL_Log("%d.%d.%d", VERSION_MAIN, VERSION_MINOR, VERSION_PATH);
        return false;
    }

    Console::WriteLine(std::format("ArtCore v{}.{}\n", VERSION_MAIN, VERSION_MINOR));

    if (const program_argument argument = _instance.GetProgramArgument("-platform"); argument.second != nullptr)
    {
        fl_platform_file = argument.second;
        Console::WriteLine("Fl_platform_file set to: '" + std::string(fl_platform_file) + "'");
    }
    if (const program_argument argument = _instance.GetProgramArgument("-game_dat"); argument.second != nullptr)
    {
        fl_game_dat_file = argument.second;
        Console::WriteLine("FL_game_dat_file set to: '" + std::string(fl_game_dat_file) + "'");
    }
    if (const program_argument argument = _instance.GetProgramArgument("-assets"); argument.second != nullptr)
    {
        fl_assets_file = argument.second;
        Console::WriteLine("FL_assets_file set to: '" + std::string(fl_assets_file) + "'");
    }
    if (const program_argument argument = _instance.GetProgramArgument("-debug"); argument.first != nullptr)
    {
        Console::SetOutputFile(
            argument.second == nullptr ? "console.log"
            : argument.second
        );
    }
    // sdl subsystem
    TRY_TO_INIT_CRITIC(
        SDL_InitSubSystem(SDL_INIT_VIDEO) == 0,
        std::string(SDL_GetError()),
        "SDL_Init SDL_INIT_VIDEO"
    )

        TRY_TO_INIT_CRITIC(
            PHYSFS_init(args[0].c_str()) != 0,
            std::string(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())),
            "PHYSFS_init"
        )

        // platform configuration data
        TRY_TO_INIT(
            LoadSetupFile(fl_platform_file, "setup.ini"),
            "File not found, Using default settings",
            "Platform settings load"
        )

        // platform configuration data, can not exists
        TRY_TO_INIT(
            SettingsData::LoadUserData("setup_user.ini"),
            "File not found, Using default settings",
            "User settings load"
        )

#ifdef AC_ENABLE_DEBUG_MODE
        GPU_SetDebugLevel(GPU_DEBUG_LEVEL_MAX);
#else
        GPU_SetDebugLevel(GPU_DEBUG_LEVEL_0);
#endif // _DEBUG

    _instance._screenTarget = GPU_Init(255, 255, GPU_INIT_DISABLE_VSYNC);
    TRY_TO_INIT_CRITIC(
        _instance._screenTarget != nullptr,
        std::string(SDL_GetError()),
        "GPU_Init"
    )
        _instance._window = SDL_GetWindowFromID(_instance._screenTarget->context->windowID);
    {
        // splash screen for core loading
        SDL_SetWindowBordered(_instance._window, SDL_FALSE);
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
    }

    // SDL timer
    TRY_TO_INIT_CRITIC(
        SDL_InitSubSystem(SDL_INIT_TIMER) == 0,
        std::string(SDL_GetError()),
        "SDL_Init SDL_INIT_TIMER"
    )

        // SDL audio
        TRY_TO_INIT_CRITIC(
            SDL_InitSubSystem(SDL_INIT_AUDIO) == 0,
            std::string(SDL_GetError()),
            "SDL_Init SDL_INIT_AUDIO"
        )

        // SDL haptic - on mobile
        if (SettingsData::GetInt("ACGameUsingHaptic", 0) == 1) {
            TRY_TO_INIT_CRITIC(
                SDL_InitSubSystem(SDL_INIT_HAPTIC) == 0,
                std::string(SDL_GetError()),
                "SDL_Init SDL_INIT_HAPTIC"
            )
        }

    // SDL controller
    if (SettingsData::GetInt("ACGameUsingController", 0) == 1) {
        TRY_TO_INIT_CRITIC(
            SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) == 0,
            std::string(SDL_GetError()),
            "SDL_Init SDL_INIT_GAMECONTROLLER"
        )
            TRY_TO_INIT_CRITIC(
                SDL_GameControllerAddMappingsFromRW(Func::ArchiveGetFileRWops("files/gamecontrollerdb.txt", nullptr), 1) > -1,
                std::string(SDL_GetError()),
                "SDL_GameControllerAddMappings"
            )
    }

    // IMG_Init
    TRY_TO_INIT_CRITIC(
        IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG,
        std::string(SDL_GetError()),
        "IMG_Init"
    )

        // Mix_Init
        TRY_TO_INIT_CRITIC(
            Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) & (MIX_INIT_MP3 | MIX_INIT_OGG),
            std::string(SDL_GetError()),
            "Mix_Init"
        )

        // Mix_OpenAudio all sounds must be 32-bit integer samples and 44100 hz
        TRY_TO_INIT_CRITIC(
            Mix_OpenAudio(
                SettingsData::GetInt("ACAudioFreq", 44100),
                AUDIO_S32LSB,
                2,
                SettingsData::GetInt("ACAudioChunkSize", 4096)
            ) == 0,
            std::string(SDL_GetError()),
            "Mix_OpenAudio"
        )

        // Mix_Init
        TRY_TO_INIT_CRITIC(
            TTF_Init() == 0,
            std::string(SDL_GetError()),
            "TTF_Init"
        )

        // SDLNet_Init
        TRY_TO_INIT_CRITIC(
            SDLNet_Init() == 0,
            std::string(SDL_GetError()),
            "SDLNet_Init"
        )

        // primary game data
        TRY_TO_INIT_CRITIC(
            PHYSFS_mount(fl_game_dat_file, nullptr, 0) != 0,
            std::string(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())),
            "PHYSFS_mount '" + std::string(fl_game_dat_file) + "'"
        )

        // assets
        TRY_TO_INIT_CRITIC(
            PHYSFS_mount(fl_assets_file, nullptr, 0) != 0,
            std::string(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())),
            "PHYSFS_mount '" + std::string(fl_assets_file) + "'"
        )

        // default font
        _instance._global_font = FC_CreateFont();
    TRY_TO_INIT_CRITIC(
        FC_LoadFont_RW(_instance._global_font, Func::ArchiveGetFileRWops("files/TitilliumWeb-Light.ttf", nullptr), 1, 24, C_BLACK, TTF_STYLE_NORMAL) == 1,
        std::string(SDL_GetError()),
        "FC_LoadFont_RW: TitilliumWeb-Light.ttf"
    )
        FC_SetDefaultColor(_instance._global_font, C_BLACK);

    srand(static_cast<unsigned int>(time(nullptr)));
    // initialize console to standard output
    Console::Init();

    _instance._asset_manager = new AssetManager();
    _instance._executor = new CodeExecutor();

    _instance._instance_to_draw = std::vector<Instance*>();
    _instance._instance_to_physic = std::vector<Instance*>();

    Console::WriteLine("rdy");

#ifdef AC_ENABLE_DEBUG_MODE
    timer.EndTest();
    timer.PrintTest("Core::Init()");
#endif
    return true;
}
#undef TRY_TO_INIT_CRITIC
#undef TRY_TO_INIT

// load all asset, objects (instances) and try to load and run first scene
bool Core::LoadData()
{
#ifdef AC_ENABLE_DEBUG_MODE
    Time timer;
    timer.StartTest();
#endif
    SDL_SetWindowBordered(_instance._window, SDL_TRUE);

    // set all settings
    Graphic.SetScreenResolution(
        SettingsData::GetInt("ACWindowResolutionX", 1920),  
        SettingsData::GetInt("ACWindowResolutionY",1080) 
    );
    Graphic.SetFrameRate(SettingsData::GetInt("ACFrameRate", 60));
    Graphic.SetFullScreen(SettingsData::GetInt("ACWindowMode", 0) == 1);
    Graphic.Apply();

    Render::SetGaussianFromPreset(SettingsData::GetInt("ACBloom", 0));

    Audio.SetMusic(SettingsData::GetInt("ACSoundEnabled", 1) == 1);
    Audio.SetSound(SettingsData::GetInt("ACMusicEnabled", 1) == 1);
    Audio.SetMusicLevel(SettingsData::GetInt("ACMusicLevel", 80));
    Audio.SetSoundLevel(SettingsData::GetInt("ACSoundLevel", 80));
    Audio.Apply();

    // save all user settings to file, if its first time create.
    SettingsData::WriteValues();

    GPU_Clear(GetScreenTarget());
    GPU_Flip(GetScreenTarget());

    BackGroundRenderer bgr = BackGroundRenderer();
    bgr.Run();

    if (!Executor()->LoadArtLib()) {
        bgr.Stop();
        return false;
    }
    bgr.SetProgress(5);

    // load assets
    if (!_instance._asset_manager->LoadData(&bgr, 5, 60)) {
        bgr.Stop();
        return false;
    }
    bgr.SetProgress(60);

    if (!Executor()->LoadObjectDefinitions(&bgr, 60, 90)) {
        bgr.Stop();
        return false;
    }
    bgr.SetProgress(90);

    if(!Scene::Create(Core::GetStartingSceneName())
        && Scene::Start(-1))
    {
        bgr.Stop();
    	return false;
    }

    Render::LoadShaders();
    bgr.SetProgress(100);

    bgr.Stop();
#ifdef AC_ENABLE_DEBUG_MODE
    timer.EndTest();
    timer.PrintTest("Core::LoadData()");
#endif
    return true;
}

//This is main program loop.
bool Core::Run()
{
    _instance.game_loop = true;
    SDL_AddTimer(static_cast<Uint32>(1000), FpsCounterCallback, nullptr);
#ifdef AC_ENABLE_DEBUG_MODE
    Time performance_all{};
    Time performance_step{};
    Time performance_physics{};
    Time performance_render{};
    Time performance_post_process{};
    Time performance_counter_gpu_flip{};

#define DEBUG_TEST_COUNTER_START(counter)   \
	if(_instance.CoreDebug._show_performance_times) {   \
    (counter).StartTest();  \
};                      \

#define DEBUG_TEST_COUNTER_END(counter)     \
	if(_instance.CoreDebug._show_performance_times) {   \
    (counter).EndTest();    \
};     \

#define DEBUG_TEST_COUNTER_GET(counter, target)             \
	if(_instance.CoreDebug._show_performance_times) {   \
    (target) += (counter).GetTestTime();    \
};     \

#else

#define DEBUG_TEST_COUNTER_START(counter)
#define DEBUG_TEST_COUNTER_END(counter) 
#define DEBUG_TEST_COUNTER_GET(counter, target) 

#endif
    while (true) {
        _instance._reset_state = false;
        DEBUG_TEST_COUNTER_START(performance_all)
        if (Scene::GetCurrentScene() == nullptr) return EXIT_FAILURE;
        // FPS measurement
        _instance.LAST = _instance.NOW;
        _instance.NOW = SDL_GetTicks64();
        DeltaTime = (static_cast<double>(_instance.NOW - _instance.LAST) / 1000.0) * GameSpeed;
        _instance._frames++;

        // Set global mouse state
        MouseState::Reset();
        event_bit current_event = event_bit::NONE;

        DEBUG_TEST_COUNTER_START(performance_step)
            if (_instance.ProcessEvents(current_event))
            { // exit call
                return true;
            }
        
        if (_instance._reset_state) continue;// reset game loop

        if (_instance.game_loop) {
            _instance.ProcessStep(current_event);
            if (_instance._reset_state) continue;// reset game loop
            DEBUG_TEST_COUNTER_END(performance_step)

                DEBUG_TEST_COUNTER_START(performance_physics)
                _instance.ProcessPhysics();
            DEBUG_TEST_COUNTER_END(performance_physics)
        }

        DEBUG_TEST_COUNTER_START(performance_render)
            Render::RenderClear();
        // render scene
        _instance.ProcessSceneRender();
        DEBUG_TEST_COUNTER_END(performance_render)


            DEBUG_TEST_COUNTER_START(performance_post_process)
            // render interface and make scene pretty
            _instance.ProcessPostProcessRender();
        DEBUG_TEST_COUNTER_END(performance_post_process)

            DEBUG_TEST_COUNTER_START(performance_counter_gpu_flip)
            // render console, debug panels etc
            _instance.ProcessSystemRender();

        // get all to screen
        GPU_Flip(_instance._screenTarget);
        DEBUG_TEST_COUNTER_END(performance_counter_gpu_flip)

            DEBUG_TEST_COUNTER_END(performance_all)


        DEBUG_TEST_COUNTER_GET(performance_all, _instance.CoreDebug._performance_counter_all_rt)
        DEBUG_TEST_COUNTER_GET(performance_step, _instance.CoreDebug._performance_counter_step_rt)
        DEBUG_TEST_COUNTER_GET(performance_physics, _instance.CoreDebug._performance_counter_psychics_rt)
        DEBUG_TEST_COUNTER_GET(performance_render, _instance.CoreDebug._performance_counter_render_rt)
        DEBUG_TEST_COUNTER_GET(performance_post_process, _instance.CoreDebug._performance_counter_post_process_rt)
        DEBUG_TEST_COUNTER_GET(performance_counter_gpu_flip, _instance.CoreDebug._performance_counter_gpu_flip_rt)
    }
}

// push event to exit application
void Core::Exit()
{
    SDL_Event sdl_event{};
    sdl_event.type = SDL_QUIT;
    SDL_PushEvent(&sdl_event);
}

/////////////////////////////////////////
//
//  setup
//
/////////////////////////////////////////

// Converts arguments list to pairs, this allow to have nullptr if something is not populate.
void Core::PopulateArguments(const str_vec& args)
{
    if (args.size() == 1) return;
    for (size_t i = 1; i < args.size();)
    {
        // is command?
        if(args[i][0] == '-')
        {
            // have next string?
	        if(i+1 < args.size())
	        {
                // next string is argument?
                if (args[i + 1][0] != '-')
                {
                    // insert command + argument
                    _program_arguments.emplace_back(args[i].c_str(), args[i + 1].c_str());
                    i += 2;
                    continue;
                }
                // next string is another command
                else
                {
                    // insert only command
                    _program_arguments.emplace_back(args[i].c_str(), nullptr);
                    i += 1;
                    continue;
                }
	        }
            // must be only command
            else
            {
                _program_arguments.emplace_back(args[i].c_str(), nullptr);
                i += 1;
                continue;
            }
        }
        // else not valid command
    }
}

Core::program_argument Core::GetProgramArgument(const std::string& argument)
{
	for (const program_argument& program_argument : _program_arguments)
	{
		if(program_argument.first == argument)
		{
            return program_argument;
		}
	}
    return { nullptr, nullptr };
}

bool Core::LoadSetupFile(const char* platform, const std::string& setup_file)
{
    if(PHYSFS_mount(platform, nullptr, 0) == 0)
    {
        Console::WriteLine(std::string(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
        return false;
    }
    if(PHYSFS_exists(setup_file.c_str()) == 0)
    {
        Console::WriteLine("File '"+setup_file+"' not found!");
        PHYSFS_unmount(platform);
	    return false;
    }

    const std::string SDL_GL = "SDL_GL_";
    const std::string SDL_HINT = "SDL_";

    for (const std::string& data : Func::ArchiveGetFileText(setup_file, nullptr, false)) {
        if ( data.length() < 2 ) continue;
        if ( data.starts_with("//") ) continue;

        str_vec line = Func::Split(data, '=');
        if (line.empty()) continue;
        if (line.size() != 2) {
            Console::WriteLine("Cannot read property '" + data + "'");
            continue;
        }
        // open gl command
        if (line[0].substr(0, SDL_GL.length()) == SDL_GL) {
            bool error = false;
            const SDL_GLattr attr = Func::GetSdlAttrFromString(line[0], &error);
            if (error) {
                Console::WriteLine("unknown property '" + data + "'");
            }
            else {
#ifdef AC_ENABLE_DEBUG_MODE
                Console::WriteLine("SDL_GL_SetAttribute: " + line[0] + " as " + line[1]);
#endif
                if (SDL_GL_SetAttribute(attr, Func::TryGetInt(line[1])) != 0) {
                    Console::WriteLine({ "sdl_error: -SDL_GL_SetAttribute-", SDL_GetError() });
                }
            }
            continue;
        }
        // sdl command
        if (line[0].substr(0, SDL_HINT.length()) == SDL_HINT) {
#ifdef AC_ENABLE_DEBUG_MODE
            Console::WriteLine("SDL_SetHint: " + line[0] + " as " + line[1]);
#endif
            if (SDL_SetHint(line[0].c_str(), line[1].c_str()) == SDL_FALSE) {
                Console::WriteLine({ "sdl_error: -SDL_SetHint-", SDL_GetError() });
            }
            continue;
        }
        // other data
#ifdef AC_ENABLE_DEBUG_MODE
        Console::WriteLine("SettingsData::SetValue: " + line[0] + " as " + line[1]);
#endif
        SettingsData::SetValue(line[0], line[1]);  
    }

    PHYSFS_unmount(platform);
    return true;
}

/////////////////////////////////////////
//
//  every frame actions
//
/////////////////////////////////////////

// reset mouse state every frame
void Core::MouseState::Reset()
{
    int x{};
    int y{};
    const Uint32 button_state = SDL_GetMouseState(&x, &y);
    Mouse.XYf = Render::ScalePoint(static_cast<float>(x) ,static_cast<float>(y));
    Mouse.XY = { static_cast<int>(Mouse.XYf.x) ,static_cast<int>(Mouse.XYf.y) };

    Mouse.LeftPressed = (button_state == SDL_BUTTON(SDL_BUTTON_LEFT));
    Mouse.RightPressed = (button_state == SDL_BUTTON(SDL_BUTTON_RIGHT));
    Mouse.Wheel = 0;
    Mouse.LeftEvent = ButtonState::NONE;
    Mouse.RightEvent = ButtonState::NONE;
}

// callback every 1 second to count fps
Uint32 Core::FpsCounterCallback(Uint32 interval, void*)
{
    Core::GetInstance()->fps = Core::GetInstance()->_frames;
    Core::GetInstance()->_frames = 0;
#ifdef AC_ENABLE_DEBUG_MODE
    Core::GetInstance()->CoreDebug.PerformanceTimeSecondPassed();
#endif
    return interval;
}

/////////////////////////////////////////
//
//  trying to change scene and load it
//
/////////////////////////////////////////

bool Core::ChangeScene(const std::string& name, const int level)
{
    ClearLevelCache();
    if ( !Scene::Create(name) ) return false;
    if ( !Scene::Start(level) ) return false;
    return true;
}

void Core::ClearLevelCache()
{
    _instance._instance_to_draw.clear();
    _instance._instance_to_physic.clear();

    CodeExecutor::Break();
    CodeExecutor::SuspendedCodeStop();
    CodeExecutor::EraseGlobalStack();

    _instance._reset_state = true;
}

std::string Core::GetStartingSceneName()
{
	if (const str_vec starting_scene = Func::ArchiveGetFileText("scene/StartingScene.txt", nullptr, false); 
        starting_scene.empty()) {
        return "";
    }
    else {
        return starting_scene[0];
    }
}

/////////////////////////////////////////
//
//  Getters
//
/////////////////////////////////////////

SDL_Window* Core::GetWindowHandle()
{
    if (_instance._screenTarget) {
        return SDL_GetWindowFromID(_instance._screenTarget->context->windowID);
    }
    return nullptr;
}



//////
//////
//////
///
///
/// ArtCore debug system. Compiles only if AC_ENABLE_DEBUG_MODE is set
///
///
//////
//////
//////

#ifdef AC_ENABLE_DEBUG_MODE

void Core::CoreDebug::PerformanceTimeSecondPassed()
{
    if (!_show_performance_times) return;
    _performance_counter_all =          _performance_counter_all_rt / _instance.fps;
	_performance_counter_step =         _performance_counter_step_rt / _instance.fps;
    _performance_counter_psychics =     _performance_counter_psychics_rt / _instance.fps;
    _performance_counter_render =       _performance_counter_render_rt / _instance.fps;
    _performance_counter_post_process = _performance_counter_post_process_rt / _instance.fps;
    _performance_counter_gpu_flip =     _performance_counter_gpu_flip_rt / _instance.fps;

    _performance_counter_other = 
        _performance_counter_all - _performance_counter_step - _performance_counter_psychics - 
        _performance_counter_render - _performance_counter_post_process - _performance_counter_gpu_flip;

    _performance_counter_all_rt = 0.0;
    _performance_counter_step_rt = 0.0;
    _performance_counter_psychics_rt = 0.0;
    _performance_counter_render_rt = 0.0;
    _performance_counter_post_process_rt = 0.0;
    _performance_counter_gpu_flip_rt = 0.0;
}

Core::CoreDebug::CoreDebug()
{
    Options.emplace_back( "Performance", &_show_performance, SDLK_F2);
    Options.emplace_back( "Draw Instance info", &_show_instance_info, SDLK_F3);
    Options.emplace_back( "Draw Collider`s", &_show_collider, SDLK_F4);
    Options.emplace_back( "Draw Direction`s", &_show_directions, SDLK_F5);
    Options.emplace_back( "Show spy window", &_show_spy_window, SDLK_F6);

    Options.emplace_back( "Show performance counters", &_show_performance_times, SDLK_F7);
}

bool Core::CoreDebug::ProcessEvent(const SDL_Event* e)
{
    switch (e->type){
    /* unused for now
    case SDL_MOUSEWHEEL:
	    {
		    
	    }
        break;
    case SDL_MOUSEBUTTONDOWN:
	    {
            
	    }
	    break;
        */
    case SDL_KEYDOWN:
    {
        if (e->key.keysym.sym == SDLK_F1) {
            _show_debug_options = !_show_debug_options;
            return true;
        }
        for (const option& option : Options)
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

void Core::CoreDebug::Draw() const
{
    if (_show_instance_info || _show_collider || _show_directions)
    {
        if (Scene::IsAnyInstances()) {
            for (plf::colony<Instance*>::iterator it = Scene::ColonyGetBegin();
                it != Scene::ColonyGetEnd();)
            {
                Instance* instance = (*it);

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
                    const float line_thickness = GPU_SetLineThickness(3.f);
                    if (instance->Body.Type == Instance::body::type::Circle) {
                        const float radius_scale = instance->Body.Radius * ((instance->SpriteScaleX + instance->SpriteScaleY) / 2.f);
                        GPU_Circle(_instance._screenTarget, instance->PosX, instance->PosY, radius_scale, C_WHITE);
                        GPU_SetLineThickness(2.f);
                        GPU_Circle(_instance._screenTarget, instance->PosX, instance->PosY, radius_scale, C_BLUE);
                    }
                    if (instance->Body.Type == Instance::body::type::Rectangle) {
                        GPU_Rectangle2(_instance._screenTarget, instance->GetBodyMask().ToGPU_Rect_wh(), C_WHITE);
                        GPU_SetLineThickness(2.f);
                        GPU_Rectangle2(_instance._screenTarget, instance->GetBodyMask().ToGPU_Rect_wh(), C_BLUE);
                    }
                    GPU_SetLineThickness(line_thickness);
                }

                if (_show_instance_info) {
                    const std::string text = instance->Name + "#" + std::to_string(instance->GetId()) + "[" + std::to_string(static_cast<int>(instance->PosX)) + "," + std::to_string(static_cast<int>(instance->PosY)) + "]";
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
            "instance count: " + std::to_string(Scene::GetInstancesCount()) + '\n' +
            "delta time: " + std::to_string(DeltaTime) + '\n' +
            "Executor global stack size[capacity]: " + std::to_string(CodeExecutor::GetGlobalStackSize()) + '[' + std::to_string(CodeExecutor::GetGlobalStackSize()) + ']' + '\n' +
            "Executor if-test stack size: " + std::to_string(Core::Executor()->DebugGetIfTestResultStackSize()) + ']' + '\n' +
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
        const str_vec text = Func::Split(Executor()->DebugGetTrackInfo(), '\n');
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

        FC_DrawEffect(_instance._global_font, _instance._screenTarget, info_rect.x + info_rect.w / 2.f, info_rect.h + info_rect.y - 42.f, { FC_ALIGN_CENTER, {1.f,1.f},C_DGREEN }, "<PG_UP> <PG_DOWN>");
    }
    if(_show_performance_times)
    {
        const std::string header = "Performance counters\n";
        const std::string text_left = 
            std::string("All time:")    + '\n' +
            std::string("Step events:") + '\n' +
            std::string("Physics:")     + '\n' +
            std::string("Render:")      + '\n' +
            std::string("Post process:")+ '\n' +
            std::string("GPU_Flip:")    + '\n' +
            std::string("Other:");

        const std::string text_right = 
            std::string("%.4f ms") + '\n' +
            std::string("%.4f ms") + '\n' +
            std::string("%.4f ms") + '\n' +
            std::string("%.4f ms") + '\n' +
            std::string("%.4f ms") + '\n' +
            std::string("%.4f ms") + '\n' +
            std::string("%.4f ms");

        GPU_Rect info_rect_left = FC_GetBounds(_instance._global_font, 0.f, 0.f, FC_ALIGN_LEFT, FC_Scale{ 1.f, 1.f }, text_left.c_str());
        GPU_Rect info_rect_right = FC_GetBounds(_instance._global_font, 0.f, 0.f, FC_ALIGN_RIGHT, FC_Scale{ 1.f, 1.f }, text_right.c_str(),
            _performance_counter_all,
            _performance_counter_step,
            _performance_counter_psychics,
            _performance_counter_render,
            _performance_counter_post_process,
            _performance_counter_gpu_flip,
            _performance_counter_other
            );
        
        GPU_Rect info_rect = {
            16.f, 16.f + (_instance._show_fps ? 64.f : 0.f),
            info_rect_left.w + 16.f + info_rect_right.w,
            std::max(info_rect_left.h, info_rect_right.h) + 16.f
        };

        const double percent_step = _performance_counter_step / _performance_counter_all * 360.0;
        const double percent_psychics = _performance_counter_psychics / _performance_counter_all * 360.0;
        const double percent_render = _performance_counter_render / _performance_counter_all * 360.0;
        const double percent_post_process = _performance_counter_post_process / _performance_counter_all * 360.0;
        const double percent_gpu_flip = _performance_counter_gpu_flip / _performance_counter_all * 360.0;
        const double percent_other = _performance_counter_other / _performance_counter_all * 360.0;

        float angle = 0.f;

        constexpr SDL_Color color_table[] = {
            C_LGREEN, C_GREEN, C_RED, C_GOLD, C_BLUE, C_DYELLOW, C_GRAY
        };

        GPU_ArcFilled(_instance._screenTarget, info_rect.x + 128.f, info_rect.y + info_rect.h + 128.f + 16.f,
            128.f, angle, angle + static_cast<float>(percent_step),
            color_table[1]);
        angle += static_cast<float>(percent_step);

        GPU_ArcFilled(_instance._screenTarget, info_rect.x + 128.f, info_rect.y + info_rect.h + 128.f + 16.f,
            128.f, angle, angle + static_cast<float>(percent_psychics),
            color_table[2]);
        angle += static_cast<float>(percent_psychics);

        GPU_ArcFilled(_instance._screenTarget, info_rect.x + 128.f, info_rect.y + info_rect.h + 128.f + 16.f,
            128.f, angle, angle + static_cast<float>(percent_render),
            color_table[3]);
        angle += static_cast<float>(percent_render);

        GPU_ArcFilled(_instance._screenTarget, info_rect.x + 128.f, info_rect.y + info_rect.h + 128.f + 16.f,
            128.f, angle, angle + static_cast<float>(percent_post_process),
            color_table[4]);
        angle += static_cast<float>(percent_post_process);

        GPU_ArcFilled(_instance._screenTarget, info_rect.x + 128.f, info_rect.y + info_rect.h + 128.f + 16.f,
            128.f, angle, angle + static_cast<float>(percent_gpu_flip),
            color_table[5]);
        angle += static_cast<float>(percent_gpu_flip);

        GPU_ArcFilled(_instance._screenTarget, info_rect.x + 128.f, info_rect.y + info_rect.h + 128.f + 16.f,
            128.f, angle, angle + static_cast<float>(percent_other),
            color_table[6]);



        GPU_RectangleFilled2(_instance._screenTarget, info_rect, C_BLACK);
        GPU_Rectangle2(_instance._screenTarget, info_rect, C_DGREEN);

        //FC_DrawColor(_instance._global_font, _instance._screenTarget, info_rect.x + 4.f, info_rect.y + 4.f, C_DGREEN, text_left.c_str());
        float current_height = info_rect.y;
        str_vec spliced_text = Func::Split(text_left, '\n');
        int i = 0;
    	for (std::string& text : spliced_text)
        {
            current_height += FC_DrawColor(_instance._global_font, _instance._screenTarget, info_rect.x + 4.f, current_height + 4.f, color_table[i++], text.c_str()).h;
        }

        FC_DrawColor(_instance._global_font, _instance._screenTarget, info_rect.x + info_rect.w - info_rect_right.w - 8.f, info_rect.y + 4.f, C_DGREEN, text_right.c_str(),
            _performance_counter_all,
            _performance_counter_step,
            _performance_counter_psychics,
            _performance_counter_render,
            _performance_counter_post_process,
            _performance_counter_gpu_flip,
            _performance_counter_other);
    }
}

#endif