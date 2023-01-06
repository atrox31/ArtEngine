#pragma once
#include "SDL_pixels.h"

// color compare
inline bool operator==(SDL_Color const& a, SDL_Color const& b)
{
	return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}
inline bool operator!=(SDL_Color const& a, SDL_Color const& b)
{
	return a.r != b.r && a.g != b.g && a.b != b.b && a.a != b.a;
}
// Mix two colors together using middle value for every color
inline SDL_Color operator*(SDL_Color const& a, SDL_Color const& b)
{
	return {
		static_cast<Uint8>((a.r + b.r) / 2),
		static_cast<Uint8>((a.g + b.g) / 2),
		static_cast<Uint8>((a.b + b.b) / 2),
		static_cast<Uint8>((a.a + b.a) / 2)
		};
}

