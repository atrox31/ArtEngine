#pragma once
#include "SDL_version.h"
#include SDL2_INCLUDE_DIR

const SDL_Color C_NONE 		{ (Uint8)0,		(Uint8)0,	(Uint8)0,	(Uint8)0	};
const SDL_Color C_FULL 		{ (Uint8)255,	(Uint8)255,	(Uint8)255,	(Uint8)255	};
const SDL_Color C_DEFAULT	{ (Uint8)0,		(Uint8)0,	(Uint8)0,	(Uint8)255	};

const SDL_Color C_RED		{ (Uint8)255,	(Uint8)0,	(Uint8)0,	(Uint8)255 };

const SDL_Color C_DGREEN	{ (Uint8)0,		(Uint8)128,	(Uint8)0,	(Uint8)255 };
const SDL_Color C_GREEN		{ (Uint8)0,		(Uint8)255,	(Uint8)0,	(Uint8)255 };
const SDL_Color C_LGREEN	{ (Uint8)64,	(Uint8)255,	(Uint8)64,	(Uint8)255 };

const SDL_Color C_DYELLOW	{ (Uint8)128,	(Uint8)128,	(Uint8)0,	(Uint8)255 };
const SDL_Color C_YELLOW	{ (Uint8)255,	(Uint8)255,	(Uint8)0,   (Uint8)255 };
const SDL_Color C_LYELLOW	{ (Uint8)255,	(Uint8)255,	(Uint8)128, (Uint8)255 };

const SDL_Color C_ORANGE	{ (Uint8)255,	(Uint8)165,	(Uint8)0,	(Uint8)255 };
const SDL_Color C_GOLD		{ (Uint8)255,	(Uint8)215,	(Uint8)0,	(Uint8)255 };

const SDL_Color C_DBLUE		{ (Uint8)0,		(Uint8)0,	(Uint8)128, (Uint8)255 };
const SDL_Color C_BLUE		{ (Uint8)0,		(Uint8)0,	(Uint8)255, (Uint8)255 };
const SDL_Color C_LBLUE		{ (Uint8)0,		(Uint8)64,	(Uint8)255, (Uint8)255 };

const SDL_Color C_WHITE		{ (Uint8)255,	(Uint8)255, (Uint8)255, (Uint8)255 };
const SDL_Color C_LGRAY		{ (Uint8)192,	(Uint8)192, (Uint8)192, (Uint8)255 };
const SDL_Color C_GRAY		{ (Uint8)128,	(Uint8)128, (Uint8)128, (Uint8)255 };
const SDL_Color C_DGRAY		{ (Uint8)64,	(Uint8)64,	(Uint8)64,	(Uint8)255 };
const SDL_Color C_BLACK		{ (Uint8)0,		(Uint8)0,	(Uint8)0,	(Uint8)255 };