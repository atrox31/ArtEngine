#pragma once
#include "Event.h"
#include <string>

struct ArtCode {
public:
	enum class Command {
		// definitions
		OBJECT_DEFINITION,
		FUNCTION_DEFINITION,
		LOCAL_VARIABLE_DEFINITION,
		TYPEDEF,
		// references
		OBJECT,
		OBJECT_VARIBLE,
		LOCAL_VARIBLE,
		VALUE, NULL_VALUE,
		FUNCTION, OTHER,
		// operators
		SET,
		OPERATOR,
		// commands
		TYPE, IF_BODY, IF_TEST, ELSE,
		END,


		// last enum to literate in loops
		INWALID
	};

	// operators
	const std::string operators[5] = {
	"+=",
	"-=",
	"*=",
	"/=",
	":="
	};
	// if statement operators
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
		(ENUM) // as string, editor is responsible for good assign
	)

		const std::string keywords[4] = {
		"set",
		"if",
		"end",
		"else",
	};
};