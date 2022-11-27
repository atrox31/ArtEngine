#pragma once
#include <vector>
#include <map>
#include <string>

#include "SDL_version.h"

#pragma warning(push)
#include SDL2_INCLUDE_DIR
#include SDL2_IMAGE_INCLUDE_DIR
#include SDL2_TTF_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR
#include "SDL2/SDL_FontCache-master/SDL_FontCache.h"
#pragma warning(pop)

class Console
{
public:
	Console();
	void Init();
	virtual ~Console();

	void ProcessTextInput(std::string txinput);

	// return true if hadled
	bool ProcessKey(SDL_KeyCode key);

	void WriteLine(std::string text);

	void BindFunction(std::string command, void(*function)(std::vector<std::string>args));
	void Execute(std::string command);

	void RenderConsole();
	void Show();
	void Hide();

	bool IsShown() {
		return m_visibled;
	};

private:
	// console functions
	bool m_show_cursor = false;
	double m_cursor_interval = 0.0;
	bool m_visibled;
	std::vector<std::string> m_console_str;
	std::vector<std::string> m_console_str_history;
	std::string m_console_buf = "";
	std::string m_console_shadow = "";
	int m_console_shadow_pos = 0;
	int m_console_page = 0;
	int m_console_block = 12;
	std::map<std::string, void(*)(std::vector<std::string> args)> m_console_fun;
	FC_Font* m_font;
};

