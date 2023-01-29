#pragma once
#include "SDL2/SDL2/include/SDL_pixels.h"

constexpr SDL_Color C_NONE 		{ static_cast<Uint8>(0),		static_cast<Uint8>(0),	static_cast<Uint8>(0),	static_cast<Uint8>(0)	  };
constexpr SDL_Color C_FULL 		{ static_cast<Uint8>(255),	static_cast<Uint8>(255),	static_cast<Uint8>(255),	static_cast<Uint8>(255) };
constexpr SDL_Color C_DEFAULT	{ static_cast<Uint8>(0),		static_cast<Uint8>(0),	static_cast<Uint8>(0),	static_cast<Uint8>(255) };

constexpr SDL_Color C_DRED		{ static_cast<Uint8>(128),	static_cast<Uint8>(0),	static_cast<Uint8>(0),	static_cast<Uint8>(255) };
constexpr SDL_Color C_RED		{ static_cast<Uint8>(255),	static_cast<Uint8>(0),	static_cast<Uint8>(0),	static_cast<Uint8>(255) };
constexpr SDL_Color C_LRED		{ static_cast<Uint8>(255),	static_cast<Uint8>(64),	static_cast<Uint8>(64),	static_cast<Uint8>(255) };

constexpr SDL_Color C_DGREEN	{ static_cast<Uint8>(0),		static_cast<Uint8>(128),	static_cast<Uint8>(0),	static_cast<Uint8>(255) };
constexpr SDL_Color C_GREEN		{ static_cast<Uint8>(0),		static_cast<Uint8>(255),	static_cast<Uint8>(0),	static_cast<Uint8>(255) };
constexpr SDL_Color C_LGREEN	{ static_cast<Uint8>(64),	static_cast<Uint8>(255),	static_cast<Uint8>(64),	static_cast<Uint8>(255) };

constexpr SDL_Color C_DYELLOW	{ static_cast<Uint8>(128),	static_cast<Uint8>(128),	static_cast<Uint8>(0),	static_cast<Uint8>(255) };
constexpr SDL_Color C_YELLOW	{ static_cast<Uint8>(255),	static_cast<Uint8>(255),	static_cast<Uint8>(0),   static_cast<Uint8>(255)  };
constexpr SDL_Color C_LYELLOW	{ static_cast<Uint8>(255),	static_cast<Uint8>(255),	static_cast<Uint8>(128), static_cast<Uint8>(255)  };

constexpr SDL_Color C_ORANGE	{ static_cast<Uint8>(255),	static_cast<Uint8>(165),	static_cast<Uint8>(0),	static_cast<Uint8>(255) };
constexpr SDL_Color C_GOLD		{ static_cast<Uint8>(255),	static_cast<Uint8>(215),	static_cast<Uint8>(0),	static_cast<Uint8>(255) };

constexpr SDL_Color C_DBLUE		{ static_cast<Uint8>(0),		static_cast<Uint8>(0),	static_cast<Uint8>(128), static_cast<Uint8>(255)  };
constexpr SDL_Color C_BLUE		{ static_cast<Uint8>(0),		static_cast<Uint8>(0),	static_cast<Uint8>(255), static_cast<Uint8>(255)  };
constexpr SDL_Color C_LBLUE		{ static_cast<Uint8>(0),		static_cast<Uint8>(64),	static_cast<Uint8>(255), static_cast<Uint8>(255)  };

constexpr SDL_Color C_WHITE		{ static_cast<Uint8>(255),	static_cast<Uint8>(255), static_cast<Uint8>(255), static_cast<Uint8>(255)   };
constexpr SDL_Color C_LGRAY		{ static_cast<Uint8>(192),	static_cast<Uint8>(192), static_cast<Uint8>(192), static_cast<Uint8>(255)   };
constexpr SDL_Color C_GRAY		{ static_cast<Uint8>(128),	static_cast<Uint8>(128), static_cast<Uint8>(128), static_cast<Uint8>(255)   };
constexpr SDL_Color C_DGRAY		{ static_cast<Uint8>(64),	static_cast<Uint8>(64),  static_cast<Uint8>(64),  static_cast<Uint8>(255)   };
constexpr SDL_Color C_BLACK		{ static_cast<Uint8>(0),		static_cast<Uint8>(0),   static_cast<Uint8>(0),	static_cast<Uint8>(255)   };