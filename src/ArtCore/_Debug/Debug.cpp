/*
 *
 * Old version of ArtCore debugger.
 * Now Core have Debug element with user interface
 * Every log writes console and if have error
 * save all to file
 *
 *
#include "Debug.h"
#include <sstream>
#include <fstream>

#include "ArtCore/System/Core.h"

#ifdef _DEBUG

bool Debug::WRITE_TO_FILE = false;
bool Debug::WRITE_TO_GAME_CONSOLE = true;
bool Debug::WRITE_TO_SYSTEM_CONSOLE = true;
#else

bool Debug::WRITE_TO_FILE = false;
bool Debug::WRITE_TO_GAME_CONSOLE = true;
bool Debug::WRITE_TO_SYSTEM_CONSOLE = false;
#endif

std::string Debug::OUTPUT_FILE = "console.log";
std::string Debug::last_WARNING = "";
int Debug::last_WARNING_c = 0;

bool Debug::WRITE_NECROLOGY = false;
void Debug::SetOutputFile(std::string output_file)
{
	WRITE_TO_FILE = true;
	OUTPUT_FILE = output_file;
}

void Debug::NOTE_DEATH(const std::string& name)
{
	SDL_LogInfo(0, "~%s", name.c_str());
}

void Console::WriteLine(std::initializer_list<std::string> messages, bool new_line)
{
	for (auto s : messages) {
		LOG(s + (new_line ? "\n" : ""));
	}

}

void Console::WriteLine(std::string message)
{
	write(message);
}

void Console::WriteLine(SDL_Rect& rect)
{
	std::stringstream string("");
	string << "SDL_Rect( x:" << rect.x << ", y:" << rect.y << ", w:" << rect.w << ", h:" << rect.h << " )";

	if (WRITE_TO_GAME_CONSOLE) Core::GetInstance()->Consola->WriteLine(string.str());
	if (WRITE_TO_SYSTEM_CONSOLE) SDL_LogInfo(0, string.str().c_str());
}

void Console::WriteLine(SDL_FRect& rect)
{
	std::stringstream string("");
	string << "SDL_Rect( x:" << rect.x << ", y:" << rect.y << ", w:" << rect.w << ", h:" << rect.h << " )";

	if (WRITE_TO_GAME_CONSOLE) Core::GetInstance()->Consola->WriteLine(string.str());
	if (WRITE_TO_SYSTEM_CONSOLE) SDL_LogInfo(0, string.str().c_str());
}

void Console::WriteLine(GPU_Rect& rect)
{
	std::stringstream string("");
	string << "SDL_Rect( x:" << rect.x << ", y:" << rect.y << ", w:" << rect.w << ", h:" << rect.h << " )";

	if (WRITE_TO_GAME_CONSOLE) Core::GetInstance()->Consola->WriteLine(string.str());
	if (WRITE_TO_SYSTEM_CONSOLE) SDL_LogInfo(0, string.str().c_str());
}

void Console::WriteLine(std::initializer_list<std::string> messages, bool new_line)
{
	for (auto s : messages) {
		WARNING(s + (new_line ? "\n" : ""));
	}
}

void Console::WriteLine(std::string message)
{
#ifndef DEBUG_EDITOR
//	SDL_TriggerBreakpoint();
#endif // !DEBUG_EDITOR

	message = "[!]" + message;

	if (last_WARNING == message) {
		last_WARNING_c++;
	}
	else {
		if (last_WARNING_c > 0) {
			message = std::to_string(last_WARNING_c) + message;
			write(message);
			last_WARNING_c = 0;
		}
		else {
			write(message);
		}
		last_WARNING = message;
	}
}

void Console::WriteLine(std::string message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ArtCore Error", message.c_str(), Core::GetInstance()->GetWindowHandle());
	message = "[AC_ERROR]" + message;
	write(message);
	Core::GetInstance()->Exit();
}

void Console::WriteLine(std::string message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Error", message.c_str(), Core::GetInstance()->GetWindowHandle());
	message = "[SDL_ERROR]" + message + SDL_GetError();
	write(message);
	Core::GetInstance()->Exit();
}

void Debug::PritSDLVersions()
{
	SDL_version compiled;
	SDL_version linked;

	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	SDL_Log("compiled SDL version %u.%u.%u ...\n",
		compiled.major, compiled.minor, compiled.patch);
	SDL_Log("linking SDL version %u.%u.%u.\n",
		linked.major, linked.minor, linked.patch);


	SDL_TTF_VERSION(&compiled);
	const SDL_version* linked2 = TTF_Linked_Version();
	SDL_Log("compiled SDL_TTF version %u.%u.%u ...\n",
		compiled.major, compiled.minor, compiled.patch);
	SDL_Log("linking SDL_TTF version %u.%u.%u.\n",
		linked2->major, linked2->minor, linked2->patch);


	SDL_MIXER_VERSION(&compiled);
	const SDL_version* linked3 = Mix_Linked_Version();
	SDL_Log("compiled SDL_MIXER version %u.%u.%u ...\n",
		compiled.major, compiled.minor, compiled.patch);
	SDL_Log("linking SDL_MIXER version %u.%u.%u.\n",
		linked3->major, linked3->minor, linked3->patch);


	SDL_IMAGE_VERSION(&compiled);
	const SDL_version* linked4 = IMG_Linked_Version();
	SDL_Log("compiled SDL_IMAGE version %u.%u.%u ...\n",
		compiled.major, compiled.minor, compiled.patch);
	SDL_Log("linking SDL_IMAGE version %u.%u.%u.\n",
		linked4->major, linked4->minor, linked4->patch);


	SDL_NET_VERSION(&compiled);
	const SDL_version* linked5 = SDLNet_Linked_Version();
	SDL_Log("compiled SDL_NET version %u.%u.%u ...\n",
		compiled.major, compiled.minor, compiled.patch);
	SDL_Log("linking SDL_NET version %u.%u.%u.\n",
		linked5->major, linked5->minor, linked5->patch);
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

void Debug::write(const std::string& string)
{
	if (WRITE_TO_FILE) write_to_file(string);
	if (WRITE_TO_GAME_CONSOLE) write_to_core_console(string);
	if (WRITE_TO_SYSTEM_CONSOLE) write_to_windows_console(string);
}

void Debug::write_to_file(const std::string& string)
{

	std::ofstream ofs(OUTPUT_FILE, std::ofstream::app);
	ofs << Debug::get_header() << string << '\n';
	ofs.close();
}

void Debug::write_to_core_console(const std::string& string)
{
	if (Core::GetInstance()->Consola != nullptr) {
		Core::GetInstance()->Consola->WriteLine(string);
	}
}

void Debug::write_to_windows_console(const std::string& string)
{
	if (WRITE_TO_SYSTEM_CONSOLE) SDL_LogInfo(0, "%s %s", Debug::get_header().c_str(), string.c_str());
}
*/