#pragma once
#include "../../MuCplGen/MuCplGen.h"

using namespace MuCplGen;
class Calculator :public SyntaxDirected<SLRParser<EasyToken, size_t>>
{

};