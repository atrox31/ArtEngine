#pragma once

#include <string>
#include "ArtCore/Enums/EnumExtend.h"

struct Mask {
public:
	ENUM_WITH_STRING_CONVERSION(mask_type, (None)(Circle)(Rectangle))
	float w = 0.f;
	float h = 0.f;
	float r = 0.f;
	mask_type type = None;

	Mask() = default;

	void Make(const std::string& mask_type, const float& arg1 = 0.f, const float& arg2 = 0.f) {
		switch (mask_type_fromString(mask_type)) {
		case None:
		case mask_typeInvalid:
		case mask_typeEND:
		default:
			type = None;
			break;
		case Rectangle:
			type = Rectangle;
			w = arg1;
			h = arg2;
			break;
		case Circle:
			type = Circle;
			r = arg1;
			break;
		}
	}

	void MakeRect(const float& width, const float& height) {
		w = width;
		h = height;
		type = Rectangle;
	}

	void MakeCircle(const float& radius) {
		r = radius;
		type = Circle;
	}

	[[nodiscard]] bool IsTypeNone() const {
		return type != None;
	}
	[[nodiscard]] bool HaveValue() const {
		switch (type) {
		case Circle: return (r > 1.f);
		case Rectangle: return ((w > 1.f) && (h > 1.f));
		case None: default: return false;
		}
	}
};
