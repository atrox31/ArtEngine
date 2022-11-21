#include "Core.h"
#include "Func.h"

SDL_sem* Core::BackGroundRenderer::bg_data_lock = nullptr;
int Core::BackGroundRenderer::bg_target_percent = 0;
Core::BackGroundRenderer::BackGroundRenderer()
{
    bg_data_lock = SDL_CreateSemaphore(1);
    bg_target_percent = 0;
    bg_renderer = nullptr;
}

void Core::BackGroundRenderer::SetProgress(int progress)
{
    if (bg_renderer == nullptr) {
        Debug::WARNING("BackGroundRenderer: can not set progress, thread is not running!");
        return;
    }
    SDL_SemWait(bg_data_lock);
    bg_target_percent = progress;
    SDL_SemPost(bg_data_lock);
}

void Core::BackGroundRenderer::Stop()
{
    if (bg_renderer == nullptr) {
        Debug::WARNING("BackGroundRenderer: can not stop process, thread is not running!");
        return;
    }
    SetProgress(-1);
    SDL_WaitThread(bg_renderer, NULL);
    SDL_DestroySemaphore(bg_data_lock);
    bg_data_lock = nullptr;
    bg_renderer = nullptr;
    bg_target_percent = 0;
}

void Core::BackGroundRenderer::Run()
{
    if (bg_renderer != nullptr) {
        Debug::WARNING("BackGroundRenderer: can not start process, thread is running!");
        return;
    }
    bg_renderer = SDL_CreateThread(BackGroundRenderer::ThreadDrawingFunction, "bg_renderer", nullptr);

}
int Core::BackGroundRenderer::ThreadDrawingFunction(void* data)
{
    // procenty wyœwietlania
    int c_level = 0;
    int d_level = 0;

    // surface i renderer dla procesu
    SDL_Surface* surface = SDL_GetWindowSurface(Core::GetInstance()->GetWindowHandle());
    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);

    // grafika
    SDL_Texture* bc_img = nullptr;
    bool bc_have_img = false;
    Sint64 siz;
    SDL_RWops* tmp_img = Func::GetFileRWops("bg_img.png", &siz);
    if (tmp_img != NULL) {
        bc_img = IMG_LoadTexture_RW(renderer, tmp_img, 1);
        if (bc_img != nullptr) {
            bc_have_img = true;
        }
    }

    // prostok¹t ekranu ³adowania
    SDL_Rect bg_rect = {
        (int)(0.25f * (float)Core::GetInstance()->GetScreenWidth()),
        (int)(0.8f * (float)Core::GetInstance()->GetScreenHeight()),

        (int)(0.5f * (float)Core::GetInstance()->GetScreenWidth()),
        48
    };
    SDL_Rect in_rect = { bg_rect.x + 4,bg_rect.y + 4,bg_rect.w - 8,bg_rect.h - 8 };

    while (true) {
        // pobierz aktualny procent
        SDL_SemWait(Core::BackGroundRenderer::bg_data_lock);
        d_level = Core::BackGroundRenderer::bg_target_percent;
        SDL_SemPost(Core::BackGroundRenderer::bg_data_lock);
        // STOP
        if (d_level == -1) {
            break;
        }
        // koniec ³adowania
        if (c_level == 100) {
            break;
        }
        // dodawanie procenta
        if (c_level < d_level) {
            c_level++;
        }
        // czyszczenie ekranu
        SDL_SetRenderDrawColor(renderer, C_BLACK.r, C_BLACK.g, C_BLACK.b, C_BLACK.a);
        SDL_RenderClear(renderer);
        // rysowanei obrazka t³a
        if (bc_have_img) {
            SDL_RenderCopy(renderer, bc_img, NULL, NULL);
        }
        // rysowanie czarnego prostok¹ta pod pasek ³¹dowania
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer,
            &bg_rect);
        // ramka paska ³adowania
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer,
            &bg_rect);
        // procent ³adowania
        SDL_Rect tmp_in_rect = {
            in_rect.x,
            in_rect.y,
            (int)((float)in_rect.w * ((float)c_level / 100.0f)),
            in_rect.h
        };
        SDL_RenderFillRect(renderer, &tmp_in_rect);

        SDL_UpdateWindowSurface(Core::GetInstance()->GetWindowHandle());

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
