#pragma once
#include "SDL2/SDL2/include/SDL.h"

class BackGroundRenderer {
public:
	BackGroundRenderer();
	void SetProgress(int progress) const;
	void Stop();
	void Run();
private:
	static int ThreadDrawingFunction(void* data);

	static int _bg_target_percent;
	static SDL_sem* _bg_data_lock;
	SDL_Thread* bg_renderer;
};