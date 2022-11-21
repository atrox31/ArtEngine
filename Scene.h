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

		void RegisterEvent(Event ev);
		void UnRegisterEvent(Event ev);

		bool Alive;
		std::string Tag;
		bool InView;
		bool E_MaskClicked = false;
		Rect Mask;

	private:
		//friend Art::Core;
		std::vector< Event > _events_registered;
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

	void RegisterEvent(Event, Instance*);
	void UnRegisterEvent(Event, Instance*);


	//private:
	int _instances_size;
	plf::colony<Instance*> _instances;
	std::vector<Instance*> _instances_new;
	std::map<Event, std::vector<Instance*>> _events;
};

