#include "Rect.h"

Rect::Rect()
{
	this->x = 0.0f;
	this->y = 0.0f;
	this->w = 0.0f;
	this->h = 0.0f;
}

Rect::Rect(GPU_Rect rect)
{
	this->x = rect.x;
	this->y = rect.y;
	this->w = rect.w;
	this->h = rect.h;
}

Rect::Rect(int x, int y, int w, int h)
{
	this->x = (float)x;
	this->y = (float)y;
	this->w = (float)w;
	this->h = (float)h;
}

Rect::Rect(float x, float y, float w, float h)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

Rect Rect::operator+(SDL_FPoint& p) const
{
	return {x + p.x, y + p.y, w + p.x, y + p.y};
}

Rect Rect::operator-(SDL_FPoint& p) const
{
	return {x - p.x, y - p.y, w - p.x, h - p.y};
}

Rect Rect::operator+=(SDL_FPoint& p)
{
	x += p.x;
	y += p.y;
	w += p.x;
	h += p.y;
	return *this;
}

Rect Rect::operator-=(SDL_FPoint& p)
{
	x -= p.x;
	y -= p.y;
	w -= p.x;
	h -= p.y;
	return *this;
}

Rect Rect::operator+(SDL_Point& p) const
{
	return {x + (float)p.x, y + (float)p.y, w + (float)p.x, h + (float)p.y};
}

Rect Rect::operator-(SDL_Point& p) const
{
	return {x - (float)p.x, y - (float)p.y, w - (float)p.x, h - (float)p.y};
}

Rect Rect::operator+=(SDL_Point& p)
{
	x += (float)p.x;
	y += (float)p.y;
	w += (float)p.x;
	h += (float)p.y;
	return *this;
}

Rect Rect::operator-=(SDL_Point& p)
{
	x -= (float)p.x;
	y -= (float)p.y;
	w -= (float)p.x;
	h -= (float)p.y;
	return *this;
}

Rect Rect::operator*(int& i) const
{
	return {x - (float)i, y - (float)i, w + (float)i, h + (float)i};
}

Rect Rect::operator*(float& i) const
{
	return {x - i, y - i, w + i, h + i};
}

Rect Rect::operator/(int& i) const
{
	return {x + (float)i, y + (float)i, w - (float)i, h - (float)i};
}

Rect Rect::operator/(float& i) const
{
	return {x + i, y + i, w - i, h - i};
}

Rect Rect::operator*=(int& i)
{
	x -= (float)i;
	y -= (float)i;
	w += (float)i;
	h += (float)i;
	return *this;
}

Rect Rect::operator*=(float& i)
{
	x -= i;
	y -= i;
	w += i;
	h += i;
	return *this;
}

Rect Rect::operator/=(int& i)
{
	x += (float)i;
	y += (float)i;
	w -= (float)i;
	h -= (float)i;
	return *this;
}

Rect Rect::operator/=(float& i)
{
	x += i;
	y += i;
	w -= i;
	h -= i;
	return *this;
}

bool Rect::operator==(Rect const& a) const
{
	return ((int)x == (int)a.x && (int)y == (int)a.y && (int)w == (int)a.w && (int)h == (int)a.h);
}
bool Rect::operator!=(Rect const& a) const
{
	return ((int)x != (int)a.x && (int)y != (int)a.y && (int)w != (int)a.w && (int)h != (int)a.h);
}

void Rect::Rotate(float deg)
{
	float theta = (float)((double)deg / 180.0 * M_PI);
	float c = cos(theta);
	float s = sin(theta);
	float tx_x = x * c - y * s;
	float ty_y = x * s + y * c;
	float tx_w = w * c - h * s;
	float ty_h = w * s + h * c;
	x = tx_x;
	y = ty_y;
	w = tx_w;
	h = ty_h;
}

SDL_FPoint Rect::GetCenter() const
{
	return SDL_FPoint { (x + w) * 0.5f, (y + h) * 0.5f };
}

bool Rect::PointInRect(const SDL_Point& p) const
{
	return ((p.x >= x) && (p.x <= (w)) &&
		(p.y >= y) && (p.y <= h)) ? true : false;
}
bool Rect::PointInRect_wh(const SDL_Point& p) const
{
	return ((p.x >= x) && (p.x <= (x+w)) &&
		(p.y >= y) && (p.y <= y+h)) ? true : false;
}

bool Rect::PointInRect(const SDL_FPoint& p) const
{
	return ((p.x >= x) && (p.x <= (w)) &&
		(p.y >= y) && (p.y <= (h))) ? true : false;
}

bool Rect::PointInRect_wh(const SDL_FPoint& p) const
{
	return ((p.x >= x) && (p.x <= (x+w)) &&
		(p.y >= y) && (p.y <= (y+h))) ? true : false;
}

bool Rect::PointInRect(const vec2f& p) const
{
	return ((p.x >= x) && (p.x <= (w)) &&
		(p.y >= y) && (p.y <= (h))) ? true : false;
}
bool Rect::PointInRect_wh(const vec2f& p) const
{
	return ((p.x >= x) && (p.x <= (x+w)) &&
		(p.y >= y) && (p.y <= (y+h))) ? true : false;
}

GPU_Rect Rect::ToGPU_Rect() const
{
	return GPU_Rect{x, y, w, h};
}

SDL_Rect Rect::ToSDL_Rect() const
{
	return SDL_Rect{(int)x, (int)y, (int)w, (int)h};
}

SDL_FRect Rect::ToSDL_FRect() const
{
	return SDL_FRect{x, y, w, h};
}

GPU_Rect Rect::ToGPU_Rect_wh() const
{
	return GPU_Rect{x, y, x + w, y + h};
}

SDL_Rect Rect::ToSDL_Rect_wh() const
{
	return SDL_Rect{(int)x, (int)y, (int)x - (int)w, (int)y - (int)h};
}

SDL_FRect Rect::ToSDL_FRect_wh() const
{
	return SDL_FRect{ x, y, x - w, y - h };
}
