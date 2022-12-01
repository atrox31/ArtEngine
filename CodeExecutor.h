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

	int GetGlobalStackSize();
	int GetGlobalStackCapacity();

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
		/*
		(NUL)
			(INT)
			(FLOAT)
			(BOOL)
			(INSTANCE)
			(OBJECT)
			(SPRITE)
			(TEXTURE)
			(SOUND)
			(MUSIC)
			(FONT)
			(POINT)
			(RECT)
			(COLOR)
			(STRING)
			*/
		void AddVarible(int type) {
			switch (type) {
				case ArtCode::INT:
					_template->Varibles_int.push_back(0);
					break;
				case ArtCode::FLOAT:
					_template->Varibles_float.push_back(0.0f);
					break;
				case ArtCode::BOOL:
					_template->Varibles_bool.push_back(false);
					break;
				case ArtCode::INSTANCE:
					_template->Varibles_instance.push_back(nullptr);
					break;
				case ArtCode::OBJECT:
					_template->Varibles_object.push_back(-1);
					break;
				case ArtCode::SPRITE:
					_template->Varibles_sprite.push_back(-1);
					break;
				case ArtCode::TEXTURE:
					_template->Varibles_texture.push_back(-1);
					break;
				case ArtCode::SOUND:
					_template->Varibles_sound.push_back(-1);
					break;
				case ArtCode::MUSIC:
					_template->Varibles_music.push_back(-1);
					break;
				case ArtCode::FONT:
					_template->Varibles_font.push_back(-1);
					break;
				case ArtCode::POINT:
					_template->Varibles_point.push_back(SDL_FPoint());
					break;
				case ArtCode::RECT:
					_template->Varibles_rect.push_back(Rect());
					break;
				case ArtCode::COLOR:
					_template->Varibles_color.push_back(SDL_Color());
					break;
				case ArtCode::STRING:
					_template->Varibles_string.push_back(std::string());
					break;
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

	void EraseGlobalStack();
	/*
	void* _nul; // only for varible type
		int _int;
		float _float;
		bool _bool;
		Instance* _instance;
		int _object;
		int _sprite;
		int _texture;
		int _sound;
		int _music;
		int _font;
		SDL_FPoint _rect;
		SDL_Color _color;
		std::string _string;
	*/
	static AStack<int> GlobalStack_int;
	static AStack<float> GlobalStack_float;
	static AStack<bool> GlobalStack_bool;
	static AStack<Instance*> GlobalStack_instance;
	static AStack<SDL_FPoint> GlobalStack_point;
	static AStack<Rect> GlobalStack_rect;
	static AStack<SDL_Color> GlobalStack_color;
	static AStack<std::string> GlobalStack_string;
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
	bool h_compare(int type, int oper);

	//std::string h_operation(int operation, std::string value1, std::string value2);
	int h_operation_int(int _operator, int val1, int val2);
	float h_operation_float(int _operator, float val1, float val2);
	bool h_operation_bool(int _operator, bool val1, bool val2);
	Instance* h_operation_instance(int _operator, Instance* val1, Instance* val2);
	int h_operation_object(int _operator, int val1, int val2);
	int h_operation_sprite(int _operator, int val1, int val2);
	int h_operation_texture(int _operator, int val1, int val2);
	int h_operation_sound(int _operator, int val1, int val2);
	int h_operation_music(int _operator, int val1, int val2);
	int h_operation_font(int _operator, int val1, int val2);
	SDL_FPoint h_operation_point(int _operator, SDL_FPoint val1, SDL_FPoint val2);
	Rect h_operation_rect(int _operator, Rect val1, Rect val2);
	SDL_Color h_operation_color(int _operator, SDL_Color val1, SDL_Color val2);
	std::string h_operation_string(int _operator, std::string val1, std::string val2);


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
	Script(sprite_next_frame);
	Script(sprite_prev_frame);
	Script(sprite_set_frame);
	Script(code_do_nothing);
	Script(set_body_type);
	Script(instance_set_tag);
	Script(collision_get_collider);
	Script(collision_get_collider_tag);
	Script(collision_get_collider_name);
	Script(collision_get_collider_id);
	Script(get_random);
	Script(get_random_range);
	Script(scene_change_transmision);
	Script(scene_change);
	Script(scene_reset);
	Script(get_direction_of);
	Script(instance_spawn);
	Script(instance_create);
	Script(set_direction_for_target);
	Script(set_direction);
	Script(convert_int_to_float);
	Script(convert_float_to_int);
	Script(instance_delete_self);
	Script(get_direction);
	Script(math_add);
	Script(math_sub);
	Script(math_mul);
	Script(math_div);
	Script(get_point_x);
	Script(get_point_y);
	Script(collision_push_other);
	Script(mouse_is_pressed);
	Script(get_delta_time);
	Script(code_break);
#undef Script
};
//end of file
