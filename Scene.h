#pragma once
#include "Core.h"
#include "Rect.h"
#include "Event.h"
#include "Instance.h"
#include "plf_colony-master/plf_colony.h"
class Instance;
class Scene
{
	
public:
	Scene();
	virtual ~Scene();

	int SceneStep();
	int SceneDraw();

	//Instance* CreateInstance(Instance*);
	//void DeleteInstance(Instance*);
	bool IsAnyInstances() { return _instances_size > 0; }

	private:
	int _instances_size;
	plf::colony<Instance*> _instances;
	std::vector<Instance*> _instances_new;
};

