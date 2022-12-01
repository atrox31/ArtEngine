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
		return Name;
	}

	Instance* GetInstanceById(int);
	Instance* GetInstanceByTag(std::string);
	Instance* GetInstanceByName(std::string);

	std::vector < Instance* > GetInstancesByTag(std::string);
	std::vector < Instance* > GetInstancesByName(std::string);

	// collision
	Uint64 CurrentCollisionInstanceId;
	Instance* CurrentCollisionInstance;
private:
	//TODO GUI GuiFile
	int Width;
	int Height;
	std::string Name;
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
		} type_wrap;
	} BackGround;

	// staring
private:
	struct Spawner {
	public:
		std::string _instance;
		int x;
		int y;
	};
	std::vector<Spawner> BeginInstances;

	// instances
public:
	bool IsAnyInstances() { return _instances_size > 0; }
	int GetInstancesCount() {return _instances_size; }
	plf::colony<Instance*> InstanceColony;
	plf::colony<Instance*>::iterator DeleteInstance(plf::colony<Instance*>::iterator ptr);
private:
	int _instances_size;
	std::vector<Instance*> _instances_new;

};

