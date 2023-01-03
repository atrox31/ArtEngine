#pragma once
#include <vector>
#include <map>
#include <string>

#include "FC_Fontcache/SDL_FontCache.h"
#include "SDL2/IncludeAll.h"

class Console final
{
public:
	Console();
	void Init();
	virtual ~Console();

	void ProcessTextInput(const std::string& txinput);

	// return true if hadled
	bool ProcessKey(SDL_KeyCode key);

	void WriteLine(std::string text);
	void Execute(const std::string& command);

	void RenderConsole();
	void Show();
	void Hide();

	bool IsShown() {
		return m_visibled;
	};

private:
	void static WrLn(std::string);
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
	std::map<std::string, void(*)(std::vector<std::string> args, int argc)> m_console_fun;
	FC_Font* m_font;
};

