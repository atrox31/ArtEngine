#pragma once
#include "ArtCore/main.h"
#if AC_ENABLE_DEBUG_MODE

#include <chrono>
#include <ostream>
#include <iostream>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;

class Time
{
public:
	Time() { _time = duration<double, std::milli>();  }
	void StartTest() {
		_start = high_resolution_clock::now();
	}
	void EndTest() {
		_end = high_resolution_clock::now();
		_time = _end - _start;
	}
	friend std::ostream& operator<<(std::ostream& os, const Time& dt) {
		return os << dt._time.count() << "ms ";
	}
	void PrintTest(const std::string& test) const
	{
		std::cout << "test '" << test << "' complite. Time: " << _time.count() << std::endl;
	}
	[[nodiscard]] double GetTestTime() const
	{
		return _time.count();
	}

private:
	std::chrono::steady_clock::time_point _start;
	std::chrono::steady_clock::time_point _end; 
	duration<double, std::milli> _time{};
};

#endif
