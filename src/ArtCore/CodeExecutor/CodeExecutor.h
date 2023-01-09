#pragma once
#include <map>
#include <string>
#include <vector>

#include "Stack.h"
#include "ArtCode.h"
#include "Inspector.h"
#include "ArtCore/Enums/Event.h"
#include "ArtCore/Scene/Instance.h"

class BackGroundRenderer;
class CodeExecutor
{
public:
	// init
	CodeExecutor();
	void MapFunctions();
	bool LoadArtLib();
	bool LoadObjectDefinitions(const BackGroundRenderer* bgr, const int p_min, const int p_max);
	bool LoadSceneTriggers();
	void Delete();

	static int GetGlobalStackSize();
	static int GetGlobalStackCapacity();

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
		std::string Name;
		Instance* Template;
		std::map<ArtCode::variable_type, std::vector<std::string>> VariablesNames;

		InstanceDefinition()
		{
			Template = nullptr;
			Name = "";
			_events = std::vector<EventData>();
			VariablesNames = std::map<ArtCode::variable_type, std::vector<std::string>>();
			for (
				ArtCode::variable_type v = (ArtCode::variable_type)(ArtCode::variable_type::variable_typeInvalid+1);
				v < ArtCode::variable_type::variable_typeEND;
				v = (ArtCode::variable_type)(v + 1)
				)
			{
				VariablesNames[v] = std::vector<std::string>();
			}
		}

		void AddVariable(int type, const std::string& name) {
			VariablesNames[(ArtCode::variable_type)type].emplace_back(name);
			switch (type) {
				case ArtCode::INT:
					Template->Variables_int.emplace_back(0);
					break;
				case ArtCode::FLOAT:
					Template->Variables_float.emplace_back(0.0f);
					break;
				case ArtCode::BOOL:
					Template->Variables_bool.emplace_back(false);
					break;
				case ArtCode::INSTANCE:
					Template->Variables_instance.emplace_back(nullptr);
					break;
				case ArtCode::OBJECT:
					Template->Variables_object.emplace_back(-1);
					break;
				case ArtCode::SPRITE:
					Template->Variables_sprite.emplace_back(-1);
					break;
				case ArtCode::TEXTURE:
					Template->Variables_texture.emplace_back(-1);
					break;
				case ArtCode::SOUND:
					Template->Variables_sound.emplace_back(-1);
					break;
				case ArtCode::MUSIC:
					Template->Variables_music.emplace_back(-1);
					break;
				case ArtCode::FONT:
					Template->Variables_font.emplace_back(-1);
					break;
				case ArtCode::POINT:
					Template->Variables_point.emplace_back();
					break;
				case ArtCode::RECT:
					Template->Variables_rect.emplace_back();
					break;
				case ArtCode::COLOR:
					Template->Variables_color.emplace_back();
					break;
				case ArtCode::STRING:
					Template->Variables_string.emplace_back();
					break;
			}
		}
		
	};
	InstanceDefinition::EventData* GetEventData(int, Event);
	std::vector< InstanceDefinition > _instance_definitions;
	InstanceDefinition* FindInstance(const std::string& name) {
		for (auto& it : _instance_definitions) {
			if (it.Name == name) return &it;
		}
		return nullptr;
	}

public:
	[[nodiscard]] Instance* SpawnInstance(const std::string& name) const;
	// not safe! use only in inner functions. this not error-proof
	[[nodiscard]] Instance* SpawnInstance(int id) const; 
	void ExecuteScript(Instance* instance, Event script);
	void ExecuteCode(Instance* instance, std::pair<const unsigned char*, Sint64>* code_data);

	std::map<std::string, void(*)(Instance*)> FunctionsMap;
	std::vector<void(*)(Instance*)> FunctionsList;

	static void EraseGlobalStack();

	static AStack<int> GlobalStack_int;
	static AStack<float> GlobalStack_float;
	static AStack<bool> GlobalStack_bool;
	static AStack<Instance*> GlobalStack_instance;
	static AStack<SDL_FPoint> GlobalStack_point;
	static AStack<Rect> GlobalStack_rect;
	static AStack<SDL_Color> GlobalStack_color;
	static AStack<std::string> GlobalStack_string;
private:
	[[nodiscard]] Inspector* CreateInspector(const std::string& code_file) const;
	// Break from current script
	static void Break();
	// list of suspended code <time, code>
	struct SuspendCodeStruct final
	{
		double Time;
		Inspector CodeData;
		Instance* Sender;
		AStack<bool> IfTestState;
		SuspendCodeStruct()
		{
			Time = 0.0;
			CodeData = Inspector();
			Sender = nullptr;
			IfTestState = AStack<bool>();
		}
		SuspendCodeStruct(const double time, const Inspector* code_data, Instance* sender,
			const AStack<bool>& if_test_state){
			Time = time;
			CodeData = Inspector(*code_data); // copy
			Sender = sender;
			IfTestState = AStack<bool>(if_test_state); // copy
			CodeExecutor::_have_suspended_code = true;
		}
		~SuspendCodeStruct()
		{
			CodeExecutor::_have_suspended_code = CodeExecutor::_suspended_code.empty();
		}
	};
	static std::vector<SuspendCodeStruct> _suspended_code;
	static bool _have_suspended_code;
public:
	static void SuspendedCodeStop();
	static void SuspendedCodeAdd(double time, Instance* sender);
	static void SuspendedCodeExecute();
	static void SuspendedCodeDeleteInstance(const Instance* sender);
private:
	Inspector* _current_inspector = nullptr;

	void	h_execute_script(Inspector*, Instance*);
	void	h_execute_function(Inspector* , Instance*);
	void	h_get_value(Inspector*, Instance*);
	int		h_if_test(Inspector* code, Instance* instance);
	// stack of if test result, 
	AStack<bool> _if_test_result;
	void	h_get_local_value(Inspector* code, Instance* instance);
	bool	h_compare(int type, int operation);

	//std::string h_operation(int operation, std::string value1, std::string value2);
	static int h_operation_int(int _operator, int val1, int val2);
	static float h_operation_float(int _operator, float val1, float val2);
	static bool h_operation_bool(int _operator, bool val1, bool val2);
	static Instance* h_operation_instance(int _operator, Instance* val1, Instance* val2);
	static int h_operation_object(int _operator, int val1, int val2);
	static int h_operation_sprite(int _operator, int val1, int val2);
	static int h_operation_texture(int _operator, int val1, int val2);
	static int h_operation_sound(int _operator, int val1, int val2);
	static int h_operation_music(int _operator, int val1, int val2);
	static int h_operation_font(int _operator, int val1, int val2);
	static SDL_FPoint h_operation_point(int _operator, SDL_FPoint val1, SDL_FPoint val2);
	static Rect h_operation_rect(int _operator, Rect val1, Rect val2);
	static SDL_Color h_operation_color(int _operator, SDL_Color val1, SDL_Color val2);
	static std::string h_operation_string(int _operator, std::string val1, std::string val2);
#ifdef _DEBUG
	// all debug things
public:
	int DebugGetIfTestResultStackSize() const
	{
		return _if_test_result.Size();
	}
	void DebugSetInstanceToTrack(Instance* instance);
	std::string DebugGetTrackInfo();
	[[nodiscard]] bool DebugGetTrackLiveStatus() const { return _debug_tracked_instance != nullptr; }
private:
	Instance* _debug_tracked_instance = nullptr;
#endif
private:
#define Script(x) static void x(Instance*)
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
	Script(move_instant);
	Script(move_to_direction);
	Script(distance_to_point);
	Script(distance_between_point);
	Script(distance_to_instance);
	Script(move_forward);
	Script(direction_to_point);
	Script(direction_between_point);
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
	Script(scene_change_transmission);
	Script(scene_change);
	Script(get_direction_of);
	Script(instance_spawn);
	Script(instance_spawn_on_point);
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
	Script(draw_text);
	Script(convert_int_to_string);
	Script(convert_float_to_string);
	Script(string_add);
	Script(sprite_set_scale);
	Script(draw_text_in_frame);
	Script(gui_change_visibility);
	Script(gui_change_enabled);
	Script(code_execute_trigger);
	Script(code_wait);
	Script(game_exit);
	Script(get_instance_position);
	Script(get_instance_position_x);
	Script(get_instance_position_y);
	Script(instance_create_point);
	Script(instance_delete_other);
	Script(instance_find_by_tag);
	Script(instance_exists);
	Script(instance_alive);
	Script(scene_get_width);
	Script(scene_get_height);
	Script(collision_bounce);
#undef Script
};
//end of file
