#pragma once
#include <vector>

#include "Instance.h"
#include "ArtCore/Gui/Gui.h"

#include "plf/plf_colony-master/plf_colony.h"

class Instance;
class Scene final
{
	
public:
	Scene();
	~Scene();
	bool Load(const std::string& name);
	void Clear();

	// also reset
	bool Start();
	void Exit();

	Instance* CreateInstance(const std::string& name, float x, float y);

	[[nodiscard]] int GetWidth() const
	{
		return _width;
	}

	[[nodiscard]] int GetHeight() const
	{
		return _height;
	}

	bool InView(const SDL_FPoint& p) const;

private:
	bool _is_any_new_instances;
	bool _enable_camera;
	Rect _view_rect;

public:
	void SpawnAll();

	std::string GetName() {
		return _name;
	}

	Instance* GetInstanceById(int);
	Instance* GetInstanceByTag(const std::string&);
	Instance* GetInstanceByName(const std::string&);

	std::vector< Instance* > GetInstancesByTag(const std::string&);
	std::vector < Instance* > GetInstancesByName(const std::string&);

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
		GPU_Image* Texture;
		SDL_Color Color;
		enum class BType {
			DrawColor, DrawTexture
		} Type;

		ENUM_WITH_STRING_CONVERSION(BTypeWrap,(Tile)(TileFlipX)(TileFlipY)(TileFlipXY))
		BTypeWrap TypeWrap;
		void SetDefault()
		{
			Type = Scene::BackGround::BType::DrawColor;
			Color = SDL_Color({ 0, 0, 0, 255 });
		}
	} BackGround{};

	// staring
private:
	struct StartingInstanceSpawner {
	public:
		std::string instance;
		int x;
		int y;
	};
	std::vector<StartingInstanceSpawner> _begin_instances{};

	// instances
public:
	[[nodiscard]] bool IsAnyInstances() const { return _instances_size > 0; }
	[[nodiscard]] int GetInstancesCount() const;
	plf::colony<Instance*> InstanceColony{};
	plf::colony<Instance*>::iterator DeleteInstance(const plf::colony<Instance*>::iterator& ptr);
private:
	int _instances_size;
	std::vector<Instance*> _instances_new{};
private:
	// Object that holds all scene variables, must be this way
	// because script system is designed this way
	Instance* _variables_holder = nullptr;
public:
	void SetVariableHolder(Instance* vh) {
		// if exists delete previous
		delete _variables_holder;  _variables_holder = vh;
	}

	[[nodiscard]] Instance* GetVariableHolder() const
	{
		return _variables_holder;
	}
public:
	void SetTriggerData(const std::string& trigger, const unsigned char* data, Sint64 length);
	std::pair<const unsigned char*, Sint64>* GetTriggerData(const std::string& trigger);
private:
	std::unordered_map<std::string, std::pair<const unsigned char*, Sint64>> _trigger_data{};
	std::string _begin_trigger;
};

inline int Scene::GetInstancesCount() const
{return _instances_size; }

