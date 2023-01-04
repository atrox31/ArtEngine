// ReSharper disable All
#include "Func.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include "ArtCore/_Debug/Debug.h"
#include "physfs-3.0.2/src/physfs.h"

// ReSharper disable once CppInconsistentNaming
bool Func::PointInGPU_Rect(SDL_FPoint point, GPU_Rect rect)
{
	return (point.x > rect.x && point.x < rect.w && point.y > rect.y && point.y < rect.h);
}

int Func::TryGetInt(const std::string& str)
{
	int a = 0;
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
	float a = 0.f;
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

bool Func::RectCircleColliding(float circle_x, float circle_y, float circle_r, GPU_Rect rectangle_wh)
{
	const float distX = std::fabsf(circle_x - rectangle_wh.x - rectangle_wh.w / 2);
	const float distY = std::fabsf(circle_y - rectangle_wh.y - rectangle_wh.h / 2);

	if (distX > (rectangle_wh.w / 2 + circle_r)) { return false; }
	if (distY > (rectangle_wh.h / 2 + circle_r)) { return false; }

	if (distX <= (rectangle_wh.w / 2)) { return true; }
	if (distY <= (rectangle_wh.h / 2)) { return true; }

	const float dx = distX - rectangle_wh.w / 2;
	const float dy = distY - rectangle_wh.h / 2;
	return (dx * dx + dy * dy <= (circle_r * circle_r));
}

std::string Func::GetFileName(const std::string& path, const char separator, bool with_extension)
{
	if (path.length() == 0) return "";
	std::vector<std::string> path_separated = Func::Split(path, separator);
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

std::size_t Func::replace_all(std::string& inout, std::string what, std::string with)
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

std::vector<std::string> Func::Explode(const std::string& string, const char separator)
{
	std::vector<std::string> tokens;
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

std::vector<std::string> Func::Split(const std::string& string, const char separator)
{
	std::vector<std::string> internal;
	std::stringstream ss(string);
	std::string tok;

	while (getline(ss, tok, separator)) {
		if (!tok.empty())
			internal.push_back(tok);
	}

	return internal;
}

std::vector<std::string> Func::GetFileText(const std::string& file, int* size, const bool replace_slashes)
{
	std::vector<std::string> _return = std::vector<std::string>();
	if(size != nullptr)
		*size = 0;
	std::string buffer(GetFileBuf(file, nullptr));
	if (buffer.length() == 0) return _return;

	std::vector<std::string> data = Func::Explode(buffer, '\n');
	if (data.empty()) return _return;

	// clean from \r on ends
	for (std::string& line : data) {
		if (line[line.length() - 1] == '\r')
			line = line.substr(0, line.length() - 1);
		if(replace_slashes)
			Func::replace_all(line, "\\", "/");
		_return.push_back(line);
	}
	if (size != nullptr)
		*size = (int)_return.size();
	return _return;
}

char* Func::GetFileBuf(const std::string& file, Sint64* len)
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
	Debug::WARNING("File not exists in archive: '" + file + "'!");
	return nullptr;
}

unsigned char* Func::GetFileBytes(const std::string& file, Sint64* len)
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
	Debug::WARNING("File not exists in archive: '" + file + "'!");
	return nullptr;
}
SDL_RWops* Func::GetFileRWops(const std::string& file, Sint64* len)
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
	Debug::WARNING("File not exists in archive: '" + file + "'!");
	return nullptr;
}

SDL_GLattr Func::GetSdlAttrFromString(const std::string& arg, bool* error)
{
		// ReSharper disable StringLiteralTypo
	if (arg == "SDL_GL_RED_SIZE") return SDL_GL_RED_SIZE;
	else if (arg == "SDL_GL_GREEN_SIZE") return SDL_GL_GREEN_SIZE;
	else if (arg == "SDL_GL_BLUE_SIZE") return SDL_GL_BLUE_SIZE;
	else if (arg == "SDL_GL_ALPHA_SIZE") return SDL_GL_ALPHA_SIZE;
	else if (arg == "SDL_GL_BUFFER_SIZE") return SDL_GL_BUFFER_SIZE;
	else if (arg == "SDL_GL_DOUBLEBUFFER") return SDL_GL_DOUBLEBUFFER;
	else if (arg == "SDL_GL_DEPTH_SIZE") return SDL_GL_DEPTH_SIZE;
	else if (arg == "SDL_GL_STENCIL_SIZE") return SDL_GL_STENCIL_SIZE;
	else if (arg == "SDL_GL_ACCUM_RED_SIZE") return SDL_GL_ACCUM_RED_SIZE;
	else if (arg == "SDL_GL_ACCUM_GREEN_SIZE") return SDL_GL_ACCUM_GREEN_SIZE;
	else if (arg == "SDL_GL_ACCUM_BLUE_SIZE") return SDL_GL_ACCUM_BLUE_SIZE;
	else if (arg == "SDL_GL_ACCUM_ALPHA_SIZE") return SDL_GL_ACCUM_ALPHA_SIZE;
	else if (arg == "SDL_GL_STEREO") return SDL_GL_STEREO;
	else if (arg == "SDL_GL_MULTISAMPLEBUFFERS") return SDL_GL_MULTISAMPLEBUFFERS;
	else if (arg == "SDL_GL_MULTISAMPLESAMPLES") return SDL_GL_MULTISAMPLESAMPLES;
	else if (arg == "SDL_GL_ACCELERATED_VISUAL") return SDL_GL_ACCELERATED_VISUAL;
	else if (arg == "SDL_GL_RETAINED_BACKING") return SDL_GL_RETAINED_BACKING;
	else if (arg == "SDL_GL_CONTEXT_MAJOR_VERSION") return SDL_GL_CONTEXT_MAJOR_VERSION;
	else if (arg == "SDL_GL_CONTEXT_MINOR_VERSION") return SDL_GL_CONTEXT_MINOR_VERSION;
	else if (arg == "SDL_GL_CONTEXT_EGL") return SDL_GL_CONTEXT_EGL;
	else if (arg == "SDL_GL_CONTEXT_FLAGS") return SDL_GL_CONTEXT_FLAGS;
	else if (arg == "SDL_GL_CONTEXT_PROFILE_MASK") return SDL_GL_CONTEXT_PROFILE_MASK;
	else if (arg == "SDL_GL_SHARE_WITH_CURRENT_CONTEXT") return SDL_GL_SHARE_WITH_CURRENT_CONTEXT;
	else if (arg == "SDL_GL_FRAMEBUFFER_SRGB_CAPABLE") return SDL_GL_FRAMEBUFFER_SRGB_CAPABLE;
	else if (arg == "SDL_GL_CONTEXT_RELEASE_BEHAVIOR") return SDL_GL_CONTEXT_RELEASE_BEHAVIOR;
	else if (arg == "SDL_GL_CONTEXT_RESET_NOTIFICATION") return SDL_GL_CONTEXT_RESET_NOTIFICATION;
	else if (arg == "SDL_GL_CONTEXT_NO_ERROR") return SDL_GL_CONTEXT_NO_ERROR;
	else if (arg == "SDL_GL_FLOATBUFFERS") return SDL_GL_FLOATBUFFERS;
		// ReSharper restore StringLiteralTypo
	else {
		*error = true;
		Debug::WARNING("wrong sdl attr parameter '" + arg + "'!");
		return SDL_GL_RED_SIZE;

	}
}

Uint32 Func::load_shader(GPU_ShaderEnum shader_type, const char* filename)
{
	Sint64 siz = 0;
	char* source = Func::GetFileBuf(std::string(filename), &siz);

	if (siz == 0) return -1;

	// Compile the shader
	const Uint32 shader = GPU_CompileShader(shader_type, source);
	free(source);
	return shader;

}

GPU_ShaderBlock Func::load_shader_program(Uint32* p, const char* vertex_shader_file, const char* fragment_shader_file)
{
	const Uint32 v = load_shader(GPU_VERTEX_SHADER, vertex_shader_file);

	if (!v)
		GPU_LogError("Failed to load vertex shader (%s): %s\n", vertex_shader_file, GPU_GetShaderMessage());

	const Uint32 f = load_shader(GPU_FRAGMENT_SHADER, fragment_shader_file);

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

Func::DataValues::DataValues(const char* data, const Sint64 size)
{
	_data = std::map <std::string, std::vector <std::string>>();
	if (size == 0) {
		Debug::WARNING("DataValues: size is 0");
		return;
	}
	std::vector<std::string> values = std::vector<std::string>();
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
				Debug::WARNING("DataValues: section '" + c_section + "' is exists!");
			}
			else {
				_data.insert({ c_section, std::vector<std::string>() });
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
		Debug::WARNING("DataValues: section '" + section + "' not found!");
		return "NULL";
	}

	for (std::string& v : _data[section]) {
		std::vector<std::string> data = Func::Split(v, '=');
		if (data[0] != field) continue;
		if (data.size() == 1) {
			// not have value at all
			return "";
		}
		return data[1];
	}
	Debug::WARNING("DataValues: section '" + section + "' field '"+field+"' not found!");
	return "";
}

std::vector<std::string> Func::DataValues::GetSection(const std::string& section)
{
	if (!_data.contains(section)) {
		Debug::WARNING("DataValues: section '" + section + "' not found!");
		std::vector<std::string> _return = std::vector<std::string>();
		return _return;
	}
	return _data[section];
}