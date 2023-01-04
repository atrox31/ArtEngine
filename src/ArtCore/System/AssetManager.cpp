#include "assetManager.h"

#include "ArtCore/Functions/Func.h"
#include "ArtCore/_Debug/Debug.h"
#include "ArtCore/Graphic/BackGroundRenderer.h"

#include <ranges>

AssetManager::AssetManager()
= default;

void AssetManager::ClearData()
{
	for (const auto v : List_sprite_id) {
		Debug::NOTE_DEATH("[AssetManager::ClearData]: ~Sprite");
		Sprite::Delete(v);
	}
	for (const auto v : List_texture_id) {
		Debug::NOTE_DEATH("[AssetManager::ClearData]: ~Texture");
		GPU_FreeImage(v);
	}
	for (const auto v : List_music_id) {
		Debug::NOTE_DEATH("[AssetManager::ClearData]: ~Mix_Music");
		Mix_FreeMusic(v);
	}
	for (const auto v : List_sound_id) {
		Debug::NOTE_DEATH("[AssetManager::ClearData]: ~Mix_Chunk");
		Mix_FreeChunk(v);
	}
	for (const auto v : List_font_id) {
		Debug::NOTE_DEATH("[AssetManager::ClearData]: ~FC_Font");
		FC_FreeFont(v);
	}
	List_sprite_id.clear();
	List_texture_id.clear();
	List_music_id.clear();
	List_sound_id.clear();
	List_font_id.clear();

	List_sprite_name.clear();
	List_texture_name.clear();
	List_music_name.clear();
	List_sound_name.clear();
	List_font_name.clear();
#ifdef _DEBUG
	Debug_List_sprite_name.clear();
	Debug_List_texture_name.clear();
	Debug_List_music_name.clear();
	Debug_List_sound_name.clear();
	Debug_List_font_name.clear();
#endif

}
AssetManager::~AssetManager()
{
	ClearData();
}

bool AssetManager::LoadData(BackGroundRenderer* bgr, const int p_min, const int p_max)
{
	
	Sint64 buffer_size(0);
	const std::string buffer = std::string(Func::GetFileBuf("filelist.txt", &buffer_size));
	if (buffer_size == 0) return true;

	std::vector<std::string> data = Func::Explode(buffer, '\n');
	if (data.empty()) return false;
	
	for (std::string& line : data) {
		if(line[line.length()-1] == '\r')
			line = line.substr(0, line.length() - 1);
		Func::replace_all(line, "\\", "/");
	}
	// current asset
	int c_pos = 0;
	for (std::string& line : data) {
		c_pos++;

		std::vector<std::string> temp = Func::Split(line, ';');
		if (temp.size() != 2) continue;

		const std::string file = temp[0];

		std::vector<std::string> path = Func::Split(temp[0], '/');
		if (path.empty()) continue;

		const std::string normal_name = temp[1];
		
		if (path[0] == "Textures") {
			
			GPU_Image* tmp = GPU_LoadImage_RW(Func::GetFileRWops(file, nullptr), 1);
			if (tmp == nullptr) return false;
			List_texture_name.insert({ normal_name, tmp });
		}else if(path[0] == "Sprites") {
			Sprite* tmp = Sprite::Load(file);
			if (tmp == nullptr) return false;
			List_sprite_name.insert({ normal_name, tmp });
		}else if(path[0] == "Music") {
			
			Mix_Music* tmp = Mix_LoadMUS_RW(Func::GetFileRWops(file, nullptr), 1);
			if (tmp == nullptr) return false;
			List_music_name.insert({ normal_name, tmp });
		}else if(path[0] == "Sounds") {
			
			Mix_Chunk* tmp = Mix_LoadWAV_RW(Func::GetFileRWops(file, nullptr), 1);
			if (tmp == nullptr) return false;
			List_sound_name.insert({ normal_name, tmp });
		}else if(path[0] == "Fonts") {
			
			FC_Font* tmp = FC_CreateFont();
			FC_LoadFont_RW(tmp, Func::GetFileRWops(file, nullptr), 1, 12, { 255,255,255 }, TTF_STYLE_NORMAL);
			if (tmp == nullptr) return false;
			List_font_name.insert({ normal_name, tmp });
		}
			Debug::LOG("AssetManager::LoadData - file loaded: " + file);
			bgr->SetProgress(
				static_cast<int>(Func::LinearScale(
					static_cast<float>(c_pos),
					0.0f,
					static_cast<float>(data.size()),
					static_cast<float>(p_min),
					static_cast<float>(p_max-10)
				))
			);
	}
#ifdef _DEBUG
	// only for debug build get asset names
	for (auto& val : List_sprite_name) {
		List_sprite_id.push_back(val.second);
		Debug_List_sprite_name.push_back(val.first);
	}
	bgr->SetProgress(p_max - 8);
	
	for (auto& val : List_texture_name) {
		List_texture_id.push_back(val.second);
		Debug_List_texture_name.push_back(val.first);
	}
	bgr->SetProgress(p_max - 6);
	
	for (auto& val : List_music_name) {
		List_music_id.push_back(val.second);
		Debug_List_music_name.push_back(val.first);
	}
	bgr->SetProgress(p_max - 4);
	
	for (auto& val : List_sound_name) {
		List_sound_id.push_back(val.second);
		Debug_List_sound_name.push_back(val.first);
	}
	bgr->SetProgress(p_max - 2);
	
	for (auto& val : List_font_name) {
		List_font_id.push_back(val.second);
		Debug_List_font_name.push_back(val.first);
	}
	bgr->SetProgress(p_max - 0);
#else
for (Sprite* val : List_sprite_name | std::views::values) {
		List_sprite_id.push_back(val);
	}
	bgr->SetProgress(p_max - 8);
	
	for (GPU_Image* val : List_texture_name | std::views::values) {
		List_texture_id.push_back(val);
	}
	bgr->SetProgress(p_max - 6);
	
	for (Mix_Music* val : List_music_name | std::views::values) {
		List_music_id.push_back(val);
	}
	bgr->SetProgress(p_max - 4);
	
	for (Mix_Chunk* val : List_sound_name | std::views::values) {
		List_sound_id.push_back(val);
	}
	bgr->SetProgress(p_max - 2);
	
	for (FC_Font* val : List_font_name | std::views::values) {
		List_font_id.push_back(val);
	}
	bgr->SetProgress(p_max - 0);
#endif
	return true;
}


Sprite* AssetManager::GetSprite(const int id) const
{
	if (id < List_sprite_id.size()) {
		return List_sprite_id[id];
	}
	return nullptr;
}

Sprite* AssetManager::GetSprite(const std::string& name)
{
	if (List_sprite_name.contains(name)) {
		return List_sprite_name[name];
	}
	return nullptr;
}

int AssetManager::GetSpriteId(const std::string& name) const
{
	int i = -1;
	for (auto& v : List_sprite_name) {
		if (v.first == name) {
			return ++i;
		}
		++i;
	}
	return -1;
}

GPU_Image* AssetManager::GetTexture(const int id) const
{
	if ((size_t)id < List_texture_id.size()) {
		return List_texture_id[id];
	}
	return nullptr;
}

GPU_Image* AssetManager::GetTexture(const std::string& name)
{
	if (List_texture_name.contains(name)) {
		return List_texture_name[name];
	}
	return nullptr;
}

int AssetManager::GetTextureId(const std::string& name) const
{
	int i = -1;
	for (const auto& key : List_texture_name | std::views::keys) {
		if (key == name) {
			return ++i;
		}
		++i;
	}
	return -1;
}

Mix_Music* AssetManager::GetMusic(const int id) const
{
	if ((size_t)id < List_music_id.size()) {
		return List_music_id[id];
	}
	return nullptr;
}

Mix_Music* AssetManager::GetMusic(const std::string& name)
{
	if (List_music_name.contains(name)) {
		return List_music_name[name];
	}
	return nullptr;
}

int AssetManager::GetMusicId(const std::string& name) const
{
	int i = -1;
	for (auto& v : List_music_name) {
		if (v.first == name) {
			return ++i;
		}
		++i;
	}
	return -1;
}

Mix_Chunk* AssetManager::GetSound(const int id) const
{
	if ((size_t)id < List_sound_id.size()) {
		return List_sound_id[id];
	}
	return nullptr;
}

Mix_Chunk* AssetManager::GetSound(const std::string& name)
{
	if (List_sound_name.contains(name)) {
		return List_sound_name[name];
	}
	return nullptr;
}

int AssetManager::GetSoundId(const std::string& name) const
{
	int i = -1;
	for (auto& v : List_sound_name) {
		if (v.first == name) {
			return ++i;
		}
		++i;
	}
	return -1;
}

FC_Font* AssetManager::GetFont(const int id) const
{
	if ((size_t)id < List_font_id.size()) {
		return List_font_id[id];
	}
	return nullptr;
}

FC_Font* AssetManager::GetFont(const std::string& name)
{
	if (List_font_name.contains(name)) {
		return List_font_name[name];
	}
	return nullptr;
}

int AssetManager::GetFontId(const std::string& name) const
{
	int i = -1;
	for (const auto& key : List_font_name | std::views::keys) {
		if (key == name) {
			return ++i;
		}
		++i;
	}
	return -1;
}