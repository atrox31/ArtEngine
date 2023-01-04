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

	void WriteLine(const std::string& text);
	void Execute(const std::string& command);

	void RenderConsole();
	void Show();
	void Hide();

	[[nodiscard]] bool IsShown() const
	{
		return _m_visible;
	}

private:
	void static WrLn(const std::string& line);
	// console functions
	bool _m_show_cursor = false;
	double _m_cursor_interval = 0.0;
	bool _m_visible;
	std::vector<std::string> _m_console_str;
	std::vector<std::string> _m_console_str_history;
	std::string _m_console_buf;
	std::string _m_console_shadow;
	int _m_console_shadow_pos = 0;
	int _m_console_page = 0;
	int _m_console_block = 12;
	static std::map<std::string, void(*)(std::vector<std::string> args, int argc)> _m_console_fun;
	FC_Font* _m_font;
};

