#include "Console.h"
#include <ranges>
#include <utility>

#include "ArtCore/Functions/Func.h"
#include "ArtCore/Graphic/ColorDefinitions.h"
#include "ArtCore/System/Core.h"
#include "ArtCore/Scene/Scene.h"
#include <ArtCore/predefined_headers/consola.h>

#include "ArtCore/main.h"
#include "ArtCore/CodeExecutor/CodeExecutor.h"
#include "ArtCore/Functions/Convert.h"

void Console::Init() {
	if (_instance != nullptr) return;
	_instance = new Console();

	//_console_lines.insert(_console_lines.begin(), "[" + GetCurrentTime() + "] " + "ArtCore v" + std::to_string(VERSION_MAIN) + '.' + std::to_string(VERSION_MINOR) + '\n');

	_instance->_font = FC_CreateFont();
	FC_LoadFont_RW(_instance->_font, SDL_RWFromConstMem(consola_ttf, 459181), 1, 16, C_BLACK, TTF_STYLE_NORMAL);
	// generate lines to test console height, +1 to input line
	std::string tmp_test;
	for (int i = 0; i < _instance->_console_lines_to_show + 1; i++)
	{
		tmp_test += "A\n";
	}
	const GPU_Rect tmp_rect = FC_GetBounds(_instance->_font, 0.f, 0.f, FC_ALIGN_LEFT, { 1.f, 1.f }, "%s", tmp_test.c_str());
	_instance->_console_lines_height = tmp_rect.h;
}

Console::~Console()
{
	FC_FreeFont(_instance->_font);
}

void Console::SetOutputFile(const std::string& str)
{
	if (_instance == nullptr) return;
	Console::WriteLine("Console output set to: " + str);
	_output_file = str;
	_instance->_write_output_to_file = true;
}

void Console::Exit()
{
	delete _instance;
}

void Console::WriteLine(const std::string& text)
{
	_console_lines.emplace_back( "[" + GetCurrentTime() +"] " + text + '\n');
#ifdef _DEBUG
#ifdef DEBUG_EDITOR
	std::cout << "INFO: " + text << std::endl;
#else
	SDL_Log("%s", text.c_str());
#endif
#endif
}

void Console::Execute(const std::string& command)
{
	if (_instance == nullptr) return;
	if (command.empty()) return;
	const str_vec arg = Func::Explode(command, ' ');
	if (arg.empty()) return;


	// handle core functions
	if (arg[0] == "exit")
	{
		Core::Exit();
		return;
	}
#ifdef _DEBUG
	if (arg[0] == "spy")
	{
		if (arg.size() == 2)
		{
			Core::Executor()->DebugSetInstanceToTrack(Core::GetCurrentScene()->GetInstanceById(Func::TryGetInt(arg[1])));
			const str_vec text = Func::Split(Core::Executor()->DebugGetTrackInfo(), '\n');
			Core::GetInstance()->CoreDebug.SetSpyLines(static_cast<int>(text.size()));
		}
		else
		{
			WriteLine("Error: spy need instance id");
		}
		return;
	}
	if (arg[0] == "game_speed") {
		if (arg.size() == 2)
		{
			Core::GameSpeed = static_cast<double>(Func::TryGetInt(arg[1]));
		}
		else
		{
			WriteLine("Error: usage game_speed <int> (default 1.0)");
		}
		return;
	}
#endif

	WriteLine("Command: '" + command + "' not found.");
}

void Console::RenderConsole(GPU_Target* surface)
{
	if (_instance == nullptr) return;
	if (!_instance->_visible) return;
	_instance->InnerRender(surface);
}


void Console::Show()
{
	if (_instance == nullptr) return;
	SDL_StartTextInput();
	_instance->_visible = true;
	_instance->_current_input = "";
	_instance->_current_cursor_pos = 0;
}

void Console::Hide()
{
	if (_instance == nullptr) return;
	SDL_StopTextInput();
	_instance->_visible = false;
}

bool Console::ProcessEvent(const SDL_Event* sdl_event)
{
	if (_instance == nullptr) return false;
	return _instance->InnerProcessInput(sdl_event);
}

void Console::ConsoleHomeButtonPressed()
{
	if (_instance == nullptr) return;
	_instance->_visible = !_instance->_visible;
	if (_instance->_visible)
	{
		Show();
	}
	else
	{
		Hide();
	}
}

void Console::SaveToFile()
{
	std::ostringstream opt;
	time_t local_time;
	tm tm_obj{};
	time(&local_time);
	localtime_s(&tm_obj, &local_time);
	opt << _output_file << "_" << tm_obj.tm_year + 1900 <<
		"_" << std::setfill('0') << std::setw(2) << tm_obj.tm_mon + 1 <<
		"_" << std::setfill('0') << std::setw(2) << tm_obj.tm_mday << "_" <<
		std::setfill('0') << std::setw(2) << tm_obj.tm_hour <<
		"_" << std::setfill('0') << std::setw(2) << tm_obj.tm_min <<
		"_" << std::setfill('0') << std::setw(2) << tm_obj.tm_sec << ".log";

	SDL_RWops* file = SDL_RWFromFile(opt.str().c_str(), "w");
	if (file != nullptr)
	{
		for (const std::string& console_line : _console_lines)
		{
			SDL_RWwrite(file, console_line.c_str(), sizeof(char), console_line.length());
		}
		//Close file handler
		SDL_RWclose(file);
	}
}

Console::Console(): _write_output_to_file(false), _visible(false), _font(nullptr), _string_input_history_pos(1),
                    _current_cursor_pos(0),
                    _show_cursor(false),
                    _cursor_interval(0.0),
                    _console_lines_height(0.0f)
{}

std::string Console::GetCurrentDate()
{
	std::ostringstream opt;
	time_t local_time;
	tm tm_obj{};
	time(&local_time);
	localtime_s(&tm_obj, &local_time);
	opt << tm_obj.tm_year + 1900 << 
	"." << std::setfill('0') << std::setw(2) << tm_obj.tm_mon + 1 << 
	"." << std::setfill('0') << std::setw(2) << tm_obj.tm_mday;
	return opt.str();
}

std::string Console::GetCurrentTime()
{
	std::ostringstream opt;
	time_t local_time;
	tm tm_obj{};
	time(&local_time);
	localtime_s(&tm_obj, &local_time);
	opt << std::setfill('0') << std::setw(2) << tm_obj.tm_hour << 
	":" << std::setfill('0') << std::setw(2) << tm_obj.tm_min << 
	":" << std::setfill('0') << std::setw(2) << tm_obj.tm_sec;
	return opt.str();
}

bool Console::InnerProcessInput(const SDL_Event* sdl_event)
{
	if (_instance == nullptr) return false;
	// not console interesting event
	if (!_visible) return false;

	if (sdl_event->type == SDL_KEYDOWN)
	{
		switch (sdl_event->key.keysym.sym) {
		case SDLK_BACKSPACE:
		{
			if ((_current_input.length() > 0) && (_current_cursor_pos > 0))
			{
				_current_input.erase(static_cast<size_t>(_current_cursor_pos)-1, 1);
				_current_cursor_pos--;
			}
			return true;
		}
		case SDLK_DELETE:
		{
			if ((_current_input.length() > 0) && (_current_cursor_pos < static_cast<int>(_current_input.length()) ))
			{
				_current_input.erase(_current_cursor_pos, 1);
				
			}
			return true;
		}
		case SDLK_RETURN:
		{
			if (_current_input.length() > 0) {
				_console_lines.emplace_back(_current_input);
				_current_cursor_pos = 0;
				_string_input_history.emplace_back(_current_input);
				_string_input_history_pos = static_cast<int>(_string_input_history.size());
				Execute(_current_input);
				_current_input.clear();
			}
			return true;
		}
		case SDLK_UP:
		{
			if (_string_input_history.empty()) return true;
			if (_string_input_history_pos > 0) {
				_string_input_history_pos--;
			}
			_current_input = _string_input_history[_string_input_history_pos];
			_current_cursor_pos = static_cast<int>(_current_input.length());
			return true;
		}
		case SDLK_DOWN:
		{
			if (_string_input_history.empty()) return true;
			if (_string_input_history_pos < static_cast<int>(_string_input_history.size()-1)) {
				_string_input_history_pos++;
			}
			_current_input = _string_input_history[_string_input_history_pos];
			_current_cursor_pos = static_cast<int>(_current_input.length());
			return true;
		}
		case SDLK_RIGHT:
		{
			if (_current_cursor_pos < static_cast<int>(_current_input.length())) {
				_current_cursor_pos++;
			}
			_show_cursor = true;
			return true;
		}
		case SDLK_LEFT:
		{
			if (_current_cursor_pos > 0) {
				_current_cursor_pos--;
			}
			_show_cursor = true;
			return true;
		}
		
		}
	}
	else {
		// if not key then text input
		if(sdl_event->type == SDL_TEXTINPUT)
		{
			//e.text.text
			if(_current_cursor_pos < static_cast<int>(_current_input.length()))
			{
				_current_input.insert(_current_cursor_pos, sdl_event->text.text);
			}else
			{
				_current_input += sdl_event->text.text;
			}
			_current_cursor_pos++;
			return true;
		}
	}

	return false;
}

void Console::InnerRender(GPU_Target* surface)
{
	if (_instance == nullptr) return;
	_cursor_interval += Core::DeltaTime;
	if(_cursor_interval >= 1.0)
	{
		_cursor_interval = 0.0;
		_show_cursor = !_show_cursor;
	}
	// input box
	float width = 0.f;
	float height = 0.f;
	float cursor_pos = 0.f;
	for(int i=0; i<static_cast<int>(_current_input.length()); i++)
	{
		const GPU_Rect char_size = FC_GetBounds(_font, 0.f, 0.f, FC_ALIGN_LEFT, { 1.f, 1.f }, "%c", _current_input[i]);
		width += char_size.w;
		height = std::max(height, char_size.h);
		if(i == _current_cursor_pos-1)
		{
			cursor_pos = width;
		}
	}
	if(_current_input.length() == 0)
	{
		height = FC_GetBounds(_font, 0.f, 0.f, FC_ALIGN_LEFT, { 1.f, 1.f }, "A").h;
	}
	float current_height = static_cast<float>(Core::GetScreenHeight() - 1) - height;

	// render
	GPU_ActivateShaderProgram(0, nullptr);
	const GPU_Rect console_area{
		1.f, static_cast<float>(Core::GetScreenHeight() - 1) - _console_lines_height,
		static_cast<float>(Core::GetScreenWidth() - 1), _console_lines_height
	};

	GPU_RectangleFilled2(surface, console_area, C_DGRAY);
	GPU_Rectangle2(surface, console_area, C_LGREEN);

	const float line_h= FC_DrawEffect(_font, surface, 4.f, current_height, { FC_ALIGN_LEFT, {1.f, 1.f}, C_LGREEN }, "%s", _current_input.c_str()).h + 2.f;
	if (_show_cursor) {
		FC_DrawEffect(_font, surface, cursor_pos, current_height, { FC_ALIGN_LEFT, {1.f, 1.f}, C_LGREEN },"|");
	}
	current_height -= line_h;

	if (_current_input.length() == 0)
	{
		current_height -= FC_GetBounds(_font, 0.f, 0.f, FC_ALIGN_LEFT, { 1.f, 1.f }, "A").h;
	}
	// rest of console
	const int star_line = static_cast<int>(_console_lines.size())-1;
	const int end_line = std::max(0, star_line - _console_lines_to_show + 1);
	for(int i = star_line; i > end_line; i--)
	{
		current_height -= FC_DrawEffect(_font, surface, 4.f, current_height, { FC_ALIGN_LEFT, {1.f, 1.f}, C_GREEN }, "%s", _console_lines[i].c_str()).h + 2.f;
		
	}

}
