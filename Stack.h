#pragma once
#include <vector>

template <class T>
class AStack {
private:
	int _size;
	std::vector<T> _data;
public:
	void Add(T) {
		_data.push_back(T);
		_size++;
	}
	T Get() {
		if (_size > 0) {
			_size--;
			T t = _data.back();
			return _data.pop_back();
		}
	}
	bool IsEmpty() {
		return (_size > 0);
	}
	int Size() {
		return _size;
	}
	AStack() {
		_data = std::vector<T>();
		_size = 0;
	}
	virtual ~AStack() {
		_size = 0;
		_data.clear();
	}
};
