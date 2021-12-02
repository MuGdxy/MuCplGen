#pragma once
#include <cmath>
#include "../MuCplGen.h"

namespace MuCplGen
{
	class BaseCalculator : public BaseSubModule
	{
	public:
		BaseCalculator(BaseSyntaxDirected* bsd, const std::string& scope)
			: BaseSubModule(bsd, scope) {}

		enum class Config
		{
			None = 0,
			Add = 1,
			Sub = 1 << 1,
			Mul = 1 << 2,
			Div = 1 << 3,
			Pow = 1 << 4,
			NoPow = Add | Sub | Mul | Div,
			All = NoPow | Pow
		} config = Config::All;
	};

	template<class Ty = float>
	class Calculator : public BaseCalculator
	{
	public:
		Calculator(BaseSyntaxDirected* bsd, const std::string& scope)
			: BaseCalculator(bsd, scope) {}

		enum class Config
		{
			None = 0,
			Add = 1,
			Sub = 1 << 1,
			Mul = 1 << 2,
			Div = 1 << 3,
			Pow = 1 << 4,
			NoPow = Add | Sub | Mul | Div,
			All = NoPow | Pow
		} config = Config::All;

		//Input: [Scope.]Num<Ty>
		//Output: [Scope.]out_nonterm<Ty>
		MU_NOINLINE void CreateRules(const std::string& out_nonterm) override
		{
			{
				auto& p = CreateParseRule();
				p.action_name = "Finish Calculate";
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

			if ((size_t)config & (size_t)Config::Pow)
			{
				auto& p = CreateParseRule();
				p.action_name = "Power()";
				p.expression = "P -> P ^ F";
				p.SetAction<Ty, Ty, Empty, Ty>(
					[this](Ty P, Empty, Ty F)->Ty
					{
						return std::pow(P, F);
					});
			}

			{
				auto& p = CreateParseRule();
				p.expression = "T -> P";
			}

			if ((size_t)config & (size_t)Config::Mul)
			{
				auto& p = CreateParseRule();
				p.action_name = "Multipy()";
				p.expression = "T -> T * P";
				p.SetAction<Ty, Ty, Empty, Ty>(
					[this](Ty T, Empty, Ty P)->Ty
					{
						return T * P;
					});
			}

			if ((size_t)config & (size_t)Config::Div)
			{
				auto& p = CreateParseRule();
				p.action_name = "Divid()";
				p.expression = "T -> T / P";
				p.SetAction<Ty, Ty, Empty, Ty>(
					[this](Ty T, Empty, Ty P)->Ty
					{
						return T / P;
					});
			}

			{
				auto& p = CreateParseRule();
				p.expression = "E -> T";
			}

			if ((size_t)config & (size_t)Config::Add)
			{
				auto& p = CreateParseRule();
				p.action_name = "Add()";
				p.expression = "E -> E + T";
				p.SetAction<Ty, Ty, Empty, Ty>(
					[this](Ty E, Empty, Ty T)->Ty
					{
						return E + T;
					});
			}

			if ((size_t)config & (size_t)Config::Sub)
			{
				auto& p = CreateParseRule();
				p.action_name = "Sub()";
				p.expression = "E -> E - T";
				p.SetAction<Ty, Ty, Empty, Ty>(
					[this](Ty E, Empty, Ty T)->Ty
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
}