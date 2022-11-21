#include "Scene.h"
#include "Debug.h"
#include "Event.h"

Uint64 Scene::Instance::_cid = 0;
Scene::Instance::Instance()
{
	this->Tag = "undefined";
	this->InView = false;
	this->Alive = true;
}

Scene::Instance::~Instance()
{

}

void Scene::Instance::RegisterEvent(Event ev)
{

	if (std::find(_events_registered.begin(), _events_registered.end(), ev) == _events_registered.end()) {
		_events_registered.push_back(ev);
	}
	else {
		Debug::WARNING("event error in instance id='" + std::to_string(_id) + "' tag='" + Tag + "' - try to add event that is added once (ev:" + std::to_string(static_cast<typename std::underlying_type<Event>::type>(ev)));
	}

}

void Scene::Instance::UnRegisterEvent(Event ev)
{
	if (std::find(_events_registered.begin(), _events_registered.end(), ev) == _events_registered.end()) {
		_events_registered.erase(std::remove(_events_registered.begin(), _events_registered.end(), ev), _events_registered.end());
	}
	else {
		Debug::WARNING("event error in instance id='" + std::to_string(_id) + "' tag='" + Tag + "' - try to erase event that is not exists (ev:" + std::to_string(static_cast<typename std::underlying_type<Event>::type>(ev)));
	}
}

Scene::Scene()
{
	_events = std::map<Event, std::vector<Instance*>>();
	_instances = plf::colony<Instance*>();
	_instances_new = std::vector<Instance*>();
	_instances_size = 0;
}

Scene::~Scene()
{
	_events.clear();
	_instances.clear();
	_instances_new.clear();
}

Scene::Instance* Scene::CreateInstance(Instance* Instance)
{
	_instances_size++;
	_instances_new.push_back(Instance);
	//Instance->Parrent = this;
	return Instance;
}

void Scene::DeleteInstance(Instance*)
{
	_instances_size--;
}

void Scene::RegisterEvent(Event ev, Instance* ins)
{
	if (ins == nullptr) {
		Debug::WARNING("event error (Art::Scene::RegisterEvent) instance is not exists");
		return;
	}

	if (_events.find(ev) == _events.end()) {
		_events[ev] = std::vector<Instance*>();
	}
	_events[ev].push_back(ins);
}

void Scene::UnRegisterEvent(Event ev, Instance* ins)
{
	if (ins == nullptr) {
		Debug::WARNING("event error (Art::Scene::UnRegisterEvent) instance is not exists");
		return;
	}

	auto _e_it = _events.find(ev);
	if (_e_it == _events.end()) {
		Debug::WARNING("event error (Art::Scene::UnRegisterEvent) in instance id='" + std::to_string(ins->GetId()) + "' tag='" + ins->Tag + "' - try to erase event that is not exists (ev:" + std::to_string(static_cast<typename std::underlying_type<Event>::type>(ev)));
		return;
	}

	auto _i_it = std::find(_events[ev].begin(), _events[ev].end(), ins);
	if (_i_it == _events[ev].end()) {
		Debug::WARNING("event error (Art::Scene::UnRegisterEvent) in instance id='" + std::to_string(ins->GetId()) + "' tag='" + ins->Tag + "' - event on this instance not found (ev:" + std::to_string(static_cast<typename std::underlying_type<Event>::type>(ev)));
		return;
	}

	_events[ev].erase(_i_it);

	//Art::Debug::WARNING("event error (Art::Scene::UnRegisterEvent) in instance id='" + std::to_string(ins->GetId()) + "' tag='" + ins->Tag + "' - try to erase event that is not exists (ev:" + std::to_string(static_cast<typename std::underlying_type<Art::Scene::Event>::type>(ev)));

}