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
		std::map<Event, const unsigned char*> _events;
		//		type, fields
		std::map<int, int> _varibles;
		std::string _name;
		InstanceDefinition() {
			_name = "";
			_varibles = std::map<int, int>();
			_events = std::map<Event, const unsigned char*>();
		}
		void AddVarible(int type) {
			auto f = _varibles.find(type);
			if (f == _varibles.end()) {
				_varibles.insert(std::pair(type, 0));
			}
			else {
				f->second++;
			}
		}
	};
	std::vector< InstanceDefinition > InstanceDefinitions;
	InstanceDefinition* FindInstance(std::string name) {
		for (auto& it : InstanceDefinitions) {
			if (it._name == name) return &it;
		}
		return nullptr;
	}

private:
	std::map<std::string, void(CodeExecutor::*)()> FunctionsMap;
	std::vector<void(CodeExecutor::*)()> FunctionsList;

	struct ArtCode {
	public:
		enum class Command {
			// definicje
			OBJECT_DEFINITION,
			FUNCTION_DEFINITION,
			LOCAL_VARIBLE_DEFINITION,
			TYPEDEF,
			// odwo³ania
			OBJECT,
			OBJECT_VARIBLE,
			LOCAL_VARIBLE,
			VALUE, NULL_VALUE,
			FUNCTION,
			// operatory
			SET,
			OPERATOR,
			// polecenia
			TYPE, IF_BODY, IF_TEST, ELSE,
			END,


			// tylko w silniku, zawsze ostatnia
			INWALID
		};

		// not used now
		const std::string operators[6] = {
		"+",
		"-",
		"*",
		"/",
		"!",
		"."
		};

		const std::string operators2[8] = {
		//logic
		"||",
		"&&",
		"<<",
		">>",
		">=",
		"<=",
		"!=",
		"=="
		};

		const std::string varible_type[15] = {
		"null",
		"int",
		"float",
		"bool",
		"instance",
		"object",
		"sprite",
		"texture",
		"sound",
		"music",
		"font",
		"point",
		"rectangle",
		"color",
		"string"
		};

		const std::string keywords[4] = {
		"set",
		"if",
		"end",
		"else",
		};
	};

	class  Inspector {
	public:
		Inspector(const unsigned char* code, Sint64 len) {
			_code = code;
			_size = len - 1;
			_pos = -1;
			_current_bit = '\0';
		}

		virtual ~Inspector() {
			delete _code;
		}

		bool IsEnd() {
			return (_pos >= _size);
		}

		std::string GetString() {
			_pos++;
			std::string _string = "";
			while (_pos < _size) {
				if (IsEnd()) return "";
				if (_code[_pos] == '\1') {
					//_pos++;
					_current_bit = _code[_pos];
					return _string;
				}
				_string += _code[_pos++];
			}
			return "";
		}

		// skip x bytes and return isEnd()
		bool Skip(int count) {
			_pos += count;
			if (IsEnd()) {
				_pos = _size;
				return true;
			}
			return false;
		}

		ArtCode::Command GetNextCommand() {
			_current_bit = _code[++_pos];
			return (ArtCode::Command)_code[_pos];
		}
		
		ArtCode::Command GetCurrentCommand() {
			_current_bit = _code[_pos];
			return (ArtCode::Command)_code[_pos];
		}

		unsigned char Current() {
			if (_pos < _size) return _code[_pos];
			return '\0';
		}

		unsigned char SeekNext() {
			if (_pos + 1 <= _size) return _code[_pos + 1];
			return '\0';
		}
		ArtCode::Command SeekNextCommand() {
			if (_pos + 1 <= _size) return  (ArtCode::Command)_code[_pos + 1];
			return ArtCode::Command::INWALID;
		}

		const unsigned char* GetChunk(const int count) {
			
			unsigned char* _return = (unsigned char* )malloc(count+1);
			memcpy_s(_return, count, _code + _pos + 1, count);
			if (_return) {
				_return[count] = '\0';
			}
			Skip(count);
			return _return;

		}

		unsigned char GetBit() {
			_current_bit = _code[++_pos];
			return _code[_pos];
		}

		int GetInt() {
			return std::stoi(GetString());
		}

		float GetFloat() {
			return std::stof(GetString());
		}

	private:
		const unsigned char* _code;
		Sint64 _size;
		Sint64 _pos;
		unsigned char _current_bit;
	};

private:
	// makers
		//point new_point(float x, float y);Make point (<float>, <float>).;New point from value or other.
		void new_point();
		//float new_direction(point from, point to);Make direction from <point> to <point>.;Value are from 0 to 359.
		void new_direction();
		//rectangle new_rectangle(int x1, int y1, int x2, int y2);Make rectangle from <int>, <int> to <int>, <int>.;This is const rectangle, not width and height.
		void new_rectangle();
		//rectangle new_rectangle_f(float x1, float y1, float x2, float y2);Make rectangle from <float>, <float> to <float>, <float>.;This is const rectangle, not width and height.
		void new_rectangle_f();
		//rectangle new_mask_from_sprite(sprite spr);Make rectangle mask from <sprite>;Cirkle mask is not supported yet.
		void new_mask_from_sprite();
	// assets
		//sprite get_sprite(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
		void get_sprite();
		//texture get_texture(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
		void get_texture();
		//music get_music(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
		void get_music();
		//sound get_sound(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
		void get_sound();
		//font get_font(string name);Get asset handle by name <string>;Expensive function, try to not call it every frame. Call it to function and store.
		void get_font();
	// sprite
		//int sprite_get_width(sprite spr);Get width of <sprite>;Get int value.
		void sprite_get_width();
		//int sprite_get_height(sprite spr);Get height of <sprite>;Get int value.
		void sprite_get_height();
		//int sprite_get_frames(sprite spr);Get frames number of <sprite>;Get int value.
		void sprite_get_frames();
		//null sprite_set_animation_speed(sprite spr, float speed);Set animation speed for <sprite> value <float> frames per seccond;Every sprite can have own animation speed
		void sprite_set_animation_speed();
		//null sprite_set_animation_loop(sprite spr, bool loop);Set animation loop for <sprite> value <bool>;Every animation end generate event ON_ANIMATION_END
		void sprite_set_animation_loop();
	// moving
		//null move_to_point(point p, float speed);Move current instance to <point> with <speed> px per seccond.;Call it every frame.
		void move_to_point();
		//null move_forward(float speed);Move current instance forward with <speed> px per seccond.;Call it every frame. Function give build-in direction varible.
		void move_forward();
		//null move_instant(point p);Move instantly to target <point>;This changes x and y. Not cheking for collision;
		void move_instant();
		//null move_to_direction(float direction, float speed);Move instance toward direction of <float> (0-359) with <float> speed px per seccond;If direction is not in range its clipped to 360.
		void move_to_direction();
	// mesurment
		//float distance_to_point(point p);Give distance to <point>;Measure from current instance to target point.
		void distance_to_point();
		//float distance_beetwen_point(point p1, point p2);Give distance from <point> to <point>;Measure distance.
		void distance_beetwen_point();
		//float distance_to_instance(instance i);Give distance to <point>;Measure from current instance to target point.
		void distance_to_instance();
	// drawing
		//null draw_sprite(sprite spr, float x, float y, float frame);Draw <sprite> on location (<float>,<float>) with target frame <frame>;Draw default sprite. To more options use draw_sprite_ex
		void draw_sprite();
		//null draw_sprite_ex(sprite spr, float x, float y, float frame, float x_scale, float y_scale, float angle, float alpha);Draw <sprite> on location (<float>,<float>) with target frame <frame>;Draw sprite.
		void draw_sprite_ex();
		//null draw_texture(texture tex, float x, float y);Draw <texture> on (<float>,<float>).;Draw standard texture with it normal dimensions. For extended option use 'Draw texture extended';
		void draw_texture();
		//null draw_texture_ex(texture tex, float x, float y, float x_scale, float y_scale, float angle, float alpha);Draw <texture> on (<float>,<float>), with scale (<float>,<float>), angle <float> and aplha <float>;Angle range is (0 - 359) alpha (0.0f - 1.0f).
		void draw_texture_ex();
		//null draw_sprite_self();Draw self sprite on self coords with sprite scale and angle;Use build-in varibles;
		void draw_sprite_self();
	// draw shapes
		//null draw_shape_rectangle(float x1, float y2, float x2, float y2, color color);Draw frame of rectangle from (<float>,<float>) to (<float>,<float>) with color <color>.;Draw rectangle on final coords;
		void draw_shape_rectangle();
		//null draw_shape_rectangle_r(rectangle rect, color color);Draw frame of <rectangle> with color <color>.;Draw rectangle;
		void draw_shape_rectangle_r();
		//null draw_shape_rectangle_filled(float x1, float y2, float x2, float y2, color color);Draw filled of rectangle from (<float>,<float>) to (<float>,<float>) with color <color>.;Draw rectangle on final coords;
		void draw_shape_rectangle_filled();
		//null draw_shape_rectangle_filled_r(rectangle rect, color color);Draw filled <rectangle> with color <color>.;Draw rectangle;
		void draw_shape_rectangle_filled_r();
		//null draw_shape_circle(float x, float y, float radius, color color);Draw circle in point (<float>,<float>) with radius <float> and color <color>;
		void draw_shape_circle();
		//null draw_shape_circle_p(point p, float radius, color color);Draw circle in point <point> with radius <float> and color <color>;
		void draw_shape_circle_p();
		//null draw_shape_circle_filled(float x, float y, float radius, color color);Draw filled circle in point (<float>,<float>) with radius <float> and color <color>;
		void draw_shape_circle_filled();
		//null draw_shape_circle_filled_p(point p, float radius, color color);Draw filled circle in point <point> with radius <float> and color <color>;
		void draw_shape_circle_filled_p();
	// math
		//int math_min_i(int a, int b);Get minimum value from <int> or <int>;
		void math_min_i();
		//int math_max_i(int a, int b);Get maximum value from <int> or <int>;
		void math_max_i();
		//float math_min(float a, float b);Get minimum value from <float> or <float>;
		void math_min();
		//float math_max(float a, float b);Get maximum value from <float> or <float>;
		void math_max();
	// globals
		//point global_get_mouse();Get point of current mouse postion;If map is bigger than screen this give map coords not screen;
		void global_get_mouse();
	// set
		//null set_self_sprite(sprite spr); Set self sprite to <sprite> with default scale, angle, speed, loop; You can mod sprite via set_sprite_ etc.;
		void set_self_sprite(); 
		//float get_pos_x(); Get x coords of instance;
		void get_pos_x();
		//float get_pos_y(); Get y coords of instance;
		void get_pos_y(); 
};

