#pragma once
#include <cmath>
#include "../../MuCplGen/MuCplGen.h"

using namespace MuCplGen;
class Calculator :public SyntaxDirected<SLRParser<EasyToken>>
{
public:
	Calculator(std::ostream& log = std::cout) : SyntaxDirected(log)
	{
		debug_option = Debug::DebugOption::ShowReductionProcess
			| Debug::DebugOption::ShowProductionTable
			| Debug::DebugOption::ShowCatchedWildTerminator
			| Debug::DebugOption::ConciseInfo;

		{
			auto& t = CreateTerminator("$");
			t.priority = 2;
			t.translation = [this](const Token& token)
			{
				if (token.name == "B")
				{
					auto a = 1;
				}
				return true;
			};
		}
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
			p.expression = "Exit -> { Expr }";
			p.SetAction(
				[this](Empty, Empty)->Empty
				{
					std::cout << "Exit Sub Parser" << std::endl;
					return Empty{};
				});
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

class MainParser :public SyntaxDirected<LR1Parser<EasyToken>>
{
	Calculator sub_parser;
	bool Start = false;
public:
	MainParser() :sub_parser(std::cout), SyntaxDirected(std::cout)
	{
		sub_parser.Build();
		debug_option = Debug::DebugOption::ShowReductionProcess
			| Debug::DebugOption::ShowProductionTable
			| Debug::DebugOption::ShowCatchedWildTerminator
			| Debug::DebugOption::ConciseInfo;
		{
			auto& p = CreateParseEntrance();
			p.expression = "S_ -> Ss";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Ss -> Ss S";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Ss -> S";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "S -> A SubParA { }";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "S -> B SubParB { }";
		}

		CreateSubParseRule("SubParA", sub_parser,
			[this](Empty)
			{
				Debug::SetConsoleColor(log, ConsoleForegroundColor::Green);
				std::cout << "Hello, We are going into SubParserA!" << std::endl;
				SubParse(sub_parser);
				Debug::SetConsoleColor();
				std::cout << std::endl;
				return Empty{};
			});

		CreateSubParseRule("SubParB", sub_parser,
			[this](Empty)
			{
				Debug::SetConsoleColor(log, ConsoleForegroundColor::Blue);
				std::cout << "Hello, We are going into SubParserB!" << std::endl;
				SubParse(sub_parser);
				Debug::SetConsoleColor();
				std::cout << std::endl;
				return Empty{};
			});

		//{
		//	auto& p = CreateParseRule();
		//	p.expression = "End -> end";
		//	p.SetAction([this](Empty)
		//		{
		//			Start = false;
		//			std::cout << "End SubPart!" << std::endl;
		//			return Empty{};
		//		});
		//}
	}
};