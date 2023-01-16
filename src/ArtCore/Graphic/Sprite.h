#pragma once
#include <vector>
#include <string>

#include "ArtCore/Structs/Rect.h"
#include "ArtCore/Structs/Mask.h"

class Sprite final
{
public:
	Sprite() = default;
	~Sprite();
	static void Delete(Sprite*);
	static Sprite* Load(const std::string& file);

	[[nodiscard]] GPU_Image* GetFrame(int frame) const;

	[[nodiscard]] int GetMaxFrame() const
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

	[[nodiscard]] constexpr Mask GetMask() const
	{
		return m_mask;
	}

	constexpr void SetMask(const Mask& new_mask)
	{
		m_mask = Mask(new_mask);
	}

private:
	int m_width = 0;
	int m_height = 0;

	std::vector<GPU_Image*> m_texture = std::vector<GPU_Image*>();
	int m_texture_size = 0;

	Mask m_mask{};

	int m_center_x = 0;
	int m_center_y = 0;
};

