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

Instance::~Instance()
{
	Debug::NOTE_DEATH("[Instance::~Instance]: "+ Name + "'[" + (_id==0?"template": std::to_string(_id)) + "]");
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
 * \param state true if add and false if sub
 * \return Answer is can add more state
 */
bool Instance::SuspendedCodeState(const bool state)
{
	if (state)
	{
		_have_suspended_code += 1;
		if (_have_suspended_code == 0)
		{
			_have_suspended_code -= 1;
			Debug::WARNING("Cannot add more suspended code to instance (max 255)!");
			return false;
		}
		return true;
	}else
	{
		_have_suspended_code -= 1;
		if(_have_suspended_code == 0xFF)
		{
			_have_suspended_code = 0;
			Debug::WARNING("Cannot remove suspended code to instance, there is no suspended state");
			return false;
		}
		return true;
	}
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

#ifdef _DEBUG
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
void Instance::DebugDrawMask() const
{
	return;
	if (SelfSprite == nullptr) return;

	const Sprite::mask_type MaskType = SelfSprite->GetMaskType();
	if (MaskType == Sprite::mask_type::None) return;

	const float MaskValue = SelfSprite->GetMaskValue();
	if (MaskValue < 1.f) return;

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
		const float radius_scale = Body.Value * ((SpriteScaleX + SpriteScaleY) / 2.f);
		Render::DrawCircle({ PosX,PosY }, radius_scale, C_BLUE);
	}
	if (Body.Type == Instance::BodyType::Rect) {
		Render::DrawRect(GetBodyMask().ToGPU_Rect(), C_BLUE);
	}
}
#endif // _DEBUG