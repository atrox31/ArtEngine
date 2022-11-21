#pragma once
#include "SDL_version.h"
#include <cmath>

#pragma warning(push)
#include SDL2_INCLUDE_DIR
#pragma warning(pop)

template <class T>
class vec2 {
private:
	const double _M_PI = 3.141592653589793;
public:
	T x, y;

	vec2() :x(0), y(0) {}
	vec2(T x, T y) : x(x), y(y) {}
	vec2(const vec2& v) : x(v.x), y(v.y) {}
	vec2(SDL_FPoint v) : x(v.x), y(v.y) {}
	vec2(SDL_Point v) : x((T)(v.x)), y((T)(v.y)) {}

	vec2& operator=(const vec2& v) {
		x = v.x;
		y = v.y;
		return *this;
	}

	vec2 operator+(vec2& v) {
		return vec2(x + v.x, y + v.y);
	}
	vec2 operator-(vec2& v) {
		return vec2(x - v.x, y - v.y);
	}

	vec2& operator+=(vec2& v) {
		x += v.x;
		y += v.y;
		return *this;
	}
	vec2& operator-=(vec2& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}

	vec2 operator+(double s) {
		return vec2(x + (T)s, y + (T)s);
	}
	vec2 operator-(double s) {
		return vec2(x - (T)s, y - (T)s);
	}
	vec2 operator*(double s) {
		return vec2(x * (T)s, y * (T)s);
	}
	vec2 operator/(double s) {
		return vec2(x / (T)s, y / (T)s);
	}


	vec2& operator+=(double s) {
		x += s;
		y += s;
		return *this;
	}
	vec2& operator-=(double s) {
		x -= s;
		y -= s;
		return *this;
	}
	vec2& operator*=(double s) {
		x *= (T)s;
		y *= (T)s;
		return *this;
	}
	vec2& operator/=(double s) {
		x /= s;
		y /= s;
		return *this;
	}

	void set(T x, T y) {
		this->x = x;
		this->y = y;
	}

	void rotate(double deg) {
		double theta = deg / 180.0 * _M_PI;
		double c = cos(theta);
		double s = sin(theta);
		double tx = x * c - y * s;
		double ty = x * s + y * c;
		x = tx;
		y = ty;
	}

	vec2& normalize() {
		if (length() == 0) return *this;
		*this *= (1.0 / length());
		return *this;
	}

	float dist(vec2 v) const {
		vec2 d(v.x - x, v.y - y);
		return d.length();
	}
	float length() const {
		return std::sqrt(x * x + y * y);
	}
	void truncate(double length) {
		double angle = atan2d(y, x);
		x = length * cos(angle);
		y = length * sin(angle);
	}

	vec2 ortho() const {
		return vec2(y, -x);
	}

	static float dot(vec2 v1, vec2 v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}
	static float cross(vec2 v1, vec2 v2) {
		return (v1.x * v2.y) - (v1.y * v2.x);
	}

};

typedef vec2<float> vec2f;
typedef vec2<double> vec2d;

