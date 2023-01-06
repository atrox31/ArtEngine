#pragma once
#include "SDL2/IncludeAll.h"
// custom assert
#ifdef _DEBUG
	#ifdef DEBUG_EDITOR
		#define __FILENAME__ (std::strrchr(__FILE__, '\\') ? std::strrchr(__FILE__, '\\') + 1 : __FILE__)
		#define ASSERT(condition, message) if(!(condition)) std::cout << __FILENAME__ << "[0x" << __LINE__ << "]" << message << std::endl;
	#else
		#define ASSERT(CONDITION, message) SDL_assert(CONDITION);
	#endif
#else
	#define ASSERT(CONDITION, message)
#endif
