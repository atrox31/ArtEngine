#include "AssetManager.h"
#include "Debug.h"
#define elseif else if

AssetManager::AssetManager()
{
	List_sprite_id = std::vector<Sprite*>();
	List_texture_id = std::vector<GPU_Image*>();
	List_music_id = std::vector<Mix_Music*>();
	List_sound_id = std::vector<Mix_Chunk*>();
	List_font_id = std::vector<FC_Font*>();

	List_sprite_name = std::map < std::string, Sprite*>();
	List_texture_name = std::map < std::string, GPU_Image*>();
	List_music_name = std::map < std::string, Mix_Music*>();
	List_sound_name = std::map < std::string, Mix_Chunk*>();
	List_font_name = std::map < std::string, FC_Font*>();
}

void AssetManager::ClearData()
{
	for (auto v : List_sprite_id) {
		Sprite::Delete(v);
	}
	for (auto v : List_texture_id) {
		GPU_FreeImage(v);
	}
	for (auto v : List_music_id) {
		Mix_FreeMusic(v);
	}
	for (auto v : List_sound_id) {
		Mix_FreeChunk(v);
	}
	for (auto v : List_font_id) {
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
}
AssetManager::~AssetManager()
{
	ClearData();
}

bool AssetManager::LoadData(BackGroundRenderer* bgr, int p_min, int p_max)
{
	
	Sint64 buffer_size(0);
	std::string buffer = std::string(Func::GetFileBuf("filelist.txt", &buffer_size));
	if (buffer_size == 0) return false;

	std::vector<std::string> data = Func::Explode(buffer, '\n');
	if (data.size() == 0) return false;

	// oczyszcza z \r na koñcu
	for (std::string& file : data) {
		if(file[file.length()-1] == '\r')
		file = file.substr(0, file.length() - 1);
		Func::replace_all(file, "\\", "/");
	}

	float c_pos = 0;
	float m_pos = (float)data.size();
	float fp_min = (float)p_min;
	float fp_max = (float)p_max;

	for (std::string& file : data) {
		std::vector<std::string> path = Func::Split(file, '/');
		if (path.size() == 0) return false;
		
		if (path[0] == "Textures") {

			std::string name = Func::Explode(path[1], '.')[0];
			GPU_Image* tmp = GPU_LoadImage_RW(Func::GetFileRWops(file, nullptr), 1);
			if (tmp == nullptr) return false;
			List_texture_id.push_back(tmp);
			List_texture_name.insert({ name, tmp });
			Debug::LOG("AssetManager::LoadData - file loaded: " + file);
			bgr->SetProgress((int) Func::LinearScale(c_pos++, 0.0f, m_pos, fp_min, fp_max) );

		}elseif(path[0] == "Sprites") {

			std::string name = Func::Explode(path[1], '.')[0];
			Sint64 size(0);
			// size
			const char* buffer = Func::GetFileBuf(file, &size);
			Sprite* tmp = Sprite::Load(buffer, size);
			if (tmp == nullptr) return false;
			List_sprite_id.push_back(tmp);
			List_sprite_name.insert({ name, tmp });
			Debug::LOG("AssetManager::LoadData - file loaded: " + file);
			bgr->SetProgress((int)Func::LinearScale(c_pos++, 0.0f, m_pos, fp_min, fp_max));

		}elseif(path[0] == "Music") {

			std::string name = Func::Explode(path[1], '.')[0];
			Mix_Music* tmp = Mix_LoadMUS_RW(Func::GetFileRWops(file, nullptr), 1);
			if (tmp == nullptr) return false;
			List_music_id.push_back(tmp);
			List_music_name.insert({ name, tmp });
			Debug::LOG("AssetManager::LoadData - file loaded: " + file);
			bgr->SetProgress((int)Func::LinearScale(c_pos++, 0.0f, m_pos, fp_min, fp_max));

		}elseif(path[0] == "Sounds") {

			std::string name = Func::Explode(path[1], '.')[0];
			Mix_Chunk* tmp = Mix_LoadWAV_RW(Func::GetFileRWops(file, nullptr), 1);
			if (tmp == nullptr) return false;
			List_sound_id.push_back(tmp);
			List_sound_name.insert({ name, tmp });
			Debug::LOG("AssetManager::LoadData - file loaded: " + file);
			bgr->SetProgress((int)Func::LinearScale(c_pos++, 0.0f, m_pos, fp_min, fp_max));

		}elseif(path[0] == "Fonts") {

			std::string name = Func::Explode(path[1], '.')[0];
			FC_Font* tmp = FC_CreateFont();
			FC_LoadFont_RW(tmp, Func::GetFileRWops(file, nullptr), 1, 12, { 255,255,255 }, TTF_STYLE_NORMAL);
			if (tmp == nullptr) return false;
			List_font_id.push_back(tmp);
			List_font_name.insert({ name, tmp });
			Debug::LOG("AssetManager::LoadData - file loaded: " + file);
			bgr->SetProgress((int)Func::LinearScale(c_pos++, 0.0f, m_pos, fp_min, fp_max));

		}
	}
	bgr->SetProgress((int)Func::LinearScale(fp_max, 0.0f, fp_max, fp_min, fp_max));
	return true;
}


Sprite* AssetManager::GetSprite(int id)
{
	return nullptr;
}

Sprite* AssetManager::GetSprite(std::string name)
{
	return nullptr;
}

GPU_Image* AssetManager::GetTexture(int id)
{
	return nullptr;
}

GPU_Image* AssetManager::GetTexture(std::string name)
{
	return nullptr;
}

Mix_Music* AssetManager::GetMusic(int id)
{
	return nullptr;
}

Mix_Music* AssetManager::GetMusic(std::string name)
{
	return nullptr;
}

Mix_Chunk* AssetManager::GetSound(int id)
{
	return nullptr;
}

Mix_Chunk* AssetManager::GetSound(std::string name)
{
	return nullptr;
}

FC_Font* AssetManager::GetFont(int id)
{
	return nullptr;
}

FC_Font* AssetManager::GetFont(std::string name)
{
	return nullptr;
}
