#pragma once
#include <map>
#include <string>
#include <vector>

#include "SDL_version.h"

// include all sdl systems
#pragma warning(push)
#include SDL2_INCLUDE_DIR
#include SDL2_IMAGE_INCLUDE_DIR
#include SDL2_MIXER_INCLUDE_DIR
#include SDL2_NET_INCLUDE_DIR
#include SDL2_TTF_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR
#pragma warning(pop)

#include "Instance.h"
#include "Event.h"
#include "Stack.h"
#include "ArtCode.h"
#include "Inspector.h"

class CodeExecutor
{
public:
	// init
	CodeExecutor();
	void MapFunctions();
	bool LoadArtLib();
	bool LoadObjectDefinitions();

private:
	struct InstanceDefinition {
	public:
		struct EventData {
		public:
			Event event;
			int size;
			const unsigned char* data;

			bool operator < (const EventData& str) const
			{
				return (event < str.event);
			}
		}; 
		std::vector<EventData> _events;
		
		//		type, fields
		std::map<int, int> _varibles;
		std::string _name;
		InstanceDefinition() {
			_name = "";
			_varibles = std::map<int, int>();
			_events = std::vector<EventData>();
		}
		void AddVarible(int type) {
			auto f = _varibles.find(type);
			if (f == _varibles.end()) {
				_varibles.insert(std::pair(type, 1));
			}
			else {
				f->second++;
			}
		}
		
	};
	InstanceDefinition::EventData* GetEventData(int, Event);
	std::vector< InstanceDefinition > InstanceDefinitions;
	InstanceDefinition* FindInstance(std::string name) {
		for (auto& it : InstanceDefinitions) {
			if (it._name == name) return &it;
		}
		return nullptr;
	}
public:
	Instance* SpawnInstance(std::string name);
	// not safe! use only in inner functions. this not error-proof
	Instance* SpawnInstance(int id); 
	void ExecuteScript(Instance* instance, Event script);
private:
	std::map<std::string, void(*)(Instance*)> FunctionsMap;
	std::vector<void(*)(Instance*)> FunctionsList;
	static AStack<std::string> GlobalStack;
private:
	// helpers
	bool _break;
	// Break from current script, rise ASSERTif in debug mode
	void Break();
	void h_execute_script(Inspector*, Instance*);
	void h_execute_function(Inspector* , Instance*);
	void h_get_value(Inspector*, Instance*);
	int	 h_if_test(Inspector* code, Instance* instance);
	void h_get_local_value(Inspector* code, Instance* instance);
	bool h_compare(Inspector*, Instance*);

	

private:
	// makers
		//point new_point(float x, float y);Make point (<float>, <float>).;New point from value or other.
		static void new_point(Instance*);
		//float new_direction(point from, point to);Make direction from <point> to <point>.;Value are from 0 to 359.
		static void new_direction(Instance*);
		//rectangle new_rectangle(int x1, int y1, int x2, int y2);Make rectangle from <int>, <int> to <int>, <int>.;This is const rectangle, not width and height.
		static void new_rectangle(Instance*);
		//rectangle new_rectangle_f(float x1, float y1, float x2, float y2);Make rectangle from <float>, <float> to <float>, <float>.;This is const rectangle, not width and height.
		static void new_rectangle_f(Instance*);
		//rectangle new_mask_from_sprite(sprite spr);Make rectangle mask from <sprite>;Cirkle mask is not supported yet.
		static void new_mask_from_sprite(Instance*);
	// assets
		//sprite get_sprite(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
		static void get_sprite(Instance*);
		//texture get_texture(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
		static void get_texture(Instance*);
		//music get_music(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
		static void get_music(Instance*);
		//sound get_sound(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
		static void get_sound(Instance*);
		//font get_font(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
		static void get_font(Instance*);
	// sprite
		//int sprite_get_width(sprite spr);Get width of <sprite>;Get int value.
		static void sprite_get_width(Instance*);
		//int sprite_get_height(sprite spr);Get height of <sprite>;Get int value.
		static void sprite_get_height(Instance*);
		//int sprite_get_frames(sprite spr);Get frames number of <sprite>;Get int value.
		static void sprite_get_frames(Instance*);
		//null sprite_set_animation_speed(sprite spr, float speed);Set animation speed for <sprite> value <float> frames per seccond;Every sprite can have own animation speed
		static void sprite_set_animation_speed(Instance*);
		//null sprite_set_animation_loop(sprite spr, bool loop);Set animation loop for <sprite> value <bool>;Every animation end generate event ON_ANIMATION_END
		static void sprite_set_animation_loop(Instance*);
	// moving
		//null move_to_point(point p, float speed);Move current instance to <point> with <speed> px per seccond.;Call it every frame.
		static void move_to_point(Instance*);
		//null move_forward(float speed);Move current instance forward with <speed> px per seccond.;Call it every frame. Function give build-in direction varible.
		static void move_forward(Instance*);
		//null move_instant(point p);Move instantly to target <point>;This changes x and y. Not cheking for collision;
		static void move_instant(Instance*);
		//null move_to_direction(float direction, float speed);Move instance toward direction of <float> (0-359) with <float> speed px per seccond;If direction is not in range its clipped to 360.
		static void move_to_direction(Instance*);
	// mesurment
		//float distance_to_point(point p);Give distance to <point>;Measure from current instance to target point.
		static void distance_to_point(Instance*);
		//float distance_beetwen_point(point p1, point p2);Give distance from <point> to <point>;Measure distance.
		static void distance_beetwen_point(Instance*);
		//float distance_to_instance(instance i);Give distance to <point>;Measure from current instance to target point.
		static void distance_to_instance(Instance*);
	// drawing
		//null draw_sprite(sprite spr, float x, float y, float frame);Draw <sprite> on location (<float>,<float>) with target frame <frame>;Draw default sprite. To more options use draw_sprite_ex
		static void draw_sprite(Instance*);
		//null draw_sprite_ex(sprite spr, float x, float y, float frame, float x_scale, float y_scale, float angle, float alpha);Draw <sprite> on location (<float>,<float>) with target frame <frame>;Draw sprite.
		static void draw_sprite_ex(Instance*);
		//null draw_texture(texture tex, float x, float y);Draw <texture> on (<float>,<float>).;Draw standard texture with it normal dimensions. For extended option use 'Draw texture extended';
		static void draw_texture(Instance*);
		//null draw_texture_ex(texture tex, float x, float y, float x_scale, float y_scale, float angle, float alpha);Draw <texture> on (<float>,<float>), with scale (<float>,<float>), angle <float> and aplha <float>;Angle range is (0 - 359) alpha (0.0f - 1.0f).
		static void draw_texture_ex(Instance*);
		//null draw_sprite_self();Draw self sprite on self coords with sprite scale and angle;Use build-in varibles;
		static void draw_sprite_self(Instance*);
	// draw shapes
		//null draw_shape_rectangle(float x1, float y2, float x2, float y2, color color);Draw frame of rectangle from (<float>,<float>) to (<float>,<float>) with color <color>.;Draw rectangle on final coords;
		static void draw_shape_rectangle(Instance*);
		//null draw_shape_rectangle_r(rectangle rect, color color);Draw frame of <rectangle> with color <color>.;Draw rectangle;
		static void draw_shape_rectangle_r(Instance*);
		//null draw_shape_rectangle_filled(float x1, float y2, float x2, float y2, color color);Draw filled of rectangle from (<float>,<float>) to (<float>,<float>) with color <color>.;Draw rectangle on final coords;
		static void draw_shape_rectangle_filled(Instance*);
		//null draw_shape_rectangle_filled_r(rectangle rect, color color);Draw filled <rectangle> with color <color>.;Draw rectangle;
		static void draw_shape_rectangle_filled_r(Instance*);
		//null draw_shape_circle(float x, float y, float radius, color color);Draw circle in point (<float>,<float>) with radius <float> and color <color>;
		static void draw_shape_circle(Instance*);
		//null draw_shape_circle_p(point p, float radius, color color);Draw circle in point <point> with radius <float> and color <color>;
		static void draw_shape_circle_p(Instance*);
		//null draw_shape_circle_filled(float x, float y, float radius, color color);Draw filled circle in point (<float>,<float>) with radius <float> and color <color>;
		static void draw_shape_circle_filled(Instance*);
		//null draw_shape_circle_filled_p(point p, float radius, color color);Draw filled circle in point <point> with radius <float> and color <color>;
		static void draw_shape_circle_filled_p(Instance*);
	// math
		//int math_min_i(int a, int b);Get minimum value from <int> or <int>;
		static void math_min_i(Instance*);
		//int math_max_i(int a, int b);Get maximum value from <int> or <int>;
		static void math_max_i(Instance*);
		//float math_min(float a, float b);Get minimum value from <float> or <float>;
		static void math_min(Instance*);
		//float math_max(float a, float b);Get maximum value from <float> or <float>;
		static void math_max(Instance*);
	// globals
		//point global_get_mouse();Get point of current mouse postion;If map is bigger than screen this give map coords not screen;
		static void global_get_mouse(Instance*);
	// set
		//null set_self_sprite(sprite spr); Set self sprite to <sprite> with default scale, angle, speed, loop; You can mod sprite via set_sprite_ etc.;
		static void set_self_sprite(Instance*);
		//float get_pos_x(); Get x coords of instance;
		static void get_pos_x(Instance*);
		//float get_pos_y(); Get y coords of instance;
		static void get_pos_y(Instance*);
};

