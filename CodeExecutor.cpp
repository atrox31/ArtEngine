#include "CodeExecutor.h"
#include "Func.h"
#include "Debug.h"

CodeExecutor::CodeExecutor()
{
	FunctionsMap = std::map<std::string, void(CodeExecutor::*)()>();
	FunctionsList = std::vector<void(CodeExecutor::*)()>() ;
	InstanceDefinitions = std::vector<InstanceDefinition>();
}

void CodeExecutor::MapFunctions()
{
	// map all functions to string
	FunctionsMap.clear();
	FunctionsMap["new_point"] = &CodeExecutor::new_point;
	FunctionsMap["new_direction"] = &CodeExecutor::new_direction;
	FunctionsMap["new_rectangle"] = &CodeExecutor::new_rectangle;
	FunctionsMap["new_rectangle_f"] = &CodeExecutor::new_rectangle_f;
	FunctionsMap["new_mask_from_sprite"] = &CodeExecutor::new_mask_from_sprite;
	FunctionsMap["get_sprite"] = &CodeExecutor::get_sprite;
	FunctionsMap["get_texture"] = &CodeExecutor::get_texture;
	FunctionsMap["get_music"] = &CodeExecutor::get_music;
	FunctionsMap["get_sound"] = &CodeExecutor::get_sound;
	FunctionsMap["get_font"] = &CodeExecutor::get_font;
	FunctionsMap["sprite_get_width"] = &CodeExecutor::sprite_get_width;
	FunctionsMap["sprite_get_height"] = &CodeExecutor::sprite_get_height;
	FunctionsMap["sprite_get_frames"] = &CodeExecutor::sprite_get_frames;
	FunctionsMap["sprite_set_animation_speed"] = &CodeExecutor::sprite_set_animation_speed;
	FunctionsMap["sprite_set_animation_loop"] = &CodeExecutor::sprite_set_animation_loop;
	FunctionsMap["move_to_point"] = &CodeExecutor::move_to_point;
	FunctionsMap["move_forward"] = &CodeExecutor::move_forward;
	FunctionsMap["move_instant"] = &CodeExecutor::move_instant;
	FunctionsMap["move_to_direction"] = &CodeExecutor::move_to_direction;
	FunctionsMap["distance_to_point"] = &CodeExecutor::distance_to_point;
	FunctionsMap["distance_beetwen_point"] = &CodeExecutor::distance_beetwen_point;
	FunctionsMap["distance_to_instance"] = &CodeExecutor::distance_to_instance;
	FunctionsMap["draw_sprite"] = &CodeExecutor::draw_sprite;
	FunctionsMap["draw_sprite_ex"] = &CodeExecutor::draw_sprite_ex;
	FunctionsMap["draw_texture"] = &CodeExecutor::draw_texture;
	FunctionsMap["draw_texture_ex"] = &CodeExecutor::draw_texture_ex;
	FunctionsMap["draw_sprite_self"] = &CodeExecutor::draw_sprite_self;
	FunctionsMap["draw_shape_rectangle"] = &CodeExecutor::draw_shape_rectangle;
	FunctionsMap["draw_shape_rectangle_r"] = &CodeExecutor::draw_shape_rectangle_r;
	FunctionsMap["draw_shape_rectangle_filled"] = &CodeExecutor::draw_shape_rectangle_filled;
	FunctionsMap["draw_shape_rectangle_filled_r"] = &CodeExecutor::draw_shape_rectangle_filled_r;
	FunctionsMap["draw_shape_circle"] = &CodeExecutor::draw_shape_circle;
	FunctionsMap["draw_shape_circle_p"] = &CodeExecutor::draw_shape_circle_p;
	FunctionsMap["draw_shape_circle_filled"] = &CodeExecutor::draw_shape_circle_filled;
	FunctionsMap["draw_shape_circle_filled_p"] = &CodeExecutor::draw_shape_circle_filled_p;
	FunctionsMap["math_min_i"] = &CodeExecutor::math_min_i;
	FunctionsMap["math_max_i"] = &CodeExecutor::math_max_i;
	FunctionsMap["math_min"] = &CodeExecutor::math_min;
	FunctionsMap["math_max"] = &CodeExecutor::math_max;
	FunctionsMap["global_get_mouse"] = &CodeExecutor::global_get_mouse;
	FunctionsMap["set_self_sprite"] = &CodeExecutor::set_self_sprite;
	FunctionsMap["get_pos_x"] = &CodeExecutor::get_pos_x;
	FunctionsMap["get_pos_y"] = &CodeExecutor::get_pos_y;

}

bool CodeExecutor::LoadArtLib()
{
	MapFunctions();
	char* buffer = Func::GetFileBuf("files/AScript.lib", nullptr);
	if (buffer == NULL) {
		return false;
	}
	std::string data = std::string(buffer);
	std::vector<std::string> lines = Func::Explode(data, '\n');
	
	if (lines.size() == 0) {
		return false;
	}

	FunctionsList.clear();
	FunctionsList.reserve(FunctionsMap.size());

	for (std::string& line : lines) {
		if (line.starts_with("//")) continue;
		//point new_point(float x,float y)
		
		// tokenize
		int phase = 0;
		std::string tmp = "";
		for (int i = 0; i < line.length(); i++) {
			switch (phase) {
			case 0:
				if (line[i] == ' ') {
					phase++;
					//ArtLib.back()._return = tmp;
					tmp = "";
				}
				else {
					tmp += line[i];
				}
				break;
			case 1:
				if (line[i] == '(') {
					phase++;
					//ArtLib.back()._name = tmp;  FunctionsList

					if (FunctionsMap.find(tmp) != FunctionsMap.end()) {
						FunctionsList.push_back(FunctionsMap[tmp]);
					}
					else {
						FunctionsList.push_back(nullptr);
						Debug::WARNING("function '"+ tmp+ "' not found");
					}

					tmp = "";
					break;
				}
				else {
					tmp += line[i];
				}
				break;
			}
		}
	}
	FunctionsList.shrink_to_fit();
	// free memory
	FunctionsMap.clear();
	return true;
}

#define command CodeExecutor::ArtCode::Command
#define err(x,y) Debug::ERROR("expected " + x + " but " + std::to_string(y) + " is given"); return false;



bool CodeExecutor::LoadObjectDefinitions()
{
	//CodeExecutor::ArtCode ac;
	Sint64 c = 0;
	const unsigned char* _code = Func::GetFileBytes("object_compile.acp", &c);
	if (_code == nullptr) return false;
	Inspector code(_code, c);

	// definicje objektów
	while (!code.IsEnd()) {
		// first is OBJECT_DEFINITION command
		if (code.GetNextCommand() != command::OBJECT_DEFINITION) {
			// last is end
			if (code.GetCurrentCommand() == command::END && code.IsEnd()) {
				break;
			}
			Debug::ERROR("expected 'OBJECT_DEFINITION' but " + std::to_string(code.Current()) + " is given");SDL_assert(false); return false;
		}

		InstanceDefinition instance;
		// seccond is string object name
		std::string o_name = code.GetString();
		instance._name = o_name;

		// varibles
		while (code.GetNextCommand() != command::END || code.IsEnd()) {
			/*
			 WriteCommand(Command::LOCAL_VARIBLE_DEFINITION);
			 WriteBit(getVaribleIndex(local.Type));
			 WriteBit(local.index);
			 WriteString(local.Name);
			 WriteBit(local.ReadOnly);
			* */
			if (code.GetCurrentCommand() == command::LOCAL_VARIBLE_DEFINITION) {
				int varible_type = code.GetBit();
				// not used now, maby later for debug
				int varible_index = code.GetBit();
				std::string varible_name = code.GetString();
				bool varible_read_only = (bool)code.GetBit();
				//
				instance.AddVarible(varible_type);
			}
			else {
				
				Debug::ERROR("instance: '"+ o_name+ "' - expected 'LOCAL_VARIBLE_DEFINITION' but " + std::to_string(code.Current()) + " is given");SDL_assert(false); return false;
			}
		} // varibles

		// events
		while (code.GetNextCommand() != command::END || code.IsEnd()) {
			if (code.GetCurrentCommand() == command::FUNCTION_DEFINITION) {
				std::string e_name = code.GetString();
				int f_size = code.GetBit();
				const unsigned char* f_code = code.GetChunk(f_size);
				if ((ArtCode::Command)f_code[f_size - 1] != command::END) {

					Debug::ERROR("instance: '" + o_name + "' - expected 'END' but " + std::to_string(f_code[f_size - 1]) + " is given"); SDL_assert(false); return false;
				}
				
				//instance._events
				if (Event_fromString(e_name) == Event::Invalid) {
					Debug::ERROR("instance: '" + o_name + "' - wrong event '" + e_name + "'"); SDL_assert(false); return false;
				}

				instance._events[Event_fromString(e_name)] = f_code;
			}
			else {
				Debug::ERROR("instance: '" + o_name + "' - expected 'FUNCTION_DEFINITION' but " + std::to_string(code.Current()) + " is given"); SDL_assert(false); return false;
			}
		} //ebvents

		// expected end
		if (code.GetCurrentCommand() != command::END) {
			
			Debug::ERROR("expected 'END' but " + std::to_string(code.Current()) + " is given");SDL_assert(false); return false;
		}

		// all ok, populate
		InstanceDefinitions.push_back(instance);
	}

	return (InstanceDefinitions.size() > 0);
}
#undef command