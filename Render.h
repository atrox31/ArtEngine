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

	static void DrawSprite(Sprite* sprite, const vec2f& postion, int frame);
	static void DrawSprite_ex(Sprite* sprite, float pos_x, float pos_y, int frame, float scale_x, float scale_y, float center_x, float center_y, float angle, float alpha);
	static void DrawSpriteBox(const Sprite* sprite, GPU_Rect& box, int frame, const float& angle, float alpha);

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

	// post procesing
	static void SetBloom(bool state) { _instance->UseBloom = state; };

private:
	// post process
	void ApplyBloom();

private:
	virtual ~Render();
	Render();
	// using shaders
	bool UseBloom;

	static Render* _instance;
	// shaders
	Uint32 _shader_bloom;
	Uint32 _shader_bloom_horizontal;
	GPU_ShaderBlock _shader_bloom_block{};

	Uint32 _shader_bright;
	GPU_ShaderBlock _shader_bright_block{};

	// global screen
	int width, height;
	GPU_Target* _screenTexture_target = nullptr;
	GPU_Image* _screenTexture = nullptr;

	// bloom
	GPU_Image* _brightTexture = nullptr;
	GPU_Target* _brightTexture_target = nullptr;

	GPU_Image* _bloomTexture = nullptr;
	GPU_Target* _bloomTexture_target = nullptr;
	GPU_Image* _bloomTexture_final = nullptr;
	GPU_Target* _bloomTexture_final_target = nullptr;

};

