// ReSharper disable CppInconsistentNaming
#include "Rect.h"

Rect::Rect()
{
	this->X = 0.0f;
	this->Y = 0.0f;
	this->W = 0.0f;
	this->H = 0.0f;
}

Rect::Rect(const GPU_Rect& rect)
{
	this->X = rect.x;
	this->Y = rect.y;
	this->W = rect.w;
	this->H = rect.h;
}

Rect::Rect(const SDL_Rect& rect)
{
	this->X = static_cast<float>(rect.x);
	this->Y = static_cast<float>(rect.y);
	this->W = static_cast<float>(rect.w);
	this->H = static_cast<float>(rect.h);
}

Rect::Rect(const SDL_FRect& rect)
{
	this->X = rect.x;
	this->Y = rect.y;
	this->W = rect.w;
	this->H = rect.h;
}

Rect::Rect(const Rect& rect)
{
	this->X = rect.X;
	this->Y = rect.Y;
	this->W = rect.W;
	this->H = rect.H;
}

Rect::Rect(const int x, const int y, const int w, const int h)
{
	this->X = static_cast<float>(x);
	this->Y = static_cast<float>(y);
	this->W = static_cast<float>(w);
	this->H = static_cast<float>(h);
}

Rect::Rect(const float x, const float y, const float w, const float h)
{
	this->X = x;
	this->Y = y;
	this->W = w;
	this->H = h;
}

Rect Rect::operator+(const SDL_FPoint& p) const
{
	return {X + p.x, Y + p.y, W + p.x, Y + p.y};
}

Rect Rect::operator-(const SDL_FPoint& p) const
{
	return {X - p.x, Y - p.y, W - p.x, H - p.y};
}

Rect Rect::operator+=(const SDL_FPoint& p)
{
	X += p.x;
	Y += p.y;
	W += p.x;
	H += p.y;
	return *this;
}

Rect Rect::operator-=(const SDL_FPoint& p)
{
	X -= p.x;
	Y -= p.y;
	W -= p.x;
	H -= p.y;
	return *this;
}

Rect Rect::operator*(const int& i) const
{
	return {X - static_cast<float>(i), Y - static_cast<float>(i), W + static_cast<float>(i), H + static_cast<float>(i)};
}

Rect Rect::operator*(const float& i) const
{
	return {X - i, Y - i, W + i, H + i};
}

Rect Rect::operator/(const int& i) const
{
	return {X + static_cast<float>(i), Y + static_cast<float>(i), W - static_cast<float>(i), H - static_cast<float>(i)};
}

Rect Rect::operator/(const float& i) const
{
	return {X + i, Y + i, W - i, H - i};
}

Rect Rect::operator*=(const int& i)
{
	X -= static_cast<float>(i);
	Y -= static_cast<float>(i);
	W += static_cast<float>(i);
	H += static_cast<float>(i);
	return *this;
}

Rect Rect::operator*=(const float& i)
{
	X -= i;
	Y -= i;
	W += i;
	H += i;
	return *this;
}

Rect Rect::operator/=(const int& i)
{
	X += static_cast<float>(i);
	Y += static_cast<float>(i);
	W -= static_cast<float>(i)*2.f;
	H -= static_cast<float>(i) * 2.f;
	return *this;
}

Rect Rect::operator/=(const float& i)
{
	X += i;
	Y += i;
	W -= i * 2.f;
	H -= i * 2.f;
	return *this;
}

bool Rect::operator==(Rect const& a) const
{
	return (
			static_cast<int>(X) == static_cast<int>(a.X) 
		&&	static_cast<int>(Y) == static_cast<int>(a.Y) 
		&&	static_cast<int>(W) == static_cast<int>(a.W) 
		&&	static_cast<int>(H) == static_cast<int>(a.H)
		);
}
bool Rect::operator!=(Rect const& a) const
{
	return (
			static_cast<int>(X) != static_cast<int>(a.X)
		&&	static_cast<int>(Y) != static_cast<int>(a.Y)
		&&	static_cast<int>(W) != static_cast<int>(a.W)
		&&	static_cast<int>(H) != static_cast<int>(a.H)
		);
}

SDL_FPoint Rect::A() const
{
	return {
		X, Y
	};
}
SDL_FPoint Rect::A_wh() const
{
	return {
		X, Y
	};
}
SDL_FPoint Rect::B() const
{
	return {
		W, Y
	};
}
SDL_FPoint Rect::B_wh() const
{
	return {
		X+W, Y
	};
}
SDL_FPoint Rect::C() const
{
	return {
		W, H
	};
}

SDL_FPoint Rect::C_wh() const
{
	return {
		   X+W, Y+H
	};
}

SDL_FPoint Rect::D() const
{
	return {
		X, H
	};
}

SDL_FPoint Rect::D_wh() const
{
	return {
		X, Y+H
	};
}

float Rect::Width() const
{
	return W - X;
}
float Rect::Height() const
{
	return H - Y;
}

void Rect::Rotate(const float deg)
{
	const float theta = static_cast<float>(static_cast<double>(deg) / 180.0 * M_PI);
	const float c = cos(theta);
	const float s = sin(theta);
	const float tx_x = X * c - Y * s;
	const float ty_y = X * s + Y * c;
	const float tx_w = W * c - H * s;
	const float ty_h = W * s + H * c;
	X = tx_x;
	Y = ty_y;
	W = tx_w;
	H = ty_h;
}

SDL_FPoint Rect::GetCenter() const
{
	return SDL_FPoint { (X + W) * 0.5f, (Y + H) * 0.5f };
}

float Rect::GetCenterX() const
{
	return (X + W) * 0.5f;
}

float Rect::GetCenterY() const
{
	return  (Y + H) * 0.5f ;
}

float Rect::Diagonal() const
{
	return ((Width() + Height()) / 2.f) * SQRT2;
}

bool Rect::PointInRect(const float& px, const float& py) const
{
	return ((px >= X) && (px <= (W)) &&
		(py >= Y) && (py <= H));
}

bool Rect::PointInRect(const SDL_Point& p) const
{
	return (
			(static_cast<float>(p.x) >= X)
		&&	(static_cast<float>(p.x) <= W)
		&&	(static_cast<float>(p.y) >= Y)
		&&	(static_cast<float>(p.y) <= H)
		);
}
bool Rect::PointInRectWh(const SDL_Point& p) const
{
	return (
			(static_cast<float>(p.x) >= X)
		&&	(static_cast<float>(p.x) <= (X+W))
		&&	(static_cast<float>(p.y) >= Y)
		&&	(static_cast<float>(p.y) <= (Y+H))
		);
}

bool Rect::PointInRect(const SDL_FPoint& p) const
{
	return (
			(p.x >= X)	
		&&	(p.x <= (W)) 
		&&	(p.y >= Y)	
		&&	(p.y <= (H))
		);
}

bool Rect::PointInRectWh(const SDL_FPoint& p) const
{
	return (
			((p.x) >= X)
		&&	((p.x) <= (X + W))
		&&	((p.y) >= Y)
		&&	((p.y) <= (Y + H))
		);
}

bool Rect::PointInRect(const vec2f& p) const
{
	return (
			(p.x >= X)
		&&	(p.x <= (W))
		&&	(p.y >= Y)
		&&	(p.y <= (H))
		);
}
bool Rect::PointInRectWh(const vec2f& p) const
{
	return (
			((p.x) >= X)
		&&	((p.x) <= (X + W))
		&&	((p.y) >= Y)
		&&	((p.y) <= (Y + H))
		);
}

GPU_Rect Rect::ToGPU_Rect() const
{
	return GPU_Rect{X, Y, W, H};
}

SDL_Rect Rect::ToSDL_Rect() const
{
	return SDL_Rect{
		static_cast<int>(X),
		static_cast<int>(Y),
		static_cast<int>(W),
		static_cast<int>(H)
	};
}

SDL_FRect Rect::ToSDL_FRect() const
{
	return SDL_FRect{X, Y, W, H};
}

GPU_Rect Rect::ToGPU_Rect_wh() const
{
	return GPU_Rect{X, Y, W-X, H-Y};
}

SDL_Rect Rect::ToSDL_Rect_wh() const
{
	return SDL_Rect{
		static_cast<int>(X),
		static_cast<int>(Y),
		static_cast<int>(W) - static_cast<int>(X),
		static_cast<int>(H) - static_cast<int>(Y)
	};
}

SDL_FRect Rect::ToSDL_FRect_wh() const
{
	return SDL_FRect{ X, Y, W-X, H-Y };
}
