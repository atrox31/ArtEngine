#pragma once

#pragma warning(push)
#include "SDL_version.h"
#include SDL2_INCLUDE_DIR
#include SDL2_IMAGE_INCLUDE_DIR
#include SDL2_MIXER_INCLUDE_DIR
#include SDL2_NET_INCLUDE_DIR
#include SDL2_TTF_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR
#pragma warning(pop)
#include "Rect.h"
#include <string>

class Instance {
public:
	Instance();
	virtual ~Instance();
	Uint64 GetId() { return _id; };
	void Delete() { Alive = false; };

	bool Alive;
	std::string Tag;
	bool InView;
	bool E_MaskClicked = false;
	Rect Mask;

private:
	Uint64 _id;
	static Uint64 _cid;
};
