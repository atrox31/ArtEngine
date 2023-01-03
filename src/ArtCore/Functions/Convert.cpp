#include "Convert.h"
#include "Func.h"
#include <sstream>

std::string Convert::Rect2Str(const Rect& rect)
{
	return std::to_string(rect.x) + ':' + std::to_string(rect.y) + ':' + std::to_string(rect.w) + ':' + std::to_string(rect.h);
}

SDL_Point Convert::Str2Point(const std::string& text)
{
	auto Point = Func::Split(text, ':');
	if (Point.size() == 2) {
		return SDL_Point({
			Func::TryGetInt(Point[0]),
			Func::TryGetInt(Point[1])
			}
		);
	}
	return {};
}

SDL_FPoint Convert::Str2FPoint(const std::string& text)
{
	auto Point = Func::Split(text, ':');
	if (Point.size() == 2) {
		return SDL_FPoint(
			{
			Func::TryGetFloat(Point[0]),
			Func::TryGetFloat(Point[1])
			}
		);
	}
	return {};
}

std::string Convert::Point2String(const SDL_Point& point)
{
	return std::to_string(point.x) + ':' + std::to_string(point.y);
}

float Convert::DegreeToRadians(float degree) {
	return 0.0174532925f * degree;
}

float Convert::RadiansToDegree(float angle)
{
	return angle * 57.2957795131f;
}

Rect Convert::Str2Rect(const std::string& str)
{
	if (const auto rect = Func::Split(str, ':'); rect.size() == 4) {
		return {
			Func::TryGetFloat(rect[0]),
			Func::TryGetFloat(rect[1]),
			Func::TryGetFloat(rect[2]),
			Func::TryGetFloat(rect[3])
		};
	}
	return {};
}

std::string Convert::FPoint2String(const SDL_FPoint& point)
{
	return std::to_string(point.x) + ':' + std::to_string(point.y);
}

bool Convert::Str2Bool(const std::string& text)
{
	if (text.length() == 0)return false;
	if (text[0] == 't' || text[0] == 'T' || text[0] == '1') return true;
	return false;
}

bool Convert::Str2Bool(const char text)
{
	if (text == 't' || text == 'T' || text == '1') return true;
	return false;
}

bool Convert::Str2Bool(const char& text)
{
	if (text == 't' || text == 'T' || text == '1') return true;
	return false;
}

SDL_Color Convert::Hex2Color(const std::string& color)
{
	if (Func::IsHex(color)) {
		try {
			const int len = (int)color.length();
			Uint8 R = (Uint8)std::stoi(color.substr(1, 2), nullptr, 16);
			Uint8 G = (Uint8)std::stoi(color.substr(3, 2), nullptr, 16);
			Uint8 B = (Uint8)std::stoi(color.substr(5, 2), nullptr, 16);
			Uint8 A = 255;
			if (len == 9)
				A = (Uint8)std::stoi(color.substr(7, 2), nullptr, 16);
			return { R,G,B,A };
		}
		catch (const std::invalid_argument& ) {
			//err
			return { 0,0,0,0 };
		}
	}
	return { 0,0,0,0 };
}

std::string Convert::Color2Hex(const SDL_Color& color)
{
	std::stringstream ss;
	ss << "#" << std::hex << (color.r << 24 | color.g << 16 | color.b << 8 | color.a);
	return ss.str();
}
