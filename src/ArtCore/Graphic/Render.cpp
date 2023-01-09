#include "Render.h"

#include "ColorDefinitions.h"
#include "ArtCore/Functions/Convert.h"
#include "ArtCore/Functions/Func.h"
#include "ArtCore/System/Core.h"
Render* Render::_instance = nullptr;

void Render::CreateRender(const int width, const int height)
{
	// if instance exists create new
	delete _instance;
	_instance = new Render();

	_instance->width = width;
	_instance->height = height;

	_instance->_screenTexture = GPU_CreateImage(static_cast<Uint16>(width), static_cast<Uint16>(height), GPU_FormatEnum::GPU_FORMAT_RGBA);
	_instance->_screenTexture_target = GPU_LoadTarget(_instance->_screenTexture);

	_instance->_shader_gaussian_texture = GPU_CreateImage(static_cast<Uint16>(width), static_cast<Uint16>(height), GPU_FormatEnum::GPU_FORMAT_RGBA);
	_instance->_shader_gaussian_texture_target = GPU_LoadTarget(_instance->_shader_gaussian_texture);
	
	GPU_Clear(_instance->_screenTexture_target);
	GPU_Flip(_instance->_screenTexture_target);

	GPU_Clear(_instance->_screenTexture_target);
	GPU_Flip(_instance->_screenTexture_target);

	_instance->_width_scale = (static_cast<float>(width) / static_cast<float>(Core::SD_GetInt("DefaultResolutionX", 1920)));
	_instance->_height_scale = (static_cast<float>(height) / static_cast<float>(Core::SD_GetInt("DefaultResolutionY", 1080)));
	_instance->_width_height_equal_scale = (_instance->_width_scale - _instance->_height_scale) < 0.1;
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
	_width_scale = 0.f;
	height = 0;
	_height_scale = 0.f;
	_screenTexture = nullptr;
	_screenTexture_target = nullptr;
}

Render::~Render()
{
	DestroyRender();
}

void Render::DrawTexture(GPU_Image* texture, const vec2f& position, const vec2f& scale, const float angle, float alpha)
{
	const bool alpha_flag = alpha < 1.0f;
	if (alpha_flag) {
		alpha = std::clamp(alpha, 0.f, 1.f);
		GPU_SetBlending(texture, true);
		//GPU_GetBlendModeFromPreset(GPU_BlendPresetEnum::GPU_BLEND_MOD_ALPHA);
		GPU_SetColor(texture, { static_cast<Uint8>(255),static_cast<Uint8>(255),static_cast<Uint8>(255),static_cast<Uint8>(alpha * 255.f) });
	}
	GPU_BlitTransform(texture, nullptr, _instance->_screenTexture_target, position.x * _instance->_width_scale, position.y * _instance->_height_scale, Convert::RadiansToDegree(angle), scale.x, scale.y);
	if (alpha_flag) {
		GPU_SetBlending(texture, false);

		GPU_SetColor(texture, C_FULL);
	}
}

void Render::DrawTextureBox(GPU_Image* texture, GPU_Rect* input_box, GPU_Rect* output_box)
{
	if(output_box != nullptr)
	{
		output_box->x = output_box->x * _instance->_width_scale;
		output_box->y = output_box->y * _instance->_height_scale;
		output_box->w = output_box->w * _instance->_width_scale;
		output_box->h = output_box->h * _instance->_height_scale;
	}
	GPU_BlitRect(texture, input_box, _instance->_screenTexture_target, output_box);
}

void Render::DrawLine(const SDL_FPoint& point_begin, const SDL_FPoint& point_end, const float& line_thickness,
	const SDL_Color& color)
{
	const float lt = GPU_SetLineThickness(line_thickness);
	GPU_Line(
		_instance->_screenTexture_target,
		point_begin.x * _instance->_width_scale,
		point_begin.y * _instance->_height_scale,
		point_end.x * _instance->_width_scale,
		point_end.y * _instance->_height_scale,
		color);
	GPU_SetLineThickness(lt);
}

void Render::DrawSprite(const Sprite* sprite, const vec2f& position, const int frame) {
	DrawSprite_ex(
		sprite,
		position.x,
		position.y,
		frame,
		1.0f,
		1.0f,
		(float)sprite->GetCenterX(),
		(float)sprite->GetCenterY(),
		0.0f,
		1.0f);
}

void Render::DrawSprite_ex(const Sprite* sprite, const float pos_x, const float pos_y, int frame, const float scale_x, const float scale_y, const float center_x, const float center_y, const float angle, float alpha) {
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
		GPU_SetColor(source, { static_cast<Uint8>(255),static_cast<Uint8>(255),static_cast<Uint8>(255),static_cast<Uint8>(alpha * 255.f) });
	}

	GPU_BlitTransformX(
		source,
		nullptr,
		_instance->_screenTexture_target,
		pos_x * _instance->_width_scale,
		pos_y * _instance->_height_scale,
		center_x, 
		center_y,
		Convert::RadiansToDegree(angle),
		scale_x * _instance->_width_scale,
		scale_y * _instance->_height_scale
	);

	if (alpha_flag) {
		GPU_SetBlending(source, false);
		GPU_SetColor(source, C_FULL);
	}
}
void Render::DrawSpriteBox(const Sprite* sprite, GPU_Rect box, int frame, float alpha) {
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
		GPU_SetColor(source, { static_cast<Uint8>(255),static_cast<Uint8>(255),static_cast<Uint8>(255),static_cast<Uint8>(alpha * 255) });
	}

	box.x = box.x * _instance->_width_scale;
	box.y = box.y * _instance->_height_scale;
	box.w = box.w * _instance->_width_scale;
	box.h = box.h * _instance->_height_scale;

	GPU_BlitRect(source, nullptr, _instance->_screenTexture_target, &box);
	
	if (alpha_flag) {
		GPU_SetBlending(source, false);
		GPU_SetColor(source, C_FULL);
	}
}

void Render::DrawRect(const GPU_Rect rect, const SDL_Color color)
{
	//GPU_Rectangle2(_instance->_screenTexture_target, rect, color);
	GPU_Rectangle(
		_instance->_screenTexture_target,
		rect.x * _instance->_width_scale,
		rect.y * _instance->_height_scale,
		rect.w * _instance->_width_scale,
		rect.h * _instance->_height_scale,
		color
	);
}

void Render::DrawRect_wh(const GPU_Rect rect, const SDL_Color color)
{
	GPU_Rectangle(
		_instance->_screenTexture_target,
		rect.x * _instance->_width_scale,
		rect.y * _instance->_height_scale,
		(rect.x + rect.w) * _instance->_width_scale,
		(rect.y + rect.h) * _instance->_height_scale,
		color
	);
}

void Render::DrawRectFilled(GPU_Rect rect, const SDL_Color color)
{
	rect.x = rect.x * _instance->_width_scale;
	rect.y = rect.y * _instance->_height_scale;
	rect.w = rect.w * _instance->_width_scale;
	rect.h = rect.h * _instance->_height_scale;
	GPU_RectangleFilled2(
		_instance->_screenTexture_target,
		rect,
		color
	);
}

void Render::DrawRectRounded( GPU_Rect rect, const float round_factor, const SDL_Color color)
{
	rect.x = rect.x * _instance->_width_scale;
	rect.y = rect.y * _instance->_height_scale;
	rect.w = rect.w * _instance->_width_scale;
	rect.h = rect.h * _instance->_height_scale;
	GPU_RectangleRound2(
		_instance->_screenTexture_target,
		rect,
		round_factor,
		color
	);
}

void Render::DrawRectRoundedFilled( GPU_Rect rect, const float round_factor, const SDL_Color color)
{
	rect.x = rect.x * _instance->_width_scale;
	rect.y = rect.y * _instance->_height_scale;
	rect.w = rect.w * _instance->_width_scale;
	rect.h = rect.h * _instance->_height_scale;
	GPU_RectangleRoundFilled2(
		_instance->_screenTexture_target,
		rect,
		round_factor,
		color
	);
}


void Render::DrawCircle(const vec2f& position, const float radius, const SDL_Color color)
{
	if(_instance->_width_height_equal_scale)
	{
		GPU_Circle(
			_instance->_screenTexture_target,
			position.x * _instance->_width_scale,
			position.y * _instance->_height_scale,
			radius * _instance->_height_scale,
			color
		);
	}
	else
	{
		GPU_Ellipse(
			_instance->_screenTexture_target,
			position.x * _instance->_width_scale,
			position.y * _instance->_height_scale,
			radius * _instance->_width_scale,
			radius * _instance->_height_scale,
			0.f,
			color
		);
	}
}

void Render::DrawCircleFilled(const vec2f& position, const float radius, const SDL_Color color)
{
	if (_instance->_width_height_equal_scale)
	{
		GPU_CircleFilled(
			_instance->_screenTexture_target,
			position.x * _instance->_width_scale,
			position.y * _instance->_height_scale,
			radius * _instance->_height_scale,
			color
		);
	}
	else
	{
		GPU_EllipseFilled(
			_instance->_screenTexture_target,
			position.x * _instance->_width_scale,
			position.y * _instance->_height_scale,
			radius * _instance->_width_scale,
			radius * _instance->_height_scale,
			0.f,
			color
		);
	}
}

void Render::DrawTriangle(const vec2f& a, const vec2f& b, const vec2f& c, const SDL_Color color)
{
	GPU_Tri(
		_instance->_screenTexture_target,
		a.x * _instance->_width_scale,
		a.y * _instance->_height_scale,
		b.x * _instance->_width_scale,
		b.y * _instance->_height_scale,
		c.x * _instance->_width_scale,
		c.y * _instance->_height_scale,
		color
	);
}

void Render::DrawTriangleFilled(const vec2f& a, const vec2f& b, const vec2f& c, const SDL_Color color)
{
	GPU_TriFilled(
		_instance->_screenTexture_target,
		a.x * _instance->_width_scale,
		a.y * _instance->_height_scale,
		b.x * _instance->_width_scale,
		b.y * _instance->_height_scale,
		c.x * _instance->_width_scale,
		c.y * _instance->_height_scale,
		color
	);
}

// Draw text, default align is left
GPU_Rect Render::DrawText(const std::string& text,FC_Font* font, const vec2f& pos, const SDL_Color color)
{
	return DrawTextAlign(
		text, 
		font, 
		pos,
		color, 
		FC_ALIGN_LEFT
	);
}

// Draw text
GPU_Rect Render::DrawTextAlign(const std::string& text, FC_Font* font, const vec2f& pos, const SDL_Color color, const FC_AlignEnum align)
{
	return FC_DrawEffect(
		font,
		_instance->_screenTexture_target,
		pos.x * _instance->_width_scale,
		pos.y * _instance->_height_scale,
		FC_MakeEffect(align,
			{
				1.f * _instance->_width_scale,
				1.f * _instance->_height_scale
			}, 
			color
		),
		text.c_str()
	);
}

// Draw text
GPU_Rect Render::DrawTextBox(const std::string& text, FC_Font* font, GPU_Rect box, const SDL_Color color, const FC_AlignEnum align)
{
	box.x = box.x * _instance->_width_scale;
	box.y = box.y * _instance->_height_scale;
	box.w = box.w * _instance->_width_scale;
	box.h = box.h * _instance->_height_scale;
	return FC_DrawBoxEffect(
		font, 
		_instance->_screenTexture_target,
		box, 
		FC_MakeEffect(
			align,
			{
				1.0f * _instance->_width_scale,
				1.0f * _instance->_height_scale
			}, 
			color
		),
		text.c_str());
}

// Render cached texture to target
void Render::RenderToTarget(GPU_Target* target)
{
	GPU_Clear(target);
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

void Render::SetGaussianProperties(const int quality, const int directions, const float distance)
{
	_instance->_shader_gaussian_var_quality = quality;
	_instance->_shader_gaussian_var_directions = directions;
	_instance->_shader_gaussian_var_distance = distance;
}

// clear all textures cache
void Render::RenderClear()
{
	GPU_Clear(_instance->_screenTexture_target);
}
void Render::RenderClearColor(const SDL_Color& color)
{
	GPU_ClearColor(_instance->_screenTexture_target, color);
}