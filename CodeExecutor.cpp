#include "CodeExecutor.h"
#include "Func.h"
#include "Debug.h"

CodeExecutor::CodeExecutor()
{
	//FunctionsMap = std::map<std::string, void(CodeExecutor::*)()>();
	//FunctionsList = std::vector<void(CodeExecutor::*)()>() ;
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

}

bool CodeExecutor::LoadArtLib()
{
	char* buffer = Func::GetFileBuf("AScript.lib", nullptr);
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
