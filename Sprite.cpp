#include "Sprite.h"
#include "Debug.h"

Sprite::Sprite()
{
	m_width = 0;
	m_height = 0;

	m_texture = std::vector<GPU_Image*>();
	m_texture_size = 0;

	m_mask_value = 0;
	m_mask_type = Sprite::MaskType::none;

	m_center_x = 0;
	m_center_y = 0;
}

Sprite::~Sprite()
{
	for (auto t : m_texture) {
		GPU_FreeImage(t);
	}
	m_texture.clear();
}

void Sprite::Delete(Sprite* target)
{
	for (auto t : target->m_texture) {
		GPU_FreeImage(t);
	}
	target->m_texture.clear();
}

Sprite* Sprite::Load(const char* data, Sint64 size)
{
    Func::DataValues dv(data, (int)size);
    if (!dv.IsOk()) return nullptr;

    Sprite* nSprite = new Sprite();
	nSprite->m_width = Func::TryGetInt(dv.GetData("sprite", "sprite_width"));
	nSprite->m_height = Func::TryGetInt(dv.GetData("sprite", "sprite_width"));

	nSprite->m_texture = std::vector<GPU_Image*>();
	// load
	int textures_count = Func::TryGetInt(dv.GetData("image_list", "count"));
	if (textures_count > 0) {
		for (int i = 0; i < textures_count; i++) {
			std::string tname = "Sprites/" + dv.GetData("sprite", "name") + "/" + std::to_string(i) + ".png";
			GPU_Image* tmp = GPU_LoadImage_RW(Func::GetFileRWops(tname, nullptr), 1);
			if (tmp != nullptr) {
				nSprite->m_texture.push_back(tmp);
			}
			else {
				Debug::WARNING("Sprite::Load - " + dv.GetData("sprite", "name") + " texture '"+tname+"' not found");
			}
		}	
	}

	nSprite->m_texture_size = (int)(nSprite->m_texture.size());
	if (nSprite->m_texture_size == 0) {
		Debug::WARNING("Sprite::Load - " + dv.GetData("sprite", "name") + " 0 textures found");
	}

	nSprite->m_mask_value = Func::TryGetInt(dv.GetData("sprite", "collision_mask_value"));
	nSprite->m_mask_type = nSprite->MaskType_fromString(dv.GetData("sprite", "collision_mask"));

	nSprite->m_center_x = Func::TryGetInt(dv.GetData("sprite", "sprite_center_x"));
	nSprite->m_center_y = Func::TryGetInt(dv.GetData("sprite", "sprite_center_y"));
	return nSprite;
}

GPU_Image* Sprite::GetFrame(int frame)
{
	//if (frame < m_texture_size) {
		return m_texture[frame];
	//}
	//return nullptr;
}
