#include "Instance.h"

Uint64 Instance::_cid = 0;
Instance::Instance()
{
	this->Tag = "undefined";
	this->InView = false;
	this->Alive = true;
}

Instance::~Instance()
{

}
