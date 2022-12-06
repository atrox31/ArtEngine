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
	explicit Instance(int instance_definition_id);
	// do not use, this give unique id for instance
	Instance* GiveId();
	virtual ~Instance();

	Uint64 GetId() const { return _id; }
	void Delete() { Alive = false; }
	int GetInstanceDefinitionId() const {	return _instanceDefinitionId; }

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

	std::vector<int> Varibles_int;
	std::vector<float> Varibles_float;
	std::vector<bool> Varibles_bool;
	std::vector<Instance*> Varibles_instance;
	std::vector<int> Varibles_object;
	std::vector<int> Varibles_sprite;
	std::vector<int> Varibles_texture;
	std::vector<int> Varibles_sound;
	std::vector<int> Varibles_music;
	std::vector<int> Varibles_font;
	std::vector<Rect> Varibles_rect;
	std::vector<SDL_FPoint> Varibles_point;
	std::vector<SDL_Color> Varibles_color;
	std::vector<std::string> Varibles_string;
	EventBit EventFlag;

	
	bool CollideTest(Instance*);
private:
		bool Collision_circle_circle(float c1x, float c1y, float c1r,float c2x, float c2y, float c2r);
		bool Collision_circle_rect(float c1x, float c1y, float c1r, GPU_Rect rect);
		bool Collision_rect_rect(GPU_Rect rect1, GPU_Rect rect2);
public:
	struct BodyType {
	public:
		ENUM_WITH_STRING_CONVERSION(Body,(None)(Sprite)(Rect)(Circle));
		Body Type;
		int Value;
		BodyType() {
			Type = Body::None;
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
