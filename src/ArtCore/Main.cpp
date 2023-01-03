#define WIN32_LEAN_AND_MEAN 
#define HAVE_M_PI // for SDL warring M_PI
#include <exception>
#include <iostream>

#include "System/Core.h"
#include "_Debug/Debug.h"

// sdl libs
#pragma comment(lib, "src/SDL2/SDL2/lib/x64/SDL2.lib")
#pragma comment(lib, "src/SDL2/SDL2/lib/x64/SDL2main.lib")
#pragma comment(lib, "src/SDL2/SDL2_image/lib/x64/SDL2_image.lib")
#pragma comment(lib, "src/SDL2/SDL2_mixer/lib/x64/SDL2_mixer.lib")
#pragma comment(lib, "src/SDL2/SDL2_net/lib/x64/SDL2_net.lib")
#pragma comment(lib, "src/SDL2/SDL2_ttf/lib/x64/SDL2_ttf.lib")

// sdl_gpu libs
#ifdef _DEBUG
	#pragma comment(lib, "src/SDL2/SDL2_gpu/SDL2_gpu/debug/x64/SDL2_gpu.lib")
#else
	#pragma comment(lib, "src/SDL2/SDL2_gpu/SDL2_gpu/release/x64/SDL2_gpu.lib")
#endif

// for using graphic card
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int protected_main(int argc, char* args[]) {
	Core::Init(argc, args);
	if (!Core::LoadData()) return EXIT_FAILURE;
	return Core::Run();
}

int SDL_main(int argc, char* argv[]) {
#ifdef _DEBUG
#ifndef DEBUG_EDITOR
		Debug::WRITE_NECROLOGY = true;
#endif
#endif

	try
	{
		return protected_main(argc, argv);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
