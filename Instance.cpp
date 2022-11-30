#include "Instance.h"
#include "Core.h"

Uint64 Instance::_cid = 0;
Instance::Instance(int InstanceDefinitionId)
{
	this->_instanceDefinitionId = InstanceDefinitionId;
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
	std::cout << "delete instance '" << Name << "'";
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
		//Render::DrawCircle(spoint, SelfSprite->GetMaskValue(), C_RED);
	}

}
#endif // _DEBUG
/*
SDL_FPoint i1{ instance->PosX, instance->PosY };
		   SDL_FPoint i2{ target->PosX, target->PosY };
		   if (Func::Distance(i1, i2) < instance->Body.Value + target->Body.Value) {
			   MakeCollision(target, instance);
		   }
CIRCLE
*/
/*
GPU_Rect p1{
		   target->PosX - target->Body.Value,
		   target->PosY - target->Body.Value,
		   target->PosX + target->Body.Value,
		   target->PosY + target->Body.Value
};
GPU_Rect p2{
	instance->PosX - instance->Body.Value,
	instance->PosY - instance->Body.Value,
	instance->PosX + instance->Body.Value,
	instance->PosY + instance->Body.Value
};
if (GPU_IntersectRect(p1, p2, nullptr)) {
	MakeCollision(target, instance);
}
RECT
*/
bool Instance::CollideTest(Instance* instance)
{
	switch (instance->Body.Type)
	{
	case Instance::BodyType::CIRCLE:
	{
		switch (Body.Type)
		{
		case Instance::BodyType::CIRCLE:
		{
			return Collision_circle_circle(PosX, PosY, (float)Body.Value, instance->PosX, instance->PosY, (float)instance->Body.Value);
		}
		break;
		case Instance::BodyType::RECT:
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
	case Instance::BodyType::RECT:
	{
		switch (Body.Type)
		{
		case Instance::BodyType::CIRCLE:
		{
			Rect collision_rect{
				instance->PosX - instance->Body.Value,
				instance->PosY - instance->Body.Value,
				instance->PosX + instance->Body.Value,
				instance->PosY + instance->Body.Value
			};
			return Collision_circle_rect(PosX, PosY, (float)Body.Value, collision_rect.ToGPU_Rect_wh());
		}
		break;
		case Instance::BodyType::RECT:
		{
			SDL_FRect collision_rect_mine{
				PosX - Body.Value,
				PosY - Body.Value,
				(float)Body.Value * 2,
				(float)Body.Value * 2
			};
			SDL_FRect collision_rect_other{
				instance->PosX - (float)instance->Body.Value,
				instance->PosY - (float)instance->Body.Value,
				(float)instance->Body.Value * 2,
				(float)instance->Body.Value * 2
			};
			return SDL_IntersectFRect(&collision_rect_mine, &collision_rect_other, NULL);
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
