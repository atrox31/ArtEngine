#pragma once
#include <vector>
#include <string>

#include "ArtCore/Structs/Rect.h"

class Sprite final
{
public:
	enum class mask_type {
		None, Circle, Rectangle, PerPixel
	};
	static mask_type MaskTypeFromString(const std::string& name)
	{
		if (name == "None") return mask_type::None;
		if (name == "Circle") return mask_type::Circle;
		if (name == "Rectangle") return mask_type::Rectangle;
		if (name == "PerPixel") return mask_type::PerPixel;
		return mask_type::None;
	}
	Sprite();
	virtual ~Sprite();
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

	[[nodiscard]] constexpr mask_type GetMaskType() const
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
	mask_type m_mask_type;

	int m_center_x;
	int m_center_y;
};

