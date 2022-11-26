#pragma once

#include "SDL_version.h"
#include SDL2_INCLUDE_DIR
//#include SDL2_IMAGE_INCLUDE_DIR
#include SDL2_MIXER_INCLUDE_DIR
#include SDL2_NET_INCLUDE_DIR
#include SDL2_TTF_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR
#include "Rect.h"
#include "Sprite.h"
#include <string>
#include "ArtCode.h"

class Instance {
public:
	Instance(int InstanceDefinitionId);
	virtual ~Instance();

	Uint64 GetId() { return _id; }
	void Delete() { Alive = false; }
	int GetInstanceDefinitionId() {	return _instanceDefinitionId; }

	std::string Tag;
	std::string Name;

	bool InView;
	bool Alive;

	bool E_MaskClicked;
	Rect Mask;

	float PosX;
	float PosY;
	float Direction;

	Sprite* SelfSprite;
	float SpriteScaleX;
	float SpriteScaleY;
	float SpriteCurrentFrame;
	float SpriteAngle;

	std::vector<std::vector<std::string>> Varibles;
private:
	Uint64 _id;
	static Uint64 _cid;
	int _instanceDefinitionId;
};
