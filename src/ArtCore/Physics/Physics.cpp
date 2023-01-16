// ReSharper disable CppClangTidyClangDiagnosticSwitchEnum
#include "Physics.h"

#include <iostream>

#include "ArtCore/Functions/Func.h"
#include "ArtCore/System/Core.h"

bool Physics::CollisionTest(const Instance* object1, const Instance* object2)
{
	if (object1 == nullptr) return false;
	if (object2 == nullptr) return false;

	if (object1 == object2) return false;

	if (!object1->IsCollider) return false;
	if (!object2->IsCollider) return false;

	if (!object1->Body.HaveValue()) return false;
	if (!object2->Body.HaveValue()) return false;

	//Instance::BodyType::Sprite is copied to circle or rect on create
	if (object1->Body.Type == Instance::body::type::Rectangle) {
		if (object2->Body.Type == Instance::body::type::Rectangle) return TestRect2Rect(object1, object2);
		if (object2->Body.Type == Instance::body::type::Circle) return TestRect2Circle(object1, object2);
	}else 
	if (object1->Body.Type == Instance::body::type::Circle) {							// this is no mistake
		if (object2->Body.Type == Instance::body::type::Rectangle) return TestRect2Circle(object2, object1);
		if (object2->Body.Type == Instance::body::type::Circle) return TestCircle2Circle(object1, object2);
	}

	return false;
}
bool Physics::TestRect2Rect(const Instance* object1, const Instance* object2)
{
	const SDL_FRect object1_collision_rect = object1->GetBodyMask().ToSDL_FRect_wh();
	const SDL_FRect object2_collision_rect = object2->GetBodyMask().ToSDL_FRect_wh();
	return SDL_HasIntersectionF(&object1_collision_rect, &object2_collision_rect);
}
bool Physics::TestRect2Circle(const Instance* object1, const Instance* object2)
{
	const float object2_radius = (object2->Body.Radius * object2->SpriteScaleX + object2->Body.Radius * object2->SpriteScaleY) / 2.f;
	return CollisionCircle2Rect( object2->PosX, object2->PosY, object2_radius, object1->GetBodyMask());
}
bool Physics::TestCircle2Circle(const Instance* object1, const Instance* object2)
{
	const float object1_radius = (object1->Body.Radius * object1->SpriteScaleX + object1->Body.Radius * object1->SpriteScaleY ) /2.f;
	const float object2_radius = (object2->Body.Radius * object2->SpriteScaleX + object2->Body.Radius * object2->SpriteScaleY) / 2.f;
	return CollisionCircle2Circle(
		object1->PosX, object1->PosY, object1_radius,
		object2->PosX, object2->PosY, object2_radius
	);
}

void Physics::BounceInstance(Instance* object1, Instance* object2)
{
	if (object1 == nullptr) return;
	if (object2 == nullptr) return;

	if (object1 == object2) return;

	if (!object1->IsCollider) return;
	if (!object2->IsCollider) return;

	if (!object1->Body.HaveValue()) return;
	if (!object2->Body.HaveValue()) return;

	// direction vector
	vec2f sender_direction_vec = Func::GetDirectionVector(object1->Direction);

	// direction of collision vector, if value is -1 then direction id opposite
	vec2f collision_direction_vector(1.f, 1.f);

	//Instance::BodyType::Sprite is copied to circle or rect on create
	if (object1->Body.Type == Instance::body::type::Rectangle) {							
		if (object2->Body.Type == Instance::body::type::Rectangle) BounceRectRect(collision_direction_vector, object1, object2); else
		if (object2->Body.Type == Instance::body::type::Circle) BounceCircleRect(collision_direction_vector, object2, object1);// this is no mistake
	}else
	if (object1->Body.Type == Instance::body::type::Circle) {
		if (object2->Body.Type == Instance::body::type::Rectangle) BounceCircleRect(collision_direction_vector, object1, object2); else
		if (object2->Body.Type == Instance::body::type::Circle) BounceCircleCircle(collision_direction_vector, object1, object2);
	}

	sender_direction_vec *= collision_direction_vector;
	object1->Direction = Func::GetVectorFromDirection(sender_direction_vec);

	// move 1 unit to avoid mistakes
	const float direction_to_object_2 = std::atan2f(object2->PosY - object1->PosY, object2->PosX - object1->PosX);
	object1->PosX -= std::cosf(direction_to_object_2);
	object1->PosY -= std::sinf(direction_to_object_2);
	
}
void Physics::BounceRectRect(vec2f& collision_vector, const Instance* object1, const Instance* object2)
{
	/*
		 0,0				+X, 0
			+----+----+----+
			| LU | MU | RU |
			+----+----+----+
			| LM |    | RM |
			+----+----+----+
			| LB | MB | RB |
			+----+----+----+
		0, +Y				+X, +Y
	 */
	 // ReSharper disable CppInconsistentNaming
	const Rect object2BodyMask = object2->GetBodyMask();
	const float object2UpBoundary = object2BodyMask.Y;
	const float object2DownBoundary = object2BodyMask.H;
	const float object2LeftBoundary = object2BodyMask.X;
	const float object2RightBoundary = object2BodyMask.W;

	const Rect object1BodyMask = object1->GetBodyMask();
	const float object1UpBoundary = object1BodyMask.H;
	const float object1DownBoundary = object1BodyMask.Y;
	const float object1LeftBoundary = object1BodyMask.W;
	const float object1RightBoundary = object1BodyMask.X;

	bool LM = (object1RightBoundary > object2LeftBoundary) && (object1RightBoundary < object2RightBoundary);
	bool RM = (object1LeftBoundary < object2RightBoundary) && (object1LeftBoundary > object2LeftBoundary);
	bool MU = (object1DownBoundary > object2UpBoundary) && (object1DownBoundary < object2DownBoundary);
	bool MB = (object1UpBoundary < object2DownBoundary) && (object1UpBoundary > object2UpBoundary);

	if ((LM == true) && (RM == true))
	{// in middle
		LM = false;
		RM = false;
	}
	if ((MU == true) && (MB == true))
	{// in middle
		MU = false;
		MB = false;
	}

	const bool LU = LM && MU;
	const bool RU = MU && RM;
	const bool RB = RM && MB;
	const bool LB = MB && LM;
	// ReSharper restore CppInconsistentNaming

	if ((LU || LM || LB) || (RU || RM || RB))
	{
		collision_vector.x = -1.f;
	}
	if ((LU || MU || RU) || (LB || MB || RB))
	{
		collision_vector.y = -1.f;
	}
}

void Physics::BounceCircleCircle(vec2f& collision_vector, const Instance* object1, const Instance* object2)
{
	// always opposite direction
	collision_vector *= -1.f;
}

void Physics::BounceCircleRect(vec2f& collision_vector, const Instance* object1, const Instance* object2)
{
	/*
		 0,0				+X, 0
			+----+----+----+
			| LU | MU | RU |
			+----+----+----+
			| LM |    | RM |
			+----+----+----+
			| LB | MB | RB | 
			+----+----+----+
		0, +Y				+X, +Y
	 */
	// ReSharper disable CppInconsistentNaming
	const Rect object2BodyMask = object2->GetBodyMask();
	const float object2UpBoundary = object2BodyMask.Y;
	const float object2DownBoundary = object2BodyMask.H;
	const float object2LeftBoundary = object2BodyMask.X;
	const float object2RightBoundary = object2BodyMask.W;

	const float radius_scale = (object1->Body.Radius * object1->SpriteScaleX + object1->Body.Radius * object1->SpriteScaleY) / 2.f;
	const float object1UpBoundary = object1->PosY - radius_scale;
	const float object1DownBoundary = object1->PosY + radius_scale;
	const float object1LeftBoundary = object1->PosX - radius_scale;
	const float object1RightBoundary = object1->PosX + radius_scale;

	bool LM = (object1RightBoundary > object2LeftBoundary) &&( object1RightBoundary < object2RightBoundary);
	bool RM = (object1LeftBoundary < object2RightBoundary) &&( object1LeftBoundary > object2LeftBoundary);
	bool MU = (object1DownBoundary > object2UpBoundary) &&( object1DownBoundary < object2DownBoundary);
	bool MB = (object1UpBoundary < object2DownBoundary) && (object1UpBoundary > object2UpBoundary);

	if( (LM == true) && (RM == true))
	{// in middle
		LM = false;
		RM = false;
	}
	if( (MU == true) && (MB == true))
	{// in middle
		MU = false;
		MB = false;
	}

	const bool LU = LM && MU;
	const bool RU = MU && RM;
	const bool RB = RM && MB;
	const bool LB = MB && LM;
	// ReSharper restore CppInconsistentNaming

	if( (LU || LM || LB) || (RU || RM || RB) )
	{
		collision_vector.x = -1.f;
	}
	if( (LU || MU || RU) || (LB || MB || RB) )
	{
		collision_vector.y = -1.f;
	}
	/*
	std::cout << "+----+----+----+" << std::endl;
	std::cout << "| " << (LU ? "LU" : "  ") << "| " << (MU ? "MU" : "  ") << "| " << (RU ? "RU" : "  ") << "|" << std::endl;
	std::cout << "+----+----+----+" << std::endl;
	std::cout << "| " << (LM ? "LM" : "  ") << "|    | " << (RM ? "RM" : "  ") << "|" << std::endl;
	std::cout << "+----+----+----+" << std::endl;
	std::cout << "| " << (LB ? "LB" : "  ") << "| " << (MB ? "MB" : "  ") << "| " << (RB ? "RB" : "  ") << "|" << std::endl;
	std::cout << "+----+----+----+" << std::endl;
	*/
}

bool Physics::CollisionCircle2Line(const float& circle_x, const float& circle_y, const float& circle_r, const SDL_FPoint& line_begin, const SDL_FPoint& line_end)
{
	// not implemented yet
	return false;
}

// rectangle must be absolute, not w=width and h=height
bool Physics::CollisionCircle2Rect(const float& circle_x, const float& circle_y, const float& circle_r, const Rect& rectangle)
{
	SDL_FPoint circleDistance;
	circleDistance.x = abs(circle_x - rectangle.GetCenterX());
	circleDistance.y = abs(circle_y - rectangle.GetCenterY());

	const float rw2 = rectangle.Width() / 2;
	const float rh2 = rectangle.Height() / 2;

	if (circleDistance.x > (rw2 + circle_r)) { return false; }
	if (circleDistance.y > (rh2 + circle_r)) { return false; }

	if (circleDistance.x <= rw2)
	{
		return true;
	}
	if (circleDistance.y <= rh2)
	{
		return true;
	}

	const float cornerDistance_sq = (circleDistance.x - rw2) * (circleDistance.x - rw2) +
		(circleDistance.y - rh2) * (circleDistance.y - rh2);

	return (cornerDistance_sq <= (circle_r * circle_r));
}

bool Physics::CollisionCircle2Circle(const float& circle1_x, const float& circle1_y, const float& circle1_r,
	const float& circle2_x, const float& circle2_y, const float& circle2_r)
{
	const float dx = std::fabs(circle2_x - circle1_x);
	const float dy = std::fabs(circle2_y - circle1_y);
	const float dist = dx * dx + dy * dy;
	return dist <= (circle1_r * circle1_r + circle2_r * circle2_r);
}
bool Physics::CollisionCircle2Point(const float& circle1_x, const float& circle1_y, const float& circle1_r,
	const SDL_FPoint& point)
{
	const float dx = std::fabs(point.x - circle1_x);
	const float dy = std::fabs(point.y - circle1_y);
	const float dist = dx * dx + dy * dy;
	return dist <= (circle1_r * circle1_r);
}