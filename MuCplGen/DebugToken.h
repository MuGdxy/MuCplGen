#pragma once
#include "Token.h"
#include "ColoredText.h"

namespace MuCplGen::Debug
{
	struct DebugToken :public BaseToken
	{
		ConsoleForegroundColor color = ConsoleForegroundColor::enmCFC_White;
	};
}