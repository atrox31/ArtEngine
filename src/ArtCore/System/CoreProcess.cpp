#include "Core.h"
#include "ArtCore/Graphic/Render.h"
#include "ArtCore/CodeExecutor/CodeExecutor.h"
#include "ArtCore/Graphic/BackGroundRenderer.h"
#include "ArtCore/Scene/Scene.h"
#include "ArtCore/Physics/Physics.h"
#include "ArtCore/System/SettingsData.h"

#include "ArtCore/Graphic/ColorDefinitions.h"

/////////////////////////////////////////
//
//  Art Core normal per frame step
//
/////////////////////////////////////////

bool Core::ProcessCoreKeys(const Sint32 sym)
{
    if (sym == SDLK_INSERT) {
        _show_fps = !_show_fps;
        return true;
    }
    if (sym == SDLK_HOME) {
        Console::ConsoleHomeButtonPressed();
        return true;
    }
    return false;
}

bool Core::ProcessEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT)
        {
            Console::WriteLine("exit request");
            return true;
        }

        switch (e.type) {
        case SDL_KEYDOWN:
        {
#ifdef _DEBUG
            if (CoreDebug.ProcessEvent(&e)) {
                return false;
            }
#endif
            if (ProcessCoreKeys(e.key.keysym.sym)) break;
            if (Console::ProcessEvent(&e)) break;
        } break;

        case SDL_TEXTINPUT:
        {
            if (Console::ProcessEvent(&e)) break;
        } break;

        case SDL_MOUSEBUTTONDOWN:
        {
            if (e.button.button == SDL_BUTTON_LEFT) {
                Mouse.LeftEvent = Core::MouseState::ButtonState::PRESSED;
            }
            if (e.button.button == SDL_BUTTON_RIGHT) {
                Mouse.RightEvent = Core::MouseState::ButtonState::PRESSED;
            }
        } break;

        case SDL_MOUSEBUTTONUP: {
            if (e.button.button == SDL_BUTTON_LEFT) {
                Mouse.LeftEvent = Core::MouseState::ButtonState::RELEASED;
            }
            if (e.button.button == SDL_BUTTON_RIGHT) {
                Mouse.RightEvent = Core::MouseState::ButtonState::RELEASED;
            }
        } break;

        case SDL_KEYUP:
        case SDL_WINDOWEVENT:
        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
        case SDL_RENDER_TARGETS_RESET:/**< The render targets have been reset and their contents need to be updated */
        case SDL_RENDER_DEVICE_RESET: /**< The device has been reset and all textures need to be recreated */
        case SDL_CONTROLLERAXISMOTION:
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
        case SDL_MOUSEWHEEL:
            break;
        default:continue;
        }
    }
    return false;
}

void Core::ProcessStep() const
{
    // interface (gui) events
    const bool gui_have_event = _current_scene->GuiSystem.Events();
    // add all new instances to scene and execute OnCreate event
    _current_scene->SpawnAll();
    if (_current_scene->IsAnyInstances()) {
        for (plf::colony<Instance*>::iterator it = _current_scene->InstanceColony.begin();
            it != _current_scene->InstanceColony.end();)
        {
            if (Instance* c_instance = (*it); c_instance->Alive) {
                // step
                Executor()->ExecuteScript(c_instance, Event::EvStep);
                const event_bit c_flag = c_instance->EventFlag;

                // in view
                if (c_instance->Alive) {
                    const bool oldInView = c_instance->InView;
                    c_instance->InView = GetCurrentScene()->InView({ c_instance->PosX, c_instance->PosY });
                    if (c_instance->InView != oldInView) {
                        if (EVENT_BIT_TEST(event_bit::HAVE_VIEW_CHANGE, c_flag)) {
                            if (oldInView == true) { // be inside, now exit view
                                Executor()->ExecuteScript(c_instance, Event::EvOnViewLeave);
                            }
                            else {
                                Executor()->ExecuteScript(c_instance, Event::EvOnViewEnter);
                            }
                        }
                    }
                }

                // mouse input
                if (c_instance->Alive && (!gui_have_event)) {
                    if (EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT, c_flag)) {
                        if (Mouse.LeftEvent == MouseState::ButtonState::PRESSED) {
                            // global click
                            if (EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT_DOWN, c_flag)) {
                                Executor()->ExecuteScript(c_instance, Event::EvOnMouseDown);
                            }
                            // on mask click
                            if (EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT_CLICK, c_flag)) {
                                if (c_instance->CheckMaskClick(Mouse.XYf)) {
                                    Executor()->ExecuteScript(c_instance, Event::EvClicked);
                                }
                            }
                        }
                        if (Mouse.LeftEvent == MouseState::ButtonState::RELEASED) {
                            if (EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT_UP, c_flag)) {
                                Executor()->ExecuteScript(c_instance, Event::EvOnMouseUp);
                            }
                        }
                    }
                }
                // go to next instance
                ++it;
            }
            else {
                // delete instance if not alive
                if (EVENT_BIT_TEST(event_bit::HAVE_ON_DESTROY, c_instance->EventFlag)) {
                    Executor()->ExecuteScript(c_instance, Event::EvOnDestroy);
                }
                delete (*it);
                it = _instance._current_scene->DeleteInstance(it);
            }
        }
        // execute all suspended code
        CodeExecutor::SuspendedCodeExecute();
    }
}

void Core::ProcessPhysics() const
{
    for (const auto instance : _current_scene->InstanceColony) {
        if (instance->Alive) {
            // collision
            if (EVENT_BIT_TEST(event_bit::HAVE_COLLISION, instance->EventFlag)) {
                for (Instance* target : _current_scene->InstanceColony) {
                    if (Physics::CollisionTest(instance, target)) {
                        _current_scene->CurrentCollisionInstance = target;
                        _current_scene->CurrentCollisionInstanceId = target->GetId();
                        Executor()->ExecuteScript(instance, Event::EvOnCollision);
                        _current_scene->CurrentCollisionInstance = nullptr;
                        _current_scene->CurrentCollisionInstanceId = -1;
                    }
                }
            }
        }
    }
}

void Core::ProcessSceneRender() const
{
    // render scene background
    if (_current_scene->BackGround.Texture != nullptr) {
        Render::DrawTextureBox(_current_scene->BackGround.Texture, nullptr, nullptr);
    }
    else
    {
        Render::RenderClearColor(_current_scene->BackGround.Color);
    }

    // draw all instances if in view (defined in step event)
    if (_current_scene->IsAnyInstances()) {
        for (Instance* instance : _current_scene->InstanceColony) {
            if (instance->InView) {
                Executor()->ExecuteScript(instance, Event::EvDraw);
            }
        }
    }
}

void Core::ProcessPostProcessRender() const
{
    // post process
    Render::ProcessImageWithGaussian();

    // draw interface
    _current_scene->GuiSystem.Render();

    // draw all scene to screen buffer
    Render::RenderToTarget(_screenTarget);
}

void Core::ProcessSystemRender() const
{
    // DEBUG DRAW
#ifdef _DEBUG
    _instance.CoreDebug.Draw();
#endif // _DEBUG

    // draw console
    Console::RenderConsole(_screenTarget);


    if (_instance._show_fps) {
        GPU_DeactivateShaderProgram();
        const std::string text = "FPS: " + std::to_string(fps);
        GPU_Rect info_rect = FC_GetBounds(_global_font, 0, 0, FC_ALIGN_LEFT, FC_Scale{ 1.2f, 1.2f }, text.c_str());
        constexpr float info_rect_move = 8.f;
        info_rect.x += info_rect_move;
        info_rect.y += info_rect_move;
        info_rect.w += info_rect_move;
        info_rect.h += info_rect_move;

        GPU_RectangleFilled2(_screenTarget, info_rect, C_BLACK);
        GPU_Rectangle2(_screenTarget, info_rect, C_DGREEN);

        FC_DrawEffect(_global_font, _screenTarget, info_rect.x + 4.f, info_rect.y, FC_MakeEffect(FC_ALIGN_LEFT, { 1.f,1.f }, C_DGREEN), text.c_str());
    }
}
