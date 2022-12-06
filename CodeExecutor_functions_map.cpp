#include "CodeExecutor.h"
//this file is auto generated from ArtLibGenerator
//do not edit anything bellow
void CodeExecutor::MapFunctions()
{
	FunctionsMap.clear();
	FunctionsMap["new_point"] = &CodeExecutor::new_point;
	FunctionsMap["new_direction"] = &CodeExecutor::new_direction;
	FunctionsMap["new_rectangle"] = &CodeExecutor::new_rectangle;
	FunctionsMap["new_rectangle_f"] = &CodeExecutor::new_rectangle_f;
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
	FunctionsMap["direction_to_point"] = &CodeExecutor::direction_to_point;
	FunctionsMap["direction_beetwen_point"] = &CodeExecutor::direction_beetwen_point;
	FunctionsMap["direction_to_instance"] = &CodeExecutor::direction_to_instance;
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
	FunctionsMap["sound_play"] = &CodeExecutor::sound_play;
	FunctionsMap["music_play"] = &CodeExecutor::music_play;
	FunctionsMap["sprite_next_frame"] = &CodeExecutor::sprite_next_frame;
	FunctionsMap["sprite_prev_frame"] = &CodeExecutor::sprite_prev_frame;
	FunctionsMap["sprite_set_frame"] = &CodeExecutor::sprite_set_frame;
	FunctionsMap["code_do_nothing"] = &CodeExecutor::code_do_nothing;
	FunctionsMap["set_body_type"] = &CodeExecutor::set_body_type;
	FunctionsMap["instance_set_tag"] = &CodeExecutor::instance_set_tag;
	FunctionsMap["collision_get_collider"] = &CodeExecutor::collision_get_collider;
	FunctionsMap["collision_get_collider_tag"] = &CodeExecutor::collision_get_collider_tag;
	FunctionsMap["collision_get_collider_name"] = &CodeExecutor::collision_get_collider_name;
	FunctionsMap["collision_get_collider_id"] = &CodeExecutor::collision_get_collider_id;
	FunctionsMap["get_random"] = &CodeExecutor::get_random;
	FunctionsMap["get_random_range"] = &CodeExecutor::get_random_range;
	FunctionsMap["scene_change_transmision"] = &CodeExecutor::scene_change_transmision;
	FunctionsMap["scene_change"] = &CodeExecutor::scene_change;
	FunctionsMap["scene_reset"] = &CodeExecutor::scene_reset;
	FunctionsMap["get_direction_of"] = &CodeExecutor::get_direction_of;
	FunctionsMap["instance_spawn"] = &CodeExecutor::instance_spawn;
	FunctionsMap["instance_create"] = &CodeExecutor::instance_create;
	FunctionsMap["set_direction_for_target"] = &CodeExecutor::set_direction_for_target;
	FunctionsMap["set_direction"] = &CodeExecutor::set_direction;
	FunctionsMap["convert_int_to_float"] = &CodeExecutor::convert_int_to_float;
	FunctionsMap["convert_float_to_int"] = &CodeExecutor::convert_float_to_int;
	FunctionsMap["instance_delete_self"] = &CodeExecutor::instance_delete_self;
	FunctionsMap["get_direction"] = &CodeExecutor::get_direction;
	FunctionsMap["math_add"] = &CodeExecutor::math_add;
	FunctionsMap["math_sub"] = &CodeExecutor::math_sub;
	FunctionsMap["math_mul"] = &CodeExecutor::math_mul;
	FunctionsMap["math_div"] = &CodeExecutor::math_div;
	FunctionsMap["get_point_x"] = &CodeExecutor::get_point_x;
	FunctionsMap["get_point_y"] = &CodeExecutor::get_point_y;
	FunctionsMap["collision_push_other"] = &CodeExecutor::collision_push_other;
	FunctionsMap["mouse_is_pressed"] = &CodeExecutor::mouse_is_pressed;
	FunctionsMap["get_delta_time"] = &CodeExecutor::get_delta_time;
	FunctionsMap["code_break"] = &CodeExecutor::code_break;
	FunctionsMap["draw_text"] = &CodeExecutor::draw_text;
	FunctionsMap["convert_int_to_string"] = &CodeExecutor::convert_int_to_string;
	FunctionsMap["convert_float_to_string"] = &CodeExecutor::convert_float_to_string;
	FunctionsMap["string_add"] = &CodeExecutor::string_add;
	FunctionsMap["sprite_set_scale"] = &CodeExecutor::sprite_set_scale;
	FunctionsMap["direction_from_deegree"] = &CodeExecutor::direction_from_deegree;
	FunctionsMap["direction_from_radians"] = &CodeExecutor::direction_from_radians;
	FunctionsMap["draw_text_in_frame"] = &CodeExecutor::draw_text_in_frame;
}
//end of file
