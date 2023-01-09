#pragma once

#include "vec2f.h"

	constexpr float SQRT2 = 1.414214f;
class Rect
{
public:
	float X, Y, W, H;
	
	Rect();
	Rect(const GPU_Rect&);
	Rect(const SDL_Rect&);
	Rect(const SDL_FRect&);
	Rect(const Rect&);
	Rect(int x, int y, int w, int h);
	Rect(float x, float y, float w, float h);

	~Rect() = default;
	Rect(Rect&&) = default;
	Rect& operator=(Rect&&) = default;
	Rect& operator=(const Rect&) = default;

	Rect operator+(const SDL_FPoint& p) const;
	Rect operator-(const SDL_FPoint& p) const;
	Rect operator+=(const SDL_FPoint& p);
	Rect operator-=(const SDL_FPoint& p);

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

	//Left upper corner
	// X-------+
	// |	|
	// +-------+
	[[nodiscard]] SDL_FPoint A() const;

	//Left upper corner
	// X-------+
	// |	|
	// +-------+
	[[nodiscard]] SDL_FPoint A_wh() const;

	//Right upper corner
	// +-------X
	// |	|
	// +-------+
	[[nodiscard]] SDL_FPoint B() const;

	//Right upper corner
	// +-------X
	// |	|
	// +-------+
	[[nodiscard]] SDL_FPoint B_wh() const;

	//Right lower corner
	// +-------+
	// |	|
	// +-------X
	[[nodiscard]] SDL_FPoint C() const;

	//Right lower corner
	// +-------+
	// |	|
	// +-------X
	[[nodiscard]] SDL_FPoint C_wh() const;

	//Left lower corner
	// +-------+
	// |	|
	// X-------+
	[[nodiscard]] SDL_FPoint D() const;

	//Left lower corner
	// +-------+
	// |	|
	// X-------+
	[[nodiscard]] SDL_FPoint D_wh() const;

	[[nodiscard]] float Width() const;
	[[nodiscard]] float Height() const;

	void Rotate(float deg);
	[[nodiscard]] SDL_FPoint GetCenter() const;
	[[nodiscard]] float GetCenterX() const;
	[[nodiscard]] float GetCenterY() const;

	[[nodiscard]] float Diagonal() const;

	[[nodiscard]] bool PointInRect(const float& px, const float& py) const;

	[[nodiscard]] bool PointInRect(const SDL_Point&) const;
	[[nodiscard]] bool PointInRectWh(const SDL_Point& p) const;
	[[nodiscard]] bool PointInRect(const SDL_FPoint&) const;
	[[nodiscard]] bool PointInRectWh(const SDL_FPoint& p) const;
	[[nodiscard]] bool PointInRect(const vec2f&) const;
	[[nodiscard]] bool PointInRectWh(const vec2f& p) const;

	// converts
	[[nodiscard]] GPU_Rect ToGPU_Rect() const;
	[[nodiscard]] SDL_Rect ToSDL_Rect() const;
	[[nodiscard]] SDL_FRect ToSDL_FRect() const;

	[[nodiscard]] GPU_Rect ToGPU_Rect_wh() const;
	[[nodiscard]] SDL_Rect ToSDL_Rect_wh() const;
	[[nodiscard]] SDL_FRect ToSDL_FRect_wh() const;
};

