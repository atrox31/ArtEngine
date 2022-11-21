#pragma once

#pragma warning(push)
#include "SDL_version.h"
#include SDL2_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR
#pragma warning(pop)

#include "vec2f.h"

class Rect
{
public:
	float x, y, w, h;
	Rect();
	Rect(int x, int y, int w, int h);
	Rect(float x, float y, float w, float h);

	Rect operator+(SDL_FPoint& p);
	Rect operator-(SDL_FPoint& p);
	Rect operator+=(SDL_FPoint& p);
	Rect operator-=(SDL_FPoint& p);

	Rect operator+(SDL_Point& p);
	Rect operator-(SDL_Point& p);
	Rect operator+=(SDL_Point& p);
	Rect operator-=(SDL_Point& p);

	Rect operator*(int& i);
	Rect operator*(float& i);
	Rect operator/(int& i);
	Rect operator/(float& i);

	Rect operator*=(int& i);
	Rect operator*=(float& i);
	Rect operator/=(int& i);
	Rect operator/=(float& i);


	void Rotate(float deg);
	SDL_FPoint GetCenter();

	bool PointInRect(SDL_Point&);
	bool PointInRect(SDL_FPoint&);
	bool PointInRect(vec2f&);

	// converts
	const GPU_Rect ToGPU_Rect();
	const SDL_Rect ToSDL_Rect();
	const SDL_FRect ToSDL_FRect();

	const GPU_Rect ToGPU_Rect_wh();
	const SDL_Rect ToSDL_Rect_wh();
	const SDL_FRect ToSDL_FRect_wh();
};

