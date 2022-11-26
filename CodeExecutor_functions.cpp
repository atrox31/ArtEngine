#include "CodeExecutor.h"
#include "AssetManager.h"
#include "Core.h"
#include "Debug.h"
#include "Render.h"
#include "Func.h"


#define StackIn CodeExecutor::GlobalStack.Get()
#define StackIn_b Func::Str2Bool(StackIn)
#define StackIn_p Func::Str2Point(StackIn)
#define StackOut CodeExecutor::GlobalStack.Add

#ifdef _DEBUG
int TryToGetInt(const std::string& str) {
	int a = -1;
	try {
		a = std::stoi(str);
	}
	catch (const std::invalid_argument& ia) {
		ASSERT(false);
		std::cerr << "Invalid argument: " << ia.what() << std::endl;
		return -1;
	}
	catch (const std::out_of_range& oor) {
		std::cerr << "Out of Range error: " << oor.what() << std::endl;
		return -2;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Undefined error: " << e.what() << std::endl;
		return -3;
	}
	return a;
}
float TryToGetFloat(const std::string& str) {
	float a = -1.0f;
	try {
		a = std::stof(str);
	}
	catch (const std::invalid_argument& ia) {
		ASSERT(false);
		std::cerr << "Invalid argument: " << ia.what() << std::endl;
		return -1;
	}
	catch (const std::out_of_range& oor) {
		std::cerr << "Out of Range error: " << oor.what() << std::endl;
		return -2;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Undefined error: " << e.what() << std::endl;
		return -3;
	}
	return a;
}

#define StackIn_f TryToGetFloat(StackIn)
#define StackIn_i TryToGetInt(StackIn)
#else
#define StackIn_f std::stof(StackIn)
#define StackIn_i std::stoi(StackIn)
#endif


// makers
	//point new_point(float x, float y);Make point (<float>, <float>).;New point from value or other.
void CodeExecutor::new_point(Instance*) {
	float p1 = StackIn_f;
	float p2 = StackIn_f;
	StackOut(std::to_string(p2) + "," + std::to_string(p1));
}

//float new_direction(point from, point to);Make direction from <point> to <point>.;Value are from 0 to 359.
void CodeExecutor::new_direction(Instance*) {

}

//rectangle new_rectangle(int x1, int y1, int x2, int y2);Make rectangle from <int>, <int> to <int>, <int>.;This is const rectangle, not width and height.
void CodeExecutor::new_rectangle(Instance*) {

}

//rectangle new_rectangle_f(float x1, float y1, float x2, float y2);Make rectangle from <float>, <float> to <float>, <float>.;This is const rectangle, not width and height.
void CodeExecutor::new_rectangle_f(Instance*) {

}

//rectangle new_mask_from_sprite(sprite spr);Make rectangle mask from <sprite>;Cirkle mask is not supported yet.
void CodeExecutor::new_mask_from_sprite(Instance*) {

}

// assets
	//sprite get_sprite(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
void CodeExecutor::get_sprite(Instance*) {
	std::string name = StackIn;
	int sprite = Core::GetInstance()->assetManager->GetSpriteId(name);
	if (sprite == -1) {
		Debug::WARNING("CodeExecutor::get_sprite() - '" + name + "' not found");
	}
	CodeExecutor::GlobalStack.Add(std::to_string(sprite));
}

//texture get_texture(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
void CodeExecutor::get_texture(Instance*) {
	std::string name = StackIn;
	int texture = Core::GetInstance()->assetManager->GetTextureId(name);
	if (texture == -1) {
		Debug::WARNING("CodeExecutor::get_texture() - '" + name + "' not found");
	}
	CodeExecutor::GlobalStack.Add(std::to_string(texture));
}

//music get_music(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
void CodeExecutor::get_music(Instance*) {

	std::string name = StackIn;
	int texture = Core::GetInstance()->assetManager->GetMusicId(name);
	if (texture == -1) {
		Debug::WARNING("CodeExecutor::get_music() - '" + name + "' not found");
	}
	CodeExecutor::GlobalStack.Add(std::to_string(texture));
}

//sound get_sound(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
void CodeExecutor::get_sound(Instance*) {
	std::string name = StackIn;
	int texture = Core::GetInstance()->assetManager->GetSoundId(name);
	if (texture == -1) {
		Debug::WARNING("CodeExecutor::get_sound() - '" + name + "' not found");
	}
	CodeExecutor::GlobalStack.Add(std::to_string(texture));
}

//font get_font(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
void CodeExecutor::get_font(Instance*) {

}

// sprite
	//int sprite_get_width(sprite spr);Get width of <sprite>;Get int value.
void CodeExecutor::sprite_get_width(Instance*) {

}

//int sprite_get_height(sprite spr);Get height of <sprite>;Get int value.
void CodeExecutor::sprite_get_height(Instance*) {

}

//int sprite_get_frames(sprite spr);Get frames number of <sprite>;Get int value.
void CodeExecutor::sprite_get_frames(Instance*) {

}

//null sprite_set_animation_speed(float speed);Set animation speed for <sprite> value <float> frames per seccond;Every sprite can have own animation speed
void CodeExecutor::sprite_set_animation_speed(Instance* instance) {
	instance->SpriteAnimationSpeed = StackIn_f;
}

//null sprite_set_animation_loop(bool loop);Set animation loop for <sprite> value <bool>;Every animation end generate event ON_ANIMATION_END
void CodeExecutor::sprite_set_animation_loop(Instance* instance) {
	instance->SpriteAnimationLoop = StackIn_b;
}

// moving
	//null move_to_point(point p, float speed);Move current instance to <point> with <speed> px per seccond.;Call it every frame.
void CodeExecutor::move_to_point(Instance* instance) {
	float speed = StackIn_f;
	SDL_FPoint dest = StackIn_p;
	float direction = std::atan2f(dest.y - instance->PosY, dest.x - instance->PosX);
	instance->PosX += std::cosf(direction) * speed * (float)Core::GetInstance()->DeltaTime;
	instance->PosY += std::sinf(direction) * speed * (float)Core::GetInstance()->DeltaTime;
}

//null move_forward(float speed);Move current instance forward with <speed> px per seccond.;Call it every frame. Function give build-in direction varible.
void CodeExecutor::move_forward(Instance*) {

}

//null move_instant(point p);Move instantly to target <point>;This changes x and y. Not cheking for collision;
void CodeExecutor::move_instant(Instance*) {

}

//null move_to_direction(float direction, float speed);Move instance toward direction of <float> (0-359) with <float> speed px per seccond;If direction is not in range its clipped to 360.
void CodeExecutor::move_to_direction(Instance*) {

}

// mesurment
	//float distance_to_point(point p);Give distance to <point>;Measure from current instance to target point.
void CodeExecutor::distance_to_point(Instance* instance) {
	SDL_FPoint dest = StackIn_p;
	SDL_FPoint src = {instance->PosX, instance->PosY};
	float distance = Func::Distance(src, dest);
	StackOut(std::to_string(distance));
}

//float distance_beetwen_point(point p1, point p2);Give distance from <point> to <point>;Measure distance.
void CodeExecutor::distance_beetwen_point(Instance*) {
	SDL_FPoint dest = StackIn_p;
	SDL_FPoint src = StackIn_p;
	float distance = Func::Distance(src, dest);
	StackOut(std::to_string(distance));
}

//float distance_to_instance(instance i);Give distance to <point>;Measure from current instance to target point.
void CodeExecutor::distance_to_instance(Instance*) {

}
	//float direction_to_point(point p);Give direction to <point>;Measure from current instance to target point.
void CodeExecutor::direction_to_point(Instance* instance) {
	SDL_FPoint dest = StackIn_p;
	float direction = std::atan2f(instance->PosY - dest.y, instance->PosX - dest.x);
	StackOut(std::to_string(direction));
}

//float direction_beetwen_point(point p1, point p2);Give direction from <point> to <point>;Measure distance.
void CodeExecutor::direction_beetwen_point(Instance*) {
	SDL_FPoint dest = StackIn_p;
	SDL_FPoint src = StackIn_p;
	float direction = std::atan2f(src.y - dest.y, src.x - dest.x);
	StackOut(std::to_string(direction));
}
//float direction_to_instance(instance i);Give direction to <instance>;Measure from current instance to target point.
void CodeExecutor::direction_to_instance(Instance*) {

}


// drawing
	//null draw_sprite(sprite spr, float x, float y, float frame);Draw <sprite> on location (<float>,<float>) with target frame <frame>;Draw default sprite. To more options use draw_sprite_ex
void CodeExecutor::draw_sprite(Instance*) {

}

//null draw_sprite_ex(sprite spr, float x, float y, float frame, float x_scale, float y_scale, float x_center, float y_center, float angle, float alpha);
void CodeExecutor::draw_sprite_ex(Instance*) {

}

//null draw_texture(texture tex, float x, float y);Draw <texture> on (<float>,<float>).;Draw standard texture with it normal dimensions. For extended option use 'Draw texture extended';
void CodeExecutor::draw_texture(Instance*) {
	float y = StackIn_f;
	float x = StackIn_f;
	int textureId = StackIn_i;
	Render::DrawTexture(Core::GetInstance()->assetManager->GetTexture(textureId), { x,y }, { 1.0f, 1.0f }, 0.0f);
}

//null draw_texture_ex(texture tex, float x, float y, float x_scale, float y_scale, float angle, float alpha);Draw <texture> on (<float>,<float>), with scale (<float>,<float>), angle <float> and aplha <float>;Angle range is (0 - 359) alpha (0.0f - 1.0f).
void CodeExecutor::draw_texture_ex(Instance*) {

}

//null draw_sprite_self();Draw self sprite on self coords with sprite scale and angle;Use build-in varibles;
void CodeExecutor::draw_sprite_self(Instance* instance) {
	if (instance->SelfSprite == nullptr) return;
	instance->DrawSelf();
}

// draw shapes
	//null draw_shape_rectangle(float x1, float y2, float x2, float y2, color color);Draw frame of rectangle from (<float>,<float>) to (<float>,<float>) with color <color>.;Draw rectangle on final coords;
void CodeExecutor::draw_shape_rectangle(Instance*) {

}

//null draw_shape_rectangle_r(rectangle rect, color color);Draw frame of <rectangle> with color <color>.;Draw rectangle;
void CodeExecutor::draw_shape_rectangle_r(Instance*) {

}

//null draw_shape_rectangle_filled(float x1, float y2, float x2, float y2, color color);Draw filled of rectangle from (<float>,<float>) to (<float>,<float>) with color <color>.;Draw rectangle on final coords;
void CodeExecutor::draw_shape_rectangle_filled(Instance*) {

}

//null draw_shape_rectangle_filled_r(rectangle rect, color color);Draw filled <rectangle> with color <color>.;Draw rectangle;
void CodeExecutor::draw_shape_rectangle_filled_r(Instance*) {

}

//null draw_shape_circle(float x, float y, float radius, color color);Draw circle in point (<float>,<float>) with radius <float> and color <color>;
void CodeExecutor::draw_shape_circle(Instance*) {

}

//null draw_shape_circle_p(point p, float radius, color color);Draw circle in point <point> with radius <float> and color <color>;
void CodeExecutor::draw_shape_circle_p(Instance*) {

}

//null draw_shape_circle_filled(float x, float y, float radius, color color);Draw filled circle in point (<float>,<float>) with radius <float> and color <color>;
void CodeExecutor::draw_shape_circle_filled(Instance*) {

}

//null draw_shape_circle_filled_p(point p, float radius, color color);Draw filled circle in point <point> with radius <float> and color <color>;
void CodeExecutor::draw_shape_circle_filled_p(Instance*) {

}

// math
	//int math_min_i(int a, int b);Get minimum value from <int> or <int>;
void CodeExecutor::math_min_i(Instance*) {

}

//int math_max_i(int a, int b);Get maximum value from <int> or <int>;
void CodeExecutor::math_max_i(Instance*) {

}

//float math_min(float a, float b);Get minimum value from <float> or <float>;
void CodeExecutor::math_min(Instance*) {

}

//float math_max(float a, float b);Get maximum value from <float> or <float>;
void CodeExecutor::math_max(Instance*) {

}

// globals
	//point global_get_mouse();Get point of current mouse postion;If map is bigger than screen this give map coords not screen;
void CodeExecutor::global_get_mouse(Instance*) {
	StackOut(std::to_string( Core::GetInstance()->gMouse.X ) + "," + std::to_string(Core::GetInstance()->gMouse.Y));
}
//null set_self_sprite(sprite spr); Set self sprite to <sprite> with default scale, angle, speed, loop; You can mod sprite via set_sprite_ etc.;
void CodeExecutor::set_self_sprite(Instance* instance) {
	int spriteId = StackIn_i;
	if (spriteId != -1) {
		Sprite* sprite = Core::GetInstance()->assetManager->GetSprite(spriteId);
		if (sprite != nullptr) {
			instance->SelfSprite = sprite;
			instance->E_MaskClicked = false;
			//instance->Mask = sprite->

			instance->SpriteScaleX = 1.0f;
			instance->SpriteScaleY = 1.0f;
			instance->SpriteCenterX = sprite->GetWidth() * 0.5f;
			instance->SpriteCenterY = sprite->GetHeight() * 0.5f;
			instance->SpriteAngle = 0.0f;
			instance->SpriteAnimationFrame = 0.0f;
			instance->SpriteAnimationSpeed = 60.0f;
			instance->SpriteAnimationLoop = true;
			return;
		}
	}
	instance->E_MaskClicked = false;
	//instance->Mask = Rect();

	instance->SelfSprite = nullptr;
	instance->SpriteScaleX = 0.0f;
	instance->SpriteScaleY = 0.0f; 
	instance->SpriteCenterX = 0.0f;
	instance->SpriteCenterY = 0.0f;
	instance->SpriteAngle = 0.0f;
	instance->SpriteAnimationFrame = 0.0f;
	instance->SpriteAnimationSpeed = 0.0f;
	instance->SpriteAnimationLoop = false;
}
//float get_pos_x(); Get x coords of instance;
void CodeExecutor::get_pos_x(Instance* instance) {
	StackOut(std::to_string(instance->PosX));
}
//float get_pos_y(); Get y coords of instance;
void CodeExecutor::get_pos_y(Instance* instance) {
	StackOut(std::to_string(instance->PosY));
}