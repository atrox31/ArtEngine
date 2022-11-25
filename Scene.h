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

	void Start();
	void Exit();

	std::string GetName() {
		return Name;
	}

private:
	//TODO GUI GuiFile
	int Width;
	int Height;
	std::string Name;
private:
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
private:
	int _instances_size;
	plf::colony<Instance*> _instances;
	std::vector<Instance*> _instances_new;

};

