	point new_point(float x, float y)
Make point (<float>, <float>).;New point from value or other.<br>
<br>
	float new_direction(point from, point to)
Make direction from <point> to <point>.;Value are from 0 to 359.<br>
<br>
	Rectangle new_rectangle(int x1, int y1, int x2, int y2)
Make Rectangle from <int>, <int> to <int>, <int>.;This is const Rectangle, not width and height.<br>
<br>
	Rectangle new_rectangle_f(float x1, float y1, float x2, float y2)
Make Rectangle from <float>, <float> to <float>, <float>.;This is const Rectangle, not width and height.<br>
<br>
	sprite get_sprite(string name)
Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.<br>
<br>
	texture get_texture(string name)
Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.<br>
<br>
	music get_music(string name)
Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.<br>
<br>
	sound get_sound(string name)
Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.<br>
<br>
	font get_font(string name)
Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.<br>
<br>
	int sprite_get_width(sprite spr)
Get width of <sprite>;Get int value.<br>
<br>
	int sprite_get_height(sprite spr)
Get height of <sprite>;Get int value.<br>
<br>
	int sprite_get_frames(sprite spr)
Get _frames number of <sprite>;Get int value.<br>
<br>
	null sprite_set_animation_speed(float speed)
Set animation value <float> _frames per second;Every sprite can have own animation speed<br>
<br>
	null sprite_set_animation_loop(bool loop)
Set animation loop value <bool>;Every animation end generate event ON_ANIMATION_END<br>
<br>
	null move_to_point(point p, float speed)
Move current instance to <point> with <speed> px per second.;Call it every frame.<br>
<br>
	null move_instant(point p)
Move instantly to target <point>;This changes x and y. Not cheking for collision;<br>
<br>
	null move_to_direction(float direction, float speed)
Move instance toward direction of <float> (0-359) with <float> speed px per seccond;If direction is not in range its clipped to 360.<br>
<br>
	float distance_to_point(point p)
Give distance to <point>;Measure from current instance to target point.<br>
<br>
	float distance_between_point(point p1, point p2)
Give distance from <point> to <point>;Measure distance.<br>
<br>
	float distance_to_instance(instance i)
Give distance to <instance> instance;Measure from current instance to target point. If target not exists return 0<br>
<br>
	null move_forward(float speed)
Move current instance forward with <speed> px per second.;Call it every frame. Function give build-in direction variable.<br>
<br>
	float direction_to_point(point p)
Give direction to <point> in degree (-180 : 180);Measure from current instance to target point.<br>
<br>
	float direction_between_point(point p1, point p2)
Give direction from <point> to <point> in degree (-180 : 180);Measure distance.<br>
<br>
	float direction_to_instance(instance i)
Give direction to <instance> in degree (-180 : 180);Measure from current instance to target point. If target not exists return own direction<br>
<br>
	null draw_sprite(sprite spr, float x, float y, float frame)
Draw <sprite> on location (<float>,<float>) with target frame <frame>;Draw default sprite. To more options use draw_sprite_ex<br>
<br>
	null draw_sprite_ex(sprite spr, float x, float y, float frame, float x_scale, float y_scale, float x_center, float y_center, float angle, float alpha)
<br>
<br>
	null draw_texture(texture tex, float x, float y)
Draw <texture> on (<float>,<float>).;Draw standard texture with it normal dimensions. For extended option use 'Draw texture extended';<br>
<br>
	null draw_texture_ex(texture tex, float x, float y, float x_scale, float y_scale, float angle, float alpha)
Draw <texture> on (<float>,<float>), with scale (<float>,<float>), angle <float> and aplha <float>;Angle range is (0 - 359) alpha (0.0f - 1.0f).<br>
<br>
	null draw_sprite_self()
Draw self sprite on self coords with sprite scale and angle;Use build-in variables;<br>
<br>
	null draw_shape_rectangle(float x1, float y2, float x2, float y2, color color)
Draw frame of Rectangle from (<float>,<float>) to (<float>,<float>) with color <color>.;Draw Rectangle on final coords;<br>
<br>
	null draw_shape_rectangle_r(Rectangle rect, color color)
Draw frame of <Rectangle> with color <color>.;Draw Rectangle;<br>
<br>
	null draw_shape_rectangle_filled(float x1, float y2, float x2, float y2, color color)
Draw filled of Rectangle from (<float>,<float>) to (<float>,<float>) with color <color>.;Draw Rectangle on final coords;<br>
<br>
	null draw_shape_rectangle_filled_r(Rectangle rect, color color)
Draw filled <Rectangle> with color <color>.;Draw Rectangle;<br>
<br>
	null draw_shape_circle(float x, float y, float radius, color color)
Draw Circle in point (<float>,<float>) with radius <float> and color <color>;<br>
<br>
	null draw_shape_circle_p(point p, float radius, color color)
Draw Circle in point <point> with radius <float> and color <color>;<br>
<br>
	null draw_shape_circle_filled(float x, float y, float radius, color color)
Draw filled Circle in point (<float>,<float>) with radius <float> and color <color>;<br>
<br>
	null draw_shape_circle_filled_p(point p, float radius, color color)
Draw filled Circle in point <point> with radius <float> and color <color>;<br>
<br>
	int math_min_i(int a, int b)
Get minimum value from <int> or <int>;<br>
<br>
	int math_max_i(int a, int b)
Get maximum value from <int> or <int>;<br>
<br>
	float math_min(float a, float b)
Get minimum value from <float> or <float>;<br>
<br>
	float math_max(float a, float b)
Get maximum value from <float> or <float>;<br>
<br>
	point global_get_mouse()
Get point of current mouse postion;If map is bigger than screen this give map coords not screen;<br>
<br>
	null set_self_sprite(sprite spr)
 Set self sprite to <sprite> with default scale, angle, speed, loop; You can mod sprite via set_sprite_ etc.;<br>
<br>
	float get_pos_x()
 Get x coords of instance;<br>
<br>
	float get_pos_y()
 Get y coords of instance;<br>
<br>
	null sound_play(sound asset)
Play <asset> sound global;For position call sound_play_at(sound asset)<br>
<br>
	null music_play(music asset)
Play <asset> music.;There is only one music at once;<br>
<br>
	null sprite_next_frame()
 Set SelfSprite next frame; If sprite loop is enable, frame = 0 if frame > frame_max;<br>
<br>
	null sprite_prev_frame()
 Set SelfSprite previous frame; If sprite loop is enable, frame = frame_max if frame < frame_max 0;<br>
<br>
	null sprite_set_frame(int frame)
 Set SelfSprite frame no <int>.; If frame is not exists nothing happen;<br>
<br>
	null code_do_nothing()
Do nothing, empty action;Use when there is no else in if<br>
<br>
	null instance_set_body_as_rect(int width, int height)
Make body for instance to enable collision check. Make body rectangle with <int> width and <int> height;Mark instance as collider.<br>
<br>
	null instance_set_body_as_circle(int radius)
Make body for instance to enable collision check. Make body circle with <int> width radius;Mark instance as collider.<br>
<br>
	null instance_set_body_from_sprite()
Make body for instance to enable collision check. Copy body value from self sprite if is set;Mark instance as collider.<br>
<br>
	null instance_set_body_none()
Delete instance body, make it non collider;<br>
<br>
	null instance_set_tag(string tag)
Set tag for current instance <string>.<br>
<br>
	instance collision_get_collider()
Return reference to instance with this object is collide;Other collider must be solid too to collide;<br>
<br>
	string collision_get_collider_tag()
Get tag of instance that is colliding with this object;Other collider must be solid too to collide;<br>
<br>
	string collision_get_collider_name()
Get name of instance that is colliding with this object;Other collider must be solid too to collide;<br>
<br>
	int collision_get_collider_id()
Get id of instance that is colliding with this object;Other collider must be solid too to collide;<br>
<br>
	int get_random(int max)
Get random value [0,<int>).;0 is include max is exclude;<br>
<br>
	int get_random_range(int min, int max)
Get random value [<int>,<int>).;0 is include max is exclude;<br>
<br>
	null scene_change_transmission(string scene, string transmission)
[NOT_IMPLEMENTED_YET];0;<br>
<br>
	null scene_change(string scene)
Change scene to <scene>;This is quick change, for transmission use scene_change_transmission[NOT_IMPLEMENTED_YET];<br>
<br>
	null scene_set_level(int level)
Set current level to <int> and reload scene;<br>
<br>
	null scene_go_to_next_level()
Load next level if exists;<br>
<br>
	bool scene_have_next_level()
Get answer if next level exists;<br>
<br>
	float get_direction_of(instance target)
Return direction of <instance> instance in degree (-180 : 180);Use with collision_get_collider, if target not exists return own direction<br>
<br>
	instance instance_spawn(string name, float x, float y)
Spawn object <string> at (<float>,<float>) and return reference to it;Ypu can use reference to pass arguments;<br>
<br>
	instance instance_spawn_on_point(string name, point xy)
Spawn object <string> at (<point>) and return reference to it;Ypu can use reference to pass arguments;<br>
<br>
	null instance_create(string name, float x, float y)
Spawn object <string> at (<float>,<float>) in current scene;This not return reference;<br>
<br>
	null set_direction_for_target(instance target, float direction)
Set <instance> direction to <float> value in degree (-180 : 180);You can get reference from id of instance<br>
<br>
	null set_direction(float direction)
Set current direction to <float> in degree (-180 : 180);<br>
<br>
	float convert_int_to_float(int value)
Convert <int> to float type;<br>
<br>
	int convert_float_to_int(float value)
Convert <float> to int type;<br>
<br>
	null instance_delete_self()
Delete self;<br>
<br>
	float get_direction()
Get current direction;<br>
<br>
	float math_add(float a, float b)
Get sum of <float> + <float>;<br>
<br>
	float math_sub(float a, float b)
Get sub of <float> - <float>;<br>
<br>
	float math_mul(float a, float b)
Get mul of <float> * <float>;<br>
<br>
	float math_div(float a, float b)
Get div of <float> / <float>;<br>
<br>
	float get_point_x(point point)
Get x of <point> point;<br>
<br>
	float get_point_y(point point)
Get y of <point> point;<br>
<br>
	null collision_push_other(bool myself)
Push other instance in other direction, if <bool> then push this instance too;Like opposite magnets;<br>
<br>
	bool mouse_is_pressed(int button)
Return state of button <int>;Left button is 1, right is 3<br>
<br>
	float get_delta_time()
Return delta time of frame.;Every build-in action of moving or collision uses delta time, do not use twice!<br>
<br>
	null code_break()
 Break from current function; Everything will be lost...<br>
<br>
	null draw_text(font font, int x, int y, string text, color color)
Use <font> and draw <string> on screen on (<int>,<int>) with <color> color;If font is null, default font is used;<br>
<br>
	string convert_int_to_string(int input)
Convert <int> to string;<br>
<br>
	string convert_float_to_string(int input)
Convert <int> to string;<br>
<br>
	null sprite_set_scale(point scale)
Set scale for self sprite <point>;Scale is from point (width, height)<br>
<br>
	null draw_text_in_frame(font font, string text, float x, float y, color text_color, color frame_color, color background_color)
Using <font> draw <text> in frame at (<float>,<float>) with <color>. Frame color is <color> and background <color>;<br>
<br>
	null gui_change_visibility(string guiTag, bool visible)
Change visibility variable for <string> to <bool>;Give path to element panel1/button1 by tags. Root is in default first tag do not include it<br>
<br>
	null gui_change_enabled(string guiTag, bool enable)
Change enabled variable for <string> to <bool>;Give path to element panel1/button1 by tags. Root is in default first tag do not include it<br>
<br>
	null code_execute_trigger(string trigger)
Execute trigger <string> on current scene;Create this triggers in scene editor.<br>
<br>
	null code_wait(int milliseconds)
Suspend this trigger for <int> milliseconds.;This suspend future execution of this trigger until time have pass.<br>
<br>
	null game_exit()
Exit the game;<br>
<br>
	point get_instance_position(instance instance)
Get position of <instance>;<br>
<br>
	float get_instance_position_x(instance instance)
Get position X of <instance>;<br>
<br>
	float get_instance_position_y(instance instance)
Get position Y of <instance>;<br>
<br>
	null instance_create_point(string name, point xy)
Spawn object <string> at (<point>) in current scene;This not return reference;<br>
<br>
	null instance_delete_other(instance instance)
Delete <instance>;<br>
<br>
	instance instance_find_by_tag(string tag)
Find instance by tag: <string>;All tags must be unique, else returned instance is first found;<br>
<br>
	instance instance_find_nearest_by_tag(string tag)
Find nearest instance by tag: <string>;<br>
<br>
	instance instance_find_nearest_by_name(string name)
Find nearest instance by name: <string>;<br>
<br>
	bool instance_exists(instance instance)
Get current status of <instance><br>
<br>
	bool instance_alive(instance instance)
Get current status of <instance><br>
<br>
	int scene_get_width()
Get scene width;<br>
<br>
	int scene_get_height()
Get scene height;<br>
<br>
	null collision_bounce()
Change direction of this object, bounce of other.;This trigger works only in OnCollision event!<br>
<br>
	int gui_get_slider_value(string gui_tag)
Get exact value from slider with <string> tag;Tag must be unique. Values are between min and max and rounded to step<br>
<br>
	bool gui_get_check_box_value(string gui_tag)
Get checked value from check box with <string> tag;Tag must be unique. Values are true or false<br>
<br>
	int gui_get_drop_down_selected_index(string gui_tag)
Get selected index of drop down gui element with tag <string>;Index is 0-based<br>
<br>
	int gui_get_drop_down_selected_value(string gui_tag)
Get selected value of drop down gui element with tag <string>;Value may be empty if nothing is selected<br>
<br>
	null gui_set_slider_value( int value, string gui_tag)
Set exact value <int> for slider with <string> tag;Tag must be unique. Values are between min and max and rounded to step<br>
<br>
	null gui_set_slider_value_min( int value, string gui_tag)
Set minimum value <int> for slider with <string> tag;Tag must be unique. Values are between min and max and rounded to step<br>
<br>
	null gui_set_slider_value_max( int value, string gui_tag)
Set maximum value <int> for slider with <string> tag;Tag must be unique. Values are between min and max and rounded to step<br>
<br>
	null gui_set_slider_value_step( int value, string gui_tag)
Set step value <int> for slider with <string> tag;Tag must be unique. Values are between min and max and rounded to step<br>
<br>
	null gui_set_check_box_value(bool value, string gui_tag)
Set checked property of <bool> for check box with <string> tag;Tag must be unique. Values are true or false<br>
<br>
	null gui_set_drop_down_selected_index(int index, string gui_tag)
Set selected index <int> for drop down list with tag <string>;Selected index is checked if can be selected<br>
<br>
	null system_set_video_mode(bool full_screen)
Set full screen to <bool>;<br>
<br>
	null system_set_video_resolution(int width, int height)
Set window resolution to <int> x <int>;<br>
<br>
	null system_set_video_resolution_from_string(string value)
Set window resolution from <string> text;Text must be 0000x0000 (width x height)<br>
<br>
	null system_set_video_bloom_factor(int mode)
Set mode for bloom post process to <int>;0-off, 1-low, 2-medium, 3-high;<br>
<br>
	null system_set_audio_master(bool mode)
Set audio mode for master to <bool>; True means sounds can be played, have higher priority than other audio modes<br>
<br>
	null system_set_audio_music(bool mode)
Set audio mode for music to <bool>; True means sounds can be played<br>
<br>
	null system_set_audio_sound(bool mode)
Set audio mode for sound to <bool>; True means sounds can be played<br>
<br>
	null system_set_audio_music_level(int level)
Set audio level to <int> percent;Level can be from 0 to 100;<br>
<br>
	null system_set_audio_sound_level(int level)
Set audio level to <int> percent;Level can be from 0 to 100;<br>
<br>
	int system_settings_data_get_int(string setting)
Get value of settings data name <string>;On error return -1<br>
<br>
	string system_settings_data_get_string(string setting)
Get value of settings data name <string>;On error return empty string ""<br>
<br>
	int string_get_length(string text)
Get length of target <string> text;<br>
<br>
	string string_join(string str1, string str2)
Create new string from <string> and <string>;<br>
<br>
	string string_replace(string target, string search, string replace)
Target text: <string>.\nSearch <string> and replace with <string>;<br>
<br>
	bool convert_int_to_bool(int input)
Convert <int> to bool. Only 1 is true, rest is false;<br>
<br>
	bool system_settings_data_save()
Save all user system data to archive;<br>
<br>
	null system_settings_apply_audio()
Apply audio settings<br>
<br>
	null system_settings_apply_graphic()
Apply graphic settings<br>
<br>
	float math_sin(float x)
Get sin of <float>;<br>
<br>
	float math_cos(float x)
Get cos of <float>;<br>
<br>
	float math_abs(float value)
Get absolute value of <float><br>
<br>
	null sprite_set_angle(float value)
Set self sprite angle <float> in degree;Get (0..359)<br>
<br>
	instance self()
Get self reference<br>
<br>
	int scene_count_instance_by_tag(string tag)
Count instances in current scene by <string> tag;<br>
<br>
	int scene_count_instance_by_name(string name)
Count instances in current scene by <string> name;<br>
<br>