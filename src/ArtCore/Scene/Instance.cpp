#include "Instance.h"

#include "ArtCore/CodeExecutor/CodeExecutor.h"
#include "ArtCore/Functions/Func.h"
#include "ArtCore/Graphic/ColorDefinitions.h"
#include "ArtCore/Graphic/Render.h"
#include "ArtCore/System/Core.h"
#include "ArtCore/_Debug/Debug.h"

Uint64 Instance::_cid = 0;
Instance::Instance(const int instance_definition_id) :
	_instance_definition_id(instance_definition_id)
{
	
}

Instance* Instance::GiveId()
{
	this->_id = ++_cid;
	return this;
}

void Instance::Delete()
{
	Alive = false;
	if(_have_suspended_code > 0)
	{
		CodeExecutor::SuspendedCodeDeleteInstance(this);
	}
}

bool Instance::SuspendedCodeAdd()
{
	_have_suspended_code += 1;
	if (_have_suspended_code == 0)
	{
		_have_suspended_code -= 1;
		Console::WriteLine("Cannot add more suspended code to instance (max 255)!");
		return false;
	}
	return true;
}

bool Instance::SuspendedCodePop()
{
	_have_suspended_code -= 1;
	if (_have_suspended_code == UINT8_MAX)
	{
		_have_suspended_code = 0;
		Console::WriteLine("Cannot remove suspended code to instance, there is no suspended state");
		return false;
	}
	return true;
}

void Instance::DrawSelf(const float alpha)
{
	SpriteAnimationFrame += (SpriteAnimationSpeed * static_cast<float>(Core::DeltaTime));
	if (!SpriteAnimationLoop && SpriteAnimationFrame > static_cast<float>(SelfSprite->GetMaxFrame())) {
		SpriteAnimationSpeed = 0.0f;
	}
	Render::DrawSprite_ex(SelfSprite, PosX, PosY , static_cast<int>(SpriteAnimationFrame),  SpriteScaleX, SpriteScaleY, static_cast<float>(SpriteCenterX), static_cast<float>(SpriteCenterY), SpriteAngle, alpha);
}

bool Instance::CheckMaskClick(SDL_FPoint& point) const
{
	if (SelfSprite == nullptr) return false;
	if (InView == false) return false;

	if (!SelfSprite->GetMask().HaveValue()) return false;
	const Mask SpriteMask = SelfSprite->GetMask();

	if (SpriteMask.type == Mask::mask_type::Circle) {
		SDL_FPoint spoint{ 
			(PosX + SelfSprite->GetCenterXRel()) * SpriteScaleX,
			(PosY + SelfSprite->GetCenterYRel()) * SpriteScaleY
		};
		Render::ScalePoint(&spoint);
		const float distance = Func::Distance(point, spoint);
		return (distance <= SpriteMask.r);
	}
	if (SpriteMask.type == Mask::mask_type::Rectangle) {
		
		Rect spoint{
			PosX - SpriteMask.w,
			PosY - SpriteMask.h,
			PosX + SpriteMask.w,
			PosY + SpriteMask.h
		};
		spoint += { SelfSprite->GetCenterXRel(), SelfSprite->GetCenterYRel() };
		return spoint.PointInRect(point);
	}

	return false;
}


Rect Instance::GetBodyMask() const
{
	switch(Body.Type)
	{
	case body::type::Circle:
		return {
			PosX - Body.Radius * SpriteScaleX,
			PosY - Body.Radius * SpriteScaleY,
			PosX + Body.Radius * SpriteScaleX,
			PosY + Body.Radius * SpriteScaleY
		};
	case body::type::Rectangle:
		return {
			PosX - (Body.Dimensions.x / 2.f) * SpriteScaleX,
			PosY - (Body.Dimensions.y / 2.f) * SpriteScaleY,
			PosX + (Body.Dimensions.x / 2.f) * SpriteScaleX,
			PosY + (Body.Dimensions.y / 2.f) * SpriteScaleY
		};
	default:
		return {};
	}
	
}
