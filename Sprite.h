#pragma once
#include <vector>

#pragma warning(push)
#include "SDL_version.h"
#include SDL2_INCLUDE_DIR
#include SDL2_IMAGE_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR
#pragma warning(pop)

#include "Rect.h"

class Sprite
{
public:
	enum class MaskType {
		NON, CIRCLE, RECT, PERPIXEL
	};

private:
	int m_width;
	int m_height;

	std::vector<GPU_Image*> m_texture;
	int m_texture_size;

	Rect m_mask_rect;
	float m_mask_circle;
	MaskType m_mask_type;

	int m_center_x;
	int m_center_y;
};

