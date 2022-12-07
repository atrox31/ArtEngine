#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "SDL_version.h"
#pragma warning(push)
#include SDL2_INCLUDE_DIR
#include SDL2_IMAGE_INCLUDE_DIR
#include SDL2_MIXER_INCLUDE_DIR
#include SDL2_NET_INCLUDE_DIR
#include SDL2_TTF_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR
#pragma warning(pop)

#include <iostream>
#include <iomanip>
#include <map>
#include "Rect.h"

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
	return a.x < b.x && a.y < b.y;
}
// float point compare
inline bool operator==(SDL_FPoint const& a, SDL_FPoint const& b)
{
	return a.x == b.x && a.y == b.y;
}
inline bool operator!=(SDL_FPoint const& a, SDL_FPoint const& b)
{
	return !(a == b);
}
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
	return a.x < b.x && a.y < b.y;
}
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
	return SDL_Color({
		(Uint8)((a.r + b.r) / 2),
		(Uint8)((a.g + b.g) / 2),
		(Uint8)((a.b + b.b) / 2),
		(Uint8)((a.a + b.a) / 2)
		});
}



class Func
{
public:
	static bool PointInGPU_Rect(SDL_FPoint, GPU_Rect);
	static int TryGetInt(std::string);
	static float TryGetFloat(std::string);

	// return distance from point to point
	static float Distance(SDL_FPoint&, SDL_FPoint&);
	// return distance from point to point
	static float Distance(float P1_X, float P1_y, float P2_X, float P2_Y);

	// if circle and rectangle have intersection
	static bool RectCircleColliding(float circle_x, float circle_y, float circle_r, GPU_Rect rectangle);

	static bool IsHex(const std::string&);

	static float LinearScale(float value, float valueMin, float valueMax, float scaleMin, float scaleMax);
	static const std::string GetHexTable(const unsigned char* data, int size, int group = 16);
	static std::size_t replace_all(std::string& inout, std::string what, std::string with);

	// check if file exists
	static bool FileExists(std::string FileName);

	// good for big data, for small use split
	static const std::vector<std::string> Explode(std::string& String, const char Delim);
	// good for small data, for big use explode
	static const std::vector<std::string> Split(std::string& String, const char Delim);

	static std::vector<std::string> GetFileText(std::string file, int* size, bool replace_slashes = false);
	static char* GetFileBuf(std::string file, Sint64* len);
	static unsigned char* GetFileBytes(std::string file, Sint64* len);
	static SDL_RWops* GetFileRWops(std::string file, Sint64* len);

	static SDL_GLattr get_sdl_attr_from_string(std::string arg, bool* error);

	static Uint32 load_shader(GPU_ShaderEnum shader_type, const char* filename);

	static GPU_ShaderBlock load_shader_program(Uint32* p, const char* vertex_shader_file, const char* fragment_shader_file);
	static void free_shader(Uint32 p)
	{
		GPU_FreeShaderProgram(p);
	}

	struct DataValues {
	public:
		DataValues(const char* data, int size);
		virtual ~DataValues();
		std::string GetData(std::string section, std::string field);
		std::vector<std::string> GetSection(std::string section);
		bool IsOk() {
			return _data.size() > 0;
		}
	private:
		std::map <std::string, std::vector <std::string>> _data;
	};
};

