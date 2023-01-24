#pragma once
// application version
static constexpr int VERSION_MAIN = 0;
static constexpr int VERSION_MINOR = 7;
static constexpr int VERSION_PATH = 0;
// required Compiler code version
static constexpr int MINIMUM_ART_COMPILER_MAIN = 1;
static constexpr int MINIMUM_ART_COMPILER_MINOR = 8;

// all debug definitions

// allow debugging in release mode to enable view debug data
	#define AC_ENABLE_DEBUG_MODE 1