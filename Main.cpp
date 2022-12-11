#define WIN32_LEAN_AND_MEAN 
#define HAVE_M_PI // for SDL warring M_PI
#include "SDL_version.h"
#include <exception>
#include <iostream>

#ifdef WIN32
	#pragma comment(lib, sdl2_image_lib_x86)
	#pragma comment(lib, sdl2_mixer_lib_x86)
	#pragma comment(lib, sdl2_ttf_lib_x86)
	#pragma comment(lib, sdl2_net_lib_x86)
	#pragma comment(lib, sdl2_lib_x86)
	#pragma comment(lib, sdl2_main_lib_x86)
#else
	#pragma comment(lib, sdl2_image_lib_x64)
	#pragma comment(lib, sdl2_mixer_lib_x64)
	#pragma comment(lib, sdl2_ttf_lib_x64)
	#pragma comment(lib, sdl2_net_lib_x64)
	#pragma comment(lib, sdl2_lib_x64)
	#pragma comment(lib, sdl2_main_lib_x64)
#endif

#ifdef _DEBUG
	#pragma comment(lib, "SDL2/SDL2_gpu/SDL2_gpu/debug/x64/SDL2_gpu.lib")
#else
	#pragma comment(lib, "SDL2/SDL2_gpu/SDL2_gpu/release/x64/SDL2_gpu.lib")
#endif

// for using graphic card
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

// include all sdl systems
#pragma warning(push)
#include SDL2_INCLUDE_DIR
#include SDL2_IMAGE_INCLUDE_DIR
#include SDL2_MIXER_INCLUDE_DIR
#include SDL2_NET_INCLUDE_DIR
#include SDL2_TTF_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR
#pragma warning(pop)

#include "Debug.h"
#include "SplashScreen.h"
#include "Core.h"

int protected_main(int argc, char* args[]) {
	Core::Init(argc, args);

	// arguments
	if (!Core::LoadData()) return EXIT_FAILURE;

	return Core::Run();
}

int SDL_main(int argc, char* argv[]) {
	try
	{
		return protected_main(argc, argv);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
