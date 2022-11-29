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
	Script(new_direction);
	Script(new_rectangle);
	Script(new_rectangle_f);
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
	Script(distance_to_point);
	Script(distance_beetwen_point);
	Script(distance_to_instance);
	Script(direction_to_point);
	Script(direction_beetwen_point);
	Script(direction_to_instance);
	Script(draw_sprite);
	Script(draw_sprite_ex);
	Script(draw_texture);
	Script(draw_texture_ex);
	Script(draw_sprite_self);
	Script(draw_shape_rectangle);
	Script(draw_shape_rectangle_r);
	Script(draw_shape_rectangle_filled);
	Script(draw_shape_rectangle_filled_r);
	Script(draw_shape_circle);
	Script(draw_shape_circle_p);
	Script(draw_shape_circle_filled);
	Script(draw_shape_circle_filled_p);
	Script(math_min_i);
	Script(math_max_i);
	Script(math_min);
	Script(math_max);
	Script(global_get_mouse);
	Script(set_self_sprite);
	Script(get_pos_x);
	Script(get_pos_y);
	Script(sound_play);
	Script(music_play);
#undef Script
};
//end of file
