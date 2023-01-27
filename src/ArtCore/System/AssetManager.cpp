#include "assetManager.h"

#include "ArtCore/Functions/Func.h"
#include "ArtCore/_Debug/Debug.h"
#include "ArtCore/Graphic/BackGroundRenderer.h"
#include "ArtCore/main.h"

#include <ranges>

#include "ArtCore/Gui/Console.h"

bool AssetManager::LoadData(const BackGroundRenderer* bgr, const int p_min, const int p_max)
{
	int progress = p_min;
	int lines = 0;
	// ReSharper disable once StringLiteralTypo
	for (std::string& asset : Func::ArchiveGetFileText("filelist.txt", &lines, true)) {
		bgr->SetProgress(
			static_cast<int>(Func::LinearScale(
				static_cast<float>(progress++),
				0.0f,
				static_cast<float>(lines),
				static_cast<float>(p_min),
				static_cast<float>(p_max)
			))
		);

		// type/name_in_hash.abc;normal_name
		auto asset_data = Func::Split2(asset, ';');
		std::string asset_type = Func::Split2(asset_data.first, '/').first;

		if constexpr (VERSION_MAIN < 1 && VERSION_MINOR == 7) {
			if (asset_type.back() == 's') asset_type.pop_back();
		}
		else {
			// Delete this after version 0.7.x
			SDL_TriggerBreakpoint();
		}

		ADD_LOAD_ASSET(Sprite,		Sprite,		Sprite::Load(asset_data.first))
		ADD_LOAD_ASSET(GPU_Image,	Texture,	GPU_LoadImage_RW( Func::ArchiveGetFileRWops( asset_data.first, nullptr), true))
		ADD_LOAD_ASSET(Mix_Music,	Music,		Mix_LoadMUS_RW(Func::ArchiveGetFileRWops(asset_data.first, nullptr), true))
		ADD_LOAD_ASSET(Mix_Chunk,	Sound,		Mix_LoadWAV_RW(Func::ArchiveGetFileRWops(asset_data.first, nullptr), true))
		ADD_LOAD_ASSET(FC_Font,		Font,		Func::LoadFontRw(Func::ArchiveGetFileRWops(asset_data.first, nullptr), 12))

	}

	FILL_ASSET_ID_LIST(Sprite)
	FILL_ASSET_ID_LIST(Texture)
	FILL_ASSET_ID_LIST(Music)
	FILL_ASSET_ID_LIST(Sound)
	FILL_ASSET_ID_LIST(Font)
	
	bgr->SetProgress(p_max);
	return true;
}

void AssetManager::ClearData()
{
	ADD_CLEAR_ASSET(Sprite, Sprite::Delete(ref))
	ADD_CLEAR_ASSET(Texture, GPU_FreeImage(ref))
	ADD_CLEAR_ASSET(Music, Mix_FreeMusic(ref))
	ADD_CLEAR_ASSET(Sound, Mix_FreeChunk(ref))
	ADD_CLEAR_ASSET(Font, FC_FreeFont(ref))
}

GENERATE_GETTER_FUNCTION(Sprite, Sprite)
GENERATE_GETTER_FUNCTION(GPU_Image, Texture)
GENERATE_GETTER_FUNCTION(Mix_Music, Music)
GENERATE_GETTER_FUNCTION(Mix_Chunk, Sound)
GENERATE_GETTER_FUNCTION(FC_Font, Font)

// cleanup
#undef GENERATE_GETTER_FUNCTION
#undef ADD_CLEAR_ASSET
#undef ADD_LOAD_ASSET
#undef FILL_ASSET_ID_LIST