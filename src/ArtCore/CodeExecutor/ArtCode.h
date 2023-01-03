#pragma once
#include <string>

#include "ArtCore/Enums/enumstring.h"

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
		OBJECT_VARIABLE,
		LOCAL_VARIABLE,
		VALUE, NULL_VALUE,
		FUNCTION, OTHER,
		// operators
		SET,
		OPERATOR,
		// commands
		TYPE, IF_BODY, IF_TEST, ELSE,
		END,


		// last enum to literate in loops
		INVALID
	};

	// operators
	const std::string Operators[5] = {
	"+=",
	"-=",
	"*=",
	"/=",
	":="
	};
	// if statement operators
	static const std::string operators2[];

	ENUM_WITH_STRING_CONVERSION(variable_type,
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

		const std::string Keywords[4] = {
		"set",
		"if",
		"end",
		"else",
	};
};
