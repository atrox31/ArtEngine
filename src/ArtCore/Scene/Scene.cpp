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
	CurrentCollisionInstanceId = -1;
	CurrentCollisionInstance = nullptr;
	_view_rect = {};
	//GuiSystem = Gui();
}

void Scene::Clear()
{
	if (!InstanceColony.empty()) {
		for (plf_it<Instance*> it = InstanceColony.begin(); it != InstanceColony.end(); ) {
			
			delete (*it);
			it = InstanceColony.erase(it);
		}
	}
	InstanceColony.clear();

	if (!_instances_new.empty()) {
		for (vec_it<Instance*> it = _instances_new.begin(); it != _instances_new.end(); ) {
			delete (*it);
			it = _instances_new.erase(it);
		}
	}
	_instances_new.clear();
}
Scene::~Scene()
{
	_trigger_data.clear();
	
	Clear();
}
bool Scene::Load(const std::string& name)
{
	Sint64 len(0);
	const char* buffer = Func::ArchiveGetFileBuffer("scene/" + name + "/" + name + ".asd", &len);
	Func::DataValues dv(buffer, len);
	if (!dv.IsOk()) {
		return false;
	}
	_width = Func::TryGetInt(dv.GetData("setup", "ViewWidth"));
	_height = Func::TryGetInt(dv.GetData("setup", "ViewHeight"));
	_begin_trigger = dv.GetData("setup", "SceneStartingTrigger");
	_name = name;
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
	for (std::string region : dv.GetSection("regions")) {

	}

	// triggers
	for (std::string trigger : dv.GetSection("triggers")) {

	}

	// instances
	for (std::string& instance : dv.GetSection("instance")) {
		str_vec data = Func::Split(instance, '|');
		if (data.size() != 3) {
			Console::WriteLine("Instance error: '" + instance + "'");
			continue;
		}
		_begin_instances.emplace_back( data[0], Func::TryGetInt(data[1]), Func::TryGetInt(data[2]) );
	}

	const char* gui_schema_json_buffer = Func::ArchiveGetFileBuffer("scene/" + _name + "/GuiSchema.json", nullptr);
	if (gui_schema_json_buffer == nullptr)
	{
		return true;
	}
	const json data = json::parse(gui_schema_json_buffer);
	if (!data.is_object())
	{
		Console::WriteLine("GuiSchema error");
	}
	else
	{
		if (!GuiSystem.LoadFromJson(data)) return false;
	}
	return true;
}
bool Scene::Start()
{
	_trigger_data.clear();
	bool have_triggers = false;
	// get gui triggers
	if (PHYSFS_exists(std::string("scene/" + _name + "/scene_triggers.acp").c_str()))
	{
		if(!Core::Executor()->LoadSceneTriggers())
		{
			return false;
		}
		have_triggers = true;
	}
	Clear();
	for (const StartingInstanceSpawner& instance : _begin_instances) {
		CreateInstance(instance.instance, static_cast<float>(instance.x), static_cast<float>(instance.y));
	}
	if(have_triggers && _begin_trigger.length() > 0)
	{
		Core::Executor()->ExecuteCode(_variables_holder, GetTriggerData(_begin_trigger));
	}
	return true;
}
Instance* Scene::CreateInstance(const std::string& name, const float x, const float y)
{
	Instance* ins = Core::Executor()->SpawnInstance(name);
	if (ins == nullptr) return nullptr;
	ins->PosX = x;
	ins->PosY = y;
	_instances_new.push_back(ins);
	_is_any_new_instances = true;
	return ins;
}

bool Scene::InView(const SDL_FPoint& p) const
{
	return _view_rect.PointInRectWh(p);
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
			InstanceColony.insert(_instances_new[i]);
			_instances_size++;
		}
		for (size_t i = 0; i < new_ins_size; i++) {
			Core::Executor()->ExecuteScript(_instances_new[i], Event::EvOnCreate);
		}
		_instances_new.erase(_instances_new.begin(), _instances_new.begin() + new_ins_size);
		_is_any_new_instances = !_instances_new.empty();
	}
}
void Scene::Exit()
{
	Clear();
	// TODO: triggers
	// TODO: regions
}

Instance* Scene::GetInstanceById(const int id)
{
	for (Instance* instance : InstanceColony) {
		if (instance->GetId() == static_cast<Uint64>(id)) return instance;
	}
	return nullptr;
}
Instance* Scene::GetInstanceByTag(const std::string& tag)
{
	for (Instance* instance : InstanceColony) {
		if (instance->Tag == tag) return instance;
	}
	return nullptr;
}
Instance* Scene::GetInstanceByName(const std::string& name)
{
	for (Instance* instance : InstanceColony) {
		if (instance->Name == name) return instance;
	}
	return nullptr;
}
std::vector<Instance*> Scene::GetInstancesByTag(const std::string& tag)
{
	std::vector<Instance*> _return;
	for (Instance* instance : InstanceColony) {
		if (instance->Tag == tag) _return.push_back(instance);
	}
	return _return;
}
std::vector<Instance*> Scene::GetInstancesByName(const std::string& name)
{
	std::vector<Instance*> _return;
	for (Instance* instance : InstanceColony) {
		if (instance->Name == name) _return.push_back(instance);
	}
	return _return;
}

plf::colony<Instance*>::iterator Scene::DeleteInstance(const plf::colony<Instance*>::iterator& ptr)
{
	_instances_size--;
	return InstanceColony.erase(ptr);
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
