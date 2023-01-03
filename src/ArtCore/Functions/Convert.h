#pragma once
#include <string>

#include "ArtCore/Structs/Rect.h"

class Convert
{
public:
	// Get radians(direction) from degree
	static float RadiansToDegree(float);

	// Get degree from radians(direction)
	static float DegreeToRadians(float);

	// translate string to Rect
	static Rect Str2Rect(const std::string&);

	// translate Rect to string
	static std::string Rect2Str(const Rect&);

	// translate string to point
	static SDL_Point Str2Point(const std::string&);

	// translate point to string
	static std::string Point2String(const SDL_Point&);

	// translate string to Fpoint
	static SDL_FPoint Str2FPoint(const std::string&);

	// translate Fpoint to string
	static std::string FPoint2String(const SDL_FPoint&);

	// fast text to bool converter, ony check first char
	static bool Str2Bool(const std::string&);

	// fast text to bool converter, ony check first char
	static bool Str2Bool(const char);

	// fast text to bool converter, ony check first char
	static bool Str2Bool(const char&);

	// translate string to color
	static SDL_Color Hex2Color(const std::string&);

	// translate color to string
	static std::string Color2Hex(const SDL_Color&);
};

