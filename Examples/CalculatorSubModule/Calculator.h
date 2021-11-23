#pragma once
#include "../../MuCplGen/MuCplGen.h"

using namespace MuCplGen;
class CalculatorSubModule : public BaseSubModule
{
	std::string scope;
public:
	CalculatorSubModule(BaseSyntaxDirected* bsd) : BaseSubModule(*bsd) {}

	ParseRule& CreateParseRule()
	{
		auto& tmp = bsd.CreateParseRule();
		tmp.scope = scope;
		return tmp;
	}

	//Input: [Scope.]Num<float>
	//Output: [Scope.]out_nonterm<float>
	MU_NOINLINE void CreateRules(const std::string& scope, const std::string& out_nonterm) override
	{
		this->scope = scope;
		{
			auto& p = CreateParseRule();
			p.head = out_nonterm;
			p.body = { "E" };

		}

		{
			auto& p = CreateParseRule();
			p.expression = "F -> Num";
			p.head = "F";
			p.body = { "Num" };
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
			p.SetAction<float, float, Empty, float>(
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
			p.SetAction<float, float, Empty, float>(
				[this](float T, Empty, float P)->float
				{
					return T * P;
				});
		}

		{
			auto& p = CreateParseRule();
			p.action_name = "Divid()";
			p.expression = "T -> T / P";
			p.SetAction<float, float, Empty, float>(
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
			p.SetAction<float, float, Empty, float>(
				[this](float E, Empty, float T)->float
				{
					return E + T;
				});
		}

		{
			auto& p = CreateParseRule();
			p.action_name = "Sub()";
			p.expression = "E -> E - T";
			p.SetAction<float, float, Empty, float>(
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

class MainCalculator : public SyntaxDirected<SLRParser<EasyToken>>
{
	CalculatorSubModule cs;
public:
	MainCalculator(std::ostream& log = std::cout) 
		: SyntaxDirected(log), cs(this)
	{
		debug_option = DebugOption::ConciseInfo | DebugOption::ShowProductionTable;
		generation_option = BuildOption::LoadAndSave;
		SetStorage("./storage/MainCalculator.bin");

		{
			//translate number token as terminator Num
			auto& t = CreateTerminator();
			t.name = "Num";
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::number;
			};
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Expr -> CalSub.Expr";
			p.SetAction<Empty, float>(
				[this](float res)->Empty
				{
					std::cout << "Result = " << res << std::endl;
					return Empty{};
				});
		}

		cs.CreateRules("CalSub", "Expr");

		{
			auto& p = CreateParseRule();
			p.expression = "CalSub.Num -> Num";
			p.SetAction<float, Empty>(
				[this](Empty)->float
				{
					auto& token = CurrentToken();
					return std::stof(token.name);
				});
		}
	}
};