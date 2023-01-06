#pragma once
#include "SDL_rect.h"
// float operations
inline SDL_FPoint operator+=(SDL_FPoint const& a, SDL_FPoint const& b)
{
	return SDL_FPoint({ a.x + b.x, a.y + b.y });
}
inline SDL_FPoint operator-=(SDL_FPoint const& a, SDL_FPoint const& b)
{
	return SDL_FPoint({ a.x - b.x, a.y - b.y });
}
inline SDL_FPoint operator*=(SDL_FPoint const& a, SDL_FPoint const& b)
{
	return SDL_FPoint({ a.x * b.x, a.y * b.y });
}
inline SDL_FPoint operator/=(SDL_FPoint const& a, SDL_FPoint const& b)
{
	return SDL_FPoint({ a.x / b.x, a.y / b.y });
}

// point compare
inline bool operator==(SDL_Point const& a, SDL_Point const& b)
{
	return a.x == b.x && a.y == b.y;
}
inline bool operator!=(SDL_Point const& a, SDL_Point const& b)
{
	return !(a == b);
}
inline bool operator>=(SDL_Point const& a, SDL_Point const& b)
{
	return a.x >= b.x && a.y >= b.y;
}
inline bool operator <= (SDL_Point const& a, SDL_Point const& b)
{
	return a.x <= b.x && a.y <= b.y;
}
inline bool operator>(SDL_Point const& a, SDL_Point const& b)
{
	return a.x > b.x && a.y > b.y;
}
inline bool operator<(SDL_Point const& a, SDL_Point const& b)
{
	return a.x < b.x&& a.y < b.y;
}