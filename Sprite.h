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

class Sprite final
{
public:
	ENUM_WITH_STRING_CONVERSION(MaskType, (none)(circle)(rectangle)(perpixel))
	Sprite();
	virtual ~Sprite();
	static void Delete(Sprite*);
	static Sprite* Load(const char* data, Sint64 size);

	GPU_Image* GetFrame(int frame) const;
	int GetMaxFrame() const
	{
		return m_texture_size;
	};

	[[nodiscard]] constexpr float GetCenterX() const
	{
		return static_cast<float>(m_center_x);
	}
	[[nodiscard]] constexpr float GetCenterY() const
	{
		return static_cast<float>(m_center_y);
	}
	[[nodiscard]] constexpr float GetCenterXRel() const
	{
		return static_cast<float>(-m_center_x + m_width / 2);
	}
	[[nodiscard]] constexpr float GetCenterYRel() const
	{
		return static_cast<float>(-m_center_y + m_height / 2);
	}

	
	[[nodiscard]] constexpr int GetWidth() const
	{
		return m_width;
	}
	[[nodiscard]] constexpr int GetHeight() const
	{
		return m_height;
	}

	[[nodiscard]] constexpr MaskType GetMaskType() const
	{
		return m_mask_type;
	}
	[[nodiscard]] constexpr int GetMaskValue() const
	{
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

