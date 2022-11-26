#pragma once
#pragma warning(push)
#include "SDL_version.h"
#include SDL2_INCLUDE_DIR
#include SDL2_IMAGE_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR
#include SDL2_MIXER_INCLUDE_DIR
#include "SDL2/SDL_FontCache-master/SDL_FontCache.h"
#pragma warning(pop)

#include <vector>
#include <map>
#include "Sprite.h"
#include <string>
#include "BackGroundRenderer.h"

class BackGroundRenderer;
class AssetManager
{
public:
	AssetManager();
	virtual ~AssetManager();

	bool LoadData(BackGroundRenderer* bgr, int p_min, int p_max);
	void ClearData();

	Sprite* GetSprite(int id);
	Sprite* GetSprite(std::string name);
	int GetSpriteId(std::string name);
	
	GPU_Image* GetTexture(int id);
	GPU_Image* GetTexture(std::string name);
	int GetTextureId(std::string name);
	
	Mix_Music* GetMusic(int id);
	Mix_Music* GetMusic(std::string name);
	int GetMusicId(std::string name);
	
	Mix_Chunk* GetSound(int id);
	Mix_Chunk* GetSound(std::string name);
	int GetSoundId(std::string name);
	
	FC_Font* GetFont(int id);
	FC_Font* GetFont(std::string name);
	int GetFontId(std::string name);

private:
	std::vector<Sprite*> List_sprite_id;
	std::vector<GPU_Image*> List_texture_id;
	std::vector<Mix_Music*> List_music_id;
	std::vector<Mix_Chunk*> List_sound_id;
	std::vector<FC_Font*> List_font_id;

	std::map < std::string, Sprite*> List_sprite_name;
	std::map < std::string, GPU_Image*> List_texture_name;
	std::map < std::string, Mix_Music*> List_music_name;
	std::map < std::string, Mix_Chunk*> List_sound_name;
	std::map < std::string, FC_Font*> List_font_name;
};

