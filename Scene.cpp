#include "Scene.h"
#include "Debug.h"
#include "Event.h"
#include "CodeExecutor.h"
#include "Convert.h"

Scene::Scene()
{
	_instances_size = 0;
	_is_any_new_instances = false;
	Width = 1;
	Height = 1;
	BackGround.type = Scene::BackGround::BType::DrawColor;
	BackGround.color = SDL_Color({ 0,0,0,255 });
	BackGround.texture = nullptr;
	BackGround.type_wrap = (BackGround::BTypeWrap)0;
	CurrentCollisionInstanceId = -1;
	CurrentCollisionInstance = nullptr;
}

void Scene::Clear()
{
	if (InstanceColony.size() > 0) {
		for (plf::colony<Instance*>::iterator it = InstanceColony.begin(); it != InstanceColony.end(); ) {
			
			delete (*it);
			it = InstanceColony.erase(it);
		}
	}
	InstanceColony.clear();

	if (_instances_new.size() > 0) {
		for (std::vector<Instance*>::iterator it = _instances_new.begin(); it != _instances_new.end(); ) {
			delete (*it);
			it = _instances_new.erase(it);
		}
	}
	_instances_new.clear();
}
Scene::~Scene()
{
	Clear();
}
bool Scene::Load(std::string name)
{
	Sint64 len(0);
	const char* data = Func::GetFileBuf(name, &len);
	Func::DataValues dv(data, (int)len);
	if (!dv.IsOk()) {
		return false;
	}
	Width = Func::TryGetInt(dv.GetData("setup", "Width"));
	Height = Func::TryGetInt(dv.GetData("setup", "Height"));
	auto tmp = Func::Split(name, '/');
	Name = tmp[tmp.size() - 1].substr(0, tmp[tmp.size() - 1].length() - 4);

	if (dv.GetData("setup", "BackGroundType") == "DrawColor") {
		BackGround.type = Scene::BackGround::BType::DrawColor;
		BackGround.color = Convert::Str2Color(dv.GetData("setup", "BackGroundColor"));
	}else if (dv.GetData("setup", "BackGroundType") == "DrawTexture") {
		BackGround.type = Scene::BackGround::BType::DrawTexture;
		if (dv.GetData("setup", "BackGroundWrapMode") == "Tile") {
			BackGround.type_wrap = Scene::BackGround::BTypeWrap::Tile;
		}
		else
		if (dv.GetData("setup", "BackGroundWrapMode") == "TileFlipX") {
			BackGround.type_wrap = Scene::BackGround::BTypeWrap::TileFlipX;
		}
		else
		if (dv.GetData("setup", "BackGroundWrapMode") == "TileFlipY") {
			BackGround.type_wrap = Scene::BackGround::BTypeWrap::TileFlipY;
		}
		else
		if (dv.GetData("setup", "BackGroundWrapMode") == "TileFlipXY") {
			BackGround.type_wrap = Scene::BackGround::BTypeWrap::TileFlipXY;
		}
		else {
			Debug::WARNING("new_scene.BackGround.type_wrap unknown");
			BackGround.type_wrap = Scene::BackGround::BTypeWrap::Tile;
		}
		BackGround.texture = Core::GetInstance()->assetManager->GetTexture(dv.GetData("setup", "BackGroundTexture"));
		if (BackGround.texture == nullptr) {
			Debug::WARNING("Background texture not exists '"+ dv.GetData("setup", "BackGroundTexture_name") + "'");
			BackGround.type = Scene::BackGround::BType::DrawColor;
			BackGround.color = SDL_Color({ 0, 0, 0, 255 });
		}
		//BackGroundWrapMode   type_wrap
	}
	else {
		Debug::WARNING("new_scene.BackGround.type unknown");
		BackGround.type = Scene::BackGround::BType::DrawColor;
		BackGround.color = SDL_Color({ 0,0,0,255 });
	}

	// regions
	for (std::string region : dv.GetSection("regions")) {

	}

	// triggers
	for (std::string trigger : dv.GetSection("triggers")) {

	}

	// instances
	for (std::string& instance : dv.GetSection("instance")) {
		std::vector<std::string> data = Func::Split(instance, '|');
		if (data.size() != 3) {
			Debug::WARNING("Instance error: '" + instance + "'");
			continue;
		}
		BeginInstances.push_back({ data[0], Func::TryGetInt(data[1]), Func::TryGetInt(data[2]) });
	}

	return true;
}
void Scene::Start()
{
	Clear();
	for (Spawner& instance : BeginInstances) {
		CreateInstance(instance._instance, (float)instance.x, (float)instance.y);
	}
}
Instance* Scene::CreateInstance(std::string name, float x, float y)
{
	Instance* ins = Core::GetInstance()->Executor.SpawnInstance(name);
	if (ins == nullptr) return nullptr;
	ins->PosX = x;
	ins->PosY = y;
	_instances_new.push_back(ins);
	_is_any_new_instances = true;
	return ins;
}
void Scene::SpawnAll()
{
	if (_is_any_new_instances) {
		size_t ins_siz = _instances_new.size();
		while (ins_siz) {
			Core::GetInstance()->Executor.ExecuteScript(_instances_new.back(), Event::EvOnCreate);
			InstanceColony.insert(_instances_new.back());
			_instances_new.pop_back();
			--ins_siz;
			_instances_size++;
		}
		_is_any_new_instances = false;
	}
}
void Scene::Exit()
{
	Clear();
	// TODO: triggers
	// TODO: regions
}

Instance* Scene::GetInstanceById(int id)
{
	for (Instance* instance : InstanceColony) {
		if (instance->GetId() == id) return instance;
	}
	return nullptr;
}
Instance* Scene::GetInstanceByTag(std::string tag)
{
	for (Instance* instance : InstanceColony) {
		if (instance->Tag == tag) return instance;
	}
	return nullptr;
}
Instance* Scene::GetInstanceByName(std::string name)
{
	for (Instance* instance : InstanceColony) {
		if (instance->Name == name) return instance;
	}
	return nullptr;
}
std::vector<Instance*> Scene::GetInstancesByTag(std::string tag)
{
	std::vector<Instance*> _return;
	for (Instance* instance : InstanceColony) {
		if (instance->Tag == tag) _return.push_back(instance);
	}
	return _return;
}
std::vector<Instance*> Scene::GetInstancesByName(std::string name)
{
	std::vector<Instance*> _return;
	for (Instance* instance : InstanceColony) {
		if (instance->Name == name) _return.push_back(instance);
	}
	return _return;
}

plf::colony<Instance*>::iterator Scene::DeleteInstance(plf::colony<Instance*>::iterator ptr)
{
	_instances_size--;
	return InstanceColony.erase(ptr);
}
