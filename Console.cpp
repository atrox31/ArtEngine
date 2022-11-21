#include "Console.h"
#include "ColorDefinitions.h"
#include "consola.h"
#include <math.h>
#include "Core.h"

Console::Console()
{
	m_font = FC_CreateFont();
	FC_LoadFont_RW(m_font, SDL_RWFromConstMem(consola_ttf, 459181), 1, 20, C_BLACK, TTF_STYLE_NORMAL);
	m_console_str = std::vector<std::string>();
	 m_console_str_history = std::vector<std::string>();
}

Console::~Console()
{
	FC_FreeFont(m_font);
}

void Console::ProcessKey(SDL_KeyCode key)
{
	if (key == SDLK_HOME) {
		m_visibled = !m_visibled;
		if (m_visibled) Show(); else Hide();
	}
	if (!m_visibled) {
		return;
	}
	switch (key) {
	case SDLK_BACKSPACE:
	{
		if (m_console_buf.length() > 0) {
			m_console_buf.pop_back();
			if (m_console_buf.length() == 0) {
				m_console_shadow = "";
			}
		}
		else {
			m_console_shadow = "";
		}
	}break;
	case SDLK_RETURN:
	{
		if (m_console_buf.length() > 0) {
			m_console_str_history.insert(m_console_str_history.begin(), m_console_buf);
			Execute(m_console_buf);
			m_console_buf = "";
			m_console_shadow = "";
			m_console_shadow_pos = 0;
			m_console_page = 0;
		}
	}break;
	case SDLK_UP:
	{
		const Uint8* state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_LCTRL]) {
			if (m_console_page < int(m_console_str.size()) - m_console_block + 1) {
				m_console_page++;
			}
		}
		else {
			if (m_console_shadow_pos < int( m_console_str_history.size() )) {
				m_console_buf = m_console_str_history[m_console_shadow_pos++];
			}
		}
	}break;
	case SDLK_DOWN: {
		const Uint8* state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_LCTRL]) {
			if (m_console_page > 0) {
				m_console_page--;
			}
		}
		else {
			if (m_console_shadow_pos > 0) {
				m_console_buf = m_console_str_history[--m_console_shadow_pos];

			}
		}
	} break;
	case SDLK_RIGHT: {
		if (!m_console_shadow.empty()) {
			m_console_buf = m_console_shadow;
		}
	}break;
	}
	m_console_buf += SDL_GetKeyName(key);
	m_console_shadow = "";
	std::string prediction = "";
	for (const auto& [key, value] : m_console_fun) {
		bool find = false;
		for (int i = 0; i < std::min(m_console_buf.length(), key.length()); i++) {
			if (key[i] == m_console_buf[i]) {
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
		m_console_shadow = prediction;
	}
	else {
		m_console_shadow = "";
	}
}

void Console::WriteLine(std::string text)
{
	m_console_str_history.insert(m_console_str_history.begin(), text);
}

void Console::BindFunction(std::string command, void(*function)(std::vector<std::string>args))
{

}

void Console::Execute(std::string command)
{
}

void Console::RenderConsole()
{
	if (!m_visibled) return;
	GPU_ActivateShaderProgram(0, NULL);
	const int block_size = FC_GetLineHeight(m_font) - 6;
	// draw back
	
	int _screen_height = Core::GetInstance()->GetScreenHeight();
	int _screen_width = Core::GetInstance()->GetScreenWidth();
	GPU_RectangleFilled(Core::GetInstance()->GetScreenTarget(), 0, (float)_screen_height, (float)_screen_width, (float)(_screen_height - block_size * m_console_block), C_DGRAY);
	FC_SetDefaultColor(m_font, C_GREEN);
	for (int i = 0; i < std::min(m_console_str.size(), (size_t)(m_console_block - 1)); i++) {
		FC_DrawAlign(m_font, Core::GetInstance()->GetScreenTarget(), 2.0f, float(_screen_height - (block_size) * (i + 2) - 4), FC_AlignEnum::FC_ALIGN_LEFT, m_console_str[(size_t)(i + m_console_page)].c_str());
	}
	FC_SetDefaultColor(m_font, C_DGREEN);
	FC_DrawAlign(m_font, Core::GetInstance()->GetScreenTarget(), 2.0f, float(_screen_height - (block_size)-4), FC_AlignEnum::FC_ALIGN_LEFT, m_console_shadow.c_str());
	FC_SetDefaultColor(m_font, C_GREEN);
	FC_DrawAlign(m_font, Core::GetInstance()->GetScreenTarget(), 2.0f, float(_screen_height - (block_size)-4), FC_AlignEnum::FC_ALIGN_LEFT, m_console_buf.c_str());

}

void Console::Show()
{
	m_visibled = true;
	m_console_buf = "";
	m_console_shadow = "";
	m_console_shadow_pos = 0;
	m_console_page = 0;
	m_console_block = 12;
}

void Console::Hide()
{
	m_visibled = false;
}
