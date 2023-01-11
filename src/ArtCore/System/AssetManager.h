#pragma once
#include <unordered_map>

#include "ArtCore/Functions/Func.h"
#include "ArtCore/Graphic/Sprite.h"
#include "FC_Fontcache/SDL_FontCache.h"

class BackGroundRenderer;
class AssetManager
{
public:
	AssetManager();
	virtual ~AssetManager();

	bool LoadData(BackGroundRenderer* bgr, int p_min, int p_max);
	void ClearData();

	Sprite* GetSprite(int id) const;
	Sprite* GetSprite(const std::string& name);
	int GetSpriteId(const std::string& name) const;
	
	GPU_Image* GetTexture(int id) const;
	GPU_Image* GetTexture(const std::string& name);
	int GetTextureId(const std::string& name) const;
	
	Mix_Music* GetMusic(int id) const;
	Mix_Music* GetMusic(const std::string& name);
	int GetMusicId(const std::string& name) const;
	
	Mix_Chunk* GetSound(int id) const;
	Mix_Chunk* GetSound(const std::string& name);
	int GetSoundId(const std::string& name) const;
	
	FC_Font* GetFont(int id) const;
	FC_Font* GetFont(const std::string& name);
	int GetFontId(const std::string& name) const;

#ifdef _DEBUG
	Func::str_vec Debug_List_sprite_name;
	Func::str_vec Debug_List_texture_name;
	Func::str_vec Debug_List_music_name;
	Func::str_vec Debug_List_sound_name;
	Func::str_vec Debug_List_font_name;
#endif

private:
	std::vector<Sprite*> List_sprite_id;
	std::vector<GPU_Image*> List_texture_id;
	std::vector<Mix_Music*> List_music_id;
	std::vector<Mix_Chunk*> List_sound_id;
	std::vector<FC_Font*> List_font_id;

	std::unordered_map< std::string, Sprite*> List_sprite_name;
	std::unordered_map < std::string, GPU_Image*> List_texture_name;
	std::unordered_map < std::string, Mix_Music*> List_music_name;
	std::unordered_map < std::string, Mix_Chunk*> List_sound_name;
	std::unordered_map < std::string, FC_Font*> List_font_name;
};

