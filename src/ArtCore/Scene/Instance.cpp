#include "Instance.h"

#include "ArtCore/CodeExecutor/CodeExecutor.h"
#include "ArtCore/Functions/Func.h"
#include "ArtCore/Graphic/ColorDefinitions.h"
#include "ArtCore/Graphic/Render.h"
#include "ArtCore/System/Core.h"
#include "ArtCore/_Debug/Debug.h"

Uint64 Instance::_cid = 0;
Instance::Instance(const int instance_definition_id)
{
	this->_instance_definition_id = instance_definition_id;
	this->Tag = "undefined";
	this->Name = "no_name";

	this->InView = false;
	this->Alive = true;
	this->IsCollider = false;

	this->PosX = 0.0f;
	this->PosY = 0.0f;
	this->Direction = 0.0f;

	this->SelfSprite = nullptr;
	this->SpriteScaleX = 1.0f;
	this->SpriteScaleY = 1.0f;
	this->SpriteCenterX = 0;
	this->SpriteCenterY = 0;
	this->SpriteAngle = 0.0f;

	this->SpriteAnimationFrame = 0.0f;
	this->SpriteAnimationSpeed = 60.0f;
	this->SpriteAnimationLoop = true;
	this->EventFlag = event_bit::NONE;

	this->_have_suspended_code = false;
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
/**
 * \brief Check if can add more suspended state to instance (max is 255)
 * \return Answer is can add more state
 */
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

/**
 * \brief Pop suspended code
 * \return Operation success state
 */
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

void Instance::DrawSelf()
{
	SpriteAnimationFrame += (SpriteAnimationSpeed * (float)Core::DeltaTime);
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

	const float MaskValue = SelfSprite->GetMaskValue();
	if (MaskValue < 1.0f) return false;

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


Rect Instance::GetBodyMask() const
{
	switch(Body.Type)
	{
	case BodyType::Circle:
		return {
		(PosX - Body.Value) * SpriteScaleX,
			(PosY - Body.Value) * SpriteScaleY,
			(PosX + Body.Value) * SpriteScaleX,
			(PosY + Body.Value) * SpriteScaleY
		};
	case BodyType::Rect:
		return {
		(PosX - Body.Value) * SpriteScaleX,
			(PosY - Body.Value) * SpriteScaleY,
			(PosX + Body.Value) * SpriteScaleX,
			(PosY + Body.Value) * SpriteScaleY
		};
	default:
		return {};
	}
	
}
