#pragma once
#include <vector>

//AStack<const char*> : 147.586ms   !!!!
//AStack<float>:151.597ms
//VectorStack<float> astack : 192.844ms
//std::stack<const char*> : 309.726ms
//VectorStack<std::string> : 1178.31ms
//std::stack<float> : 2286.32ms

template <typename T>
struct AStack {
private:
	size_t _size;
	size_t _c_element;
	std::vector<T> _data;
	size_t _current_capacity;
public:
	// Add varible to stack, does not call for contructor so object must be created first
	void Add(T value) {
		if (_size == _current_capacity) {
			_data.resize(_size + 8);
			_current_capacity += 8;
		}
		if (_c_element >= _size) {
			_data.push_back(value);
			_c_element++;
			_size++;
		}
		else {
			_data[_c_element++] = value;
		}
	}
	// Get fresh value on stack or return NULL
	T Get() {
		if (_c_element > 0) {
			return _data[--_c_element];
		}
		return T();
	}
	constexpr inline bool IsEmpty() {
		return (_c_element == 0);
	}
	constexpr inline int Size() {
		return (int)_c_element;
	}
	constexpr inline int Capacity() {
		return (int)_size;
	}
	void Erase() {
		_size = 0;
		_c_element = 0;
		_data.clear();
	}
	AStack(size_t DefaultStackSize = 8) {
		_size = 0;
		_c_element = 0;
		_data.reserve(DefaultStackSize);
		_current_capacity = DefaultStackSize;
	}
	virtual ~AStack() {
		_size = 0;
		_c_element = 0;
		_data.clear();
	}
};

/* TODO: Add(T value) -> musi kopiowaæ wartoœæ cost char*
template <typename T>
struct AStack{
	 static_assert(!std::is_class_v<T>, "AStack<std::string> is not valid, use 'const char*'");
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
	void Erase() {
		_size = 0;
		free(_data);
		_next_resize = _default_stack_size;
		_data = (T*)malloc(_default_stack_size * sizeof(T));
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
*/