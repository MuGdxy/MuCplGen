#pragma once
#include <cmath>
#include <MuCplGen/MuCplGen.h>

using namespace MuCplGen;
class Calculator :public SyntaxDirected<SLRParser<EasyToken>>
{
public:
	Calculator(std::ostream& log = std::cout) : SyntaxDirected(log)
	{
		debug_option = Debug::DebugOption::ShowReductionProcess;
		generation_option = BuildOption::LoadAndSave;
		SetStorage("./storage/Calculator.bin");

		{
			//translate number token as terminator "num"
			auto& t = CreateTerminator("num");
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::number;
			};
		}


		{
			auto& p = CreateParseRule();
			p.expression = "Expr -> E";
			p.SetAction(
				[this](float res)->Empty
				{
					std::cout << "Result = " << res << std::endl;
					return Empty{};
				});
		}


		{
			auto& p = CreateParseRule();
			p.expression = "F -> num";
			p.SetAction(
				[this](Empty)->float
				{
					auto& token = CurrentToken();
					return std::stof(token.name);
				});
		}

		{
			auto& p = CreateParseRule();
			p.expression = "P -> F";
			p.SetAction(PassOn(0));
		}

		{
			auto& p = CreateParseRule();
			p.action_name = "Power()";
			p.expression = "P -> P ^ F";
			p.SetAction(
				[this](float P, Empty, float F)->float
				{
					return std::pow(P, F);
				});
		}

		{
			auto& p = CreateParseRule();
			p.expression = "T -> P";
		}

		{
			auto& p = CreateParseRule();
			p.action_name = "Multipy()";
			p.expression = "T -> T * P";
			p.SetAction(
				[this](float T, Empty, float P)->float
				{
					return T * P;
				});
		}

		{
			auto& p = CreateParseRule();
			p.action_name = "Divid()";
			p.expression = "T -> T / P";
			p.SetAction(
				[this](float T, Empty, float P)->float
				{
					return T / P;
				});
		}

		{
			auto& p = CreateParseRule();
			p.expression = "E -> T";
		}

		{
			auto& p = CreateParseRule();
			p.action_name = "Add()";
			p.expression = "E -> E + T";
			p.SetAction(
				[this](float E, Empty, float T)->float
				{
					return E + T;
				});
		}

		{
			auto& p = CreateParseRule();
			p.action_name = "Sub()";
			p.expression = "E -> E - T";
			p.SetAction(
				[this](float E, Empty, float T)->float
				{
					return E - T;
				});
		}

		{
			auto& p = CreateParseRule();
			p.action_name = "Compress()";
			p.expression = "F -> ( E )";
			p.SetAction(PassOn(1));
		}
	}
};