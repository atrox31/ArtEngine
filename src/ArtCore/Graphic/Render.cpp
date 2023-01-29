#include "Render.h"

#include "ColorDefinitions.h"
#include "ArtCore/Functions/Convert.h"
#include "ArtCore/Functions/Func.h"
#include "ArtCore/Scene/Scene.h"
#include "ArtCore/System/Core.h"
#include "ArtCore/System/SettingsData.h"
Render* Render::_instance = nullptr;

void Render::CreateRender(const int width, const int height)
{
	// if instance exists create new
	if (_instance == nullptr) {
		_instance = new Render();
		
		_instance->_screen_texture = GPU_CreateImage(static_cast<Uint16>(width), static_cast<Uint16>(height), GPU_FormatEnum::GPU_FORMAT_RGBA);
		_instance->_screen_texture_target = GPU_LoadTarget(_instance->_screen_texture);

		_instance->_shader_gaussian_texture = GPU_CreateImage(static_cast<Uint16>(width), static_cast<Uint16>(height), GPU_FormatEnum::GPU_FORMAT_RGBA);
		_instance->_shader_gaussian_texture_target = GPU_LoadTarget(_instance->_shader_gaussian_texture);

		_instance->_width = width;
		_instance->_height = height;

	}

	if(width != _instance->_width)
	{
		_instance->_scale_width_value = static_cast<float>(width) / static_cast<float>(_instance->_width);
		_instance->_scale_width = true;
	}else
	{
		_instance->_scale_width = false;
	}

	if (height != _instance->_height)
	{
		_instance->_scale_height_value = static_cast<float>(height) / static_cast<float>(_instance->_height);
		_instance->_scale_height = true;
	}else
	{
		_instance->_scale_height = false;
	}

	GPU_Clear(_instance->_screen_texture_target);
	GPU_Flip(_instance->_screen_texture_target);

	GPU_Clear(_instance->_screen_texture_target);
	GPU_Flip(_instance->_screen_texture_target);
}

void Render::DestroyRender()
{
	if (_instance == nullptr) return;

	GPU_FreeTarget(_instance->_shader_gaussian_texture_target);
	GPU_FreeImage(_instance->_shader_gaussian_texture);
	_instance->_shader_gaussian_texture_target = nullptr;
	_instance->_shader_gaussian_texture = nullptr;

	GPU_FreeTarget(_instance->_screen_texture_target);
	GPU_FreeImage(_instance->_screen_texture);
	_instance->_screen_texture = nullptr;
	_instance->_screen_texture_target = nullptr;

	GPU_FreeShaderProgram(_instance->_shader_gaussian);
}

void Render::LoadShaders() {
	_instance->_shader_gaussian = 0;
	_instance->_shader_gaussian_block = Func::LoadShaderProgram(&_instance->_shader_gaussian, "files/common.vert", "files/bloom.frag");
	_instance->_shader_gaussian_var_quality_location = GPU_GetUniformLocation(_instance->_shader_gaussian, "Quality");
	_instance->_shader_gaussian_var_directions_location = GPU_GetUniformLocation(_instance->_shader_gaussian, "Directions");
	_instance->_shader_gaussian_var_distance_location = GPU_GetUniformLocation(_instance->_shader_gaussian, "Distance");
	SetGaussianProperties(8, 8, 0.02f); // load medium as default
}


SDL_FPoint Render::ScalePoint(const SDL_FPoint& point)
{
	return SDL_FPoint{
		_instance->_scale_width ? point.x * _instance->_scale_width_value : point.x,
		_instance->_scale_height ? point.y * _instance->_scale_height_value : point.y
	};
}


void Render::ScalePoint( SDL_FPoint* point)
{
	if (_instance->_scale_width)  point->x *= _instance->_scale_width_value;
	if (_instance->_scale_height) point->y *= _instance->_scale_height_value;
}

void Render::ScaleCoords(float& x, float& y)
{
	if (_instance->_scale_width)  x *= _instance->_scale_width_value;
	if (_instance->_scale_height) y *= _instance->_scale_height_value;
}
void Render::ScaleRect(Rect& rect)
{
	if (_instance->_scale_width) {
		rect.X *= _instance->_scale_width_value;
		rect.W *= _instance->_scale_width_value;
	}
	if (_instance->_scale_height) {
		rect.Y *= _instance->_scale_height_value;
		rect.H *= _instance->_scale_height_value;
	}
}

SDL_FPoint Render::ScalePoint(const float& x, const float& y)
{
	return SDL_FPoint{
		_instance->_scale_width ? x * _instance->_scale_width_value : x,
		_instance->_scale_height ? y * _instance->_scale_height_value : y
	};
}

Render::Render()
{
	_use_shader_gaussian = false;
	_shader_gaussian = 0;
	_shader_gaussian_mode = 0;
	_shader_gaussian_var_quality = 8;
	_shader_gaussian_var_directions = 8;
	_shader_gaussian_var_distance = 0.02f;
	_shader_gaussian_var_quality_location = 0;
	_shader_gaussian_var_directions_location = 0;
	_shader_gaussian_var_distance_location = 0;
	//_shader_gaussian_block;
	_scale_height = false;
	_scale_width = false;
	_width = 0;
	_height = 0;
	_scale_width_value = 0.f;
	_scale_height_value = 0.f;
	_screen_texture = nullptr;
	_screen_texture_target = nullptr;
}

void Render::SetGaussianFromPreset(const int get)
{
	switch (std::clamp(get, 0, 3))
	{
	case 0:
		Render::SetGaussianEnabled(false);
		break;
	case 1:
		Render::SetGaussianEnabled(true);
		Render::SetGaussianProperties(4, 4, 0.0205f);
		break;
	case 2:
		Render::SetGaussianEnabled(true);
		Render::SetGaussianProperties(8, 8, 0.0205f);
		break;
	case 3:
		Render::SetGaussianEnabled(true);
		Render::SetGaussianProperties(16, 16, 0.0205f);
		break;
	default: break;
	}
	SettingsData::SetValue("ACBloom", std::to_string(Render::GetGaussianMode()));
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
	GPU_BlitTransform(texture, nullptr, _instance->_screen_texture_target, position.x, position.y, Convert::RadiansToDegree(angle), scale.x, scale.y);
	if (alpha_flag) {
		GPU_SetBlending(texture, false);

		GPU_SetColor(texture, C_FULL);
	}
}

void Render::DrawTextureBox(GPU_Image* texture, GPU_Rect* input_box, GPU_Rect* output_box)
{
	if(output_box != nullptr)
	{
		output_box->x = output_box->x;
		output_box->y = output_box->y;
		output_box->w = output_box->w;
		output_box->h = output_box->h;
	}
	GPU_BlitRect(texture, input_box, _instance->_screen_texture_target, output_box);
}

void Render::DrawLine(const SDL_FPoint& point_begin, const SDL_FPoint& point_end, const float& line_thickness,
	const SDL_Color& color)
{
	const float lt = GPU_SetLineThickness(line_thickness);
	GPU_Line(
		_instance->_screen_texture_target,
		point_begin.x,
		point_begin.y,
		point_end.x,
		point_end.y,
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
		sprite->GetCenterX(),
		sprite->GetCenterY(),
		0.0f,
		1.0f);
}

void Render::DrawSprite_ex(const Sprite* sprite, const float pos_x, const float pos_y, int frame, const float scale_x, const float scale_y, const float center_x, const float center_y, const float angle, float alpha) {
	if (sprite == nullptr) return;

	if (const int spr_max_frames = sprite->GetMaxFrame(); spr_max_frames == 1) {
		frame = 0;
	}
	else {
		if (frame > spr_max_frames) {
			frame = frame % spr_max_frames;
		}
	}

	GPU_Image* source = sprite->GetFrame(frame);

	alpha = std::clamp(alpha, 0.f, 1.f);

	GPU_SetBlending(source, true);
	GPU_SetColor(source, { static_cast<Uint8>(255),static_cast<Uint8>(255),static_cast<Uint8>(255),static_cast<Uint8>(alpha * 255.f) });

	GPU_BlitTransformX(
		source,
		nullptr,
		_instance->_screen_texture_target,
		pos_x,
		pos_y,
		center_x, 
		center_y,
		Convert::RadiansToDegree(angle),
		scale_x,
		scale_y
	);

	GPU_SetBlending(source, false);
	GPU_SetColor(source, C_FULL);
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

	box.x = box.x;
	box.y = box.y;
	box.w = box.w;
	box.h = box.h;

	GPU_BlitRect(source, nullptr, _instance->_screen_texture_target, &box);
	
	if (alpha_flag) {
		GPU_SetBlending(source, false);
		GPU_SetColor(source, C_FULL);
	}
}

void Render::DrawRect(const GPU_Rect rect, const SDL_Color color)
{
	//GPU_Rectangle2(_instance->_screenTexture_target, rect, color);
	GPU_Rectangle(
		_instance->_screen_texture_target,
		rect.x,
		rect.y,
		rect.w,
		rect.h,
		color
	);
}

void Render::DrawRect_wh(const GPU_Rect rect, const SDL_Color color)
{
	GPU_Rectangle(
		_instance->_screen_texture_target,
		rect.x,
		rect.y,
		(rect.x + rect.w),
		(rect.y + rect.h),
		color
	);
}

void Render::DrawRectFilled(GPU_Rect rect, const SDL_Color color)
{
	rect.x = rect.x;
	rect.y = rect.y;
	rect.w = rect.w;
	rect.h = rect.h;
	GPU_RectangleFilled2(
		_instance->_screen_texture_target,
		rect,
		color
	);
}

void Render::DrawRectRounded( GPU_Rect rect, const float round_factor, const SDL_Color color)
{
	rect.x = rect.x;
	rect.y = rect.y;
	rect.w = rect.w;
	rect.h = rect.h;
	GPU_RectangleRound2(
		_instance->_screen_texture_target,
		rect,
		round_factor,
		color
	);
}

void Render::DrawRectRoundedFilled( GPU_Rect rect, const float round_factor, const SDL_Color color)
{
	rect.x = rect.x;
	rect.y = rect.y;
	rect.w = rect.w;
	rect.h = rect.h;
	GPU_RectangleRoundFilled2(
		_instance->_screen_texture_target,
		rect,
		round_factor,
		color
	);
}


void Render::DrawCircle(const vec2f& position, const float radius, const SDL_Color color)
{
	GPU_Circle(
		_instance->_screen_texture_target,
		position.x,
		position.y,
		radius,
		color
	);
}

void Render::DrawCircleFilled(const vec2f& position, const float radius, const SDL_Color color)
{
	GPU_CircleFilled(
		_instance->_screen_texture_target,
		position.x,
		position.y,
		radius,
		color
	);
}

void Render::DrawTriangle(const vec2f& a, const vec2f& b, const vec2f& c, const SDL_Color color)
{
	GPU_Tri(
		_instance->_screen_texture_target,
		a.x,
		a.y,
		b.x,
		b.y,
		c.x,
		c.y,
		color
	);
}

void Render::DrawTriangleFilled(const vec2f& a, const vec2f& b, const vec2f& c, const SDL_Color color)
{
	GPU_TriFilled(
		_instance->_screen_texture_target,
		a.x,
		a.y,
		b.x,
		b.y,
		c.x,
		c.y,
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
		_instance->_screen_texture_target,
		pos.x,
		pos.y,
		FC_MakeEffect(align,
			{
				1.f,
				1.f
			}, 
			color
		),
		text.c_str()
	);
}

// Draw text
GPU_Rect Render::DrawTextBox(const std::string& text, FC_Font* font, GPU_Rect box, const SDL_Color color, const FC_AlignEnum align)
{
	box.x = box.x;
	box.y = box.y;
	box.w = box.w;
	box.h = box.h;
	return FC_DrawBoxEffect(
		font, 
		_instance->_screen_texture_target,
		box, 
		FC_MakeEffect(
			align,
			{
				1.0f,
				1.0f
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
	GPU_BlitRect(_instance->_screen_texture, nullptr, target, nullptr);
}

void Render::ProcessImageWithGaussian()
{
	if (!_instance->_use_shader_gaussian) return;
	// draw screen with gausan blur
	GPU_ActivateShaderProgram(_instance->_shader_gaussian, &_instance->_shader_gaussian_block);
	GPU_SetUniformi(_instance->_shader_gaussian_var_quality_location, _instance->_shader_gaussian_var_quality);
	GPU_SetUniformi(_instance->_shader_gaussian_var_directions_location, _instance->_shader_gaussian_var_directions);
	GPU_SetUniformf(_instance->_shader_gaussian_var_distance_location, _instance->_shader_gaussian_var_distance);
	GPU_BlitRect(_instance->_screen_texture, nullptr, _instance->_shader_gaussian_texture_target, nullptr);

	GPU_DeactivateShaderProgram();
	GPU_BlitRect(_instance->_shader_gaussian_texture, nullptr, _instance->_screen_texture_target, nullptr);
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
	GPU_Clear(_instance->_screen_texture_target);
}
void Render::RenderClearColor(const SDL_Color& color)
{
	GPU_ClearColor(_instance->_screen_texture_target, color);
}