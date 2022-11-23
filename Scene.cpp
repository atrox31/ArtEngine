#include "Scene.h"
#include "Debug.h"
#include "Event.h"

Scene::Scene()
{
	_instances = plf::colony<Instance*>();
	_instances_new = std::vector<Instance*>();
	_instances_size = 0;
}

Scene::~Scene()
{
	_instances.clear();
	_instances_new.clear();
}
/*
Instance* Scene::CreateInstance(Instance* Instance)
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
*/