#pragma once
#include <string>
#include "SDL_version.h"
#include SDL2_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR

// rid of SDL do while thing
#ifdef _DEBUG
#define ASSERT(CONDITION) \
SDL_assert(CONDITION);
#else
#define ASSERT(CONDITION)
#endif

class Debug
{
public:
	static bool WRITE_TO_FILE;
	static bool WRITE_TO_GAME_CONSOLE;
	static bool WRITE_TO_SYSTEM_CONSOLE;

	static void SetOutputFile(std::string output_file);

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
	static void WARNING(std::string);

	/*! Show message to console, write to file if set [WRITE_TO_FILE = true], shutdown appplication
	 * \param messages { .. } list of string to write
	 */
	static void ERROR(std::string);

	/*! Show sdl error, shutdown appplication
	 * \param message where irror ocurs
	 */
	static void SDL_ERROR(std::string);


private:
	static std::string get_header();
	static std::string OUTPUT_FILE;

	static void write(std::string&);

	static void write_to_file(std::string&);
	static void write_to_core_console(std::string&);
	static void write_to_windows_console(std::string&);
};
