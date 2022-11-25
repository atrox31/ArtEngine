#pragma once
#include "SDL_version.h"
#include SDL2_INCLUDE_DIR
#include <string>
#include "ArtCode.h"


class  Inspector {
public:
	Inspector(const unsigned char* code, Sint64 len) {
		_code = code;
		_size = len - 1;
		_pos = -1;
		_current_bit = '\0';
	}

	virtual ~Inspector() {
		//delete _code;
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

		unsigned char* _return = (unsigned char*)malloc((size_t)(count + 1));
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