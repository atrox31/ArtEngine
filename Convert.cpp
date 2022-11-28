#include "Convert.h"
#include "Func.h"
#include <sstream>
const std::string Convert::Rect2Str(Rect& rect)
{
	return std::to_string(rect.x) + ':' + std::to_string(rect.y) + ':' + std::to_string(rect.w) + ':' + std::to_string(rect.h);
}
const SDL_FPoint Convert::Str2Point(std::string text)
{
	auto Point = Func::Split(text, ':');
	if (Point.size() == 2) {
		return SDL_FPoint(
			std::stof(Point[0]),
			std::stof(Point[1])
		);
	}
	return SDL_FPoint();
}

const std::string Convert::Point2String(SDL_FPoint& point)
{
	return std::to_string(point.x) + ':' + std::to_string(point.y);
}

const Rect Convert::Str2Rect(std::string str)
{
	auto rect = Func::Split(str, ':');
	if (rect.size() == 4) {
		return Rect(
			std::stof(rect[0]),
			std::stof(rect[1]),
			std::stof(rect[2]),
			std::stof(rect[3])
		);
	}
	return Rect();
}

const bool Convert::Str2Bool(std::string text)
{
	if (text.length() == 0)return false;
	if (text[0] == 't' || text[0] == 'T' || text[0] == '1') return true;
	return false;
}
const bool Convert::Str2Bool(const char text)
{
	if (text == 't' || text == 'T' || text == '1') return true;
	return false;
}
const bool Convert::Str2Bool(const char& text)
{
	if (text == 't' || text == 'T' || text == '1') return true;
	return false;
}

SDL_Color Convert::Str2Color(std::string color)
{
	if (Func::IsHex(color)) {
		const int len = (int)color.length();
		Uint8 R = (Uint8)std::stoi(color.substr(1, 2), nullptr, 16);
		Uint8 G = (Uint8)std::stoi(color.substr(3, 2), nullptr, 16);
		Uint8 B = (Uint8)std::stoi(color.substr(5, 2), nullptr, 16);
		Uint8 A = 0;
		if (len == 9)
			A = (Uint8)std::stoi(color.substr(7, 2), nullptr, 16);
		return SDL_Color({ R,G,B,A });
	}
	return SDL_Color();
}

std::string Convert::Color2Str(SDL_Color& color)
{
	std::stringstream ss;
	ss << "#" << std::hex << (color.r << 24 | color.g << 16 | color.b << 8 | color.a);
	return ss.str();
}
