#include "Console.h"
#include <ranges>
#include <utility>

#include "ArtCore/Functions/Func.h"
#include "ArtCore/Graphic/ColorDefinitions.h"
#include "ArtCore/System/Core.h"
#include "ArtCore/Scene/Scene.h"
#include <ArtCore/predefined_headers/consola.h>

#include "ArtCore/CodeExecutor/CodeExecutor.h"

// must be static only for show list of functions
std::map<std::string, void(*)(std::vector<std::string> args, int argc)> Console::_m_console_fun;

Console::Console(): _m_visible(false), _m_font(nullptr)
{
	_m_console_str = std::vector<std::string>();
	_m_console_str_history = std::vector<std::string>();
}

#define AddFunction(name,argc,body,info)														\
_m_console_fun[name]=[](std::vector<std::string> args, int arg){									\
	if((argc)+1 == arg){																			\
		body;																					\
		WrLn(info);																				\
	}else{																						\
		WrLn(std::string("Error, '")+(name)+"' require "+std::to_string(argc)+" arguments");		\
	}																							\
};																								\

void Console::Init() {
	_m_font = FC_CreateFont();
	FC_LoadFont_RW(_m_font, SDL_RWFromConstMem(consola_ttf, 459181), 1, 16, C_BLACK, TTF_STYLE_NORMAL);

	AddFunction("pause", 0, Core::Pause() , "Game paused")
	AddFunction("resume", 0, Core::Play() , "Game resume")
	AddFunction("exit", 0, Core::GetInstance()->Exit();, "Game exit")
	AddFunction("spawn_instance", 3, {
		Core::GetInstance()->_current_scene->CreateInstance(args[1], Func::TryGetFloat(args[2]), Func::TryGetFloat(args[3]));
		}, "Instance spawned")
#ifdef _DEBUG
	AddFunction("spy", 1, 
		{
			Core::GetInstance()->Executor->DebugSetInstanceToTrack(
				Core::GetInstance()->_current_scene->GetInstanceById(Func::TryGetInt(args[1]))
			);
		}, "Get target info, press F4 to show info")
#endif
		AddFunction("help", 0, {
					for (const std::string& function_name : _m_console_fun | std::views::keys)
					{
						WrLn(function_name);
					}
			}, "... end list")
}

Console::~Console()
{
	FC_FreeFont(_m_font);
}

void Console::ProcessTextInput(const std::string& txinput) {
	_m_console_buf += txinput;
	_m_console_shadow = "";
	std::string prediction;
	for (const auto& key : _m_console_fun | std::views::keys) {
		bool find = false;
		for (int i = 0; i < std::min(_m_console_buf.length(), key.length()); i++) {
			if (key[i] == _m_console_buf[i]) {
				find = true;
			}
			else {
				find = false;
			}
		}
		if (find) {
			prediction = key;
		}
	}
	if (prediction.length() > 0) {
		_m_console_shadow = prediction;
	}
	else {
		_m_console_shadow = "";
	}
}

bool Console::ProcessKey(const SDL_KeyCode key)
{
	if (key == SDLK_HOME) {
		_m_visible = !_m_visible;
		if (_m_visible) Show(); else Hide();
		return true;
	}
	if (!_m_visible) {
		return false;
	}
	switch (key) {
	case SDLK_BACKSPACE:
	{
		if (_m_console_buf.length() > 0) {
			_m_console_buf.pop_back();
			if (_m_console_buf.length() == 0) {
				_m_console_shadow = "";
			}
		}
		else {
			_m_console_shadow = "";
		}
	}break;
	case SDLK_RETURN:
	{
		if (_m_console_buf.length() > 0) {
			if (!_m_console_str_history.empty() && _m_console_str_history[0] != _m_console_buf) {
				_m_console_str_history.insert(_m_console_str_history.begin(), _m_console_buf);
			}
			else {
				if (_m_console_str_history.empty()) {
					_m_console_str_history.insert(_m_console_str_history.begin(), _m_console_buf);
				}
			}
			Execute(_m_console_buf);
			_m_console_buf = "";
			_m_console_shadow = "";
			_m_console_shadow_pos = 0;
			_m_console_page = 0;
		}
	}break;
	case SDLK_UP:
	{
		const Uint8* state = SDL_GetKeyboardState(nullptr);
		if (state[SDL_SCANCODE_LCTRL]) {
			if (_m_console_page < int(_m_console_str.size()) - _m_console_block + 1) {
				_m_console_page++;
			}
		}
		else {
			if (_m_console_shadow_pos < int( _m_console_str_history.size() )) {
				_m_console_buf = _m_console_str_history[_m_console_shadow_pos++];
			}
		}
	}break;
	case SDLK_DOWN: {
		const Uint8* state = SDL_GetKeyboardState(nullptr);
		if (state[SDL_SCANCODE_LCTRL]) {
			if (_m_console_page > 0) {
				_m_console_page--;
			}
		}
		else {
			if (_m_console_shadow_pos > 0) {
				_m_console_buf = _m_console_str_history[--_m_console_shadow_pos];

			}
		}
	} break;
	case SDLK_RIGHT: {
		if (!_m_console_shadow.empty()) {
			_m_console_buf = _m_console_shadow;
		}
	}break;
	}
	return _m_visible;
}

void Console::WriteLine(const std::string& text)
{
	_m_console_str.insert(_m_console_str.begin(), text + '\n');
}

void Console::Execute(const std::string& command)
{
	const std::vector<std::string> arg = Func::Explode(command, ' ');
	if (_m_console_fun.contains(arg[0])) {
		WriteLine(command);
		_m_console_fun[arg[0]](arg, static_cast<int>(arg.size()));
	}
	else {
		WriteLine("err: " + arg[0] + " - not found");
	}
}

void Console::RenderConsole()
{
	if (!_m_visible) return;
	GPU_ActivateShaderProgram(0, nullptr);
	const int block_size = FC_GetLineHeight(_m_font) - 2;
	// draw back

	const int _screen_height = Core::GetScreenHeight();
	const int _screen_width = Core::GetScreenWidth();
	GPU_RectangleFilled(Core::GetScreenTarget(), 0, static_cast<float>(_screen_height),
	                    static_cast<float>(_screen_width),
	                    static_cast<float>(_screen_height - block_size * _m_console_block), C_DGRAY);
	FC_SetDefaultColor(_m_font, C_GREEN);
	for (int i = 0; i < std::min(_m_console_str.size(), (static_cast<size_t>(_m_console_block) - 1)); i++)
	{
		FC_DrawAlign(_m_font, Core::GetScreenTarget(), 2.0f,
		             static_cast<float>(_screen_height - (block_size) * (i + 2) - 4),
		             FC_ALIGN_LEFT, _m_console_str[static_cast<size_t>(i + _m_console_page)].c_str());
	}
	FC_SetDefaultColor(_m_font, C_DGREEN);
	FC_DrawAlign(_m_font, Core::GetScreenTarget(), 2.0f,
	             static_cast<float>(_screen_height - (block_size) - 4),
	             FC_ALIGN_LEFT, _m_console_shadow.c_str());
	FC_SetDefaultColor(_m_font, C_GREEN);
	_m_cursor_interval += Core::GetInstance()->DeltaTime;
	if (_m_cursor_interval > 0.5)
	{
		_m_cursor_interval = 0.0;
		_m_show_cursor = !_m_show_cursor;
	}
	std::string content = ">" + _m_console_buf + (_m_show_cursor ? "|" : "");
	FC_DrawAlign(_m_font, Core::GetInstance()->GetScreenTarget(), 2.0f,
	             static_cast<float>(_screen_height - (block_size) - 4),
	             FC_ALIGN_LEFT, content.c_str());
}

void Console::Show()
{
	SDL_StartTextInput();
	_m_visible = true;
	_m_console_buf = "";
	_m_console_shadow = "";
	_m_console_shadow_pos = 0;
	_m_console_page = 0;
	_m_console_block = 12;
}

void Console::Hide()
{
	SDL_StopTextInput();
	_m_visible = false;
}

void Console::WrLn(const std::string& line)
{
	Core::GetInstance()->Consola->WriteLine(line);
}
