#include "Func.h"
#include "physfs-3.0.2/src/physfs.h"
#include "Debug.h"

bool Func::PointInGPU_Rect(SDL_FPoint point, GPU_Rect rect)
{
	return (point.x > rect.x && point.x < rect.w && point.y > rect.y && point.y < rect.h);
}

int Func::TryGetInt(std::string str)
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

float Func::TryGetFloat(std::string str)
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
float Func::Distance(float P1_X, float P1_y, float P2_X, float P2_Y)
{
	return std::hypotf(P2_X - P1_X, P2_Y - P1_y);
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

float Func::LinearScale(float value, float valueMin, float valueMax, float scaleMin, float scaleMax)
{
	const float percentOfValue = ((value - valueMin) / (valueMax - valueMin));
	return percentOfValue * (scaleMax - scaleMin) + scaleMin;
}

std::string const Func::GetHexTable(const unsigned char* data, int size, int group)
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

bool Func::FileExists(std::string FileName)
{
	struct stat buffer;
	return (stat(FileName.c_str(), &buffer) == 0);
}

const std::vector<std::string> Func::Explode(std::string& String, const char Delim)
{
	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = String.find(Delim, prev);
		if (pos == std::string::npos) pos = String.length();
		std::string token = String.substr(prev, pos - prev);
		if (!token.empty()) tokens.push_back(token);
		prev = pos + 1;
	} while (pos < String.length() && prev < String.length());
	return tokens;
}

const std::vector<std::string> Func::Split(std::string& String, const char Delim)
{
	std::vector<std::string> internal;
	std::stringstream ss(String);
	std::string tok;

	while (getline(ss, tok, Delim)) {
		if (!tok.empty())
			internal.push_back(tok);
	}

	return internal;
}

std::vector<std::string> Func::GetFileText(std::string file, int* size, bool replace_slashes)
{
	std::vector<std::string> _return = std::vector<std::string>();
	if(size != nullptr)
		*size = 0;
	std::string buffer(GetFileBuf(file, nullptr));
	if (buffer.length() == 0) return _return;

	std::vector<std::string> data = Func::Explode(buffer, '\n');
	if (data.size() == 0) return _return;

	// oczyszcza z \r na ko�cu
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

char* Func::GetFileBuf(std::string file, Sint64* len)
{
	if (PHYSFS_exists(file.c_str()))
	{
		char* myBuf;
		PHYSFS_File* myfile = PHYSFS_openRead(file.c_str());
		auto fileLngth = PHYSFS_fileLength(myfile);
		myBuf = new char[fileLngth + 1];
		auto length_read = PHYSFS_readBytes(myfile, myBuf, fileLngth);

		myBuf[fileLngth] = '\0';
		PHYSFS_close(myfile);
		if (len != nullptr) {
			*len = (Sint64)fileLngth;
		}
		return myBuf;
	}
	Debug::WARNING("File not exists in archive: '" + file + "'!");
	return nullptr;
}

unsigned char* Func::GetFileBytes(std::string file, Sint64* len)
{
	if (PHYSFS_exists(file.c_str()))
	{
		PHYSFS_File* myfile = PHYSFS_openRead(file.c_str());
		auto fileLngth = PHYSFS_fileLength(myfile);


		unsigned char* myBuf;

		myBuf = new unsigned char[fileLngth];
		auto length_read = PHYSFS_readBytes(myfile, myBuf, fileLngth);

		PHYSFS_close(myfile);
		if (len != nullptr) {
			*len = (Sint64)fileLngth;
		}
		return myBuf;
	}
	Debug::WARNING("File not exists in archive: '" + file + "'!");
	return nullptr;
}
SDL_RWops* Func::GetFileRWops(std::string file, Sint64* len)
{
	if (PHYSFS_exists(file.c_str()))
	{
		PHYSFS_File* myfile = PHYSFS_openRead(file.c_str());
		auto fileLngth = PHYSFS_fileLength(myfile);

		void* buffer = malloc((size_t)fileLngth);
		auto length_read = PHYSFS_readBytes(myfile, buffer, fileLngth);

		PHYSFS_close(myfile);
		if (len != nullptr) {
			*len = (Sint64)fileLngth;
		}
		return SDL_RWFromMem(buffer, (int)length_read);
	}
	Debug::WARNING("File not exists in archive: '" + file + "'!");
	return nullptr;
}

SDL_GLattr Func::get_sdl_attr_from_string(std::string arg, bool* error)
{
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
	else {
		*error = true;
		Debug::WARNING("wrong sdl attr parametr '" + arg + "'!");
		return SDL_GL_RED_SIZE;

	}
}

Uint32 Func::load_shader(GPU_ShaderEnum shader_type, const char* filename)
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

GPU_ShaderBlock Func::load_shader_program(Uint32* p, const char* vertex_shader_file, const char* fragment_shader_file)
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

Func::DataValues::DataValues(const char* data, int size)
{
	_data = std::map <std::string, std::vector <std::string>>();
	if (size == 0) {
		Debug::WARNING("DataValues: size is 0");
		return;
	}
	std::vector<std::string> values = std::vector<std::string>();
	std::string tmp_value = "";
	int pos = -1;
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

	std::string c_section = "";
	for (std::string& v : values) {
		if (v[0] == '[') {
			// section
			if (v[1] == '/') continue; // niekt�re pliki maj� zamkni�cia w sekcjach [aaa] ... [/aaa]
			//oczyszczenie z \r
			//Func::replace_all(v, "\r", "");
			c_section = v.substr(1, v.size() - 2);

			if (_data.find(c_section) != _data.end()) {
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

std::string Func::DataValues::GetData(std::string section, std::string field)
{
	if (_data.find(section) == _data.end()) {
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

std::vector<std::string> Func::DataValues::GetSection(std::string section)
{
	if (_data.find(section) == _data.end()) {
		Debug::WARNING("DataValues: section '" + section + "' not found!");
		std::vector<std::string> _return = std::vector<std::string>();
		return _return;
	}
	return _data[section];
}
