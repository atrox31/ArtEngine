#include "Scene.h"

#include "ArtCore/Functions/Convert.h"
#include "ArtCore/Functions/Func.h"
#include "ArtCore/CodeExecutor/CodeExecutor.h"
#include "ArtCore/Enums/Event.h"
#include "ArtCore/System/Core.h"
#include "ArtCore/System/AssetManager.h"
#include "ArtCore/ShortTypenames.h"

#include "nlohmann/json.hpp"
#include "physfs-release-3.2.0/src/physfs.h"
using nlohmann::json;

Scene::Scene()
{
	_instances_size = 0;
	_is_any_new_instances = false;
	_enable_camera = false;
	_width = 1;
	_height = 1;
	BackGround.Type = Scene::BackGround::BType::DrawColor;
	BackGround.Color = SDL_Color({ 0,0,0,255 });
	BackGround.Texture = nullptr;
	BackGround.TypeWrap = (BackGround::BTypeWrap)0;
	_level_current = -1;
	_level_last = -1;
	_view_rect = {};
}


Scene::~Scene()
{
	_trigger_data.clear();
	delete _variables_holder;

	if (!_instance_colony.empty()) {
		for (plf_it<Instance*> it = _instance_colony.begin(); it != _instance_colony.end(); ) {

			delete (*it);
			it = _instance_colony.erase(it);
		}
	}
	_instance_colony.clear();

	if (!_instances_new.empty()) {
		for (vec_it<Instance*> it = _instances_new.begin(); it != _instances_new.end(); ) {
			delete (*it);
			it = _instances_new.erase(it);
		}
	}
	_instances_new.clear();

	if(_current_scene != nullptr && _current_scene == this)
	{
		_current_scene = nullptr;
	}
}


bool Scene::LoadFromFile(const std::string& file)
{
	Sint64 len(0);
	const char* buffer = Func::ArchiveGetFileBuffer("scene/" + file + "/" + file + ".asd", &len);
	Func::DataValues dv(buffer, len);
	if (!dv.IsOk()) {
		return false;
	}
	_width = Func::TryGetInt(dv.GetData("setup", "ViewWidth"));
	_height = Func::TryGetInt(dv.GetData("setup", "ViewHeight"));
	_begin_trigger = dv.GetData("setup", "SceneStartingTrigger");
	_name = file;
	_enable_camera = Convert::Str2Bool(dv.GetData("setup", "EnableCamera"));
	//TODO if camera system be implemented
	//_starting_x = Func::TryGetInt(dv.GetData("setup", "StartX"));
	//_starting_y = Func::TryGetInt(dv.GetData("setup", "StartY"));
	//_view_rect = { _starting_x, _starting_y, _width, _height };
	_view_rect = { 0, 0, _width, _height };

	// get scene background type
	if (dv.GetData("setup", "BackGroundType") == "DrawColor") {
		BackGround.Type = Scene::BackGround::BType::DrawColor;
		BackGround.Color = Convert::Hex2Color(dv.GetData("setup","BackGroundColor"));
		BackGround.Texture = nullptr;
	}
	else if (dv.GetData("setup", "BackGroundType") == "DrawTexture") {
		BackGround.Type = Scene::BackGround::BType::DrawTexture;
		BackGround.TypeWrap = Scene::BackGround::BTypeWrap_fromString(dv.GetData("setup", "BackGroundWrapMode"));
		BackGround.Texture = Core::GetAssetManager()->GetTexture(dv.GetData("setup", "BackGroundTexture"));
		if (BackGround.Texture == nullptr) {
			Console::WriteLine("Background texture not exists '" + dv.GetData("setup","BackGroundTexture") + "'");
			BackGround.SetDefault();
		}
	}
	else {
		Console::WriteLine("new_scene.BackGround.type unknown");
		BackGround.SetDefault();
	}

	// regions
	//for (std::string region : dv.GetSection("regions")) {}
	// triggers
	//for (std::string trigger : dv.GetSection("triggers")) {}

	// instances
	for (std::string& instance : dv.GetSection("instance")) {
		str_vec data = Func::Split(instance, '|');
		if (data.size() != 3) {
			Console::WriteLine("Instance error: '" + instance + "'");
			continue;
		}
		// spawn instance to scene but get copy to first instance list
		_instances_scene.emplace_back(
			CreateInstance(
				data[0],
				static_cast<float>(Func::TryGetInt(data[1])),
				static_cast<float>(Func::TryGetInt(data[2])),
				this
			)
		);
	}

	// gui
	if (const char* gui_schema_json_buffer = Func::ArchiveGetFileBuffer("scene/" + _name + "/GuiSchema.json", nullptr);
		gui_schema_json_buffer != nullptr)
	{
		if (const json data = json::parse(gui_schema_json_buffer);
			!data.is_object())
		{
			Console::WriteLine("GuiSchema error");
			return false;
		}
		else
		{
			if (!_gui_system.LoadFromJson(data)) return false;
		}
	}

	// get gui triggers
	if (PHYSFS_exists(std::string("scene/" + _name + "/scene_triggers.acp").c_str()))
	{
		if (!Core::Executor()->LoadSceneTriggers(this))
		{
			return false;
		}
	}

	return true;
}

bool Scene::SpawnLevelInstances(Scene* target) const
{
	if (target == nullptr) target = _current_scene;
	if (target == nullptr) return false;

	const std::string level_path = std::format("scene/{}/instances_level{}.txt", target->_name, target->_level_current);
	if (!PHYSFS_exists(level_path.c_str())) return false;

	for (std::string& instance_line : Func::ArchiveGetFileText(level_path, nullptr))
	{
		// big if statement but why make 3 alone ifs?
		if (str_vec instance = Func::Split(instance_line, '|');		// get data from line name|x|y
			instance.size() != 3											// data must be 3 size
			|| CreateInstance(												// create target instance from data
				instance[0],
				static_cast<float>(Func::TryGetInt(instance[1])),
				static_cast<float>(Func::TryGetInt(instance[2])),
				target
			) == nullptr)													// check if spawned, nullptr on error
		{
			Console::WriteLine("Instance error: '" + instance_line + "'"); // write error message but this is not end of world, continue
		}
	}

	return true;
}

bool Scene::Create(const std::string& name)
{
	Scene* new_scene = new Scene();
	if(!new_scene->LoadFromFile(name))
	{
		delete new_scene;
		return false;
	}
	
	// count levels, bad method but safe
	for(int i=0; i<SCENE_MAX_LEVEL_COUNT; i++)
	{
		if (PHYSFS_exists(std::format("scene/{}/instances_level{}.txt", new_scene->_name,i).c_str()))
		{
			new_scene->_level_last = i;
			continue;
		}
		break;
	}

	// all ok swap scenes
	delete _current_scene;
	_current_scene = new_scene;

	// if scene have at least one level set to first (0) else -1 <- scene have no levels
	_current_scene->_level_current = _current_scene->_level_last >= 0 ? 0 : -1;

	_current_scene->SpawnAll(); // scene instances
	Core::Executor()->ExecuteCode(_current_scene->_variables_holder, _current_scene->GetTriggerData(_current_scene->_begin_trigger));
	_current_scene->SpawnAll(); // scene on create trigger may have some instances to spawn
	
	return true;
}

plf_it<Instance*> Scene::ColonyErase(const plf_it<Instance*>& it)
{
	_current_scene->_instances_size--;
	// if instance is scene scene instance mark as dead.
	// respawn on scene reset

	if (!_current_scene->_instances_scene.empty()
		&& std::ranges::find(_current_scene->_instances_scene, *it) != _current_scene->_instances_scene.end())
	{
		(*it)->Alive = false;
	}

	return _current_scene->_instance_colony.erase(it);
}

void Scene::Reset(const bool hard_reset)
{
	ClearListNewInstance(_current_scene);
	if (hard_reset) {
		ClearListColonyInstance(_current_scene);
		for (Instance* instances_scene : _current_scene->_instances_scene)
		{
			CreateInstance(instances_scene);
		}
	}
	else {
		// spawn only dead instances
		for (Instance* instances_scene : _current_scene->_instances_scene)
		{
			if (!instances_scene->Alive)
			{
				instances_scene->Alive = true;
				_current_scene->_instances_new.emplace_back(instances_scene);
				_current_scene->_is_any_new_instances = true;
			}
		}
	}

	// spawn level instances
	_current_scene->SpawnLevelInstances();
	_current_scene->SpawnAll();	// level instances
}

bool Scene::Start(const int level)
{
	if (level > _current_scene->_level_last) return false; // level not exists
	_current_scene->_level_current = level;
	Reset(false);
	return true;
}

bool Scene::StartNextLevel()
{
	if(_current_scene->_level_current < _current_scene->_level_last)
	{
		_current_scene->_level_current++;
		Reset(false);
		return true;
	}
	return false;
}

bool Scene::HaveNextLevel()
{
	return _current_scene->_level_current < _current_scene->_level_last;
}
Instance* Scene::CreateInstance(const std::string& name, const float x, const float y, Scene* target)
{
	Instance* ins = Core::Executor()->SpawnInstance(name);
	if (ins == nullptr) return nullptr;
	ins->PosX = x;
	ins->PosY = y;
	if (target == nullptr) {
		_current_scene->_instances_new.emplace_back(ins);
		_current_scene->_is_any_new_instances = true;
	}else
	{
		target->_instances_new.emplace_back(ins);
		target->_is_any_new_instances = true;
	}
	return ins;
}

Instance* Scene::CreateInstance(Instance* instance, Scene* target)
{
	if (target == nullptr) {
		_current_scene->_instances_new.emplace_back(instance);
		_current_scene->_is_any_new_instances = true;
		return _current_scene->_instances_new.back();
	}
	else
	{
		target->_instances_new.emplace_back(instance);
		target->_is_any_new_instances = true;
		return target->_instances_new.back();
	}
}

void Scene::SpawnAll()
{
	/*
	 Problem: when instance on create get reference to another created instance cannot find it
	 Fix: First create all instances, after that execute onCreate on all of it
	 if instance in onCreate script have to get reference to own 'child' it must be
	 assign to variable. Instance reference in Instance variables is const, always valid.
	 If instance on spawn create another instance, 'child' will be born in next frame
	 */
	if (_is_any_new_instances) {
		const size_t new_ins_size = _instances_new.size();
		for(size_t i = 0; i < new_ins_size; i++){
			_instance_colony.insert(_instances_new[i]);
			_instances_size++;
		}
		for (size_t i = 0; i < new_ins_size; i++) {
			Core::Executor()->ExecuteScript(_instances_new[i], Event::EvOnCreate);
		}
		_instances_new.erase(_instances_new.begin(), _instances_new.begin() + static_cast<long long>(new_ins_size));
		_is_any_new_instances = !_instances_new.empty();
	}
}
Instance* Scene::GetInstanceById(const int id)
{
	for (Instance* instance : _current_scene->_instance_colony) {
		if (instance->GetId() == static_cast<Uint64>(id)) return instance;
	}
	return nullptr;
}
Instance* Scene::GetInstanceByTag(const std::string& tag)
{
	for (Instance* instance : _current_scene->_instance_colony) {
		if (instance->Tag == tag) return instance;
	}
	return nullptr;
}
Instance* Scene::GetInstanceByName(const std::string& name)
{
	for (Instance* instance : _current_scene->_instance_colony) {
		if (instance->Name == name) return instance;
	}
	return nullptr;
}
std::vector<Instance*> Scene::GetInstancesByTag(const std::string& tag)
{
	std::vector<Instance*> _return;
	for (Instance* instance : _current_scene->_instance_colony) {
		if (instance->Tag == tag) _return.push_back(instance);
	}
	return _return;
}
std::vector<Instance*> Scene::GetInstancesByName(const std::string& name)
{
	std::vector<Instance*> _return;
	for (Instance* instance : _current_scene->_instance_colony) {
		if (instance->Name == name) _return.push_back(instance);
	}
	return _return;
}

void Scene::SetTriggerData(const std::string& trigger, const unsigned char* data, Sint64 length)
{
	_trigger_data[trigger] = std::pair< const unsigned char*, Sint64>(data, length);
}

std::pair<const unsigned char*, Sint64>* Scene::GetTriggerData(const std::string& trigger)
{
	if(_trigger_data.contains(trigger))
	{
		return &_trigger_data[trigger];
	}
	return nullptr;
}

void Scene::Delete()
{
	delete _current_scene;
}

void Scene::ClearListNewInstance(Scene* target)
{
	if (!target->_instances_new.empty()) {
		for (vec_it<Instance*> it = target->_instances_new.begin(); it != target->_instances_new.end(); ) {
			delete (*it);
			it = target->_instances_new.erase(it);
		}
	}
	target->_instances_new.clear();
}

void Scene::ClearListColonyInstance(Scene* target)
{
	if (!target->_instance_colony.empty()) {
		for (plf_it<Instance*> it = target->_instance_colony.begin(); it != target->_instance_colony.end(); ) {
			delete (*it);
			it = target->_instance_colony.erase(it);
		}
	}
	target->_instance_colony.clear();
}
