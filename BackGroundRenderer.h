#pragma once
#include "Core.h"
#include "Func.h"

class BackGroundRenderer {
public:
	BackGroundRenderer();
	void SetProgress(int progress);
	void Stop();
	void Run();
private:
	static int ThreadDrawingFunction(void* data);

	static int bg_target_percent;
	static SDL_sem* bg_data_lock;
	SDL_Thread* bg_renderer;
};