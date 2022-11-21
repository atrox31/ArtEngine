#include "Debug.h"
#include <sstream>
#include <fstream>
#include "Core.h"

bool Debug::WRITE_TO_FILE = false;
bool Debug::WRITE_TO_GAME_CONSOLE = true;
#ifdef _DEBUG
bool Debug::WRITE_TO_SYSTEM_CONSOLE = true;
#else
bool Art::Debug::WRITE_TO_SYSTEM_CONSOLE = false;
#endif
std::string Debug::OUTPUT_FILE = "console.log";

void Debug::SetOutputFile(std::string output_file)
{
	WRITE_TO_FILE = true;
	OUTPUT_FILE = output_file;
}

void Debug::LOG(std::initializer_list<std::string> messages, bool new_line)
{
	for (auto s : messages) {
		LOG(s + (new_line ? "\n" : ""));
	}

}

void Debug::LOG(std::string message)
{
	write(message);
}

void Debug::LOG(SDL_Rect& rect)
{
	std::stringstream string("");
	string << "SDL_Rect( x:" << rect.x << ", y:" << rect.y << ", w:" << rect.w << ", h:" << rect.h << " )";

	//if (WRITE_TO_GAME_CONSOLE) :Core::GetConsoleHandle()->WriteLine(string.str());
	if (WRITE_TO_SYSTEM_CONSOLE) SDL_LogInfo(0, string.str().c_str());
}

void Debug::LOG(SDL_FRect& rect)
{
	std::stringstream string("");
	string << "SDL_Rect( x:" << rect.x << ", y:" << rect.y << ", w:" << rect.w << ", h:" << rect.h << " )";

	//if (WRITE_TO_GAME_CONSOLE) Core::GetConsoleHandle()->WriteLine(string.str());
	if (WRITE_TO_SYSTEM_CONSOLE) SDL_LogInfo(0, string.str().c_str());
}

void Debug::LOG(GPU_Rect& rect)
{
	std::stringstream string("");
	string << "SDL_Rect( x:" << rect.x << ", y:" << rect.y << ", w:" << rect.w << ", h:" << rect.h << " )";

	//if (WRITE_TO_GAME_CONSOLE) Core::GetConsoleHandle()->WriteLine(string.str());
	if (WRITE_TO_SYSTEM_CONSOLE) SDL_LogInfo(0, string.str().c_str());
}

void Debug::WARNING(std::initializer_list<std::string> messages, bool new_line)
{
	for (auto s : messages) {
		WARNING(s + (new_line ? "\n" : ""));
	}
}

void Debug::WARNING(std::string message)
{
	message = "[!]" + message;
	write(message);
}

void Debug::ERROR(std::string message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ArtCore Error", message.c_str(), Core::GetInstance()->GetWindowHandle());
	message = "[AC_ERROR]" + message;
	write(message);
	Core::GetInstance()->Exit();
}

void Debug::SDL_ERROR(std::string message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Error", message.c_str(), Core::GetInstance()->GetWindowHandle());
	message = "[SDL_ERROR]" + message + SDL_GetError();
	write(message);
	Core::GetInstance()->Exit();
}

std::string Debug::get_header()
{
	std::ostringstream opt;
	time_t localTime;
	tm tm_obj;
	time(&localTime);
	localtime_s(&tm_obj, &localTime);
	opt << "AC: " << "[" << tm_obj.tm_year + 1900 << "." << tm_obj.tm_mon + 1 << "." << tm_obj.tm_mday << "] " << tm_obj.tm_hour << ":" << tm_obj.tm_min << ":" << tm_obj.tm_sec << " | ";
	return opt.str();
}

void Debug::write(std::string& string)
{
	if (WRITE_TO_FILE) write_to_file(string);
	if (WRITE_TO_GAME_CONSOLE) write_to_core_console(string);
	if (WRITE_TO_SYSTEM_CONSOLE) write_to_windows_console(string);
}

void Debug::write_to_file(std::string& string)
{
	std::ofstream ofs(OUTPUT_FILE, std::ofstream::app);
	ofs << Debug::get_header() << string;
	ofs.close();
}

void Debug::write_to_core_console(std::string& string)
{
	//Core::GetConsoleHandle()->WriteLine(string);
}

void Debug::write_to_windows_console(std::string& string)
{
	if (WRITE_TO_SYSTEM_CONSOLE) SDL_LogInfo(0, "%s %s", Debug::get_header().c_str(), string.c_str());
}
