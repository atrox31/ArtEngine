#include "Render.h"

#include "ColorDefinitions.h"
#include "ArtCore/Functions/Convert.h"
#include "ArtCore/Functions/Func.h"
Render* Render::_instance = nullptr;

void Render::CreateRender(const int width, const int height)
{
	// if instance exists create new
	delete _instance;
	_instance = new Render();

	_instance->width = width;
	_instance->height = height;

	_instance->_screenTexture = GPU_CreateImage(width, height, GPU_FormatEnum::GPU_FORMAT_RGBA);
	_instance->_screenTexture_target = GPU_LoadTarget(_instance->_screenTexture);

	_instance->_shader_gaussian_texture = GPU_CreateImage(width, height, GPU_FormatEnum::GPU_FORMAT_RGBA);
	_instance->_shader_gaussian_texture_target = GPU_LoadTarget(_instance->_shader_gaussian_texture);
	
	GPU_Clear(_instance->_screenTexture_target);
	GPU_Flip(_instance->_screenTexture_target);

	GPU_Clear(_instance->_screenTexture_target);
	GPU_Flip(_instance->_screenTexture_target);
}

void Render::DestroyRender()
{
	if (_instance == nullptr) return;

	GPU_FreeTarget(_instance->_shader_gaussian_texture_target);
	GPU_FreeImage(_instance->_shader_gaussian_texture);
	_instance->_shader_gaussian_texture_target = nullptr;
	_instance->_shader_gaussian_texture = nullptr;

	GPU_FreeTarget(_instance->_screenTexture_target);
	GPU_FreeImage(_instance->_screenTexture);
	_instance->_screenTexture = nullptr;
	_instance->_screenTexture_target = nullptr;

	GPU_FreeShaderProgram(_instance->_shader_gaussian);
}

void Render::LoadShaders() {
	_instance->_shader_gaussian = 0;
	_instance->_shader_gaussian_block = Func::load_shader_program(&_instance->_shader_gaussian, "files/common.vert", "files/bloom.frag");
	_instance->_shader_gaussian_var_quality_location = GPU_GetUniformLocation(_instance->_shader_gaussian, "Quality");
	_instance->_shader_gaussian_var_directions_location = GPU_GetUniformLocation(_instance->_shader_gaussian, "Directions");
	_instance->_shader_gaussian_var_distance_location = GPU_GetUniformLocation(_instance->_shader_gaussian, "Distance");
	SetGaussianProperties(8, 8, 0.02f); // load medium as default
}
Render::Render()
{
	_shader_gaussian = 0;
	_shader_gaussian_var_quality = 8;
	_shader_gaussian_var_directions = 8;
	_shader_gaussian_var_distance = 0.02f;
	_shader_gaussian_var_quality_location = 0;
	_shader_gaussian_var_directions_location = 0;
	_shader_gaussian_var_distance_location = 0;
	//_shader_gaussian_block;
	width = 0;
	height = 0;
	_screenTexture = nullptr;
	_screenTexture_target = nullptr;
}

Render::~Render()
{
	DestroyRender();
}

void Render::DrawTexture(GPU_Image* texture, const vec2f& postion, const vec2f& scale, float angle, float alpha)
{
	const bool alpha_flag = alpha < 1.0f;
	if (alpha_flag) {
		if (alpha < 0.0f) alpha = 0.0f;
		if (alpha > 1.0f) alpha = 1.0f;
		GPU_SetBlending(texture, true);
		//GPU_GetBlendModeFromPreset(GPU_BlendPresetEnum::GPU_BLEND_MOD_ALPHA);
		GPU_SetColor(texture, { (Uint8)255,(Uint8)255,(Uint8)255,(Uint8)(alpha * 255.f) });
	}
	GPU_BlitTransform(texture, nullptr, _instance->_screenTexture_target, postion.x, postion.y, Convert::RadiansToDegree(angle), scale.x, scale.y);
	if (alpha_flag) {
		GPU_SetBlending(texture, false);

		GPU_SetColor(texture, C_FULL);
	}
}

void Render::DrawTextureBox(GPU_Image* texture, GPU_Rect* input_box, GPU_Rect* output_box)
{
	GPU_BlitRect(texture, input_box, _instance->_screenTexture_target, output_box);
}

void Render::DrawLine(const SDL_FPoint& point_begin, const SDL_FPoint& point_end, const float& line_thickness,
	const SDL_Color& color)
{
	const float lt = GPU_SetLineThickness(line_thickness);
	GPU_Line(_instance->_screenTexture_target, point_begin.x, point_begin.y, point_end.x, point_end.y, color);
	GPU_SetLineThickness(lt);
}

void Render::DrawSprite(Sprite* sprite, const vec2f& postion, int frame) {
	DrawSprite_ex(sprite, postion.x, postion.y, frame, 1.0f, 1.0f, (float)sprite->GetCenterX(), (float)sprite->GetCenterY(), 0.0f, 1.0f);
}

void Render::DrawSprite_ex(Sprite* sprite, float pos_x, float pos_y, int frame, float scale_x, float scale_y, float center_x, float center_y, float angle, float alpha) {
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

	GPU_Image* source = sprite->GetFrame(frame);

	bool alpha_flag = alpha < 1.0f;
	if (alpha_flag) {
		if (alpha < 0.0f) alpha = 0.0f;
		if (alpha > 1.0f) alpha = 1.0f;
		GPU_SetBlending(source, true);
		//GPU_GetBlendModeFromPreset(GPU_BlendPresetEnum::GPU_BLEND_MOD_ALPHA);
		GPU_SetColor(source, { (Uint8)255,(Uint8)255,(Uint8)255,(Uint8)(alpha * 255) });
	}

	GPU_BlitTransformX(
		source,
		nullptr,
		_instance->_screenTexture_target,
		pos_x, pos_y,
		center_x, center_y,
		Convert::RadiansToDegree(angle),
		scale_x, scale_y
	);

	if (alpha_flag) {
		GPU_SetBlending(source, false);
		GPU_SetColor(source, C_FULL);
	}
}
void Render::DrawSpriteBox(const Sprite* sprite, GPU_Rect box, int frame, const float& angle, float alpha) {
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

	GPU_Image* source = sprite->GetFrame(frame);

	const bool alpha_flag = alpha < 1.0f;
	if (alpha_flag) {
		if (alpha < 0.0f) alpha = 0.0f;
		if (alpha > 1.0f) alpha = 1.0f;
		GPU_SetBlending(source, true);
		//GPU_GetBlendModeFromPreset(GPU_BlendPresetEnum::GPU_BLEND_MOD_ALPHA);
		GPU_SetColor(source, { (Uint8)255,(Uint8)255,(Uint8)255,(Uint8)(alpha * 255) });
	}

	GPU_BlitRect(source, nullptr, _instance->_screenTexture_target, &box);
	
	if (alpha_flag) {
		GPU_SetBlending(source, false);
		GPU_SetColor(source, C_FULL);
	}
}

void Render::DrawRect(const GPU_Rect rect, const SDL_Color color)
{
	//GPU_Rectangle2(_instance->_screenTexture_target, rect, color);
	GPU_Rectangle(_instance->_screenTexture_target, rect.x, rect.y, rect.w, rect.h, color);
}

void Render::DrawRect_wh(const GPU_Rect rect, const SDL_Color color)
{
	GPU_Rectangle(_instance->_screenTexture_target, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, color);
}

void Render::DrawRectFilled(const GPU_Rect rect, const SDL_Color color)
{
	GPU_RectangleFilled2(_instance->_screenTexture_target, rect, color);
}

void Render::DrawRectRounded(const GPU_Rect rect, const float round_factor, const SDL_Color color)
{
	GPU_RectangleRound(_instance->_screenTexture_target, rect.x, rect.y, rect.w, rect.h, round_factor, color);
}

void Render::DrawRectRoundedFilled(const GPU_Rect rect, const float round_factor, const SDL_Color color)
{
	GPU_RectangleRoundFilled(_instance->_screenTexture_target, rect.x, rect.y, rect.w, rect.h, round_factor, color);
}


void Render::DrawCircle(const vec2f& postion, const float radius, const SDL_Color color)
{
	GPU_Circle(_instance->_screenTexture_target, postion.x, postion.y, radius, color);
}

void Render::DrawCircleFilled(const vec2f& postion, const float radius, SDL_Color color)
{
	GPU_CircleFilled(_instance->_screenTexture_target, postion.x, postion.y, radius, color);
}

void Render::DrawTriangle(const vec2f& a, const vec2f& b, const vec2f& c, const SDL_Color color)
{
	GPU_Tri(_instance->_screenTexture_target, a.x, a.y, b.x, b.y, c.x, c.y, color);
}

void Render::DrawTriangleFilled(const vec2f& a, const vec2f& b, const vec2f& c, const SDL_Color color)
{
	GPU_TriFilled(_instance->_screenTexture_target, a.x, a.y, b.x, b.y, c.x, c.y, color);
}

// Draw text, default align is left
void Render::DrawText(const std::string& text,FC_Font* font, const vec2f& pos, const SDL_Color color)
{
	DrawTextAlign(text, font, pos, color, FC_ALIGN_LEFT);
}

// Draw text
void Render::DrawTextAlign(const std::string& text, FC_Font* font, const vec2f& pos, const SDL_Color color, const FC_AlignEnum align)
{
	FC_DrawEffect(font, _instance->_screenTexture_target, pos.x, pos.y, FC_MakeEffect(align, { 1.f,1.f }, color), text.c_str());
}

// Draw text
void Render::DrawTextBox(const std::string& text, FC_Font* font, const GPU_Rect& box, const SDL_Color color, const FC_AlignEnum align)
{
	FC_DrawBoxEffect(font, _instance->_screenTexture_target, box, FC_MakeEffect(align, { 1.0f,1.0f }, color), text.c_str());
}

// Render cached texture to target
void Render::RenderToTarget(GPU_Target* target)
{
	GPU_DeactivateShaderProgram();
	GPU_BlitRect(_instance->_screenTexture, nullptr, target, nullptr);
}

void Render::ProcessImageWithGaussian()
{
	// draw screen with gausan blur
	GPU_ActivateShaderProgram(_instance->_shader_gaussian, &_instance->_shader_gaussian_block);
	GPU_SetUniformi(_instance->_shader_gaussian_var_quality_location, _instance->_shader_gaussian_var_quality);
	GPU_SetUniformi(_instance->_shader_gaussian_var_directions_location, _instance->_shader_gaussian_var_directions);
	GPU_SetUniformf(_instance->_shader_gaussian_var_distance_location, _instance->_shader_gaussian_var_distance);
	GPU_BlitRect(_instance->_screenTexture, nullptr, _instance->_shader_gaussian_texture_target, nullptr);

	GPU_DeactivateShaderProgram();
	GPU_BlitRect(_instance->_shader_gaussian_texture, nullptr, _instance->_screenTexture_target, nullptr);
}

void Render::SetGaussianProperties(int quality, int directions, float distance)
{
	_instance->_shader_gaussian_var_quality = quality;
	_instance->_shader_gaussian_var_directions = directions;
	_instance->_shader_gaussian_var_distance = distance;
}

// clear all textures cache
void Render::RenderClear(const SDL_Color& color)
{
	GPU_ClearColor(_instance->_screenTexture_target, color);
}