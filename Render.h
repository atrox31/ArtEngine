#pragma once
// include all sdl systems
#include "SDL_version.h"
#include "vec2f.h"
#include "Sprite.h"
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
	static void DrawTexture(GPU_Image* texture, vec2f postion, vec2f scale, float angle, float alpha);

	static void DrawSprite(Sprite* sprite, vec2f postion, int frame);
	static void DrawSprite_ex(Sprite* sprite, float posX, float posY, int frame, float scaleX, float scaleY, float centerX, float centerY, float angle, float alpha);

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

