#include "Render.h"
#include "Func.h"
#include "ColorDefinitions.h"
Render* Render::_instance = nullptr;

void Render::CreateRender(int width, int height)
{
	if (_instance != nullptr) {
		delete _instance;
	}
	_instance = new Render();

	_instance->width = width;
	_instance->height = height;

	_instance->_screenTexture = GPU_CreateImage(width, height, GPU_FormatEnum::GPU_FORMAT_RGBA);
	_instance->_screenTexture_target = GPU_LoadTarget(_instance->_screenTexture);

	_instance->_bloomTexture = GPU_CreateImage(width, height, GPU_FormatEnum::GPU_FORMAT_RGBA);
	_instance->_bloomTexture_target = GPU_LoadTarget(_instance->_bloomTexture);
	GPU_SetBlendMode(_instance->_bloomTexture, GPU_BlendPresetEnum::GPU_BLEND_ADD);

	_instance->_brightTexture = GPU_CreateImage(width, height, GPU_FormatEnum::GPU_FORMAT_RGBA);
	_instance->_brightTexture_target = GPU_LoadTarget(_instance->_brightTexture);

	GPU_Clear(_instance->_screenTexture_target);
	GPU_Flip(_instance->_screenTexture_target);

	GPU_Clear(_instance->_screenTexture_target);
	GPU_Flip(_instance->_screenTexture_target);

	GPU_Clear(_instance->_brightTexture_target);
	GPU_Flip(_instance->_brightTexture_target);

}

void Render::DestroyRender()
{
	if (_instance == nullptr) return;
	GPU_FreeTarget(_instance->_brightTexture_target);
	GPU_FreeImage(_instance->_brightTexture);
	_instance->_brightTexture_target = nullptr;
	_instance->_brightTexture = nullptr;

	GPU_FreeTarget(_instance->_bloomTexture_target);
	GPU_FreeImage(_instance->_bloomTexture);
	_instance->_bloomTexture_target = nullptr;
	_instance->_bloomTexture = nullptr;

	GPU_FreeTarget(_instance->_screenTexture_target);
	GPU_FreeImage(_instance->_screenTexture);
	_instance->_screenTexture = nullptr;
	_instance->_screenTexture_target = nullptr;

	GPU_FreeShaderProgram(_instance->_shader_bloom);
	GPU_FreeShaderProgram(_instance->_shader_bright);
}

void Render::LoadShaders() {
	_instance->_shader_bloom = 0;
	_instance->_shader_bloom_block = Func::load_shader_program(&_instance->_shader_bloom, "files/common.vert", "files/bloom.frag");
	_instance->_shader_bloom_horizontal = GPU_GetUniformLocation(_instance->_shader_bloom, "horizontal");

	_instance->_shader_bright = 0;
	_instance->_shader_bright_block = Func::load_shader_program(&_instance->_shader_bright, "files/common.vert", "files/color.frag");
}
Render::Render()
{
	_shader_bloom = 0;
	_shader_bloom_horizontal = 0;
	//_shader_bloom_block;
	_shader_bright = 0;
	//_shader_bright_block;
	width = 0;
	height = 0;
	_screenTexture = nullptr;
	_screenTexture_target = nullptr;
	UseBloom = true;
}

Render::~Render()
{
	if(_instance->_brightTexture_target != nullptr)
	DestroyRender();
}

void Render::DrawTexture(GPU_Image* texture, vec2f postion, vec2f scale, float angle, float alpha)
{
	bool alpha_flag = alpha < 1.0f;
	if (alpha_flag) {
		if (alpha < 0.0f) alpha = 0.0f;
		if (alpha > 1.0f) alpha = 1.0f;
		GPU_SetBlending(texture, true);
		//GPU_GetBlendModeFromPreset(GPU_BlendPresetEnum::GPU_BLEND_MOD_ALPHA);
		GPU_SetColor(texture, { (Uint8)255,(Uint8)255,(Uint8)255,(Uint8)(alpha * 255) });
	}
	GPU_BlitTransform(texture, NULL, _instance->_screenTexture_target, postion.x, postion.y, angle, scale.x, scale.y);
	if (alpha_flag) {
		GPU_SetBlending(texture, false);

		GPU_SetColor(texture, C_FULL);
	}
}
void Render::DrawSprite(Sprite* sprite, vec2f postion, int frame) {
	DrawSprite_ex(sprite, postion.x, postion.y, frame, 1.0f, 1.0f, (float)sprite->GetCenterX(), (float)sprite->GetCenterY(), 0.0f, 1.0f);
}

void Render::DrawSprite_ex(Sprite* sprite, float posX, float posY, int frame, float scaleX, float scaleY, float centerX, float centerY, float angle, float alpha) {
	if (sprite == nullptr) return;

	const int spr_max_frames = sprite->GetMaxFrame();
	if (spr_max_frames == 1) {
		frame = 0;
	}
	else {
		if (frame > spr_max_frames) {
			frame = frame % spr_max_frames;
		}
	}

	GPU_Image* target = sprite->GetFrame(frame);

	bool alpha_flag = alpha < 1.0f;
	if (alpha_flag) {
		if (alpha < 0.0f) alpha = 0.0f;
		if (alpha > 1.0f) alpha = 1.0f;
		GPU_SetBlending(target, true);
		//GPU_GetBlendModeFromPreset(GPU_BlendPresetEnum::GPU_BLEND_MOD_ALPHA);
		GPU_SetColor(target, { (Uint8)255,(Uint8)255,(Uint8)255,(Uint8)(alpha * 255) });
	}

	GPU_BlitTransformX(
		target,
		NULL,
		_instance->_screenTexture_target,
		posX, posY,
		centerX, centerY,
		angle,
		scaleX, scaleY
	);

	if (alpha_flag) {
		GPU_SetBlending(target, false);
		GPU_SetColor(target, C_FULL);
	}
}

void Render::DrawRect(Rect rect, SDL_Color color)
{
	GPU_Rectangle(_instance->_screenTexture_target, rect.x, rect.y, rect.w, rect.h, color);
}

void Render::DrawRect_wh(Rect rect, SDL_Color color)
{
	GPU_Rectangle(_instance->_screenTexture_target, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, color);
}

void Render::DrawRectFilled(Rect rect, SDL_Color color)
{
	GPU_RectangleFilled(_instance->_screenTexture_target, rect.x, rect.y, rect.w, rect.h, color);
}


void Render::DrawCircle(vec2f postion, float radius, SDL_Color color)
{
	GPU_Circle(_instance->_screenTexture_target, postion.x, postion.y, radius, color);
}

void Render::DrawCircleFilled(vec2f postion, float radius, SDL_Color color)
{
	GPU_CircleFilled(_instance->_screenTexture_target, postion.x, postion.y, radius, color);
}

void Render::DrawTriangle(vec2f a, vec2f b, vec2f c, SDL_Color color)
{
	GPU_Tri(_instance->_screenTexture_target, a.x, a.y, b.x, b.y, c.x, c.y, color);
}

void Render::DrawTriangleFilled(vec2f a, vec2f b, vec2f c, SDL_Color color)
{
	GPU_TriFilled(_instance->_screenTexture_target, a.x, a.y, b.x, b.y, c.x, c.y, color);
}

void Render::ApplyBloom()
{
	// wybiera tylko jasne kolory do textury
	GPU_ActivateShaderProgram(_shader_bright, &_shader_bright_block);
	GPU_BlitRect(_screenTexture, NULL, _brightTexture_target, NULL);

	// aktywuje rozmycie
	GPU_ActivateShaderProgram(_shader_bloom, &_shader_bloom_block);

	// rozmazanie w pionie
	GPU_SetUniformi(_shader_bloom_horizontal, 0);
	GPU_BlitRect(_brightTexture, NULL, _bloomTexture_target, NULL);
	// rozmazanie w poziomie
	GPU_SetUniformi(_shader_bloom_horizontal, 1);
	GPU_BlitRect(_bloomTexture, NULL, _bloomTexture_target, NULL);

	GPU_ActivateShaderProgram(0, NULL);
	//GPU_BlitRect(_bloomTexture, NULL, _screenTexture_target, NULL);
}

void Render::DrawText(std::string text,FC_Font* font, vec2f pos, SDL_Color color)
{
	DrawTextAlign(text, font, pos, color, FC_ALIGN_CENTER);
}

void Render::DrawTextAlign(std::string text, FC_Font* font, vec2f pos, SDL_Color color, FC_AlignEnum align)
{
	FC_DrawAlignColor(font, _instance->_screenTexture_target, pos.x, pos.y, align, color, text.c_str());
}

void Render::RenderToTarget(GPU_Target* target)
{
	if (_instance->UseBloom) _instance->ApplyBloom();
	//GPU_BlitRect(_instance->_screenTexture, NULL, target, NULL);
	GPU_BlitRect(_instance->_screenTexture, NULL, target, NULL);

	if (_instance->UseBloom)GPU_BlitRect(_instance->_bloomTexture, NULL, target, NULL);
}

void Render::RenderClear()
{
	GPU_Clear(_instance->_screenTexture_target);
	GPU_Clear(_instance->_bloomTexture_target);
	GPU_Clear(_instance->_brightTexture_target);
}