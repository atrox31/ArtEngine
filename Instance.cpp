#include "Instance.h"

Uint64 Instance::_cid = 0;
Instance::Instance(int InstanceDefinitionId)
{
	this->_instanceDefinitionId = InstanceDefinitionId;
	this->_id = _cid++;
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
	this->SpriteCurrentFrame = 0.0f;
	this->SpriteAngle = 0.0f;
}

Instance::~Instance()
{

}
