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
// color compare
inline bool operator==(SDL_Color const& a, SDL_Color const& b)
{
	return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}
inline bool operator!=(SDL_Color const& a, SDL_Color const& b)
{
	return a.r != b.r && a.g != b.g && a.b != b.b && a.a != b.a;
}



class Func
{
public:

	static int TryGetInt(std::string);
	static float TryGetFloat(std::string);

	// return distance from point to point
	static const float Distance(SDL_FPoint&, SDL_FPoint&);
	// return distance from point to point
	static const float Distance(float P1_X, float P1_y, float P2_X, float P2_Y);

	// if circle and rectangle have intersection
	static const bool RectCircleColliding(float circle_x, float circle_y, float circle_r, GPU_Rect rectangle);

	static bool IsHex(std::string&);

	static float LinearScale(float value, float valueMin, float valueMax, float scaleMin, float scaleMax);
	static const std::string GetHexTable(const unsigned char* data, int size, int group = 16);
	static std::size_t replace_all(std::string& inout, std::string what, std::string with);

	// chek if file exists
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

	static Uint32 load_shader(GPU_ShaderEnum shader_type, const char* filename)
	{	
		Uint32 shader;
		Sint64 siz = 0;
		char* source = Func::GetFileBuf(std::string(filename), &siz);

		if (siz == 0) return -1;

		// Compile the shader
		shader = GPU_CompileShader(shader_type, source);
		free(source);
		return shader;

	}

	static GPU_ShaderBlock load_shader_program(Uint32* p, const char* vertex_shader_file, const char* fragment_shader_file)
	{
		Uint32 v, f;
		v = load_shader(GPU_VERTEX_SHADER, vertex_shader_file);

		if (!v)
			GPU_LogError("Failed to load vertex shader (%s): %s\n", vertex_shader_file, GPU_GetShaderMessage());

		f = load_shader(GPU_FRAGMENT_SHADER, fragment_shader_file);

		if (!f)
			GPU_LogError("Failed to load fragment shader (%s): %s\n", fragment_shader_file, GPU_GetShaderMessage());

		*p = GPU_LinkShaders(v, f);

		if (!*p)
		{
			GPU_ShaderBlock b = { -1, -1, -1, -1 };
			GPU_LogError("Failed to link shader program (%s + %s): %s\n", vertex_shader_file, fragment_shader_file, GPU_GetShaderMessage());
			return b;
		}

		{
			GPU_ShaderBlock block = GPU_LoadShaderBlock(*p, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix");
			GPU_ActivateShaderProgram(*p, &block);

			return block;
		}
	}

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

