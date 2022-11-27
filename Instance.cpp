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

	this->E_MaskClicked = false;
	this->Mask = Rect();

	this->PosX = 0.0f;
	this->PosY = 0.0f;
	this->Direction = 0.0f;

	this->SelfSprite = nullptr;
	this->SpriteScaleX = 0.0f;
	this->SpriteScaleY = 0.0f;
	this->SpriteCenterX = 0.0f;
	this->SpriteCenterY = 0.0f;
	this->SpriteAngle = 0.0f;

	this->SpriteAnimationFrame = 0.0f;
	this->SpriteAnimationSpeed = 60.0f;
	this->SpriteAnimationLoop = true;
}

Instance* Instance::GiveId()
{
	this->_id = ++_cid;
	return this;
}

Instance::~Instance()
{

}

void Instance::DrawSelf()
{
	SpriteAnimationFrame += (float)((double)SpriteAnimationSpeed * Core::GetInstance()->DeltaTime);
	if (!SpriteAnimationLoop && SpriteAnimationFrame > SelfSprite->GetMaxFrame()) {
		SpriteAnimationSpeed = 0.0f;
	}
	Render::DrawSprite_ex(SelfSprite, PosX, PosY , (int)SpriteAnimationFrame,  SpriteScaleX, SpriteScaleY, SpriteCenterX, SpriteCenterY, SpriteAngle);
}
