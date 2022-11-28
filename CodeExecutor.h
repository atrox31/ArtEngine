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
	void Delete();

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
		Instance* _template;
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
#define Script(x) static void x(Instance*);
//#AUTO_GENERATOR_START
	Script(new_point);
	Script(new_direction)
	Script(new_rectangle)
	Script(new_rectangle_f);
	Script(new_mask_from_sprite);
	Script(get_sprite);
	Script(get_texture);
	Script(get_music);
	Script(get_sound);
	Script(get_font);
	Script(sprite_get_width);
	Script(sprite_get_height);
	Script(sprite_get_frames);
	Script(sprite_set_animation_speed);
	Script(sprite_set_animation_loop);
	Script(move_to_point);
	Script(move_forward);
	Script(move_instant);
	Script(move_to_direction);
	Script(direction_to_point);
	Script(direction_beetwen_point);
	Script(direction_to_instance);
	Script(distance_to_point);
	Script(distance_beetwen_point);
	Script(distance_to_instance);
	Script(draw_sprite);
	Script(draw_sprite_ex);
	Script(draw_texture);
	Script(draw_texture_ex);
	Script(draw_sprite_self);
	// draw shapes
		//null draw_shape_rectangle(float x1, float y2, float x2, float y2, color color);Draw frame of rectangle from (<float>,<float>) to (<float>,<float>) with color <color>.;Draw rectangle on final coords;
	Script(draw_shape_rectangle);
	//null draw_shape_rectangle_r(rectangle rect, color color);Draw frame of <rectangle> with color <color>.;Draw rectangle;
	Script(draw_shape_rectangle_r);
	//null draw_shape_rectangle_filled(float x1, float y2, float x2, float y2, color color);Draw filled of rectangle from (<float>,<float>) to (<float>,<float>) with color <color>.;Draw rectangle on final coords;
	Script(draw_shape_rectangle_filled);
	//null draw_shape_rectangle_filled_r(rectangle rect, color color);Draw filled <rectangle> with color <color>.;Draw rectangle;
	Script(draw_shape_rectangle_filled_r);
	//null draw_shape_circle(float x, float y, float radius, color color);Draw circle in point (<float>,<float>) with radius <float> and color <color>;
	Script(draw_shape_circle);
	//null draw_shape_circle_p(point p, float radius, color color);Draw circle in point <point> with radius <float> and color <color>;
	Script(draw_shape_circle_p);
	//null draw_shape_circle_filled(float x, float y, float radius, color color);Draw filled circle in point (<float>,<float>) with radius <float> and color <color>;
	Script(draw_shape_circle_filled);
	//null draw_shape_circle_filled_p(point p, float radius, color color);Draw filled circle in point <point> with radius <float> and color <color>;
	Script(draw_shape_circle_filled_p);
	// math
		//int math_min_i(int a, int b);Get minimum value from <int> or <int>;
	Script(math_min_i);
	//int math_max_i(int a, int b);Get maximum value from <int> or <int>;
	Script(math_max_i);
	//float math_min(float a, float b);Get minimum value from <float> or <float>;
	Script(math_min);
	//float math_max(float a, float b);Get maximum value from <float> or <float>;
	Script(math_max);
	// globals
		//point global_get_mouse();Get point of current mouse postion;If map is bigger than screen this give map coords not screen;
	Script(global_get_mouse);
	// set
		//null set_self_sprite(sprite spr); Set self sprite to <sprite> with default scale, angle, speed, loop; You can mod sprite via set_sprite_ etc.;
	Script(set_self_sprite);
	//float get_pos_x(); Get x coords of instance;
	Script(get_pos_x);
	//float get_pos_y(); Get y coords of instance;
	Script(get_pos_y);
	//
	Script(sound_play);
	Script(music_play);
#undef Script
};

