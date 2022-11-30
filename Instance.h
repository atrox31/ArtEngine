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
#include "Render.h"
#include "Event.h"

class Instance {
public:
	Instance(int InstanceDefinitionId);
	// do not use, this give unique id for instance
	Instance* GiveId();
	virtual ~Instance();

	Uint64 GetId() { return _id; }
	void Delete() { Alive = false; }
	int GetInstanceDefinitionId() {	return _instanceDefinitionId; }

	void DrawSelf();
	bool CheckMaskClick(SDL_FPoint&);

	std::string Tag;
	std::string Name;

	bool InView;
	bool Alive;
	bool IsCollider;

	float PosX;
	float PosY;
	float Direction;

	Sprite* SelfSprite;
	float SpriteScaleX;
	float SpriteScaleY;
	int SpriteCenterX;
	int SpriteCenterY;
	float SpriteAngle;

	float SpriteAnimationFrame;
	float SpriteAnimationSpeed;
	bool SpriteAnimationLoop;

	std::vector<std::vector<std::string>> Varibles;
	EventBit EventFlag;

	
	bool CollideTest(Instance*);
private:
		bool Collision_circle_circle(float c1x, float c1y, float c1r,float c2x, float c2y, float c2r);
		bool Collision_circle_rect(float c1x, float c1y, float c1r, GPU_Rect rect);
		bool Collision_rect_rect(GPU_Rect rect1, GPU_Rect rect2);
public:
	struct BodyType {
	public:
		ENUM_WITH_STRING_CONVERSION(Body,(NONE)(SPRITE)(RECT)(CIRCLE));
		Body Type;
		int Value;
		BodyType() {
			Type = Body::NONE;
			Value = 0;
		}

	private:
	};
	BodyType Body;
#ifdef _DEBUG
	void DebugDrawMask();
	void DebugDrawCollision();
#endif // _DEBUG

private:
	Uint64 _id = 0;
	static Uint64 _cid;
	int _instanceDefinitionId;
};
