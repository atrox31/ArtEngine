#include "Instance.h"
/*
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

void Instance::RegisterEvent(Core::Event ev)
{

	if (std::find(_events_registered.begin(), _events_registered.end(), ev) == _events_registered.end()) {
		_events_registered.push_back(ev);
	}
	else {
		Debug::WARNING("event error in instance id='" + std::to_string(_id) + "' tag='" + Tag + "' - try to add event that is added once (ev:" + std::to_string(static_cast<typename std::underlying_type<Core::Event>::type>(ev)));
	}

}

void Instance::UnRegisterEvent(Core::Event ev)
{
	if (std::find(_events_registered.begin(), _events_registered.end(), ev) == _events_registered.end()) {
		_events_registered.erase(std::remove(_events_registered.begin(), _events_registered.end(), ev), _events_registered.end());
	}
	else {
		Debug::WARNING("event error in instance id='" + std::to_string(_id) + "' tag='" + Tag + "' - try to erase event that is not exists (ev:" + std::to_string(static_cast<typename std::underlying_type<Core::Event>::type>(ev)));
	}
}
*/