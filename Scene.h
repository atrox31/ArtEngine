#pragma once
#include "Core.h"
#include "Rect.h"
#include "Event.h"
#include "Gui.h"
#include "Instance.h"
#include "plf_colony-master/plf_colony.h"
class Instance;
class Scene final
{
	
public:
	Scene();
	~Scene();
	bool Load(std::string name);
	void Clear();

	// also reset
	void Start();
	void Exit();

	Instance* CreateInstance(std::string name, float x, float y);
private:
	bool _is_any_new_instances;
public:
	void SpawnAll();

	std::string GetName() {
		return _name;
	}

	Instance* GetInstanceById(int);
	Instance* GetInstanceByTag(std::string);
	Instance* GetInstanceByName(std::string);

	std::vector < Instance* > GetInstancesByTag(std::string);
	std::vector < Instance* > GetInstancesByName(std::string);

	// collision
	Uint64 CurrentCollisionInstanceId;
	Instance* CurrentCollisionInstance;

	Gui GuiSystem;
private:
	int _width;
	int _height;
	std::string _name;
public:
	struct BackGround {
	public:
		GPU_Image* texture;
		SDL_Color color;
		enum class BType {
			DrawColor, DrawTexture
		} type;
		enum class BTypeWrap {
			Tile,
			TileFlipX,
			TileFlipY,
			TileFlipXY
		} TypeWrap;
	} BackGround;

	// staring
private:
	struct StartingInstanceSpawner {
	public:
		std::string instance;
		int x;
		int y;
	};
	std::vector<StartingInstanceSpawner> _begin_instances;

	// instances
public:
	bool IsAnyInstances() const { return _instances_size > 0; }
	int GetInstancesCount() const {return _instances_size; }
	plf::colony<Instance*> InstanceColony;
	plf::colony<Instance*>::iterator DeleteInstance(plf::colony<Instance*>::iterator ptr);
private:
	int _instances_size;
	std::vector<Instance*> _instances_new;

};

