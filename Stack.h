#pragma once
#include <vector>

template <class T>
class AStack {
private:
	size_t _size;
	size_t _next_resize;
	T* _data;
	size_t _default_stack_size;
public:
	// Add varible to stack, does not call for contructor so object must be created first
	void Add(T value) {
		if (_size == _next_resize) {
			_next_resize += _default_stack_size;
			T* new_data = (T*)malloc(_next_resize * sizeof(T));
			memcpy_s(new_data, _next_resize, _data, _size);
			new_data[_size] = value;
			free(_data);
			_data = new_data;
		}else{
			_data[_size] = value;
		}
		_size++;
	}
	// Get fresh value on stack or return NULL
	T Get() {
		if (_size > 0) {
			return _data[--_size];
		}
		return NULL;
	}
	constexpr inline bool IsEmpty() {
		return (_size == 0);
	}
	constexpr inline int Size() {
		return (int)_size;
	}
	AStack(size_t DefaultStackSize = 4) {
		_default_stack_size = DefaultStackSize;
		_size = 0;
		_next_resize = DefaultStackSize ;
		_data = (T*)malloc(_default_stack_size * sizeof(T));
	}
	virtual ~AStack() {
		_size = 0;
		free(_data);
	}
};
