// ReSharper disable All
#include "Func.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include "ArtCore/Structs/Rect.h"
#include "ArtCore/_Debug/Debug.h"
#include "physfs-release-3.2.0/src/physfs.h"
#include "ArtCore/Functions/SDL_FPoint_extend.h"
#include "ArtCore/Gui/Console.h"

// ReSharper disable once CppInconsistentNaming
bool Func::PointInGPU_Rect(const SDL_FPoint& point, const GPU_Rect& rect)
{
	return (point.x > rect.x && point.x < rect.w && point.y > rect.y && point.y < rect.h);
}
bool Func::PointInGPU_Rect(const float& x, const float& y, const GPU_Rect& rect)
{
	return (x > rect.x && x < rect.w && y > rect.y && y < rect.h);
}

int Func::TryGetInt(const std::string& str)
{
	int a;
	try {
		a = std::stoi(str);
	}
	catch (const std::invalid_argument& ia) {
		ASSERT(false, "TryToGetFloat - Invalid argument : " + str);
		std::cerr << "Invalid argument: " << ia.what() << std::endl;
		return 0;
	}
	catch (const std::out_of_range& oor) {
		std::cerr << "Out of Range error: " << oor.what() << std::endl;
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Undefined error: " << e.what() << std::endl;
		return 0;
	}
	return a;
}

float Func::TryGetFloat(const std::string& str)
{
	float a;
	try {
		a = std::stof(str);
	}
	catch (const std::invalid_argument& ia) {
		//ASSERT(false, "TryToGetInt - Invalid argument : " << str);
		std::cerr << "Invalid argument: " << ia.what() << std::endl;
		return 0.f;
	}
	catch (const std::out_of_range& oor) {
		std::cerr << "Out of Range error: " << oor.what() << std::endl;
		return 0.f;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Undefined error: " << e.what() << std::endl;
		return 0.f;
	}
	return a;
}

float Func::Distance(SDL_FPoint& p1, SDL_FPoint& p2)
{
	return std::hypotf(p2.x - p1.x, p2.y - p1.y);
}
float Func::Distance(float p1_x, float p1_y, float p2_x, float p2_y)
{
	return std::hypotf(p2_x - p1_x, p2_y - p1_y);
}

vec2f Func::GetDirectionVector(const float& direction)
{
	return {
		std::cosf(direction),
		std::sinf(direction)
	};
}

float Func::GetVectorFromDirection(const vec2f& vector)
{
	return std::atan2f(vector.y, vector.x);
}

str_vec Func::VectorFromCharArray(char* arr[], int size)
{
	str_vec _return;
	for(int i=0; i< size; i++)
	{
		_return.emplace_back(arr[i]);
	}
	return _return;
}


std::string Func::GetFileName(const std::string& path, const char separator, bool with_extension)
{
	if (path.length() == 0) return "";
	str_vec path_separated = Func::Split(path, separator);
	if (with_extension)
	{
		return path_separated.back();
	}else
	{
		return Func::Split(path_separated.back(), '.').front();
	}
}

bool Func::IsHex(const std::string& value)
{
	const int len = (int)value.length();
	// rgb or rgba
	if (!(len == 7 || len == 9)) return false;
	if (value[0] != '#') return false;
	for (int i = 1; i < len; i++) {
		if ((
			(value[i] >= '0' && value[i] <= '9') ||
			(value[i] >= 'a' && value[i] <= 'f') ||
			(value[i] >= 'A' && value[i] <= 'F')
			) == false) return false;
	}
	return true;
}

float Func::LinearScale(float value, float value_min, float value_max, float scale_min, float scale_max)
{
	const float percentOfValue = ((value - value_min) / (value_max - value_min));
	return percentOfValue * (scale_max - scale_min) + scale_min;
}

std::string Func::GetHexTable(const unsigned char* data, int size, int group)
{
	std::stringstream stringstream;
	stringstream << std::endl;

	for (int i = 0; i < size; i++) {
		std::cout << "" << std::setw(2) << std::setfill('0') << std::hex << (int)data[i] << " ";
		if (i % group == 0 && i != 0) std::cout << std::endl;
	}

	return stringstream.str();
}

std::size_t Func::ReplaceAll(std::string& inout, std::string what, std::string with)
{
	std::size_t count{};
	for (std::string::size_type pos{};
		inout.npos != (pos = inout.find(what.data(), pos, what.length()));
		pos += with.length(), ++count) {
		inout.replace(pos, what.length(), with.data(), with.length());
	}
	return count;
}

bool Func::FileExists(const std::string& file_name)
{
	struct stat buffer;
	return (stat(file_name.c_str(), &buffer) == 0);
}

str_vec Func::Explode(const std::string& string, const char separator)
{
	str_vec tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = string.find(separator, prev);
		if (pos == std::string::npos) pos = string.length();
		std::string token = string.substr(prev, pos - prev);
		if (!token.empty()) tokens.push_back(token);
		prev = pos + 1;
	} while (pos < string.length() && prev < string.length());
	return tokens;
}

str_vec Func::Split(const std::string& string, const char separator)
{
	str_vec internal;
	std::stringstream ss(string);
	std::string tok;

	while (getline(ss, tok, separator)) {
		if (!tok.empty())
			internal.push_back(tok);
	}

	return internal;
}

std::pair<std::string, std::string> Func::Split2(const std::string& string, char separator)
{
	str_vec internal;
	std::stringstream ss(string);
	std::string tok;

	while (getline(ss, tok, separator)) {
		if (!tok.empty())
			internal.push_back(tok);
	}
	if (internal.size() == 2)
		return std::make_pair(internal[0], internal[1]);
	if (internal.size() == 1)
		return std::make_pair(internal[0], "");
	return std::make_pair("", "");
}

str_vec Func::ArchiveGetFileText(const std::string& file, int* size, const bool replace_slashes)
{
	str_vec _return = str_vec();
	if(size != nullptr)
		*size = 0;
	std::string buffer(ArchiveGetFileBuffer(file, nullptr));
	if (buffer.length() == 0) return _return;

	str_vec data = Func::Explode(buffer, '\n');
	if (data.empty()) return _return;

	// clean from \r on ends
	for (std::string& line : data) {
		if (line[line.length() - 1] == '\r')
			line = line.substr(0, line.length() - 1);
		if(replace_slashes)
			Func::ReplaceAll(line, "\\", "/");
		_return.push_back(line);
	}
	if (size != nullptr)
		*size = (int)_return.size();
	return _return;
}

char* Func::ArchiveGetFileBuffer(const std::string& file, Sint64* len)
{
	if (PHYSFS_exists(file.c_str()))
	{
		PHYSFS_File* my_file = PHYSFS_openRead(file.c_str());
		const auto file_length = PHYSFS_fileLength(my_file);
		char* my_buf = new char[file_length + 1];
		auto length_read = PHYSFS_readBytes(my_file, my_buf, file_length);

		my_buf[file_length] = '\0';
		PHYSFS_close(my_file);
		if (len != nullptr) {
			*len = (Sint64)file_length;
		}
		return my_buf;
	}
	Console::WriteLine("File not exists in archive: '" + file + "'!");
	return nullptr;
}

unsigned char* Func::ArchiveGetFileBytes(const std::string& file, Sint64* len)
{
	if (PHYSFS_exists(file.c_str()))
	{
		PHYSFS_File* my_file = PHYSFS_openRead(file.c_str());
		const auto file_length = PHYSFS_fileLength(my_file);

		unsigned char* my_buf = new unsigned char[file_length];
		auto length_read = PHYSFS_readBytes(my_file, my_buf, file_length);

		PHYSFS_close(my_file);
		if (len != nullptr) {
			*len = (Sint64)file_length;
		}
		return my_buf;
	}
	Console::WriteLine("File not exists in archive: '" + file + "'!");
	return nullptr;
}
SDL_RWops* Func::ArchiveGetFileRWops(const std::string& file, Sint64* len)
{
	if (PHYSFS_exists(file.c_str()))
	{
		PHYSFS_File* my_file = PHYSFS_openRead(file.c_str());
		const PHYSFS_sint64 file_length = PHYSFS_fileLength(my_file);

		void* buffer = malloc((size_t)file_length);
		const PHYSFS_sint64 length_read = PHYSFS_readBytes(my_file, buffer, file_length);

		PHYSFS_close(my_file);
		if (len != nullptr) {
			*len = (Sint64)file_length;
		}
		return SDL_RWFromMem(buffer, (int)length_read);
	}
	Console::WriteLine("File not exists in archive: '" + file + "'!");
	return nullptr;
}

SDL_GLattr Func::GetSdlAttrFromString(const std::string& arg, bool* error)
{
		// ReSharper disable StringLiteralTypo
		 if (arg == "SDL_GL_RED_SIZE")					return SDL_GL_RED_SIZE;
	else if (arg == "SDL_GL_GREEN_SIZE")				return SDL_GL_GREEN_SIZE;
	else if (arg == "SDL_GL_BLUE_SIZE")					return SDL_GL_BLUE_SIZE;
	else if (arg == "SDL_GL_ALPHA_SIZE")				return SDL_GL_ALPHA_SIZE;
	else if (arg == "SDL_GL_BUFFER_SIZE")				return SDL_GL_BUFFER_SIZE;
	else if (arg == "SDL_GL_DOUBLEBUFFER")				return SDL_GL_DOUBLEBUFFER;
	else if (arg == "SDL_GL_DEPTH_SIZE")				return SDL_GL_DEPTH_SIZE;
	else if (arg == "SDL_GL_STENCIL_SIZE")				return SDL_GL_STENCIL_SIZE;
	else if (arg == "SDL_GL_ACCUM_RED_SIZE")			return SDL_GL_ACCUM_RED_SIZE;
	else if (arg == "SDL_GL_ACCUM_GREEN_SIZE")			return SDL_GL_ACCUM_GREEN_SIZE;
	else if (arg == "SDL_GL_ACCUM_BLUE_SIZE")			return SDL_GL_ACCUM_BLUE_SIZE;
	else if (arg == "SDL_GL_ACCUM_ALPHA_SIZE")			return SDL_GL_ACCUM_ALPHA_SIZE;
	else if (arg == "SDL_GL_STEREO")					return SDL_GL_STEREO;
	else if (arg == "SDL_GL_MULTISAMPLEBUFFERS")		return SDL_GL_MULTISAMPLEBUFFERS;
	else if (arg == "SDL_GL_MULTISAMPLESAMPLES")		return SDL_GL_MULTISAMPLESAMPLES;
	else if (arg == "SDL_GL_ACCELERATED_VISUAL")		return SDL_GL_ACCELERATED_VISUAL;
	else if (arg == "SDL_GL_RETAINED_BACKING")			return SDL_GL_RETAINED_BACKING;
	else if (arg == "SDL_GL_CONTEXT_MAJOR_VERSION")		return SDL_GL_CONTEXT_MAJOR_VERSION;
	else if (arg == "SDL_GL_CONTEXT_MINOR_VERSION")		return SDL_GL_CONTEXT_MINOR_VERSION;
	else if (arg == "SDL_GL_CONTEXT_EGL")				return SDL_GL_CONTEXT_EGL;
	else if (arg == "SDL_GL_CONTEXT_FLAGS")				return SDL_GL_CONTEXT_FLAGS;
	else if (arg == "SDL_GL_CONTEXT_PROFILE_MASK")		return SDL_GL_CONTEXT_PROFILE_MASK;
	else if (arg == "SDL_GL_SHARE_WITH_CURRENT_CONTEXT")return SDL_GL_SHARE_WITH_CURRENT_CONTEXT;
	else if (arg == "SDL_GL_FRAMEBUFFER_SRGB_CAPABLE")	return SDL_GL_FRAMEBUFFER_SRGB_CAPABLE;
	else if (arg == "SDL_GL_CONTEXT_RELEASE_BEHAVIOR")	return SDL_GL_CONTEXT_RELEASE_BEHAVIOR;
	else if (arg == "SDL_GL_CONTEXT_RESET_NOTIFICATION")return SDL_GL_CONTEXT_RESET_NOTIFICATION;
	else if (arg == "SDL_GL_CONTEXT_NO_ERROR")			return SDL_GL_CONTEXT_NO_ERROR;
	else if (arg == "SDL_GL_FLOATBUFFERS")				return SDL_GL_FLOATBUFFERS;
		// ReSharper restore StringLiteralTypo
	else {
		*error = true;
		Console::WriteLine("wrong sdl attr parameter '" + arg + "'!");
		return (SDL_GLattr)0;

	}
}

Uint32 Func::LoadShader(GPU_ShaderEnum shader_type, const char* filename)
{
	Sint64 siz = 0;
	char* source = Func::ArchiveGetFileBuffer(std::string(filename), &siz);

	if (siz == 0) return -1;

	// Compile the shader
	const Uint32 shader = GPU_CompileShader(shader_type, source);
	free(source);
	return shader;

}

GPU_ShaderBlock Func::LoadShaderProgram(Uint32* p, const char* vertex_shader_file, const char* fragment_shader_file)
{
	const Uint32 v = LoadShader(GPU_VERTEX_SHADER, vertex_shader_file);

	if (!v)
		GPU_LogError("Failed to load vertex shader (%s): %s\n", vertex_shader_file, GPU_GetShaderMessage());

	const Uint32 f = LoadShader(GPU_FRAGMENT_SHADER, fragment_shader_file);

	if (!f)
		GPU_LogError("Failed to load fragment shader (%s): %s\n", fragment_shader_file, GPU_GetShaderMessage());

	*p = GPU_LinkShaders(v, f);

	if (!*p)
	{
		const GPU_ShaderBlock b = { -1, -1, -1, -1 };
		GPU_LogError("Failed to link shader program (%s + %s): %s\n", vertex_shader_file, fragment_shader_file, GPU_GetShaderMessage());
		return b;
	}

	{
		GPU_ShaderBlock block = GPU_LoadShaderBlock(*p, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix");
		GPU_ActivateShaderProgram(*p, &block);

		return block;
	}
}

FC_Font* Func::LoadFontRw(SDL_RWops* sdl_r_wops, int pt_size)
{
	FC_Font* font = FC_CreateFont();
	FC_LoadFont_RW(font, sdl_r_wops, 1, pt_size, { 255,255,255 }, TTF_STYLE_NORMAL);
	return font;
}

Func::DataValues::DataValues(const char* data, const Sint64 size)
{
	_data = std::map <std::string, std::vector <std::string>>();
	if (size == 0) {
		Console::WriteLine("DataValues: size is 0");
		return;
	}
	str_vec values = str_vec();
	std::string tmp_value;
	Sint64 pos = -1;
	while (pos++ < size) {
		if (data[pos] == '\r') continue;
		if (data[pos] == '\n') {
			values.push_back(tmp_value);
			tmp_value = "";
		}
		else {
			tmp_value += data[pos];
		}
	}

	std::string c_section;
	for (std::string& v : values) {
		if (v[0] == '[') {
			// section
			if (v[1] == '/') continue; // some files have end tag [aaa] ... [/aaa]
			//clean from \r
			//Func::replace_all(v, "\r", "");
			c_section = v.substr(1, v.size() - 2);

			if (_data.contains(c_section)) {
				Console::WriteLine("DataValues: section '" + c_section + "' is exists!");
			}
			else {
				_data.insert({ c_section, str_vec() });
			}
			continue;
		}
		_data[c_section].push_back(v);
	}
}

Func::DataValues::~DataValues()
{
	_data.clear();
}

/**
 * \brief 
 * \param section [section] of file
 * \param field field=value of file
 * \return data with target section, if not exists return "NULL", if value not exists return empty string ""
 */
std::string Func::DataValues::GetData(const std::string& section, const std::string& field)
{
	if (!_data.contains(section)) {
		Console::WriteLine("DataValues: section '" + section + "' not found!");
		return "NULL";
	}

	for (std::string& v : _data[section]) {
		str_vec data = Func::Split(v, '=');
		if (data[0] != field) continue;
		if (data.size() == 1) {
			// not have value at all
			return "";
		}
		return data[1];
	}
	Console::WriteLine("DataValues: section '" + section + "' field '"+field+"' not found!");
	return "";
}

str_vec Func::DataValues::GetSection(const std::string& section)
{
	if (!_data.contains(section)) {
		Console::WriteLine("DataValues: section '" + section + "' not found!");
		str_vec _return = str_vec();
		return _return;
	}
	return _data[section];
}
