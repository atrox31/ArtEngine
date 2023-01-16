#include "Sprite.h"

#include "ArtCore/Functions/Func.h"
#include "ArtCore/Gui/Console.h"
#include "ArtCore/_Debug/Debug.h"

#include "nlohmann/json.hpp"
using nlohmann::json;

Sprite::~Sprite()
{
	for (const auto t : m_texture) {
		GPU_FreeImage(t);
	}
	m_texture.clear();
}

void Sprite::Delete(Sprite* target)
{
	for (const auto t : target->m_texture) {
		GPU_FreeImage(t);
	}
	target->m_texture.clear();
}

Sprite* Sprite::Load(const std::string& file)
{
	const char* sprite_buffer = Func::ArchiveGetFileBuffer(file, nullptr);
	const json data = json::parse(sprite_buffer);
	if (!data.is_object()) return nullptr;

	Sprite* new_sprite = new Sprite();
	new_sprite->m_width = data["SpriteWidth"].get<int>();
	new_sprite->m_height = data["SpriteHeight"].get<int>();
	new_sprite->m_center_x = data["SpriteCenterX"].get<int>();
	new_sprite->m_center_y = data["SpriteCenterY"].get<int>();

	new_sprite->m_mask.Make(
		data["CollisionMask"].get<std::string>(),
		data["CollisionMaskValue1"].get<float>(),
		data["CollisionMaskValue2"].get<float>()
	);

	new_sprite->m_texture_size = data["SpriteFrames"].get<int>();
	new_sprite->m_texture = std::vector<GPU_Image*>();
	const std::string sprite_name = data["Name"].get<std::string>();
	if(new_sprite->m_texture_size > 0)
	{
		new_sprite->m_texture.resize(new_sprite->m_texture_size);
		for (int i = 0; i < new_sprite->m_texture_size; i++) {
			const std::string tex_name = std::format("Sprites/{}/{}.png", sprite_name, i);
			new_sprite->m_texture[i] = GPU_LoadImage_RW(Func::ArchiveGetFileRWops(tex_name, nullptr), true);
			GPU_GenerateMipmaps(new_sprite->m_texture[i]);
			if(new_sprite->m_texture[i] == nullptr){
				Console::WriteLine("Sprite::Load - " + sprite_name + " texture '" + tex_name + "' not found");
			}
		}
	}else
	{
		Console::WriteLine("Sprite::Load - " + sprite_name + " 0 textures found");
	}

	return new_sprite;

}

GPU_Image* Sprite::GetFrame(const int frame) const
{
	if (frame < m_texture_size) {
		return m_texture[frame];
	}
	return nullptr;
}
