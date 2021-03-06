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
			auto& t = CreateTerminator("num");
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::number;
			};
		}

		{
			auto& t = CreateTerminator("str");
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::raw_string;
			};
		}

		{
			auto& t = CreateTerminator("vecType");
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::keyword 
					&& (token.name == "color"
						||token.name == "sampler"
						||token.name.find("vec")!=-1);
			};
		}

		{
			auto& t = CreateTerminator("baseType");
			t.priority = 1;
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::keyword;
			};
		}

		{
			auto& t = CreateTerminator("id");
			t.priority = 1;
			t.translation = [this](const Token& token)
			{
				// for name
				return token.type == Token::TokenType::identifier;
			};
		}


		//Entrance:
		{
			auto& p = CreateParseEntrance();
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
			p.expression = "Stnc -> vecType Loc Name ;";
			//Action: Finish variable
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Stnc -> baseType Loc Name ;";
			//Action: Finish variable
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Stnc -> vecType Loc Name = Vec ;";
			//Action: Finish variable
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Stnc -> baseType Loc Name = Base ;";
			//Action: Finish variable
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Name -> id";
			//Action: Set AutoVariable' name
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Loc -> < Cal.Expr >";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Loc -> epsilon";
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
			p.expression = "Base -> str";
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
			p.SetAction(
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
			p.SetAction(
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
			p.expression = "StrVec.Comp -> str";
			p.SetAction(
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
			p.expression = "Cal.Num -> num";
			p.SetAction(
				[this](Empty)->float
				{
					auto& token = CurrentToken();
					return std::stof(token.name);
				});
		}
	}
};