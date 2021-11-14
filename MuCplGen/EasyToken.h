#pragma once
#include <iostream>
#include "DebugToken.h"

namespace MuCplGen
{
	struct EasyToken : public Debug::DebugToken
	{
		enum class TokenType
		{
			none,
			rel_op,
			arith_op,
			log_op,
			number,
			identifier,
			assign,
			keyword,
			separator,
			raw_string,
			custom_type
		};
		TokenType type = TokenType::none;
		friend std::ostream& operator << (std::ostream& os, const EasyToken& token)
		{
			os << dynamic_cast<const BaseToken&>(token) << std::endl;
			return os;
		}
	};
}