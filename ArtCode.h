#pragma once
#include "Event.h"
#include <string>

struct ArtCode {
public:
	enum class Command {
		// definicje
		OBJECT_DEFINITION,
		FUNCTION_DEFINITION,
		LOCAL_VARIBLE_DEFINITION,
		TYPEDEF,
		// odwo³ania
		OBJECT,
		OBJECT_VARIBLE,
		LOCAL_VARIBLE,
		VALUE, NULL_VALUE,
		FUNCTION, OTHER,
		// operatory
		SET,
		OPERATOR,
		// polecenia
		TYPE, IF_BODY, IF_TEST, ELSE,
		END,


		// tylko w silniku, zawsze ostatnia
		INWALID
	};

	// not used now
	const std::string operators[6] = {
	"+",
	"-",
	"*",
	"/",
	"!",
	"."
	};

	static const std::string operators2[];

	ENUM_WITH_STRING_CONVERSION(varible_type,
		(NUL)
		(INT)
		(FLOAT)
		(BOOL)
		(INSTANCE)
		(OBJECT)
		(SPRITE)
		(TEXTURE)
		(SOUND)
		(MUSIC)
		(FONT)
		(POINT)
		(RECT)
		(COLOR)
		(STRING)
	)
		/*
		const std::string varible_type[15] = {
		"null",
		"int",
		"float",
		"bool",
		"instance",
		"object",
		"sprite",
		"texture",
		"sound",
		"music",
		"font",
		"point",
		"rectangle",
		"color",
		"string"
		};*/

		const std::string keywords[4] = {
		"set",
		"if",
		"end",
		"else",
	};
};