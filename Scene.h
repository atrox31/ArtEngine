#pragma once
#include "Core.h"
#include "Rect.h"
#include "Event.h"
class Scene
{
	class Instance {
	public:
		Instance();
		virtual ~Instance();
		Uint64 GetId() { return _id; };
		void Delete() { Alive = false; };

		bool Alive;
		std::string Tag;
		bool InView;
		bool E_MaskClicked = false;
		Rect Mask;

	private:
		Uint64 _id;
		static Uint64 _cid;
	};
public:
	Scene();
	virtual ~Scene();

	virtual void OnEnter() {};
	virtual void OnExit() {};

	Scene::Instance* CreateInstance(Instance*);
	void DeleteInstance(Instance*);
	bool IsAnyInstances() { return _instances_size > 0; }

	//private:
	int _instances_size;
	plf::colony<Instance*> _instances;
	std::vector<Instance*> _instances_new;
};

