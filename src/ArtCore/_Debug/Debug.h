#pragma once
#include "SDL2/IncludeAll.h"
#include <string>

// rid of SDL do while thing
#ifdef _DEBUG
	#ifdef DEBUG_EDITOR
		#define __FILENAME__ (std::strrchr(__FILE__, '\\') ? std::strrchr(__FILE__, '\\') + 1 : __FILE__)
		#define ASSERT(condition, message) if(!(condition)) std::cout << __FILENAME__ << "[0x" << __LINE__ << "]" << message << std::endl;
	#else
		#define ASSERT(CONDITION, message) SDL_assert(CONDITION);
	#endif
#else
	#define ASSERT(CONDITION, message)
#endif

class Debug
{
private:
	static std::string last_WARNING;
	static int last_WARNING_c;
public:
	static bool WRITE_TO_FILE;
	static bool WRITE_TO_GAME_CONSOLE;
	static bool WRITE_TO_SYSTEM_CONSOLE;
	static bool WRITE_NECROLOGY;

	static void SetOutputFile(std::string output_file);


	static void NOTE_DEATH(const std::string& name);

	/*! Show message to console if [SHOW_LOG = true], write to file if set [WRITE_TO_FILE = true]
	 * \param messages { .. } list of string to write
	 * \param new_line - defines if every string is write in new line
	 */
	static void LOG(std::initializer_list<std::string> messages, bool new_line = true);

	/*! Show message to console if [SHOW_LOG = true], write to file if set [WRITE_TO_FILE = true]
	 * \param message string to write
	 */
	static void LOG(std::string message);

	/*! Show message to console if [SHOW_LOG = true], write to file if set [WRITE_TO_FILE = true]
	 * \param rect Struct to debug, SDL_Rect, SDL_FRect, GPU_Rect...
	 */
	static void LOG(SDL_Rect& rect);

	/*! Show message to console if [SHOW_LOG = true], write to file if set [WRITE_TO_FILE = true]
	 * \param rect Struct to debug, SDL_Rect, SDL_FRect, GPU_Rect...
	 */
	static void LOG(SDL_FRect& rect);

	/*! Show message to console if [SHOW_LOG = true], write to file if set [WRITE_TO_FILE = true]
	 * \param rect Struct to debug, SDL_Rect, SDL_FRect, GPU_Rect...
	 */
	static void LOG(GPU_Rect& rect);

	/*! Show message to console if [SHOW_WARNING = true], write to file if set [WRITE_TO_FILE = true]
	 * \param messages { .. } list of string to write
	 * \param new_line - defines if every string is write in new line
	 */
	static void WARNING(std::initializer_list<std::string> messages, bool new_line = true);

	/*! Show message to console, write to file if set [WRITE_TO_FILE = true]
	 * \param message string to write
	 */
	static void WARNING(std::string message);

	/*! Show message to console, write to file if set [WRITE_TO_FILE = true], shutdown appplication
	 * \param messages { .. } list of string to write
	 */
	static void ERROR(std::string messages);

	/*! Show sdl error, shutdown appplication
	 * \param message where irror ocurs
	 */
	static void SDL_ERROR(std::string message);


	/*! Print all sdl2 versions
	 */
	static void PritSDLVersions();

private:
	static std::string get_header();
	static std::string OUTPUT_FILE;

	static void write(const std::string&);

	static void write_to_file(const std::string&);
	static void write_to_core_console(const std::string&);
	static void write_to_windows_console(const std::string&);
};
