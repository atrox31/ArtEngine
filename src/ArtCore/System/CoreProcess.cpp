#include "Core.h"
#include "ArtCore/CodeExecutor/CodeExecutor.h"
#include "ArtCore/Enums/Event.h"
#include "ArtCore/Graphic/BackGroundRenderer.h"
#include "ArtCore/Graphic/Render.h"
#include "ArtCore/Physics/Physics.h"
#include "ArtCore/Scene/Scene.h"
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

bool Core::ProcessEvents(event_bit& global_events_flag)
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
#if AC_ENABLE_DEBUG_MODE
            if (CoreDebug.ProcessEvent(&e)) {
                return false;
            }
#endif
            if (ProcessCoreKeys(e.key.keysym.sym)) break;
            if (Console::ProcessEvent(&e)) break;
            global_events_flag = global_events_flag | event_bit::HAVE_KEYBOARD_EVENT;
            global_events_flag = global_events_flag | event_bit::HAVE_KEYBOARD_EVENT_DOWN;
        } break;
        case SDL_KEYUP:
        {
            global_events_flag = global_events_flag | event_bit::HAVE_KEYBOARD_EVENT;
            global_events_flag = global_events_flag | event_bit::HAVE_KEYBOARD_EVENT_UP;
        } break;

        case SDL_TEXTINPUT:
        {
            if (Console::ProcessEvent(&e)) break;
        } break;

        case SDL_MOUSEBUTTONDOWN:
        {
            global_events_flag = global_events_flag | event_bit::HAVE_MOUSE_EVENT;
            global_events_flag = global_events_flag | event_bit::HAVE_MOUSE_EVENT_DOWN;
            global_events_flag = global_events_flag | event_bit::HAVE_MOUSE_EVENT_CLICK;
            if (e.button.button == SDL_BUTTON_LEFT) {
                Mouse.LeftEvent = Core::MouseState::ButtonState::PRESSED;
            }
            if (e.button.button == SDL_BUTTON_RIGHT) {
                Mouse.RightEvent = Core::MouseState::ButtonState::PRESSED;
            }
        } break;

        case SDL_MOUSEBUTTONUP: {
            global_events_flag = global_events_flag | event_bit::HAVE_MOUSE_EVENT;
            global_events_flag = global_events_flag | event_bit::HAVE_MOUSE_EVENT_UP;
            if (e.button.button == SDL_BUTTON_LEFT) {
                Mouse.LeftEvent = Core::MouseState::ButtonState::RELEASED;
            }
            if (e.button.button == SDL_BUTTON_RIGHT) {
                Mouse.RightEvent = Core::MouseState::ButtonState::RELEASED;
            }
        } break;
        case SDL_MOUSEWHEEL:
        {
            global_events_flag = global_events_flag | event_bit::HAVE_MOUSE_EVENT;
            global_events_flag = global_events_flag | event_bit::HAVE_MOUSE_EVENT_SCROLL;
        } break;

        case SDL_WINDOWEVENT:
        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
        case SDL_RENDER_TARGETS_RESET:/**< The render targets have been reset and their contents need to be updated */
        case SDL_RENDER_DEVICE_RESET: /**< The device has been reset and all textures need to be recreated */
        case SDL_CONTROLLERAXISMOTION:
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            break;
        default:continue;
        }
    }

    return false;
}

void Core::ProcessStep(const event_bit& global_events_flag)
{
    // interface (gui) events
    const bool gui_have_event = Scene::GuiSystem().Events();

    if (!_instance.game_loop) return;
    // add all new instances to scene and execute OnCreate event
    Scene::GetCurrentScene()->SpawnAll();
    if (!Scene::IsAnyInstances()) return;

    for (plf::colony<Instance*>::iterator it = Scene::ColonyGetBegin();
        it != Scene::ColonyGetEnd();)
    {
        if (Instance* c_instance = (*it); !c_instance->Alive) {
            // delete instance if not alive
            if (EVENT_BIT_TEST(event_bit::HAVE_ON_DESTROY, c_instance->EventFlag)) {
                Executor()->ExecuteScript(c_instance, Event::EvOnDestroy);
            }
            it = Scene::ColonyErase(it);
        }else{ 
            // instance exists and alive
            // step
            Executor()->ExecuteScript(c_instance, Event::EvStep);
            const event_bit c_instance_events = c_instance->EventFlag;

            // check if instance is still alive
            if (!c_instance->Alive) {
                ++it;
                continue;
            }

            // view change event
            const bool old_in_view = c_instance->InView;
            c_instance->InView = Scene::InView({ c_instance->PosX, c_instance->PosY });
            if (EVENT_BIT_TEST(event_bit::HAVE_VIEW_CHANGE, c_instance_events)
            && c_instance->InView != old_in_view) {

                if (EVENT_BIT_TEST(event_bit::HAVE_VIEW_CHANGE, c_instance_events)) {
                    if (old_in_view == true) { // be inside, now exit view
                        Executor()->ExecuteScript(c_instance, Event::EvOnViewLeave);
                    }
                    else {
                        Executor()->ExecuteScript(c_instance, Event::EvOnViewEnter);
                    }
                }
            }
            

            // check if instance is still alive
            if (!c_instance->Alive) {
                ++it;
                continue;
            }

            // mouse input
            // check mouse event and gui not handle it
            if (EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT, global_events_flag) 
                && (!gui_have_event)) {
                // global mouse down
                if (   EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT_DOWN, global_events_flag)     // first check if event happen
                    && EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT_DOWN, c_instance_events))     // second check if instance have this event
                    Executor()->ExecuteScript(c_instance, Event::EvOnMouseDown);                // execute event

                // global mouse up
                if (   EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT_UP, global_events_flag)       // first check if event happen
                    && EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT_UP, c_instance_events))       // second check if instance have this event
                    Executor()->ExecuteScript(c_instance, Event::EvOnMouseUp);                  // execute event

                // local mouse clock - on instance sprite mask
                if (   EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT_DOWN, global_events_flag)     // first check if event happen
                    && EVENT_BIT_TEST(event_bit::HAVE_MOUSE_EVENT_CLICK, c_instance_events)     // second check if instance have this event
                    && c_instance->CheckMaskClick(Mouse.XYf))                                   // third check if sprite mask is clicked
                    Executor()->ExecuteScript(c_instance, Event::EvClicked);

            }

            // check if instance is still alive
            if (!c_instance->Alive) {
                ++it;
                continue;
            }

            // keyboard input
            if (EVENT_BIT_TEST(event_bit::HAVE_KEYBOARD_EVENT, global_events_flag)) {
                if (EVENT_BIT_TEST(event_bit::HAVE_KEYBOARD_EVENT_DOWN, global_events_flag)     // first check if event happen
                &&  EVENT_BIT_TEST(event_bit::HAVE_KEYBOARD_EVENT_DOWN, c_instance_events))     // second check if instance have this event
                    Executor()->ExecuteScript(c_instance, Event::EvOnKeyDown);                  // execute event

                if (EVENT_BIT_TEST(event_bit::HAVE_KEYBOARD_EVENT_UP, global_events_flag)       // first check if event happen
                &&  EVENT_BIT_TEST(event_bit::HAVE_KEYBOARD_EVENT_UP, c_instance_events))       // second check if instance have this event
                    Executor()->ExecuteScript(c_instance, Event::EvOnKeyUp);                    // execute event
            }

            // go to next instance
            ++it;
        }
    }
    // execute all suspended code
    CodeExecutor::SuspendedCodeExecute();
}

void Core::ProcessPhysics()
{
    if (!_instance.game_loop) return;
    for (plf_it<Instance*> it = Scene::ColonyGetBegin();
        it != Scene::ColonyGetEnd();++it)
    {
	    if (Instance* instance = (*it); 
            instance->Alive
            && EVENT_BIT_TEST(event_bit::HAVE_COLLISION, instance->EventFlag)) {
            for (plf_it<Instance*> it2 = Scene::ColonyGetBegin();
                it2 != Scene::ColonyGetEnd();++it2)
            {
                if (Instance* target = (*it2);
                    target->Alive && Physics::CollisionTest(instance, target)) {
                    Executor()->CollisionSetTarget(target);
                    Executor()->ExecuteScript(instance, Event::EvOnCollision);
                    Executor()->CollisionResetTarget();
                }
            }
        }

    }
}


void Core::ProcessSceneRender()
{
    // render scene background
    if (Scene::GetCurrentScene()->BackGround.Texture != nullptr) {
        Render::DrawTextureBox(Scene::GetCurrentScene()->BackGround.Texture, nullptr, nullptr);
    }
    else
    {
        Render::RenderClearColor(Scene::GetCurrentScene()->BackGround.Color);
    }

    // draw all instances if in view (defined in step event)
    if (Scene::IsAnyInstances()) {
        for (plf_it<Instance*> it = Scene::ColonyGetBegin();
            it != Scene::ColonyGetEnd(); ++it)
        {
            if ((*it)->InView) {
                Executor()->ExecuteScript((*it), Event::EvDraw);
            }
        }
    }
}

void Core::ProcessPostProcessRender() const
{
    // post process
    Render::ProcessImageWithGaussian();

    // draw interface
    Scene::GuiSystem().Render();

    // draw all scene to screen buffer
    Render::RenderToTarget(_screenTarget);
}

void Core::ProcessSystemRender() const
{
    // DEBUG DRAW
#if AC_ENABLE_DEBUG_MODE
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
