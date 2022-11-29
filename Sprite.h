#pragma once
#include <vector>

#pragma warning(push)
#include "SDL_version.h"
#include SDL2_INCLUDE_DIR
#include SDL2_IMAGE_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR
#pragma warning(pop)

#include "Rect.h"
#include "Func.h"
// for string enum
#include "Event.h"

class Sprite
{
public:
	ENUM_WITH_STRING_CONVERSION(MaskType, (none)(circle)(rectangle)(perpixel))
	Sprite();
	virtual ~Sprite();
	static void Delete(Sprite*);
	static Sprite* Load(const char* data, Sint64 size);

	GPU_Image* GetFrame(int frame);
	int GetMaxFrame() {
		return m_texture_size;
	};

	constexpr inline float GetCenterX() {
		return (float)m_center_x;
	};
	constexpr inline float GetCenterY() {
		return (float)m_center_y;
	};
	
	constexpr inline int GetWidth() {
		return m_width;
	};
	constexpr inline int GetHeight() {
		return m_height;
	};

	constexpr inline MaskType GetMaskType() {
		return m_mask_type;
	}
	constexpr inline int GetMaskValue() {
		return m_mask_value;
	}

private:
	int m_width;
	int m_height;

	std::vector<GPU_Image*> m_texture;
	int m_texture_size;

	int m_mask_value;
	MaskType m_mask_type;

	int m_center_x;
	int m_center_y;
};

