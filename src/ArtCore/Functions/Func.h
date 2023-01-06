#pragma once
#include <map>
#include <string>
#include <vector>

#include "ArtCore/Structs/Rect.h"
#include "SDL2/IncludeAll.h"
class Func
{
public:
	static bool PointInGPU_Rect(const SDL_FPoint&, const  GPU_Rect&);
	static bool PointInGPU_Rect(const float& x, const float& y, const GPU_Rect&);
	static int TryGetInt(const std::string&);
	static float TryGetFloat(const std::string&);

	// return distance from point to point
	static float Distance(SDL_FPoint&, SDL_FPoint&);
	// return distance from point to point
	static float Distance(float p1_x, float p1_y, float p2_x, float p2_y);

	static vec2f GetDirectionVector(const float& direction);
	static float GetVectorFromDirection(const vec2f& vector);
	
	//get file name with or without extension
	static std::string GetFileName(const std::string& path, const char separator = '/', bool with_extension = false);

	static bool IsHex(const std::string&);

	static float LinearScale(float value, float value_min, float value_max, float scale_min, float scale_max);
	static std::string GetHexTable(const unsigned char* data, int size, int group = 16);
	static std::size_t replace_all(std::string& inout, std::string what, std::string with);

	// check if file exists
	static bool FileExists(const std::string& file_name);

	// good for big data, for small use split
	static std::vector<std::string> Explode(const std::string& string, const char separator);
	// good for small data, for big use explode
	static std::vector<std::string> Split(const std::string& string, const char separator);

	static std::vector<std::string> GetFileText(const std::string& file, int* size, bool replace_slashes = false);
	static char* GetFileBuf(const std::string& file, Sint64* len);
	static unsigned char* GetFileBytes(const std::string& file, Sint64* len);
	static SDL_RWops* GetFileRWops(const std::string& file, Sint64* len);

	static SDL_GLattr GetSdlAttrFromString(const std::string& arg, bool* error);

	static Uint32 load_shader(GPU_ShaderEnum shader_type, const char* filename);

	static GPU_ShaderBlock load_shader_program(Uint32* p, const char* vertex_shader_file, const char* fragment_shader_file);
	static void free_shader(Uint32 p)
	{
		GPU_FreeShaderProgram(p);
	}

	struct DataValues {
	public:
		DataValues(const char* data, Sint64 size);
		virtual ~DataValues();
		std::string GetData(const std::string& section, const std::string& field);
		std::vector<std::string> GetSection(const std::string& section);
		bool IsOk() const
		{
			return !_data.empty();
		}
	private:
		std::map<std::string, std::vector <std::string>> _data;
	};
};
