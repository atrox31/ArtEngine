#pragma once

#include "vec2f.h"

class Rect
{
public:
	float x, y, w, h;
	Rect();
	Rect(GPU_Rect);
	Rect(int x, int y, int w, int h);
	Rect(float x, float y, float w, float h);

	Rect operator+(const SDL_FPoint& p) const;
	Rect operator-(const SDL_FPoint& p) const;
	Rect operator+=(const SDL_FPoint& p);
	Rect operator-=(const SDL_FPoint& p);

	Rect operator+(const SDL_Point& p) const;
	Rect operator-(const SDL_Point& p) const;
	Rect operator+=(const SDL_Point& p);
	Rect operator-=(const SDL_Point& p);

	Rect operator*(const int& i) const;
	Rect operator*(const float& i) const;
	Rect operator/(const int& i) const;
	Rect operator/(const float& i) const;

	Rect operator*=(const int& i);
	Rect operator*=(const float& i);
	Rect operator/=(const int& i);
	Rect operator/=(const float& i);

	bool operator==(Rect const& a) const;
	bool operator!=(Rect const& a) const;


	void Rotate(float deg);
	[[nodiscard]] SDL_FPoint GetCenter() const;

	[[nodiscard]] bool PointInRect(const SDL_Point&) const;
	[[nodiscard]] bool PointInRect_wh(const SDL_Point& p) const;
	[[nodiscard]] bool PointInRect(const SDL_FPoint&) const;
	[[nodiscard]] bool PointInRect_wh(const SDL_FPoint& p) const;
	[[nodiscard]] bool PointInRect(const vec2f&) const;
	[[nodiscard]] bool PointInRect_wh(const vec2f& p) const;

	// converts
	[[nodiscard]] GPU_Rect ToGPU_Rect() const;
	[[nodiscard]] SDL_Rect ToSDL_Rect() const;
	[[nodiscard]] SDL_FRect ToSDL_FRect() const;

	[[nodiscard]] GPU_Rect ToGPU_Rect_wh() const;
	[[nodiscard]] SDL_Rect ToSDL_Rect_wh() const;
	[[nodiscard]] SDL_FRect ToSDL_FRect_wh() const;
};

