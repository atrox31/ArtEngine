#pragma once
#include <map>
#include <string>
#include <vector>

#include "ArtCore/ShortTypenames.h"
#include "ArtCore/Structs/Rect.h"
#include "FC_Fontcache/SDL_FontCache.h"
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

	static str_vec VectorFromCharArray(char* arr[], int size);
	
	//get file name with or without extension
	static std::string GetFileName(const std::string& path, const char separator = '/', bool with_extension = false);

	static bool IsHex(const std::string&);
	
	static float LinearScale(float value, float value_min, float value_max, float scale_min, float scale_max);
	static std::string GetHexTable(const unsigned char* data, int size, int group = 16);
	static std::size_t ReplaceAll(std::string& inout, std::string what, std::string with);

	// check if file exists
	static bool FileExists(const std::string& file_name);

	// good for big data, for small use split
	static str_vec Explode(const std::string& string, char separator);
	// good for small data, for big use explode
	static str_vec Split(const std::string& string, char separator);

	/**
	 * \brief Split string into 2 parts
	 * \param string Text to split
	 * \param separator Separator
	 * \return Separated string or empty string if separator not found
	 */
	static std::pair<std::string, std::string> Split2(const std::string& string, char separator);

	/**
	 * \brief Get file text in separate lines, size can be nullptr
	 * \param file File name in archive
	 * \param size Set this value to number of output lines
	 * \param replace_slashes replace \ to /
	 * \return String vector of file lines
	 */
	static str_vec ArchiveGetFileText(const std::string& file, int* size, bool replace_slashes = false);

	static char* ArchiveGetFileBuffer(const std::string& file, Sint64* len);

	static unsigned char* ArchiveGetFileBytes(const std::string& file, Sint64* len);

	static SDL_RWops* ArchiveGetFileRWops(const std::string& file, Sint64* len);

	static SDL_GLattr GetSdlAttrFromString(const std::string& arg, bool* error);

	static Uint32 LoadShader(GPU_ShaderEnum shader_type, const char* filename);

	static GPU_ShaderBlock LoadShaderProgram(Uint32* p, const char* vertex_shader_file, const char* fragment_shader_file);
	static void FreeShader(const Uint32 p)	{		GPU_FreeShaderProgram(p);	}

	static FC_Font* LoadFontRw(SDL_RWops* sdl_r_wops, int pt_size);

	struct DataValues {
	public:
		DataValues(const char* data, Sint64 size);
		virtual ~DataValues();
		std::string GetData(const std::string& section, const std::string& field);
		str_vec GetSection(const std::string& section);

		[[nodiscard]] bool IsOk() const
		{
			return !_data.empty();
		}
	private:
		std::map<std::string, std::vector <std::string>> _data;
	};
};
