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
			PosX - (SelfSprite->GetWidth() / 2 - SelfSprite->GetCenterX()),
			PosY - (SelfSprite->GetHeight() / 2 - SelfSprite->GetCenterY())
		};
		const float distance = Func::Distance(point, spoint);
		return (distance <= MaskValue);
	}

	return false;
}
