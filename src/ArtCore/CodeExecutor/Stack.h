#pragma once
#include <vector>

template <typename T>
struct AStack final
{
private:
	size_t _size;
	size_t _c_element;
	std::vector<T> _data;
	size_t _current_capacity;
public:
	// Add variable to stack
	void Add(T value) {
		if (_size == _current_capacity) {
			_data.resize(_size + 8);
			_current_capacity += 8;
		}
		if (_c_element >= _size) {
			_data.emplace_back(value);
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

	[[nodiscard]] bool IsEmpty() const
	{
		return (_c_element == 0);
	}

	[[nodiscard]] int Size() const
	{
		return (int)_c_element;
	}

	[[nodiscard]] int Capacity() const
	{
		return (int)_size;
	}
	void Erase() {
		_size = 0;
		_c_element = 0;
		_data.clear();
	}

	// Delete last element on stack
	void PopBack()
	{
		if (_c_element > 0) {
			--_c_element;
		}
	}

	AStack(size_t default_stack_size = 8) {
		_size = 0;
		_c_element = 0;
		_data.reserve(default_stack_size);
		_current_capacity = default_stack_size;
	}
	virtual ~AStack() {
		_size = 0;
		_c_element = 0;
		_data.clear();
	}
};