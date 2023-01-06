#pragma once
#include <vector>
#include <map>
#include <string>

#include "FC_Fontcache/SDL_FontCache.h"
#include "SDL2/IncludeAll.h"

class Console final
{
public:
	// Initialize console system
	static void Init();
	static void Create();
	~Console();
	static void SetOutputFile(const std::string& str);
	static void Exit();

	// write line to console
	static void WriteLine(const std::string& text);
	// execute command
	static void Execute(const std::string& command);

	// render console output to given target. This avoid Render class,
	// use clean SDL_Gpu
	// if console is not shown do nothing
	static void RenderConsole(GPU_Target* surface);

	// Start render console output and revive sdl input
	static void Show();
	// Stop render console output and revive sdl input
	static void Hide();

	// process events and return if handle something
	static bool ProcessEvent(const SDL_Event* sdl_event);

	// save all content to file
	static void SaveToFile();
private:
	static Console* _instance;
	inline static std::string _output_file = "console_output";
	bool _write_output_to_file = false;
	// show on screen flag
	bool _visible;
	// console font
	FC_Font* _font;

	// user input history
	std::vector<std::string> _string_input_history;
	// starting position of history
	int _string_input_history_pos = 1;

	// string list of all lines
	inline static std::vector<std::string> _console_lines;
	// current string input 
	std::string _current_input;

	// position of cursor
	int _current_cursor_pos = 0;
	// graphic
	const char _console_cursor_char = '|';
	bool _show_cursor = false;
	double _cursor_interval = 0.0;
	const int _console_lines_to_show = 12;
	float _console_lines_height;

	Console() = default;
	static std::string GetCurrentDate();
	static std::string GetCurrentTime();
	bool InnerProcessInput(const SDL_Event* sdl_event);
	void InnerRender(GPU_Target* surface);
};

