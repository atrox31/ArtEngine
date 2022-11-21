#include "Func.h"
#include "physfs-3.0.2/src/physfs.h"
#include "Debug.h"

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
