#pragma once
#include "Rect.h"
#include <string>
class Convert
{
private:
	static const float pi_180;
	static const float one_radian;
public:
	// Get radians(direction) from degree
	static const float RadiansToDegree(float);

	// Get degree from radians(direction)
	static const float DegreeToRadians(float);

	// translate string to Rect
	static const Rect Str2Rect(std::string);

	// translate Rect to string
	static const std::string Rect2Str(Rect&);

	// translate string to point
	static const SDL_Point Str2Point(std::string);

	// translate point to string
	static const std::string Point2String(SDL_Point&);

	// translate string to Fpoint
	static const SDL_FPoint Str2FPoint(std::string);

	// translate Fpoint to string
	static const std::string FPoint2String(SDL_FPoint&);

	// fast text to bool converter, ony check first char
	static const bool Str2Bool(std::string);

	// fast text to bool converter, ony check first char
	static const bool Str2Bool(const char);

	// fast text to bool converter, ony check first char
	static const bool Str2Bool(const char&);

	// translate string to color
	static SDL_Color Str2Color(std::string);

	// translate color to string
	static std::string Color2Str(SDL_Color&);
};

