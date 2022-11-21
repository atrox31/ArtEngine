#pragma once
// include all sdl systems
#include "SDL_version.h"
#pragma warning(push)
#include SDL2_INCLUDE_DIR
#include SDL2_IMAGE_INCLUDE_DIR
#include SDL2_MIXER_INCLUDE_DIR
#include SDL2_NET_INCLUDE_DIR
#include SDL2_TTF_INCLUDE_DIR
#include SDL2_GPU_INCLUDE_DIR
#pragma warning(pop)

#include "vec2f.h";
#include "Sprite.h";

class Render
{
public:
	static void CreateRender(int width, int height);
	static void LoadShaders();

	// drawing
	static void DrawTexture(GPU_Image* texture, vec2f postion, vec2f scale, float angle);

	static void DrawSprite(Sprite* sprite, vec2f postion, float frame);
	static void DrawSprite_ex(Sprite* sprite, vec2f postion, float frame, vec2f scale, float angle);

	static void DrawRect(GPU_Rect rect, SDL_Color color);
	static void DrawRectFilled(GPU_Rect rect, SDL_Color color);

	static void DrawCircle(vec2f postion, float radius, SDL_Color color);
	static void DrawCircleFilled(vec2f postion, float radius, SDL_Color color);

	static void DrawTriangle(vec2f a, vec2f b, vec2f c, SDL_Color color);
	static void DrawTriangleFilled(vec2f a, vec2f b, vec2f c, SDL_Color color);

	// system
	static void RenderToTarget(GPU_Target* target);
	static void RenderClear();

	// post procesing
	static void SetBloom(bool state) { _instance->UseBloom = state; };

	virtual ~Render();
private:
	// post process
	void ApplyBloom();

private:
	Render();
	// using shaders
	bool UseBloom;

	static Render* _instance;
	// shaders
	Uint32 _shader_bloom;
	Uint32 _shader_bloom_horizontal;
	GPU_ShaderBlock _shader_bloom_block;

	Uint32 _shader_bright;
	GPU_ShaderBlock _shader_bright_block;

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

