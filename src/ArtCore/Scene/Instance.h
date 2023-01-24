#pragma once
#include <string>

#include "ArtCore/Enums/Event.h"
#include "ArtCore/Graphic/Sprite.h"
#include "FC_Fontcache/SDL_FontCache.h"

class Instance final
{
public:
	explicit Instance(int instance_definition_id);
	// do not use, this give unique id for instance
	Instance* GiveId();

	[[nodiscard]] Uint64 GetId() const { return _id; }
	void Delete();
	[[nodiscard]] int GetInstanceDefinitionId() const {	return _instance_definition_id; }

	void DrawSelf();
	bool CheckMaskClick(SDL_FPoint&) const;

	[[nodiscard]] Rect GetBodyMask() const;

	std::string Tag = "undefined";
	std::string Name = "undefined";

	bool InView = false;
	bool Alive = true;
	bool IsCollider = false;

	float PosX = 0.f;
	float PosY = 0.f;
	float Direction = 0.f;

	Sprite* SelfSprite = nullptr;
	float SpriteScaleX = 1.f;
	float SpriteScaleY = 1.f;
	int SpriteCenterX = 0;
	int SpriteCenterY = 0;
	float SpriteAngle = 0.f;

	float SpriteAnimationFrame = 0.f;
	float SpriteAnimationSpeed = 60.f;
	bool SpriteAnimationLoop = true;

	// ReSharper disable CppInconsistentNaming
	std::vector<int> Variables_int{};
	std::vector<float> Variables_float{};
	std::vector<bool> Variables_bool{};
	std::vector<Instance*> Variables_instance{};
	std::vector<int> Variables_object{};
	std::vector<int> Variables_sprite{};
	std::vector<int> Variables_texture{};
	std::vector<int> Variables_sound{};
	std::vector<int> Variables_music{};
	std::vector<int> Variables_font{};
	std::vector<Rect> Variables_rect{};
	std::vector<SDL_FPoint> Variables_point{};
	std::vector<SDL_Color> Variables_color{};
	std::vector<std::string> Variables_string{};
	// ReSharper restore CppInconsistentNaming
	
	event_bit EventFlag = event_bit::NONE;

	struct body {
	public:
		enum class type {
			None, Circle, Rectangle//, Polygon
		};
		type Type = type::None;
		// Circle type radius
		float Radius = 0.f;
		// Rectangle type dimensions
		SDL_FPoint Dimensions = { 0.f, 0.f };
		body() = default;

		explicit body(const type body_type, const float dim_1 = 0.f, const float dim_2 = 0.f) {
			switch (body_type) {
			using enum Instance::body::type;
			case Circle:
				Radius = dim_1;
				Type = Circle;
				return;
			case Rectangle:
				Dimensions.x = dim_1;
				Dimensions.y = dim_2;
				Type = Rectangle;
				return;
			case None:
				Type = None;
				return;
			}
		}

		[[nodiscard]] bool HaveValue() const {
			switch (Type) {
				using enum Instance::body::type;
			case Circle:
				return (Radius > 1.f );
			case Rectangle:
				return ((Dimensions.x > 1.f) && (Dimensions.y > 1.f));
			case None: default:
				return false;
			}
		}

		void MakeRectangle(const float& width, const float& height) {
			Dimensions.x = width;
			Dimensions.y = height;
			Type = type::Rectangle;
		}
		void MakeCircle(const float& radius) {
			Radius = radius;;
			Type = type::Circle;
		}

		void FromSpriteMask(const Mask& mask) {
			switch (mask.type) {
			case Mask::Circle:
				Radius = mask.r;
				Type = type::Circle;
				return;
			case Mask::Rectangle:
				Dimensions.x = mask.w;
				Dimensions.y = mask.h;
				Type = type::Rectangle;
				return;
			case Mask::None:
			case Mask::mask_typeInvalid: 
			case Mask::mask_typeEND:
				Type = type::None;
				return;
			}
		}
	} Body{};

	/**
	 * \brief Check if can add more suspended state to instance (max is 255)
	 * \return Answer is can add more state
	 */
	bool SuspendedCodeAdd();

	/**
	 * \brief Pop suspended code
	 * \return Operation success state
	 */
	bool SuspendedCodePop();
	// get count of suspended code
	[[nodiscard]] Uint8 SuspendedCodeStateCount() const { return _have_suspended_code;  }
	// get info if have at least one suspended code
	[[nodiscard]] bool SuspendedCodeStateHave() const { return _have_suspended_code > (uint8_t)0;  }
private:
	Uint64 _id = 0;
	static Uint64 _cid;
	int _instance_definition_id;
	uint8_t _have_suspended_code = false;
};
