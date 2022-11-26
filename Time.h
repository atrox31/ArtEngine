#pragma once
#ifdef _DEBUG
#include <chrono>
#include <ostream>
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
class Time
{
public:
	Time() { time = duration<double, std::milli>();  };
	void StartTest() {
		start = high_resolution_clock::now();
	}
	void EndTest() {
		end = high_resolution_clock::now();
		time = end - start;
	}
	friend std::ostream& operator<<(std::ostream& os, const Time& dt) {
		return os << dt.time.count() << "ms ";
	}
private:
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end; 
	duration<double, std::milli> time;
	/*auto t2 = high_resolution_clock::now();
	duration<double, std::milli> astack_time_float = t2 - t1;
	std::cout << "AStack<float>:" << astack_time_float.count() << "ms" << std::endl;*/
};

#endif
