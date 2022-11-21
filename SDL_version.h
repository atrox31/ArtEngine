#pragma once
#define SDL2_VERSION SDL2/SDL2-2.24.2
#define SDL2_IMAGE_VERSION SDL2/SDL2_image-2.6.2
#define SDL2_MIXER_VERSION SDL2/SDL2_mixer-2.6.2
#define SDL2_NET_VERSION SDL2/SDL2_net-2.2.0
#define SDL2_TTF_VERSION SDL2/SDL2_ttf-2.20.1
#define SDL2_GPU_VERSION SDL2/SDL2_gpu-0.12.0-msvc
/////////////////////////////////////////////
#define STR_IMPL(A) #A
#define STR(A) STR_IMPL(A)

#define sdl2_file include/SDL.h
#define sdl2_file_begin_code include/begin_code.h
#define sdl2_file_end_code include/close_code.h
#define sdl2_file_version include/SDL_version.h
#define sdl2_file_SDL_stdinc include/SDL_stdinc.h
#define sdl2_file_SDL_rwops include/SDL_rwops.h
#define sdl2_file_SDL_audio include/SDL_audio.h
#define sdl2_file_SDL_endian include/SDL_endian.h

#define sdl2_image_file include/SDL_image.h
#define sdl2_mixer_file include/SDL_mixer.h
#define sdl2_net_file include/SDL_net.h
#define sdl2_ttf_file include/SDL_ttf.h
#define sdl2_gpu_file include/SDL_gpu.h

#define SDL2_INCLUDE_DIR STR(SDL2_VERSION/sdl2_file)
#define SDL2_INCLUDE_DIR_BEGIN STR(SDL2_VERSION/sdl2_file_begin_code)
#define SDL2_INCLUDE_DIR_CLOSE STR(SDL2_VERSION/sdl2_file_end_code)
#define SDL2_INCLUDE_DIR_VERSION STR(SDL2_VERSION/sdl2_file_version)
#define SDL2_INCLUDE_DIR_SDL_stdinc STR(SDL2_VERSION/sdl2_file_version)
#define SDL2_INCLUDE_DIR_SDL_rwops STR(SDL2_VERSION/sdl2_file_version)
#define SDL2_INCLUDE_DIR_SDL_audio STR(SDL2_VERSION/sdl2_file_version)
#define SDL2_INCLUDE_DIR_SDL_endian STR(SDL2_VERSION/sdl2_file_version)

#define SDL2_IMAGE_INCLUDE_DIR STR(SDL2_IMAGE_VERSION/sdl2_image_file)
#define SDL2_MIXER_INCLUDE_DIR STR(SDL2_MIXER_VERSION/sdl2_mixer_file)
#define SDL2_NET_INCLUDE_DIR STR(SDL2_NET_VERSION/sdl2_net_file)
#define SDL2_TTF_INCLUDE_DIR STR(SDL2_TTF_VERSION/sdl2_ttf_file)
#define SDL2_GPU_INCLUDE_DIR STR(SDL2_GPU_VERSION/sdl2_gpu_file)

// lib
#define sdl2_image_lib_x86 STR(SDL2_IMAGE_VERSION/lib/x86/SDL2_image.lib)
#define sdl2_mixer_lib_x86 STR(SDL2_MIXER_VERSION/lib/x86/SDL2_mixer.lib)
#define sdl2_ttf_lib_x86 STR(SDL2_TTF_VERSION/lib/x86/SDL2_ttf.lib)
#define sdl2_net_lib_x86 STR(SDL2_NET_VERSION/lib/x86/SDL2_net.lib)
#define sdl2_lib_x86 STR(SDL2_VERSION/lib/x86/SDL2.lib)
#define sdl2_main_lib_x86 STR(SDL2_VERSION/lib/x86/SDL2main.lib)

#define sdl2_image_lib_x64 STR(SDL2_IMAGE_VERSION/lib/x64/SDL2_image.lib)
#define sdl2_mixer_lib_x64 STR(SDL2_MIXER_VERSION/lib/x64/SDL2_mixer.lib)
#define sdl2_ttf_lib_x64 STR(SDL2_TTF_VERSION/lib/x64/SDL2_ttf.lib)
#define sdl2_net_lib_x64 STR(SDL2_NET_VERSION/lib/x64/SDL2_net.lib)
#define sdl2_lib_x64 STR(SDL2_VERSION/lib/x64/SDL2.lib)
#define sdl2_main_lib_x64 STR(SDL2_VERSION/lib/x64/SDL2main.lib)

