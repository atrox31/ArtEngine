#pragma once
#include <iostream>
#include <string>
#include "ArtCode.h"
#include "ArtCore/Functions/Func.h"
#include "ArtCore/_Debug/Debug.h"


class Inspector final
{
public:
#ifndef _DEBUG
	Inspector(const unsigned char* code, Sint64 len) {
#else
	std::string DebugGetFName() {
		return _f_name;
	}

	[[nodiscard]] int DebugGetCurrentPos() const
	{
		return (int)_pos;
	}

	Inspector(const unsigned char* code, const Sint64 len, const std::string& fname) {
		_f_name = fname;
#endif
		_code = code;
		_size = len - 1;
		_pos = -1;
		_current_bit = '\0';
	}

	virtual ~Inspector() {
		//delete _code;
		Debug::NOTE_DEATH(" ~Inspector");
	}

	[[nodiscard]] Sint64 GetLength() const
	{
		return _size;
	}

	[[nodiscard]] Sint64 GetPosition() const
	{
		return _pos;
	}

	[[nodiscard]] Sint64 GetRemains() const
	{
		if (IsEnd()) return 0;
		return _size - _pos;
	}

	[[nodiscard]] bool IsEnd() const
	{
		return (_pos >= _size);
	}

	std::string GetString() {
		_pos++;
		std::string string;
		while (_pos < _size) {
			if (IsEnd()) return "";
			if (_code[_pos] == '\1') {
				//_pos++;
				_current_bit = _code[_pos];
				return string;
			}
			string += _code[_pos++];
		}
		return "";
	}

	// skip x bytes and return isEnd()
	bool Skip(const int count) {
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

	[[nodiscard]] unsigned char Current() const
	{
		if (_pos < _size) return _code[_pos];
		return '\0';
	}

	[[nodiscard]] unsigned char SeekNext() const
	{
		if (_pos + 1 <= _size) return _code[_pos + 1];
		return '\0';
	}

	[[nodiscard]] ArtCode::Command SeekNextCommand() const
	{
		if (_pos + 1 <= _size) return  (ArtCode::Command)_code[_pos + 1];
		return ArtCode::Command::INVALID;
	}

	const unsigned char* GetChunk(const int count) {

		unsigned char* _return = (unsigned char*)malloc(((size_t)count + 1));
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
		return Func::TryGetInt(GetString());
	}

	float GetFloat() {
		return Func::TryGetFloat(GetString());
	}

private:
	const unsigned char* _code;
	Sint64 _size;
	Sint64 _pos;
	unsigned char _current_bit;
	std::string _f_name;
};
