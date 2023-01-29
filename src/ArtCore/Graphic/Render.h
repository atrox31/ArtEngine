#pragma once

#include "Sprite.h"
#include "ArtCore/CodeExecutor/Stack.h"
#include "FC_Fontcache/SDL_FontCache.h"


class Render final
{
public:
	static void CreateRender(int width, int height);
	static void DestroyRender();
	static void LoadShaders();
	static SDL_FPoint ScalePoint(const SDL_FPoint& point);
	static void ScalePoint(SDL_FPoint* point);
	static void ScaleCoords(float& x, float& y);
	static void ScaleRect(Rect& rect);
	static SDL_FPoint ScalePoint(const float& x, const float& y);

	// drawing
	static void DrawTexture(GPU_Image* texture, const vec2f& position, const vec2f& scale, float angle, float alpha);

	/**
	 * \brief Draw texture in box
	 * \param texture texture object
	 * \param input_box part of texture to copy, nullptr for entire texture
	 * \param output_box part of screen space where texture is copied, nullptr for entire texture
	 */
	static void DrawTextureBox(GPU_Image* texture, GPU_Rect* input_box, GPU_Rect* output_box);

	static void DrawLine(const SDL_FPoint& point_begin, const SDL_FPoint& point_end, const float& line_thickness, const SDL_Color& color);

	static void DrawSprite(const Sprite* sprite, const vec2f& position, int frame);
	static void DrawSprite_ex(const Sprite* sprite, float pos_x, float pos_y, int frame, float scale_x, float scale_y, float center_x, float center_y, float angle, float alpha);
	static void DrawSpriteBox(const Sprite* sprite, GPU_Rect box, int frame = 0, float alpha = 1.0f);

	static void DrawRect(GPU_Rect rect, SDL_Color color);
	static void DrawRect_wh(GPU_Rect rect, SDL_Color color);
	static void DrawRectFilled(GPU_Rect rect, SDL_Color color);

	static void DrawRectRounded(GPU_Rect rect, float round_factor, SDL_Color color);
	static void DrawRectRoundedFilled(GPU_Rect rect, float round_factor, SDL_Color color);

	static void DrawCircle(const vec2f& position, float radius, SDL_Color color);
	static void DrawCircleFilled(const vec2f& position, float radius, SDL_Color color);

	static void DrawTriangle(const vec2f& a, const vec2f& b, const vec2f& c, SDL_Color color);
	static void DrawTriangleFilled(const vec2f& a, const vec2f& b, const vec2f& c, SDL_Color color);

	// text
	static GPU_Rect DrawText(const std::string&, FC_Font*, const vec2f&, SDL_Color);
	static GPU_Rect DrawTextAlign(const std::string&, FC_Font*, const vec2f&, SDL_Color, FC_AlignEnum);
	static GPU_Rect DrawTextBox(const std::string& text, FC_Font* font, GPU_Rect box, SDL_Color color, FC_AlignEnum align);

	// system
	static void RenderToTarget(GPU_Target* target);
	static void RenderClear();
	static void RenderClearColor(const SDL_Color& color);

public:
	// post process
	static void ProcessImageWithGaussian();
	/**
	 * \brief Set post process quality for Gaussian bloom, default values are low, medium, high
	 * \param quality default is (4,8,12)
	 * \param directions default is (6,8,12)
	 * \param distance blur amount default is 0.02
	 */
	static void SetGaussianProperties(int quality, int directions, float distance);
	static void SetGaussianEnabled(const bool mode)
	{
		_instance->_use_shader_gaussian = mode;
	}
	static int GetGaussianMode() { return _instance->_shader_gaussian_mode; }
	static void SetGaussianFromPreset(int get);

	// Create render via CreateRender()
	Render(const Render&) = delete;
	// Create render via CreateRender()
	Render& operator=(const Render&) = delete;
	// Create render via CreateRender()
	Render& operator=(Render&&) = delete;
	// Create render via CreateRender()
	Render(Render&&) = delete;
private:
	~Render();
	Render();


	static Render* _instance;
	// global screen
	int _width, _height;
	// mouse scale point
	float _scale_width_value, _scale_height_value;
	bool _scale_width, _scale_height;

	GPU_Target* _screen_texture_target = nullptr;
	GPU_Image* _screen_texture = nullptr;

	// bloom
		// shaders
	bool _use_shader_gaussian;
	int _shader_gaussian_mode;
	Uint32 _shader_gaussian;
	GPU_ShaderBlock _shader_gaussian_block{};
	int _shader_gaussian_var_quality;
	int _shader_gaussian_var_directions;
	float _shader_gaussian_var_distance;
	int _shader_gaussian_var_quality_location;
	int _shader_gaussian_var_directions_location;
	int _shader_gaussian_var_distance_location;
		// textures
	GPU_Image* _shader_gaussian_texture = nullptr;
	GPU_Target* _shader_gaussian_texture_target = nullptr;
};

