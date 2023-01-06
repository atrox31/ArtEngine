#pragma once
#include "ArtCore/Scene/Instance.h"

class Physics
{
public:
	// check if two objects have collision
	static bool CollisionTest(const Instance* object1, const Instance* object2);
	// Change direction of object1 as its bounce of object2
	static void BounceInstance(Instance* object1, Instance* object2);

	static bool CollisionCircle2Line(const float& circle_x, const float& circle_y, const float& circle_r,
		const SDL_FPoint& line_begin, const SDL_FPoint& line_end);

	// if Circle and Rectangle have intersection
	static bool CollisionCircle2Rect(const float& circle_x, const float& circle_y, const float& circle_r, const Rect& rectangle);

	// if Circle and Rectangle have intersection
	static bool CollisionCircle2Circle(const float& circle1_x, const float& circle1_y, const float& circle1_r,
		const float& circle2_x, const float& circle2_y, const float& circle2_r);
	static bool CollisionCircle2Point(const float& circle1_x, const float& circle1_y, const float& circle1_r,
		const SDL_FPoint& point);

private:
	static bool TestRect2Rect(const Instance* object1, const Instance* object2);
	static bool TestRect2Circle(const Instance* object1, const Instance* object2);
	static bool TestCircle2Circle(const Instance* object1, const Instance* object2);

	// object1 bounce against object2
	static void BounceRectRect(vec2f& collision_vector, const Instance* object1, const Instance* object2);
	// object1 bounce against object2
	static void BounceCircleCircle(vec2f& collision_vector, const Instance* object1, const Instance* object2);
	// object1 bounce against object2
	static void BounceCircleRect(vec2f& collision_vector, const Instance* object1, const Instance* object2);
};

