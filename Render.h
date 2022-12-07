#pragma once
// include all sdl systems
#include "SDL_version.h"
#include "vec2f.h"
#include "Sprite.h"
#include "Rect.h"
#include "SDL2/SDL_FontCache-master/SDL_FontCache.h"
#pragma warning(push)
#include SDL2_INCLUDE_DIR
#include SDL2_IMAGE_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR
#pragma warning(pop)


class Render
{
public:
	static void CreateRender(int width, int height);
	static void DestroyRender();
	static void LoadShaders();

	// drawing
	static void DrawTexture(GPU_Image* texture, const vec2f& postion, const vec2f& scale, float angle, float alpha);

	/**
	 * \brief Draw texture in box
	 * \param texture texture object
	 * \param input_box part of texture to copy, nullptr for entire texture
	 * \param output_box part of screen space where texture is copied, nullptr for entire texture
	 */
	static void DrawTextureBox(GPU_Image* texture, GPU_Rect* input_box, GPU_Rect* output_box);

	static void DrawSprite(Sprite* sprite, const vec2f& postion, int frame);
	static void DrawSprite_ex(Sprite* sprite, float pos_x, float pos_y, int frame, float scale_x, float scale_y, float center_x, float center_y, float angle, float alpha);
	static void DrawSpriteBox(const Sprite* sprite, GPU_Rect& box, int frame = 0, const float& angle = 0.0f, float alpha = 1.0f);

	static void DrawRect(GPU_Rect rect, SDL_Color color);
	static void DrawRect_wh(GPU_Rect rect, SDL_Color color);
	static void DrawRectFilled(GPU_Rect rect, SDL_Color color);

	static void DrawRectRounded(GPU_Rect rect, float round_factor, SDL_Color color);
	static void DrawRectRoundedFilled(GPU_Rect rect, float round_factor, SDL_Color color);

	static void DrawCircle(const vec2f& postion, float radius, SDL_Color color);
	static void DrawCircleFilled(const vec2f& postion, float radius, SDL_Color color);

	static void DrawTriangle(const vec2f& a, const vec2f& b, const vec2f& c, SDL_Color color);
	static void DrawTriangleFilled(const vec2f& a, const vec2f& b, const vec2f& c, SDL_Color color);

	// text
	static void DrawText(const std::string&, FC_Font*, const vec2f&, SDL_Color);
	static void DrawTextAlign(const std::string&, FC_Font*, const vec2f&, SDL_Color, FC_AlignEnum);
	static void DrawTextBox(const std::string& text, FC_Font* font, const GPU_Rect& box, SDL_Color color, FC_AlignEnum align);

	// system
	static void RenderToTarget(GPU_Target* target);
	static void RenderClear();

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

private:
	virtual ~Render();
	Render();

	static Render* _instance;
	// global screen
	int width, height;
	GPU_Target* _screenTexture_target = nullptr;
	GPU_Image* _screenTexture = nullptr;

	// bloom
		// shaders
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

