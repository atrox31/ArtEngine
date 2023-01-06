#pragma once
#include "SDL_rect.h"


inline bool operator>=(SDL_FPoint const& a, SDL_FPoint const& b)
{
	return a.x >= b.x && a.y >= b.y;
}
inline bool operator <= (SDL_FPoint const& a, SDL_FPoint const& b)
{
	return a.x <= b.x && a.y <= b.y;
}
inline bool operator>(SDL_FPoint const& a, SDL_FPoint const& b)
{
	return a.x > b.x && a.y > b.y;
}
inline bool operator<(SDL_FPoint const& a, SDL_FPoint const& b)
{
	return a.x < b.x&& a.y < b.y;
}
inline SDL_FPoint operator-(const SDL_FPoint& lhs, const SDL_FPoint& rhs)
{
	return {
	lhs.x - rhs.x,
	lhs.y - rhs.y
	};
}
inline SDL_FPoint operator+(const SDL_FPoint& lhs, const SDL_FPoint& rhs)
{
	return {
	lhs.x + rhs.x,
	lhs.y + rhs.y
	};
}
inline SDL_FPoint operator*(const SDL_FPoint& lhs, const SDL_FPoint& rhs)
{
	return {
	lhs.x * rhs.x,
	lhs.y * rhs.y
	};
}
inline SDL_FPoint operator/(const SDL_FPoint& lhs, const SDL_FPoint& rhs)
{
	return {
	lhs.x / rhs.x,
	lhs.y / rhs.y
	};
}