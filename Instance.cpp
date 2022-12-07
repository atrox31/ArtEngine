#include "Instance.h"
#include "Core.h"

Uint64 Instance::_cid = 0;
Instance::Instance(int instance_definition_id)
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
	this->EventFlag = EventBit::NONE;
}

Instance* Instance::GiveId()
{
	this->_id = ++_cid;
	return this;
}

Instance::~Instance()
{
	std::cout << "delete instance '" << Name << "'[" << _id << "]";
	if (this->_id == 0) {
		std::cout << " template";
	}
	std::cout << std::endl;
}

void Instance::DrawSelf()
{
	SpriteAnimationFrame += (float)((double)SpriteAnimationSpeed * Core::GetInstance()->DeltaTime);
	if (!SpriteAnimationLoop && SpriteAnimationFrame > SelfSprite->GetMaxFrame()) {
		SpriteAnimationSpeed = 0.0f;
	}
	Render::DrawSprite_ex(SelfSprite, PosX, PosY , (int)SpriteAnimationFrame,  SpriteScaleX, SpriteScaleY, (float)SpriteCenterX, (float)SpriteCenterY, SpriteAngle, 1.0f);
}

bool Instance::CheckMaskClick(SDL_FPoint& point)
{
	if (SelfSprite == nullptr) return false;
	if (InView == false) return false;

	const Sprite::MaskType MaskType = SelfSprite->GetMaskType();
	if (MaskType == Sprite::MaskType::none) return false;

	const int MaskValue = SelfSprite->GetMaskValue();
	if (MaskValue < 1) return false;

	if (MaskType == Sprite::MaskType::circle) {
		SDL_FPoint spoint{ 
			PosX + SelfSprite->GetCenterXRel(),
			PosY + SelfSprite->GetCenterYRel()
		};
		const float distance = Func::Distance(point, spoint);
		return (distance <= MaskValue);
	}
	if (MaskType == Sprite::MaskType::rectangle) {
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
void Instance::DebugDrawMask() {
	if (SelfSprite == nullptr) return;

	const Sprite::MaskType MaskType = SelfSprite->GetMaskType();
	if (MaskType == Sprite::MaskType::none) return;

	const int MaskValue = SelfSprite->GetMaskValue();
	if (MaskValue < 1) return;

	if (MaskType == Sprite::MaskType::circle) {
		SDL_FPoint spoint{
			PosX + SelfSprite->GetCenterXRel(),
			PosY + SelfSprite->GetCenterYRel()
		};
		Render::DrawCircle(spoint, (float)SelfSprite->GetMaskValue(), C_RED);
		return;
	}
	if (MaskType == Sprite::MaskType::rectangle) {
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
		Rect collision_rect_mine{
				PosX - (float)Body.Value / 2.f - 1,
				PosY - (float)Body.Value / 2.f - 1,
				(float)Body.Value + 2,
				(float)Body.Value + 2
		};
		Render::DrawRect_wh(collision_rect_mine.ToGPU_Rect(), C_BLUE);
	}
}
#endif // _DEBUG

bool Instance::CollideTest(const Instance* instance)
{
	switch (instance->Body.Type)
	{
	case Instance::BodyType::Circle:
	{
		switch (Body.Type)
		{
		case Instance::BodyType::Circle:
		{
			return Collision_circle_circle(PosX, PosY, (float)Body.Value / 2.f, instance->PosX, instance->PosY, (float)instance->Body.Value / 2.f);
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
			return Collision_circle_rect(instance->PosX, instance->PosY, (float)instance->Body.Value, collision_rect.ToGPU_Rect_wh());
		}
		break;
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
			return Collision_circle_rect(PosX, PosY, (float)Body.Value / 2.f, collision_rect.ToGPU_Rect_wh());
		}
		break;
		case Instance::BodyType::Rect:
		{
			SDL_FRect collision_rect_mine{
				PosX - (float)Body.Value / 2.f,
				PosY - (float)Body.Value / 2.f,
				(float)Body.Value,
				(float)Body.Value
			};
			SDL_FRect collision_rect_other{
				instance->PosX - (float)instance->Body.Value / 2.f,
				instance->PosY - (float)instance->Body.Value / 2.f,
				(float)instance->Body.Value,
				(float)instance->Body.Value
			};
			SDL_FRect res;
			return SDL_IntersectFRect(&collision_rect_mine, &collision_rect_other, &res);
		}
		break;
		}
		break;
	default:
		return false;
		break;
	}
	}
	return false;
}

	bool Instance::Collision_circle_circle(float c1x, float c1y, float c1r, float c2x, float c2y, float c2r)
	{
		return Func::Distance(c1x, c1y, c2x, c2y) < c1r + c2r;
	}

	bool Instance::Collision_circle_rect(float c1x, float c1y, float c1r, GPU_Rect rect)
	{
		return Func::RectCircleColliding(c1x, c1y, c1r, rect);
	}
