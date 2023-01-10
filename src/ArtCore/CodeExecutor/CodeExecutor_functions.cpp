// ReSharper disable CppInconsistentNaming
// names are for ALib to use in ArtScript in editor
#include "CodeExecutor.h"
#include "ArtCore/Functions/Convert.h"
#include "ArtCore/Graphic/Render.h"
#include "ArtCore/Gui/GuiElement/CheckButton.h"
#include "ArtCore/Gui/GuiElement/DropDownList.h"
#include "ArtCore/Gui/GuiElement/Slider.h"
#include "ArtCore/Physics/Physics.h"
#include "ArtCore/Scene/Instance.h"
#include "ArtCore/System/Core.h"
#include "ArtCore/System/AssetManager.h"
#include "ArtCore/Scene/Scene.h"
#include "ArtCore/_Debug/Debug.h"

#define StackIn_b CodeExecutor::GlobalStack_bool.Get()
#define StackIn_p CodeExecutor::GlobalStack_point.Get()
#define StackIn_c CodeExecutor::GlobalStack_color.Get()
#define StackIn_r CodeExecutor::GlobalStack_rect.Get()
#define StackIn_f CodeExecutor::GlobalStack_float.Get()
#define StackIn_i CodeExecutor::GlobalStack_int.Get()
#define StackIn_s CodeExecutor::GlobalStack_string.Get()
#define StackIn_ins CodeExecutor::GlobalStack_instance.Get()

#define StackOut_b(X) CodeExecutor::GlobalStack_bool.Add(X)
#define StackOut_p(X) CodeExecutor::GlobalStack_point.Add(X)
#define StackOut_c(X) CodeExecutor::GlobalStack_color.Add(X)
#define StackOut_r(X) CodeExecutor::GlobalStack_rect.Add(X)
#define StackOut_f(X) CodeExecutor::GlobalStack_float.Add(X)
#define StackOut_i(X) CodeExecutor::GlobalStack_int.Add(X)
#define StackOut_ins(X) CodeExecutor::GlobalStack_instance.Add(X)
#define StackOut_s(X) CodeExecutor::GlobalStack_string.Add(X)



// all functions is executed as ArtCode 
// down from this line code is read by small program that generate ArtLib from comments of functions.
// syntax is:
// returnValue functionName(args);main text with <arguments> to replace as links;And another helper text
//#AUTO_GENERATOR_START

//point new_point(float x, float y);Make point (<float>, <float>).;New point from value or other.
void CodeExecutor::new_point(Instance*) {
	const float p2 = StackIn_f;
	const float p1 = StackIn_f;
	StackOut_p(SDL_FPoint({ p1, p2 }));
}

//float new_direction(point from, point to);Make direction from <point> to <point>.;Value are from 0 to 359.
void CodeExecutor::new_direction(Instance*) {
	//redirect
	direction_between_point(nullptr);
}

//Rectangle new_rectangle(int x1, int y1, int x2, int y2);Make Rectangle from <int>, <int> to <int>, <int>.;This is const Rectangle, not width and height.
void CodeExecutor::new_rectangle(Instance*) {
	const int y2 = StackIn_i;
	const int x2 = StackIn_i;
	const int y1 = StackIn_i;
	const int x1 = StackIn_i;
	const Rect output{ x1,y1,x2,y2 };
	StackOut_r(output);
}

//Rectangle new_rectangle_f(float x1, float y1, float x2, float y2);Make Rectangle from <float>, <float> to <float>, <float>.;This is const Rectangle, not width and height.
void CodeExecutor::new_rectangle_f(Instance*) {
	const float y2 = StackIn_f;
	const float x2 = StackIn_f;
	const float y1 = StackIn_f;
	const float x1 = StackIn_f;
	const Rect output{ x1,y1,x2,y2 };
	StackOut_r(output);
}

//sprite get_sprite(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
void CodeExecutor::get_sprite(Instance*) {
	const std::string name = StackIn_s;
	const int sprite = Core::GetAssetManager()->GetSpriteId(name);
	if (sprite == -1) {
		Console::WriteLine("CodeExecutor::get_sprite() - '" + name + "' not found");
	}
	StackOut_i(sprite);
}

//texture get_texture(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
void CodeExecutor::get_texture(Instance*) {
	const std::string name = StackIn_s;
	const int texture = Core::GetAssetManager()->GetTextureId(name);
	if (texture == -1) {
		Console::WriteLine("CodeExecutor::get_texture() - '" + name + "' not found");
	}
	StackOut_i(texture);
}

//music get_music(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
void CodeExecutor::get_music(Instance*) {
	const std::string name = StackIn_s;
	const int texture = Core::GetAssetManager()->GetMusicId(name);
	if (texture == -1) {
		Console::WriteLine("CodeExecutor::get_music() - '" + name + "' not found");
	}
	StackOut_i(texture);
}

//sound get_sound(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
void CodeExecutor::get_sound(Instance*) {
	const std::string name = StackIn_s;
	const int texture = Core::GetAssetManager()->GetSoundId(name);
	if (texture == -1) {
		Console::WriteLine("CodeExecutor::get_sound() - '" + name + "' not found");
	}
	StackOut_i(texture);
}

//font get_font(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
void CodeExecutor::get_font(Instance*) {
	const std::string name = StackIn_s;
	const int font = Core::GetAssetManager()->GetFontId(name);
	if (font == -1) {
		Console::WriteLine("CodeExecutor::get_font() - '" + name + "' not found");
	}
	StackOut_i(font);
}

//int sprite_get_width(sprite spr);Get width of <sprite>;Get int value.
void CodeExecutor::sprite_get_width(Instance*) {
	const int id = StackIn_i;
	const Sprite* sprite = Core::GetAssetManager()->GetSprite(id);
	if (sprite == nullptr) StackOut_i(0);
	else StackOut_i(sprite->GetWidth());
}

//int sprite_get_height(sprite spr);Get height of <sprite>;Get int value.
void CodeExecutor::sprite_get_height(Instance*) {
	const int id = StackIn_i;
	const Sprite* sprite = Core::GetAssetManager()->GetSprite(id);
	if (sprite == nullptr) StackOut_i(0);
	else StackOut_i(sprite->GetHeight());
}

//int sprite_get_frames(sprite spr);Get _frames number of <sprite>;Get int value.
void CodeExecutor::sprite_get_frames(Instance*) {
	const int id = StackIn_i;
	const Sprite* sprite = Core::GetAssetManager()->GetSprite(id);
	if (sprite == nullptr) StackOut_i(0);
	else StackOut_i(sprite->GetMaxFrame());
}

//null sprite_set_animation_speed(float speed);Set animation value <float> _frames per second;Every sprite can have own animation speed
void CodeExecutor::sprite_set_animation_speed(Instance* sender) {
	sender->SpriteAnimationSpeed = StackIn_f;
}

//null sprite_set_animation_loop(bool loop);Set animation loop value <bool>;Every animation end generate event ON_ANIMATION_END
void CodeExecutor::sprite_set_animation_loop(Instance* sender) {
	sender->SpriteAnimationLoop = StackIn_b;
}

//null move_to_point(point p, float speed);Move current instance to <point> with <speed> px per second.;Call it every frame.
void CodeExecutor::move_to_point(Instance* sender) {
	const float speed = StackIn_f;
	const SDL_FPoint dest = StackIn_p;
	const float direction = std::atan2f(dest.y - sender->PosY, dest.x - sender->PosX);
	sender->PosX += std::cosf(direction) * speed * static_cast<float>(Core::DeltaTime);
	sender->PosY += std::sinf(direction) * speed * static_cast<float>(Core::DeltaTime);
}

//null move_instant(point p);Move instantly to target <point>;This changes x and y. Not cheking for collision;
void CodeExecutor::move_instant(Instance* sender) {
	const SDL_FPoint dest = StackIn_p;
	sender->PosX = dest.x;
	sender->PosY = dest.y;
}

//null move_to_direction(float direction, float speed);Move instance toward direction of <float> (0-359) with <float> speed px per seccond;If direction is not in range its clipped to 360.
void CodeExecutor::move_to_direction(Instance* sender) {
	const float speed = StackIn_f;
	const float direction = StackIn_f;
	sender->PosX += std::cosf(direction) * speed * static_cast<float>(Core::DeltaTime);
	sender->PosY += std::sinf(direction) * speed * static_cast<float>(Core::DeltaTime);
}

//float distance_to_point(point p);Give distance to <point>;Measure from current instance to target point.
void CodeExecutor::distance_to_point(Instance* sender) {
	SDL_FPoint dest = StackIn_p;
	SDL_FPoint src = { sender->PosX, sender->PosY };
	const float distance = Func::Distance(src, dest);
	StackOut_f(distance);
}

//float distance_between_point(point p1, point p2);Give distance from <point> to <point>;Measure distance.
void CodeExecutor::distance_between_point(Instance*) {
	SDL_FPoint dest = StackIn_p;
	SDL_FPoint src = StackIn_p;
	const float distance = Func::Distance(src, dest);
	StackOut_f(distance);
}

//float distance_to_instance(instance i);Give distance to <instance> instance;Measure from current instance to target point. If target not exists return 0
void CodeExecutor::distance_to_instance(Instance* sender) {
	const Instance* target = StackIn_ins;
	if (target == nullptr) {
		StackOut_f(0.0f);
	}
	else {
		StackOut_f(Func::Distance(sender->PosX, sender->PosY, target->PosX, target->PosY));
	}
}

//null move_forward(float speed);Move current instance forward with <speed> px per second.;Call it every frame. Function give build-in direction variable.
void CodeExecutor::move_forward(Instance* sender) {
	const float speed = StackIn_f;
	sender->PosX += std::cosf(sender->Direction) * speed * static_cast<float>(Core::DeltaTime);
	sender->PosY += std::sinf(sender->Direction) * speed * static_cast<float>(Core::DeltaTime);
}

//float direction_to_point(point p);Give direction to <point> in degree (-180 : 180);Measure from current instance to target point.
void CodeExecutor::direction_to_point(Instance* instance) {
	const SDL_FPoint dest = StackIn_p;
	const SDL_FPoint src = { instance->PosX , instance->PosY};

	const float direction = std::atan2f(dest.y - src.y, dest.x - src.x);
	StackOut_f(Convert::RadiansToDegree(direction));
}

//float direction_between_point(point p1, point p2);Give direction from <point> to <point> in degree (-180 : 180);Measure distance.
void CodeExecutor::direction_between_point(Instance*) {
	const SDL_FPoint dest = StackIn_p;
	const SDL_FPoint src = StackIn_p;

	const float direction = std::atan2f(dest.y- src.y,  dest.x- src.x);
	StackOut_f(Convert::RadiansToDegree(direction));
}
//float direction_to_instance(instance i);Give direction to <instance> in degree (-180 : 180);Measure from current instance to target point. If target not exists return own direction
void CodeExecutor::direction_to_instance(Instance* self) {
	const Instance* instance = StackIn_ins;
	if(instance == nullptr)
	{
		StackOut_f(Convert::RadiansToDegree(self->Direction));
		return;
	}
	const float direction = std::atan2f(self->PosY - instance->PosY, self->PosX - instance->PosX);
	StackOut_f(Convert::RadiansToDegree(direction));
}

//null draw_sprite(sprite spr, float x, float y, float frame);Draw <sprite> on location (<float>,<float>) with target frame <frame>;Draw default sprite. To more options use draw_sprite_ex
void CodeExecutor::draw_sprite(Instance*) {
	const float frame = StackIn_f;
	float y = StackIn_f;
	float x = StackIn_f;
	const int spriteId = StackIn_i;
	Render::DrawSprite(Core::GetAssetManager()->GetSprite(spriteId), { x, y }, static_cast<int>(frame));
}

//null draw_sprite_ex(sprite spr, float x, float y, float frame, float x_scale, float y_scale, float x_center, float y_center, float angle, float alpha);
void CodeExecutor::draw_sprite_ex(Instance*) {
	const float alpha = StackIn_f;
	const float angle = StackIn_f;
	const float y_center = StackIn_f;
	const float x_center = StackIn_f;
	const float y_scale = StackIn_f;
	const float x_scale = StackIn_f;
	const float frame = StackIn_f;
	const float y = StackIn_f;
	const float x = StackIn_f;
	const int spriteId = StackIn_i;
	Render::DrawSprite_ex(Core::GetAssetManager()->GetSprite(spriteId), x, y, static_cast<int>(frame), x_scale, y_scale, x_center, y_center, angle, alpha);
}

//null draw_texture(texture tex, float x, float y);Draw <texture> on (<float>,<float>).;Draw standard texture with it normal dimensions. For extended option use 'Draw texture extended';
void CodeExecutor::draw_texture(Instance*) {
	float y = StackIn_f;
	float x = StackIn_f;
	const int textureId = StackIn_i;
	Render::DrawTexture(Core::GetAssetManager()->GetTexture(textureId), { x,y }, { 1.0f, 1.0f }, 0.0f, 1.0f);
}

//null draw_texture_ex(texture tex, float x, float y, float x_scale, float y_scale, float angle, float alpha);Draw <texture> on (<float>,<float>), with scale (<float>,<float>), angle <float> and aplha <float>;Angle range is (0 - 359) alpha (0.0f - 1.0f).
void CodeExecutor::draw_texture_ex(Instance*) {
	const float alpha = StackIn_f;
	const float angle = StackIn_f;
	float y_scale = StackIn_f;
	float x_scale = StackIn_f;
	float y = StackIn_f;
	float x = StackIn_f;
	const int textureId = StackIn_i;
	Render::DrawTexture(Core::GetAssetManager()->GetTexture(textureId), { x,y }, { x_scale, y_scale }, angle, alpha);
}

//null draw_sprite_self();Draw self sprite on self coords with sprite scale and angle;Use build-in variables;
void CodeExecutor::draw_sprite_self(Instance* instance) {
	if (instance->SelfSprite == nullptr) return;
	instance->DrawSelf();
}

//null draw_shape_rectangle(float x1, float y2, float x2, float y2, color color);Draw frame of Rectangle from (<float>,<float>) to (<float>,<float>) with color <color>.;Draw Rectangle on final coords;
void CodeExecutor::draw_shape_rectangle(Instance*) {
	const SDL_Color color = StackIn_c;
	const float y2 = StackIn_f;
	const float x2 = StackIn_f;
	const float y1 = StackIn_f;
	const float x1 = StackIn_f;
	Render::DrawRect({ x1,y1,x2,y2 }, color);
}

//null draw_shape_rectangle_r(Rectangle rect, color color);Draw frame of <Rectangle> with color <color>.;Draw Rectangle;
void CodeExecutor::draw_shape_rectangle_r(Instance*) {
	Rect rect = StackIn_r;
	const SDL_Color color = StackIn_c;
	Render::DrawRect(rect.ToGPU_Rect(), color);
}

//null draw_shape_rectangle_filled(float x1, float y2, float x2, float y2, color color);Draw filled of Rectangle from (<float>,<float>) to (<float>,<float>) with color <color>.;Draw Rectangle on final coords;
void CodeExecutor::draw_shape_rectangle_filled(Instance*) {
	const SDL_Color color = StackIn_c;
	const float y2 = StackIn_f;
	const float x2 = StackIn_f;
	const float y1 = StackIn_f;
	const float x1 = StackIn_f;
	Render::DrawRectFilled({ x1,y1,x2,y2 }, color);
}

//null draw_shape_rectangle_filled_r(Rectangle rect, color color);Draw filled <Rectangle> with color <color>.;Draw Rectangle;
void CodeExecutor::draw_shape_rectangle_filled_r(Instance*) {
	Rect rect = StackIn_r;
	const SDL_Color color = StackIn_c;
	Render::DrawRectFilled(rect.ToGPU_Rect(), color);
}

//null draw_shape_circle(float x, float y, float radius, color color);Draw Circle in point (<float>,<float>) with radius <float> and color <color>;
void CodeExecutor::draw_shape_circle(Instance*) {
	const SDL_Color color = StackIn_c;
	const float radius = StackIn_f;
	float y = StackIn_f;
	float x = StackIn_f;
	Render::DrawCircle({ x, y }, radius, color);
}

//null draw_shape_circle_p(point p, float radius, color color);Draw Circle in point <point> with radius <float> and color <color>;
void CodeExecutor::draw_shape_circle_p(Instance*) {
	const SDL_Color color = StackIn_c;
	const float radius = StackIn_f;
	const SDL_FPoint p = StackIn_p;
	Render::DrawCircle(p, radius, color);
}

//null draw_shape_circle_filled(float x, float y, float radius, color color);Draw filled Circle in point (<float>,<float>) with radius <float> and color <color>;
void CodeExecutor::draw_shape_circle_filled(Instance*) {
	const SDL_Color color = StackIn_c;
	const float radius = StackIn_f;
	float y = StackIn_f;
	float x = StackIn_f;
	Render::DrawCircleFilled({ x, y }, radius, color);
}

//null draw_shape_circle_filled_p(point p, float radius, color color);Draw filled Circle in point <point> with radius <float> and color <color>;
void CodeExecutor::draw_shape_circle_filled_p(Instance*) {
	const SDL_Color color = StackIn_c;
	const float radius = StackIn_f;
	const SDL_FPoint p = StackIn_p;
	Render::DrawCircleFilled(p, radius, color);
}

//int math_min_i(int a, int b);Get minimum value from <int> or <int>;
void CodeExecutor::math_min_i(Instance*) {
	const int b = StackIn_i;
	const int a = StackIn_i;
	StackOut_i(std::max(a, b));
}

//int math_max_i(int a, int b);Get maximum value from <int> or <int>;
void CodeExecutor::math_max_i(Instance*) {
	const int b = StackIn_i;
	const int a = StackIn_i;
	StackOut_i(std::max(a, b));
}

//float math_min(float a, float b);Get minimum value from <float> or <float>;
void CodeExecutor::math_min(Instance*) {
	const float b = StackIn_f;
	const float a = StackIn_f;
	StackOut_f(std::min(a, b));
}

//float math_max(float a, float b);Get maximum value from <float> or <float>;
void CodeExecutor::math_max(Instance*) {
	const float b = StackIn_f;
	const float a = StackIn_f;
	StackOut_f( std::max(a, b) );
}

//point global_get_mouse();Get point of current mouse postion;If map is bigger than screen this give map coords not screen;
void CodeExecutor::global_get_mouse(Instance*) {
	StackOut_p(Core::Mouse.XYf);
}
//null set_self_sprite(sprite spr); Set self sprite to <sprite> with default scale, angle, speed, loop; You can mod sprite via set_sprite_ etc.;
void CodeExecutor::set_self_sprite(Instance* instance) {
	const int spriteId = StackIn_i;
	if (spriteId != -1) {
		Sprite* sprite = Core::GetAssetManager()->GetSprite(spriteId);
		if (sprite != nullptr) {
			instance->SelfSprite = sprite;
			instance->SpriteScaleX = 1.0f;
			instance->SpriteScaleY = 1.0f;
			instance->SpriteCenterX = static_cast<int>(sprite->GetCenterX());
			instance->SpriteCenterY = static_cast<int>(sprite->GetCenterY());
			instance->SpriteAngle = 0.0f;
			instance->SpriteAnimationFrame = 0.0f;
			instance->SpriteAnimationSpeed = 60.0f;
			instance->SpriteAnimationLoop = true;

			// set body type from sprite values
			if (instance->Body.Type == Instance::BodyType::Sprite) {
				//null set_body_type(string type, int value);Set body type for instance, of <string> and optional <int> value; type is enum: None,Sprite,Rect,Circle
				StackOut_s("Sprite");
				StackOut_f(instance->SelfSprite->GetMaskValue());
				set_body_type(instance);
			}
			return;
		}
	}
	instance->SelfSprite = nullptr;
	instance->SpriteScaleX = 0.0f;
	instance->SpriteScaleY = 0.0f;
	instance->SpriteCenterX = 0;
	instance->SpriteCenterY = 0;
	instance->SpriteAngle = 0.0f;
	instance->SpriteAnimationFrame = 0.0f;
	instance->SpriteAnimationSpeed = 0.0f;
	instance->SpriteAnimationLoop = false;
}

//float get_pos_x(); Get x coords of instance;
void CodeExecutor::get_pos_x(Instance* instance) {
	StackOut_f(instance->PosX);
}

//float get_pos_y(); Get y coords of instance;
void CodeExecutor::get_pos_y(Instance* instance) {
	StackOut_f(instance->PosY);
}

//null sound_play(sound asset);Play <asset> sound global;For position call sound_play_at(sound asset)
void CodeExecutor::sound_play(Instance*) {
	const int SoundId = StackIn_i;
	Mix_Chunk* sound = Core::GetAssetManager()->GetSound(SoundId);
	if (sound == nullptr) return;
	Mix_PlayChannel(-1, sound, 0);
}

//null music_play(music asset);Play <asset> music.;There is only one music at once;
void CodeExecutor::music_play(Instance*) {
	const int SoundId = StackIn_i;
	Mix_Music* music = Core::GetAssetManager()->GetMusic(SoundId);
	if (music == nullptr) return;
	Mix_PlayMusic(music, 0);
}

//null sprite_next_frame(); Set SelfSprite next frame; If sprite loop is enable, frame = 0 if frame > frame_max;
void CodeExecutor::sprite_next_frame(Instance* sender) {
	if (sender->SelfSprite == nullptr) return;
	if ( static_cast<int>(++sender->SpriteAnimationFrame) >= sender->SelfSprite->GetMaxFrame()) {
		if (sender->SpriteAnimationLoop) {
			sender->SpriteAnimationFrame = 0;
		}
		else {
			sender->SpriteAnimationFrame = static_cast<float>(sender->SelfSprite->GetMaxFrame());
		}
	}
}

//null sprite_prev_frame(); Set SelfSprite previous frame; If sprite loop is enable, frame = frame_max if frame < frame_max 0;
void CodeExecutor::sprite_prev_frame(Instance* sender) {
	if (sender->SelfSprite == nullptr) return;
	if ( static_cast<int>(--sender->SpriteAnimationFrame) < 0) {
		if (sender->SpriteAnimationLoop) {
			sender->SpriteAnimationFrame = static_cast<float>(sender->SelfSprite->GetMaxFrame()) - 1;
		}
		else {
			sender->SpriteAnimationFrame = 0.0f;
		}
	}
}

//null sprite_set_frame(int frame); Set SelfSprite frame no <int>.; If frame is not exists nothing happen;
void CodeExecutor::sprite_set_frame(Instance* sender) {
	const int frame = StackIn_i;
	if (frame < 0 || frame >= sender->SelfSprite->GetMaxFrame()) return;
	sender->SpriteAnimationFrame = static_cast<float>(frame);
}

//null code_do_nothing();Do nothing, empty action;Use when there is no else in if
void CodeExecutor::code_do_nothing(Instance*) {
	return;
}
//null set_body_type(string type, float value);Set body type for instance, of <string> and optional <float> value; type is enum: None,Sprite,Rect,Circle
void CodeExecutor::set_body_type(Instance* sender) {
	const float value = StackIn_f;
	const std::string type = StackIn_s;
	if (Instance::BodyType::Body_fromString(type) == Instance::BodyType::BodyInvalid) return;
	if (Instance::BodyType::Body_fromString(type) == Instance::BodyType::Sprite) {
		if (sender->SelfSprite != nullptr) {
			switch (sender->SelfSprite->GetMaskType()) {
			case Sprite::mask_type::None:
			{
				sender->Body.Value = 0;
				sender->Body.Type = Instance::BodyType::None;
				sender->IsCollider = false;
			}break;
			case Sprite::mask_type::Rectangle:
			{
				sender->Body.Value = sender->SelfSprite->GetMaskValue();
				sender->Body.Type = Instance::BodyType::Rect;
				sender->IsCollider = true;
			}break;
			case Sprite::mask_type::Circle:
			{
				sender->Body.Value = sender->SelfSprite->GetMaskValue();
				sender->Body.Type = Instance::BodyType::Circle;
				sender->IsCollider = true;
			}break;

			}
		}
		else {
			sender->Body.Value = 0;
			sender->Body.Type = Instance::BodyType::None;
			sender->IsCollider = false;
		}
	}
	else {
		sender->Body.Value = value;
		sender->Body.Type = Instance::BodyType::Body_fromString(type);
		sender->IsCollider = true;
	}
}

//null instance_set_tag(string tag);Set tag for current instance <string>.
void CodeExecutor::instance_set_tag(Instance* sender) {
	const std::string tag = StackIn_s;
	sender->Tag = tag;
}

//instance collision_get_collider();Return reference to instance with this object is collide;Other colliders must be solid too to collide;
void CodeExecutor::collision_get_collider(Instance*) {
	StackOut_ins( Core::GetCurrentScene()->CurrentCollisionInstance );
}
//string collision_get_collider_tag();Get tag of instance that is coliding with this object;Other colliders must be solid too to collide;
void CodeExecutor::collision_get_collider_tag(Instance*) {
	if (Core::GetCurrentScene()->CurrentCollisionInstance != nullptr)
		StackOut_s(Core::GetCurrentScene()->CurrentCollisionInstance->Tag);
	else
		StackOut_s("nul");
}
//string collision_get_collider_name();Get name of instance that is coliding with this object;Other colliders must be solid too to collide;
void CodeExecutor::collision_get_collider_name(Instance*) {
	if (Core::GetCurrentScene()->CurrentCollisionInstance != nullptr)
		StackOut_s(Core::GetCurrentScene()->CurrentCollisionInstance->Name);
	else
		StackOut_s("nul");
}
//int collision_get_collider_id();Get id of instance that is coliding with this object;Other colliders must be solid too to collide;
void CodeExecutor::collision_get_collider_id(Instance*) {
	if (Core::GetCurrentScene()->CurrentCollisionInstance != nullptr)
		StackOut_i(static_cast<int>(Core::GetCurrentScene()->CurrentCollisionInstanceId));
	else
		StackOut_i(-1);
}
//int get_random(int max);Get random value [0,<int>).;0 is include max is exclude;
void CodeExecutor::get_random(Instance*) {
	const int max = StackIn_i;
	StackOut_i(rand() % max);
}
//int get_random_range(int min, int max);Get random value [<int>,<int>).;0 is include max is exclude;
void CodeExecutor::get_random_range(Instance*) {
	const int max = StackIn_i;
	const int min = StackIn_i;
	StackOut_i(min + (std::rand() % (max - min + 1)));
}
//null scene_change_transmission(string scene, string transmission);[NOT_IMPLEMENTED_YET];0;
void CodeExecutor::scene_change_transmission(Instance*) {
	std::string transmission = StackIn_s;
	std::string scene = StackIn_s;
}
//null scene_change(string scene);Change scene to <scene>;This is quick change, for transmission use scene_change_transmission[NOT_IMPLEMENTED_YET];
void CodeExecutor::scene_change(Instance*) {
	Core::GetInstance()->ChangeScene(StackIn_s);
}
//float get_direction_of(instance target);Return direction of <instance> instance in degree (-180 : 180);Use with collision_get_collider, if target not exists return own direction
void CodeExecutor::get_direction_of(Instance* sender) {
	const Instance* target = StackIn_ins;
	if (target == nullptr) {
		StackOut_f(Convert::RadiansToDegree(sender->Direction));
	}
	else {
		StackOut_f(Convert::RadiansToDegree(target->Direction));
	}
}
//instance instance_spawn(string name, float x, float y);Spawn object <string> at (<float>,<float>) and return reference to it;Ypu can use reference to pass arguments;
void CodeExecutor::instance_spawn(Instance*) {
	const float y = StackIn_f;
	const float x = StackIn_f;
	const std::string obj_name = StackIn_s;
	Instance* ref = Core::GetCurrentScene()->CreateInstance(obj_name, x, y);
	StackOut_ins(ref);
}
//instance instance_spawn_on_point(string name, point xy);Spawn object <string> at (<point>) and return reference to it;Ypu can use reference to pass arguments;
void CodeExecutor::instance_spawn_on_point(Instance*) {
	const SDL_FPoint xy = StackIn_p;
	const std::string obj_name = StackIn_s;
	Instance* ref = Core::GetCurrentScene()->CreateInstance(obj_name, xy.x, xy.y);
	StackOut_ins(ref);
}
//null instance_create(string name, float x, float y);Spawn object <string> at (<float>,<float>) in current scene;This not return reference;
void CodeExecutor::instance_create(Instance*) {
	const float y = StackIn_f;
	const float x = StackIn_f;
	const std::string obj_name = StackIn_s;
	Core::GetCurrentScene()->CreateInstance(obj_name, x, y);
}
//null set_direction_for_target(instance target, float direction);Set <instance> direction to <float> value in degree (-180 : 180);You can get reference from id of instance
void CodeExecutor::set_direction_for_target(Instance*) {
	const float direction = StackIn_f;
	Instance* instance = StackIn_ins;
	if (instance != nullptr) {
		instance->Direction = Convert::DegreeToRadians(direction);
	}
}
//null set_direction(float direction);Set current direction to <float> in degree (-180 : 180);
void CodeExecutor::set_direction(Instance* sender) {
	const float direction = StackIn_f;
	sender->Direction = Convert::DegreeToRadians(direction);
}

//float convert_int_to_float(int value);Convert <int> to float type;
void CodeExecutor::convert_int_to_float(Instance* sender) {
	StackOut_f((float)StackIn_i);
}

//int convert_float_to_int(float value);Convert <float> to int type;
void CodeExecutor::convert_float_to_int(Instance* sender) {
	StackOut_i((int) SDL_roundf(StackIn_f));
}
//null instance_delete_self();Delete self;
void CodeExecutor::instance_delete_self(Instance* sender) {
	sender->Delete();
}
//float get_direction();Get current direction;
void CodeExecutor::get_direction(Instance* sender) {
	StackOut_f(sender->Direction);
}
//float math_add(float a, float b);Get sum of <float> + <float>;
void CodeExecutor::math_add(Instance* sender) {
	const float b = StackIn_f;
	const float a = StackIn_f;
	StackOut_f(a + b);
}
//float math_sub(float a, float b);Get sub of <float> - <float>;
void CodeExecutor::math_sub(Instance* sender) {
	const float b = StackIn_f;
	const float a = StackIn_f;
	StackOut_f(a - b);
}
//float math_mul(float a, float b);Get mul of <float> * <float>;
void CodeExecutor::math_mul(Instance* sender) {
	const float b = StackIn_f;
	const float a = StackIn_f;
	StackOut_f(a * b);
}
//float math_div(float a, float b);Get div of <float> / <float>;
void CodeExecutor::math_div(Instance* sender) {
	const float b = StackIn_f;
	const float a = StackIn_f;
	StackOut_f(a / b);
}
//float get_point_x(point point);Get x of <point> point;
void CodeExecutor::get_point_x(Instance* sender) {
	StackOut_f(StackIn_p.x);
}
//float get_point_y(point point);Get y of <point> point;
void CodeExecutor::get_point_y(Instance* sender) {
	StackOut_f(StackIn_p.y);
}
//null collision_push_other(bool myself);Push other instance in other direction, if <bool> then push this instance too;Like opposite magnets;
void CodeExecutor::collision_push_other(Instance* self) {
	const bool myself = StackIn_b;

	Instance* other = Core::GetCurrentScene()->CurrentCollisionInstance;
	if (other == nullptr) return;

	const float direction = std::atan2f(other->PosY - self->PosY, other->PosX - self->PosX);
	float move;
	if (myself) {
		other->PosX += std::cosf(direction) * 16.f * static_cast<float>(Core::DeltaTime);
		other->PosY += std::sinf(direction) * 16.f * static_cast<float>(Core::DeltaTime);
		move = 16.f;
	}
	else {
		move = 32.f;
	}
	other->PosX += std::cosf(direction - 180.f) * move * static_cast<float>(Core::DeltaTime);
	other->PosY += std::sinf(direction - 180.f) * move * static_cast<float>(Core::DeltaTime);

}
//bool mouse_is_pressed(int button);Return state of button <int>;Left button is 1, right is 3
void CodeExecutor::mouse_is_pressed(Instance* sender) {
	const int button = StackIn_i;
	if (button == 1) {
		StackOut_b(Core::Mouse.LeftPressed);
		return;
	}
	if ((button == 3)) {
		StackOut_b(Core::Mouse.RightPressed);
		return;
	}
	// else
	StackOut_b(false);
}
//float get_delta_time();Return delta time of frame.;Every build-in action of moving or collision uses delta time, do not use twice!
void CodeExecutor::get_delta_time(Instance* sender) {
	StackOut_f( static_cast<float>(Core::DeltaTime) );
}
//null code_break(); Break from current function; Everything will be lost...
void CodeExecutor::code_break(Instance*) {
	CodeExecutor::Break();
}
//null draw_text(font font, int x, int y, string text, color color);Use <font> and draw <string> on screen on (<int>,<int>) with <color> color;If font is null, default font is used;
void CodeExecutor::draw_text(Instance*) {
	const SDL_Color color = StackIn_c;
	const std::string text = StackIn_s;
	float y = static_cast<float>(StackIn_i);
	float x = static_cast<float>(StackIn_i);
	const int fontId = StackIn_i;
	FC_Font* font = Core::GetAssetManager()->GetFont(fontId);
	if (font == nullptr) {
		font = Core::GetGlobalFont();
	}

	Render::DrawText(text, font, { x,y }, color);

}
//string convert_int_to_string(int input);Convert <int> to string;
void CodeExecutor::convert_int_to_string(Instance*) {
	StackOut_s(std::to_string(StackIn_i));
}
//string convert_float_to_string(int input);Convert <int> to string;
void CodeExecutor::convert_float_to_string(Instance*) {
	StackOut_s(std::to_string(StackIn_f));
}
//string string_add(string str1, string str2);Create new string from <string> and <string>
void CodeExecutor::string_add(Instance*) {
	const std::string string2 = StackIn_s;
	const std::string string1 = StackIn_s;
	StackOut_s(string1 + string2);
}
//null sprite_set_scale(point scale);Set scale for self sprite <point>;Scale is from point (width, height)
void CodeExecutor::sprite_set_scale(Instance* sender) {
	const SDL_FPoint scale = StackIn_p;
	if (sender->SelfSprite != nullptr) {
		sender->SpriteScaleX = scale.x;
		sender->SpriteScaleY = scale.y;
	}
}
//null draw_text_in_frame(font font, string text, float x, float y, color text_color, color frame_color, color background_color);Using <font> draw <text> in frame at (<float>,<float>) with <color>. Frame color is <color> and background <color>;
void CodeExecutor::draw_text_in_frame(Instance*)
{
	const SDL_Color background_color = StackIn_c;
	const SDL_Color frame_color = StackIn_c;
	const SDL_Color text_color = StackIn_c;
	const float y = StackIn_f;
	const float x = StackIn_f;
	const std::string text = StackIn_s;
	FC_Font* font = Core::GetAssetManager()->GetFont(StackIn_i);
	if (font == nullptr) {
		font = Core::GetGlobalFont();
	}
	const GPU_Rect box = FC_GetBounds(font, x, y, FC_ALIGN_CENTER, FC_MakeScale(1.f, 1.f), text.c_str());
	Render::DrawRectFilled(box, background_color);
	Render::DrawRect(box, frame_color);
	Render::DrawTextAlign(text, font, { x,y }, text_color, FC_ALIGN_CENTER);
}
//null gui_change_visibility(string guiTag, bool visible);Change visibility variable for <string> to <bool>;Give path to element panel1/button1 by tags. Root is in default first tag do not include it
void CodeExecutor::gui_change_visibility(Instance*)
{
	const bool visible = StackIn_b;
	const std::string guiTag = StackIn_s;

	Gui::GuiElementTemplate* element = Core::GetCurrentScene()->GuiSystem.Element(guiTag);
	if (element == nullptr) return;
	element->SetVisible(visible);
}
//null gui_change_enabled(string guiTag, bool enable);Change enabled variable for <string> to <bool>;Give path to element panel1/button1 by tags. Root is in default first tag do not include it
void CodeExecutor::gui_change_enabled(Instance*)
{
	const bool enable = StackIn_b;
	const std::string guiTag = StackIn_s;

	Gui::GuiElementTemplate* element = Core::GetCurrentScene()->GuiSystem.Element(guiTag);
	if (element == nullptr) return;
	element->SetEnabled(enable);
}
//null code_execute_trigger(string trigger);Execute trigger <string> on current scene;Create this triggers in scene editor.
void CodeExecutor::code_execute_trigger(Instance*)
{
	const std::string trigger = StackIn_s;
	Core::Executor()->ExecuteCode(
		Core::GetCurrentScene()->GetVariableHolder(),
		Core::GetCurrentScene()->GetTriggerData(trigger));

}
//null code_wait(int milliseconds);Suspend this trigger for <int> milliseconds.;This suspend future execution of this trigger until time have pass.
void CodeExecutor::code_wait(Instance* sender)
{
	const int milliseconds = StackIn_i;
	CodeExecutor::SuspendedCodeAdd((double)milliseconds, sender);
	Break();
}
//null game_exit();Exit the game;
void CodeExecutor::game_exit(Instance*)
{
	Core::Exit();
}
//point get_instance_position(instance instance);Get position of <instance>;
void CodeExecutor::get_instance_position(Instance*)
{
	const Instance* instance = StackIn_ins;
	if (instance == nullptr)
	{
		Break();
		return;
	}
	const SDL_FPoint new_point = { instance->PosX, instance->PosY };
	StackOut_p(new_point);
}
//float get_instance_position_x(instance instance);Get position X of <instance>;
void CodeExecutor::get_instance_position_x(Instance*)
{
	const Instance* instance = StackIn_ins;
	if (instance == nullptr)
	{
		Break();
		return;
	}
	StackOut_f(instance->PosX);
}
//float get_instance_position_y(instance instance);Get position Y of <instance>;
void CodeExecutor::get_instance_position_y(Instance*)
{
	const Instance* instance = StackIn_ins;
	if (instance == nullptr)
	{
		Break();
		return;
	}
	StackOut_f(instance->PosY);
}

//null instance_create_point(string name, point xy);Spawn object <string> at (<point>) in current scene;This not return reference;
void CodeExecutor::instance_create_point(Instance*) {
	const SDL_FPoint xy = StackIn_p;
	const std::string obj_name = StackIn_s;
	Core::GetCurrentScene()->CreateInstance(obj_name, xy.x, xy.y);
}

//null instance_delete_other(instance instance);Delete <instance>;
void CodeExecutor::instance_delete_other(Instance*) {
	Instance* instance = StackIn_ins;
	if (instance == nullptr)
	{
		Break();
		return;
	}
	instance->Delete();
}

//instance instance_find_by_tag(string tag);Find instance by tag: <string>;All tags must be unique, else returned instance is first found;
void CodeExecutor::instance_find_by_tag(Instance*) {
	const std::string tag = StackIn_s;
	Instance* instance = Core::GetCurrentScene()->GetInstanceByTag(tag);
	StackOut_ins(instance);
}

//bool instance_exists(instance instance);Get current status of <instance>
void CodeExecutor::instance_exists(Instance*) {
	const Instance* instance = StackIn_ins;
	if(instance != nullptr)
	{
		StackOut_b(instance->Alive);
	}else
	{
		StackOut_b(false);
	}
}
//bool instance_alive(instance instance);Get current status of <instance>
void CodeExecutor::instance_alive(Instance*) {
	const Instance* instance = StackIn_ins;
	if(instance == nullptr)
	{
		StackOut_b(false);
	}else
	{
		StackOut_b(instance->Alive);
	}
}

//int scene_get_width();Get scene width;
void CodeExecutor::scene_get_width(Instance*) {
	StackOut_i(Core::GetCurrentScene()->GetWidth());
}
//int scene_get_height();Get scene height;
void CodeExecutor::scene_get_height(Instance*) {
	StackOut_i(Core::GetCurrentScene()->GetHeight());
}

//null collision_bounce();Change direction of this object, bounce of other.;This trigger works only in OnCollision event!
void CodeExecutor::collision_bounce(Instance* sender) {
	Instance* target = Core::GetCurrentScene()->CurrentCollisionInstance;
	// all error checks is in Physics
	Physics::BounceInstance(sender, target);
}

//int gui_get_slider_value(string gui_tag);Get exact value from slider with <string> tag;Tag must be unique. Values are between min and max and rounded to step
void CodeExecutor::gui_get_slider_value(Instance*) {
	const std::string gui_tag = StackIn_s;
	try {
		const GuiElement::Slider* element = dynamic_cast<GuiElement::Slider*>(Core::GetCurrentScene()->GuiSystem.GetElementById(gui_tag));
		if (element == nullptr)
		{
			// error
			ASSERT(true, "element not found id='" + gui_tag + "'");
			Break();
			return;
		}
		StackOut_i(element->GetValue());
	} catch (...)
	{
		// error cast
		ASSERT(true, "error dynamic_cast<GuiElement::Slider*>");
	}
}

//bool gui_get_check_box_value(string gui_tag);Get checked value from check box with <string> tag;Tag must be unique. Values are true or false
void CodeExecutor::gui_get_check_box_value(Instance*) {
	const std::string gui_tag = StackIn_s;
	try {
		const GuiElement::CheckButton* element = dynamic_cast<GuiElement::CheckButton*>(Core::GetCurrentScene()->GuiSystem.GetElementById(gui_tag));
		if (element == nullptr)
		{
			// error
			ASSERT(true, "element not found id='" + gui_tag + "'");
			Break();
			return;
		}
		StackOut_b(element->Checked());
	}
	catch (...)
	{
		// error cast
		ASSERT(true, "error dynamic_cast<GuiElement::CheckButton*>");
	}
}

//int gui_get_drop_down_selected_index(string gui_tag);Get selected index of drop down gui element with tag <string>;Index is 0-based
void CodeExecutor::gui_get_drop_down_selected_index(Instance*) {
	const std::string gui_tag = StackIn_s;
	try {
		const GuiElement::DropDownList* element = dynamic_cast<GuiElement::DropDownList*>(Core::GetCurrentScene()->GuiSystem.GetElementById(gui_tag));
		if (element == nullptr)
		{
			// error
			ASSERT(true, "element not found id='" + gui_tag + "'");
			Break();
			return;
		}
		StackOut_i(element->GetSelectedIndex());
	}
	catch (...)
	{
		// error cast
		ASSERT(true, "error dynamic_cast<GuiElement::DropDownList*>");
	}
}

//int gui_get_drop_down_selected_value(string gui_tag);Get selected value of drop down gui element with tag <string>;Value may be empty if nothing is selected
void CodeExecutor::gui_get_drop_down_selected_value(Instance*) {
	const std::string gui_tag = StackIn_s;
	try {
		const GuiElement::DropDownList* element = dynamic_cast<GuiElement::DropDownList*>(Core::GetCurrentScene()->GuiSystem.GetElementById(gui_tag));
		if (element == nullptr)
		{
			// error
			ASSERT(true, "element not found id='" + gui_tag + "'");
			Break();
			return;
		}
		StackOut_s(element->GetSelectedValue());
	}
	catch (...)
	{
		// error cast
		ASSERT(true, "error dynamic_cast<GuiElement::DropDownList*>");
	}
}

//null gui_set_slider_value( int value, string gui_tag);Set exact value <int> for slider with <string> tag;Tag must be unique. Values are between min and max and rounded to step
void CodeExecutor::gui_set_slider_value(Instance*) {
	const std::string gui_tag = StackIn_s;
	const int value = StackIn_i;
	try {
		GuiElement::Slider* element = dynamic_cast<GuiElement::Slider*>(Core::GetCurrentScene()->GuiSystem.GetElementById(gui_tag));
		if (element == nullptr)
		{
			// error
			ASSERT(true, "element not found id='" + gui_tag + "'");
			Break();
			return;
		}
		element->SetValue(value);
	}
	catch (...)
	{
		// error cast
		ASSERT(true, "error dynamic_cast<GuiElement::Slider*>");
	}
}

//null gui_set_slider_value_min( int value, string gui_tag);Set minimum value <int> for slider with <string> tag;Tag must be unique. Values are between min and max and rounded to step
void CodeExecutor::gui_set_slider_value_min(Instance*) {
	const std::string gui_tag = StackIn_s;
	const int value = StackIn_i;
	try {
		GuiElement::Slider* element = dynamic_cast<GuiElement::Slider*>(Core::GetCurrentScene()->GuiSystem.GetElementById(gui_tag));
		if (element == nullptr)
		{
			// error
			ASSERT(true, "element not found id='" + gui_tag + "'");
			Break();
			return;
		}
		element->SetValueMin(value);
	}
	catch (...)
	{
		// error cast
		ASSERT(true, "error dynamic_cast<GuiElement::Slider*>");
	}
}

//null gui_set_slider_value_max( int value, string gui_tag);Set maximum value <int> for slider with <string> tag;Tag must be unique. Values are between min and max and rounded to step
void CodeExecutor::gui_set_slider_value_max(Instance*) {
	const std::string gui_tag = StackIn_s;
	const int value = StackIn_i;
	try {
		GuiElement::Slider* element = dynamic_cast<GuiElement::Slider*>(Core::GetCurrentScene()->GuiSystem.GetElementById(gui_tag));
		if (element == nullptr)
		{
			// error
			ASSERT(true, "element not found id='" + gui_tag + "'");
			Break();
			return;
		}
		element->SetValueMax(value);
	}
	catch (...)
	{
		// error cast
		ASSERT(true, "error dynamic_cast<GuiElement::Slider*>");
	}
}

//null gui_set_slider_value_step( int value, string gui_tag);Set step value <int> for slider with <string> tag;Tag must be unique. Values are between min and max and rounded to step
void CodeExecutor::gui_set_slider_value_step(Instance*) {
	const std::string gui_tag = StackIn_s;
	const int value = StackIn_i;
	try {
		GuiElement::Slider* element = dynamic_cast<GuiElement::Slider*>(Core::GetCurrentScene()->GuiSystem.GetElementById(gui_tag));
		if (element == nullptr)
		{
			// error
			ASSERT(true, "element not found id='" + gui_tag + "'");
			Break();
			return;
		}
		element->SetValueStep(value);
	}
	catch (...)
	{
		// error cast
		ASSERT(true, "error dynamic_cast<GuiElement::Slider*>");
	}
}

//null gui_set_check_box_value(bool value, string gui_tag);Set checked property of <bool> for check box with <string> tag;Tag must be unique. Values are true or false
void CodeExecutor::gui_set_check_box_value(Instance*) {
	const std::string gui_tag = StackIn_s;
	const bool value = StackIn_b;
	try {
		GuiElement::CheckButton* element = dynamic_cast<GuiElement::CheckButton*>(Core::GetCurrentScene()->GuiSystem.GetElementById(gui_tag));
		if (element == nullptr)
		{
			// error
			ASSERT(true, "element not found id='" + gui_tag + "'");
			Break();
			return;
		}
		element->SetState(value);
	}
	catch (...)
	{
		// error cast
		ASSERT(true, "error dynamic_cast<GuiElement::CheckButton*>");
	}
}

//null gui_set_drop_down_selected_index(int index, string gui_tag);Set selected index <int> for drop down list with tag <string>;Selected index is checked if can be selected
void CodeExecutor::gui_set_drop_down_selected_index(Instance*) {
	const std::string gui_tag = StackIn_s;
	const int value = StackIn_i;
	try {
		GuiElement::DropDownList* element = dynamic_cast<GuiElement::DropDownList*>(Core::GetCurrentScene()->GuiSystem.GetElementById(gui_tag));
		if (element == nullptr)
		{
			// error
			ASSERT(true, "element not found id='" + gui_tag + "'");
			Break();
			return;
		}
		element->SetSelectedIndex(value);
	}
	catch (...)
	{
		// error cast
		ASSERT(true, "error dynamic_cast<GuiElement::DropDownList*>");
	}
}

//null system_set_video_mode(bool full_screen);Set full screen to <bool>;
void CodeExecutor::system_set_video_mode(Instance*) {
	const bool mode = StackIn_b;
	Core::Graphic.SetFullScreen(mode);
	Core::Graphic.Apply();
}

//null system_set_video_resolution(int width, int height);Set window resolution to <int> x <int>;
void CodeExecutor::system_set_video_resolution(Instance*) {
	const int height = StackIn_i;
	const int width = StackIn_i;
	if (width == 0 || height == 0)
	{
		// error
		ASSERT(true, "error argument can not convert to int");
	}
	Core::Graphic.SetScreenResolution(width, height);
	Core::Graphic.Apply();
}

//null system_set_video_resolution_from_string(string value);Set window resolution from <string> text;Text must be 0000x0000 (width x height)
void CodeExecutor::system_set_video_resolution_from_string(Instance*) {
	const std::string video_resolution = StackIn_s;
	const std::vector<std::string> video_resolution_s = Func::Split(video_resolution, 'x');
	if(video_resolution_s.size() != 2)
	{
		// error
		ASSERT(true, "error argument '"+ video_resolution+"' can not split");
	}
	const int width = Func::TryGetInt(video_resolution_s[0]);
	const int height = Func::TryGetInt(video_resolution_s[1]);
	if(width == 0 || height == 0)
	{
		// error
		ASSERT(true, "error argument '" + video_resolution + "' can not convert to int");
	}
	Core::Graphic.SetScreenResolution(width, height);
	Core::Graphic.Apply();
}

//null system_set_video_bloom_factor(int mode);Set mode for bloom post process to <int>;0-off, 1-low, 2-medium, 3-high;
void CodeExecutor::system_set_video_bloom_factor(Instance*) {
	switch(std::clamp(StackIn_i, 0, 3))
	{
	case 0:
		Render::SetGaussianEnabled(false);
		break;
	case 1:
		Render::SetGaussianEnabled(true);
		Render::SetGaussianProperties(4, 4, 0.0205f);
		break;
	case 2:
		Render::SetGaussianEnabled(true);
		Render::SetGaussianProperties(8, 8, 0.0205f);
		break;
	case 3:
		Render::SetGaussianEnabled(true);
		Render::SetGaussianProperties(16, 16, 0.0205f);
		break;
	default: break;
	}
}

//null system_set_audio_master(bool mode);Set audio mode for master to <bool>; True means sounds can be played, have higher priority than other audio modes
void CodeExecutor::system_set_audio_master(Instance*) {
	Core::Audio.SetMaster(StackIn_b);
}

//null system_set_audio_music(bool mode);Set audio mode for music to <bool>; True means sounds can be played
void CodeExecutor::system_set_audio_music(Instance*) {
	Core::Audio.SetMusic(StackIn_b);
}

//null system_set_audio_sound(bool mode);Set audio mode for sound to <bool>; True means sounds can be played
void CodeExecutor::system_set_audio_sound(Instance*) {
	Core::Audio.SetSound(StackIn_b);
}

//null system_set_audio_music_level(int level);Set audio level to <int> percent;Level can be from 0 to 100;
void CodeExecutor::system_set_audio_music_level(Instance*) {
	Core::Audio.SetSoundLevel(StackIn_i);
}

//null system_set_audio_sound_level(int level);Set audio level to <int> percent;Level can be from 0 to 100;
void CodeExecutor::system_set_audio_sound_level(Instance*) {
	Core::Audio.SetMusicLevel(StackIn_i);
}
