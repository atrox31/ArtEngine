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
	FunctionsMap["move_instant"] = &CodeExecutor::move_instant;
	FunctionsMap["move_to_direction"] = &CodeExecutor::move_to_direction;
	FunctionsMap["distance_to_point"] = &CodeExecutor::distance_to_point;
	FunctionsMap["distance_between_point"] = &CodeExecutor::distance_between_point;
	FunctionsMap["distance_to_instance"] = &CodeExecutor::distance_to_instance;
	FunctionsMap["move_forward"] = &CodeExecutor::move_forward;
	FunctionsMap["direction_to_point"] = &CodeExecutor::direction_to_point;
	FunctionsMap["direction_between_point"] = &CodeExecutor::direction_between_point;
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
	FunctionsMap["instance_set_body_as_rect"] = &CodeExecutor::instance_set_body_as_rect;
	FunctionsMap["instance_set_body_as_circle"] = &CodeExecutor::instance_set_body_as_circle;
	FunctionsMap["instance_set_body_from_sprite"] = &CodeExecutor::instance_set_body_from_sprite;
	FunctionsMap["instance_set_body_none"] = &CodeExecutor::instance_set_body_none;
	FunctionsMap["instance_set_tag"] = &CodeExecutor::instance_set_tag;
	FunctionsMap["collision_get_collider"] = &CodeExecutor::collision_get_collider;
	FunctionsMap["collision_get_collider_tag"] = &CodeExecutor::collision_get_collider_tag;
	FunctionsMap["collision_get_collider_name"] = &CodeExecutor::collision_get_collider_name;
	FunctionsMap["collision_get_collider_id"] = &CodeExecutor::collision_get_collider_id;
	FunctionsMap["get_random"] = &CodeExecutor::get_random;
	FunctionsMap["get_random_range"] = &CodeExecutor::get_random_range;
	FunctionsMap["scene_change_transmission"] = &CodeExecutor::scene_change_transmission;
	FunctionsMap["scene_change"] = &CodeExecutor::scene_change;
	FunctionsMap["get_direction_of"] = &CodeExecutor::get_direction_of;
	FunctionsMap["instance_spawn"] = &CodeExecutor::instance_spawn;
	FunctionsMap["instance_spawn_on_point"] = &CodeExecutor::instance_spawn_on_point;
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
	FunctionsMap["sprite_set_scale"] = &CodeExecutor::sprite_set_scale;
	FunctionsMap["draw_text_in_frame"] = &CodeExecutor::draw_text_in_frame;
	FunctionsMap["gui_change_visibility"] = &CodeExecutor::gui_change_visibility;
	FunctionsMap["gui_change_enabled"] = &CodeExecutor::gui_change_enabled;
	FunctionsMap["code_execute_trigger"] = &CodeExecutor::code_execute_trigger;
	FunctionsMap["code_wait"] = &CodeExecutor::code_wait;
	FunctionsMap["game_exit"] = &CodeExecutor::game_exit;
	FunctionsMap["get_instance_position"] = &CodeExecutor::get_instance_position;
	FunctionsMap["get_instance_position_x"] = &CodeExecutor::get_instance_position_x;
	FunctionsMap["get_instance_position_y"] = &CodeExecutor::get_instance_position_y;
	FunctionsMap["instance_create_point"] = &CodeExecutor::instance_create_point;
	FunctionsMap["instance_delete_other"] = &CodeExecutor::instance_delete_other;
	FunctionsMap["instance_find_by_tag"] = &CodeExecutor::instance_find_by_tag;
	FunctionsMap["instance_exists"] = &CodeExecutor::instance_exists;
	FunctionsMap["instance_alive"] = &CodeExecutor::instance_alive;
	FunctionsMap["scene_get_width"] = &CodeExecutor::scene_get_width;
	FunctionsMap["scene_get_height"] = &CodeExecutor::scene_get_height;
	FunctionsMap["collision_bounce"] = &CodeExecutor::collision_bounce;
	FunctionsMap["gui_get_slider_value"] = &CodeExecutor::gui_get_slider_value;
	FunctionsMap["gui_get_check_box_value"] = &CodeExecutor::gui_get_check_box_value;
	FunctionsMap["gui_get_drop_down_selected_index"] = &CodeExecutor::gui_get_drop_down_selected_index;
	FunctionsMap["gui_get_drop_down_selected_value"] = &CodeExecutor::gui_get_drop_down_selected_value;
	FunctionsMap["gui_set_slider_value"] = &CodeExecutor::gui_set_slider_value;
	FunctionsMap["gui_set_slider_value_min"] = &CodeExecutor::gui_set_slider_value_min;
	FunctionsMap["gui_set_slider_value_max"] = &CodeExecutor::gui_set_slider_value_max;
	FunctionsMap["gui_set_slider_value_step"] = &CodeExecutor::gui_set_slider_value_step;
	FunctionsMap["gui_set_check_box_value"] = &CodeExecutor::gui_set_check_box_value;
	FunctionsMap["gui_set_drop_down_selected_index"] = &CodeExecutor::gui_set_drop_down_selected_index;
	FunctionsMap["system_set_video_mode"] = &CodeExecutor::system_set_video_mode;
	FunctionsMap["system_set_video_resolution"] = &CodeExecutor::system_set_video_resolution;
	FunctionsMap["system_set_video_resolution_from_string"] = &CodeExecutor::system_set_video_resolution_from_string;
	FunctionsMap["system_set_video_bloom_factor"] = &CodeExecutor::system_set_video_bloom_factor;
	FunctionsMap["system_set_audio_master"] = &CodeExecutor::system_set_audio_master;
	FunctionsMap["system_set_audio_music"] = &CodeExecutor::system_set_audio_music;
	FunctionsMap["system_set_audio_sound"] = &CodeExecutor::system_set_audio_sound;
	FunctionsMap["system_set_audio_music_level"] = &CodeExecutor::system_set_audio_music_level;
	FunctionsMap["system_set_audio_sound_level"] = &CodeExecutor::system_set_audio_sound_level;
	FunctionsMap["system_settings_data_get_int"] = &CodeExecutor::system_settings_data_get_int;
	FunctionsMap["system_settings_data_get_string"] = &CodeExecutor::system_settings_data_get_string;
	FunctionsMap["string_get_length"] = &CodeExecutor::string_get_length;
	FunctionsMap["string_join"] = &CodeExecutor::string_join;
	FunctionsMap["string_replace"] = &CodeExecutor::string_replace;
	FunctionsMap["convert_int_to_bool"] = &CodeExecutor::convert_int_to_bool;
	FunctionsMap["system_settings_data_save"] = &CodeExecutor::system_settings_data_save;
	FunctionsMap["system_settings_apply_audio"] = &CodeExecutor::system_settings_apply_audio;
	FunctionsMap["system_settings_apply_graphic"] = &CodeExecutor::system_settings_apply_graphic;
	FunctionsMap["math_sin"] = &CodeExecutor::math_sin;
	FunctionsMap["math_cos"] = &CodeExecutor::math_cos;
	FunctionsMap["math_abs"] = &CodeExecutor::math_abs;
	FunctionsMap["sprite_set_angle"] = &CodeExecutor::sprite_set_angle;
}
//end of file
