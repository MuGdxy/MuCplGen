#pragma once

#include <MuCplGen/MuCplGen.h>
#include <MuCplGen/SubModules/Calculator.h>
#include <MuCplGen/SubModules/Sequence.h>

class VariableReader : public MuCplGen::SyntaxDirected<MuCplGen::LR1Parser<MuCplGen::EasyToken>>
{
	MuCplGen::Sequence<std::string> str_sq;
	MuCplGen::Sequence<float> num_sq;
	MuCplGen::Sequence<MuCplGen::Empty> stmt_sq;
	MuCplGen::Calculator<float> cal;
public:
	VariableReader(std::ostream& log = std::cout)
		: SyntaxDirected(log),
		num_sq(this, "NumVec"), str_sq(this, "StrVec"), stmt_sq(this, "Stmt"),
		cal(this, "Cal")
	{
		using namespace MuCplGen;
		debug_option = DebugOption::ConciseInfo
			| DebugOption::ShowCatchedVariables
			| DebugOption::ShowProductionTable
			| DebugOption::ShowReductionProcess;

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
			//translate number token as terminator Num
			auto& t = CreateTerminator();
			t.name = "Str";
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::raw_string;
			};
		}

		{
			//translate number token as terminator Num
			auto& t = CreateTerminator();
			t.name = "VecType";
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::keyword 
					&& (token.name == "color"
						||token.name == "sampler"
						||token.name.find("vec")!=-1);
			};
		}

		{
			//translate number token as terminator Num
			auto& t = CreateTerminator();
			t.name = "BaseType";
			t.priority = 1;
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::keyword;
			};
		}

		{
			//translate number token as terminator Num
			auto& t = CreateTerminator();
			t.name = "Id";
			t.priority = 1;
			t.translation = [this](const Token& token)
			{
				// for name
				return token.type == Token::TokenType::identifier;
			};
		}


		//Entrance:
		{
			auto& p = CreateParseRule();
			p.expression = "Prgm -> Stmt.Vec";
		}

		stmt_sq.CreateRules("Vec");

		{
			auto& p = CreateParseRule();
			p.expression = "Stmt.Comp -> Stnc";
			p.action_name = "Finish Sentence";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Stnc -> VecDef ;";
			//Action: Finish variable
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Stnc -> BaseDef ;";
			//Action: Finish variable
		}

		//{
		//	auto& p = CreateParseRule();
		//	p.expression = "Stnc -> VecDef = Vec ;";
		//	//Action: Finish variable
		//}

		//{
		//	auto& p = CreateParseRule();
		//	p.expression = "Stnc -> BaseDef = Base ;";
		//	//Action: Finish variable
		//}

		{
			auto& p = CreateParseRule();
			p.expression = "Stnc -> Def ;";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Def -> BaseDef";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Def -> VecDef";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "VecDef -> VecType Name";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "VecDef -> VecType < Loc > Name";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "BaseDef -> BaseType Name";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "BaseDef -> BaseType < Loc > Name";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Name -> Id";
			//Action: Set AutoVariable' name
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Loc -> Cal.Expr";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Vec -> NumVec";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Vec -> StrVec";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Base -> Str";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Base -> Cal.Expr";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Base -> true";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Base -> false";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "NumVec -> ( NumVec.Vec )";
			p.SetAction<Empty, Empty, std::vector<float>&, Empty>(
				[this](Empty, std::vector<float>& v, Empty)->Empty
				{
					std::cout << "Vector = (";
					for (size_t i = 0; i < v.size(); ++i)
					{
						if (i != v.size() - 1) std::cout << v[i] << ", ";
						else std::cout << v[i];
					}
					std::cout << ")" << std::endl;
					return Empty{};
				});
		}

		{
			auto& p = CreateParseRule();
			p.expression = "StrVec -> ( StrVec.Vec )";
			p.SetAction<Empty, Empty, std::vector<std::string>&, Empty>(
				[this](Empty, std::vector<std::string>& v, Empty)->Empty
				{
					std::cout << "Vector = (";
					for (size_t i = 0; i < v.size(); ++i)
					{
						if (i != v.size() - 1) std::cout << v[i] << ", ";
						else std::cout << v[i];
					}
					std::cout << ")" << std::endl;
					return Empty{};
				});
		}

		str_sq.SetSeparator(",");
		str_sq.CreateRules("Vec");

		{
			auto& p = CreateParseRule();
			p.expression = "StrVec.Comp -> Str";
			p.SetAction<std::string, Empty>(
				[this](Empty)->std::string
				{
					auto& token = CurrentToken();
					return token.name;
				});
		}

		num_sq.SetSeparator(",");
		num_sq.CreateRules("Vec");

		{
			auto& p = CreateParseRule();
			p.expression = "NumVec.Comp -> Cal.Expr";
		}

		cal.CreateRules("Expr");

		{
			auto& p = CreateParseRule();
			p.expression = "Cal.Num -> Num";
			p.SetAction<float, Empty>(
				[this](Empty)->float
				{
					auto& token = CurrentToken();
					return std::stof(token.name);
				});
		}
	}
};