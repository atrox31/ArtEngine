#include "BackGroundRenderer.h"

#include <iostream>

#include "ColorDefinitions.h"
#include "ArtCore/Functions/Func.h"
#include "ArtCore/System/Core.h"
#include "ArtCore/_Debug/Debug.h"

SDL_sem* BackGroundRenderer::_bg_data_lock = nullptr;
int BackGroundRenderer::_bg_target_percent = 0;
BackGroundRenderer::BackGroundRenderer()
{
    _bg_data_lock = SDL_CreateSemaphore(1);
    _bg_target_percent = 0;
    bg_renderer = nullptr;
}

void BackGroundRenderer::SetProgress(const int progress) const
{
    if (bg_renderer == nullptr) {
        Console::WriteLine("BackGroundRenderer: can not set progress, thread is not running!");
        return;
    }
    SDL_SemWait(_bg_data_lock);
    _bg_target_percent = progress;
    SDL_SemPost(_bg_data_lock);
}

void BackGroundRenderer::Stop()
{
    if (bg_renderer == nullptr) {
        Console::WriteLine("BackGroundRenderer: can not stop process, thread is not running!");
        return;
    }

    SDL_SemWait(_bg_data_lock);
    Console::WriteLine( "bg stop at:" + std::to_string(_bg_target_percent) + "%");
    SDL_SemPost(_bg_data_lock);

    SetProgress(-1);
    SDL_WaitThread(bg_renderer, nullptr);
    SDL_DestroySemaphore(_bg_data_lock);
    _bg_data_lock = nullptr;
    bg_renderer = nullptr;
    _bg_target_percent = 0;
}

void BackGroundRenderer::Run()
{
    if (bg_renderer != nullptr) {
        Console::WriteLine("BackGroundRenderer: can not start process, thread is running!");
        return;
    }
    bg_renderer = SDL_CreateThread(BackGroundRenderer::ThreadDrawingFunction, "bg_renderer", nullptr);
}
int BackGroundRenderer::ThreadDrawingFunction(void* data)
{
    // percents
    int c_level = 0;
    int d_level = 0;

    // surface and renderer for process
    SDL_Surface* surface = SDL_GetWindowSurface(Core::GetWindowHandle());
    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);

    // graphic
    SDL_Texture* bc_img = nullptr;
    if (SDL_RWops* tmp_img = Func::ArchiveGetFileRWops("bg_img.png", nullptr); tmp_img != nullptr) {
        bc_img = IMG_LoadTexture_RW(renderer, tmp_img, 1);
    }

    // rectangle load
    SDL_FRect bg_rect = {
        0.25f * static_cast<float>(Core::GetScreenWidth()),
        0.8f * static_cast<float>(Core::GetScreenHeight()),

        0.5f * static_cast<float>(Core::GetScreenWidth()),
        48.f
    };
    SDL_FRect in_rect = { bg_rect.x + 4,bg_rect.y + 4,bg_rect.w - 8,bg_rect.h - 8 };

    while (true) {
        // get actual percent
        SDL_SemWait(BackGroundRenderer::_bg_data_lock);
        d_level = BackGroundRenderer::_bg_target_percent;
        SDL_SemPost(BackGroundRenderer::_bg_data_lock);
        // STOP
        if (d_level == -1) {
            break;
        }
        // loading 100%
        if (c_level == 100) {
            break;
        }
        // add percent
        if (c_level < d_level) {
            c_level++;
        }
        // clean screen
        SDL_SetRenderDrawColor(renderer, C_BLACK.r, C_BLACK.g, C_BLACK.b, C_BLACK.a);
        SDL_RenderClear(renderer);
        // draw background
        if (bc_img != nullptr) {
            SDL_RenderCopy(renderer, bc_img, nullptr, nullptr);
        }
        // black rectangle for background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRectF(renderer,
            &bg_rect);
        // frame
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRectF(renderer,
            &bg_rect);
        // percent of loading
        SDL_FRect tmp_in_rect = {
            in_rect.x,
            in_rect.y,
            in_rect.w * (static_cast<float>(c_level) / 100.0f),
            in_rect.h
        };
        SDL_RenderFillRectF(renderer, &tmp_in_rect);

        SDL_UpdateWindowSurface(Core::GetWindowHandle());

        SDL_Delay(1000 / 60);
    }
    if (bc_img != nullptr) {
        SDL_DestroyTexture(bc_img);
        bc_img = nullptr;
    }
    //surface = nullptr;
    SDL_DestroyRenderer(renderer);
    return 0;
}
