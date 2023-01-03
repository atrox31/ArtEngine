#include "Instance.h"

#include "ArtCore/Functions/Func.h"
#include "ArtCore/Graphic/ColorDefinitions.h"
#include "ArtCore/Graphic/Render.h"
#include "ArtCore/System/Core.h"
#include "ArtCore/_Debug/Debug.h"

Uint64 Instance::_cid = 0;
Instance::Instance(const int instance_definition_id)
{
	this->_instanceDefinitionId = instance_definition_id;
	this->Tag = "undefined";
	this->Name = "no_name";

	this->InView = false;
	this->Alive = true;
	this->IsCollider = false;

	this->PosX = 0.0f;
	this->PosY = 0.0f;
	this->Direction = 0.0f;

	this->SelfSprite = nullptr;
	this->SpriteScaleX = 0.0f;
	this->SpriteScaleY = 0.0f;
	this->SpriteCenterX = 0;
	this->SpriteCenterY = 0;
	this->SpriteAngle = 0.0f;

	this->SpriteAnimationFrame = 0.0f;
	this->SpriteAnimationSpeed = 60.0f;
	this->SpriteAnimationLoop = true;
	this->EventFlag = event_bit::NONE;
}

Instance* Instance::GiveId()
{
	this->_id = ++_cid;
	return this;
}

Instance::~Instance()
{
	Debug::NOTE_DEATH("[Instance::~Instance]: "+ Name + "'[" + (_id==0?"template": std::to_string(_id)) + "]");
}

void Instance::DrawSelf()
{
	SpriteAnimationFrame += (SpriteAnimationSpeed * (float)Core::GetInstance()->DeltaTime);
	if (!SpriteAnimationLoop && SpriteAnimationFrame > float(SelfSprite->GetMaxFrame())) {
		SpriteAnimationSpeed = 0.0f;
	}
	Render::DrawSprite_ex(SelfSprite, PosX, PosY , (int)SpriteAnimationFrame,  SpriteScaleX, SpriteScaleY, (float)SpriteCenterX, (float)SpriteCenterY, SpriteAngle, 1.0f);
}

bool Instance::CheckMaskClick(SDL_FPoint& point) const
{
	if (SelfSprite == nullptr) return false;
	if (InView == false) return false;

	const Sprite::mask_type MaskType = SelfSprite->GetMaskType();
	if (MaskType == Sprite::mask_type::None) return false;

	const int MaskValue = SelfSprite->GetMaskValue();
	if (MaskValue < 1) return false;

	if (MaskType == Sprite::mask_type::Circle) {
		SDL_FPoint spoint{ 
			PosX + SelfSprite->GetCenterXRel(),
			PosY + SelfSprite->GetCenterYRel()
		};
		const float distance = Func::Distance(point, spoint);
		return (distance <= MaskValue);
	}
	if (MaskType == Sprite::mask_type::Rectangle) {
		Rect spoint{
			PosX - SelfSprite->GetMaskValue(),
			PosY - SelfSprite->GetMaskValue(),
			PosX + SelfSprite->GetMaskValue(),
			PosY + SelfSprite->GetMaskValue()
		};
		SDL_FPoint mov{ SelfSprite->GetCenterXRel(), SelfSprite->GetCenterYRel() };
		spoint += mov;
		return spoint.PointInRect(point);
	}

	return false;
}

#ifdef _DEBUG
void Instance::DebugDrawMask() const
{
	if (SelfSprite == nullptr) return;

	const Sprite::mask_type MaskType = SelfSprite->GetMaskType();
	if (MaskType == Sprite::mask_type::None) return;

	const int MaskValue = SelfSprite->GetMaskValue();
	if (MaskValue < 1) return;

	if (MaskType == Sprite::mask_type::Circle) {
		const SDL_FPoint spoint{
			PosX + SelfSprite->GetCenterXRel(),
			PosY + SelfSprite->GetCenterYRel()
		};
		Render::DrawCircle(spoint, (float)SelfSprite->GetMaskValue(), C_RED);
		return;
	}
	if (MaskType == Sprite::mask_type::Rectangle) {
		Rect spoint{
			PosX -  SelfSprite->GetMaskValue(),
			PosY -  SelfSprite->GetMaskValue(),
			PosX +  SelfSprite->GetMaskValue(),
			PosY + SelfSprite->GetMaskValue()
		};
		SDL_FPoint mov{ SelfSprite->GetCenterXRel(), SelfSprite->GetCenterYRel() };
		spoint += mov;
		Render::DrawRect(spoint.ToGPU_Rect(), C_RED);
	}

}
void Instance::DebugDrawCollision() {
	if (Body.Type == Instance::BodyType::Circle) {
		Render::DrawCircle({ PosX,PosY }, (float)Body.Value / 2, C_BLUE);
	}
	if (Body.Type == Instance::BodyType::Rect) {
		const Rect collision_rect_mine{
				PosX - (float)Body.Value / 2.f - 1,
				PosY - (float)Body.Value / 2.f - 1,
				(float)Body.Value + 2,
				(float)Body.Value + 2
		};
		Render::DrawRect_wh(collision_rect_mine.ToGPU_Rect(), C_BLUE);
	}
}
#endif // _DEBUG

bool Instance::CollideTest(const Instance* instance) const
{
	switch (instance->Body.Type)
	{
	case Instance::BodyType::Circle:
	{
		switch (Body.Type)
		{
		case Instance::BodyType::Circle:
		{
			return CollisionCircleCircle(PosX, PosY, (float)Body.Value / 2.f, instance->PosX, instance->PosY, (float)instance->Body.Value / 2.f);
		}
		break;
		case Instance::BodyType::Rect:
		{
			Rect collision_rect{
				PosX - Body.Value,
				PosY - Body.Value,
				PosX + Body.Value,
				PosY + Body.Value
			};
			return CollisionCircleRect(instance->PosX, instance->PosY, (float)instance->Body.Value, collision_rect.ToGPU_Rect_wh());
		}
		break;
		// all other enums, if body = Sprite body type is copied from sprite data
		case BodyType::BodyInvalid:
		case BodyType::None:
		case BodyType::Sprite:
		case BodyType::BodyEND: return false;
		}
	}
	break;
	case Instance::BodyType::Rect:
	{
		switch (Body.Type)
		{
		case Instance::BodyType::Circle:
		{
			Rect collision_rect{
				instance->PosX - (float)instance->Body.Value / 2.f,
				instance->PosY - (float)instance->Body.Value / 2.f,
				(float)instance->Body.Value,
				(float)instance->Body.Value
			};
			return CollisionCircleRect(PosX, PosY, (float)Body.Value / 2.f, collision_rect.ToGPU_Rect_wh());
		}
		break;
		case Instance::BodyType::Rect:
		{
			const SDL_FRect collision_rect_mine{
				PosX - (float)Body.Value / 2.f,
				PosY - (float)Body.Value / 2.f,
				(float)Body.Value,
				(float)Body.Value
			};
			const SDL_FRect collision_rect_other{
				instance->PosX - (float)instance->Body.Value / 2.f,
				instance->PosY - (float)instance->Body.Value / 2.f,
				(float)instance->Body.Value,
				(float)instance->Body.Value
			};
			return CollisionRectRect(collision_rect_mine, collision_rect_other);
		}
		break;
			// all other enums, if body = Sprite body type is copied from sprite data
		case BodyType::BodyInvalid:
		case BodyType::None:
		case BodyType::Sprite:
		case BodyType::BodyEND: return false;
		}
		break;
	}
	// all other enums, if body = Sprite body type is copied from sprite data
case BodyType::BodyInvalid:
case BodyType::None:
case BodyType::Sprite:
case BodyType::BodyEND: return false;
	}
	return false;
}

bool Instance::CollisionCircleCircle(const float c1x, const float c1y, const float c1r, const float c2x, const float c2y, const float c2r) const
{
	return Func::Distance(c1x, c1y, c2x, c2y) < c1r + c2r;
}

bool Instance::CollisionCircleRect(const float c1x, const float c1y, const float c1r, const GPU_Rect rect)
{
	return Func::RectCircleColliding(c1x, c1y, c1r, rect);
}

bool Instance::CollisionRectRect(const SDL_FRect& rect1, const SDL_FRect& rect2)
{
	SDL_FRect res;
	return SDL_IntersectFRect(&rect1, &rect2, &res);
}
