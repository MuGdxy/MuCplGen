#pragma once
#include <string>
#include <iostream>
#include "Platform.h"
#include "FileLoader.h"
#include "EasyToken.h"
#include "Scanner.h"

namespace MuCplGen
{
	using namespace MuCplGen::Debug;
	class EasyScanner : public Scanner<EasyToken>
	{
		using Token = EasyToken;
		ScannRule* keyword;
	public:
		MU_NOINLINE
		EasyScanner()
		{
			auto& blank = CreateRule();
			blank.tokenType = "Blank";
			blank.expression = CommonRegex::Blank;
			blank.onSucceed = [this](std::smatch&, Token&)->ScannActionResult
			{
				return DiscardThisToken;
			};

			auto& comment = CreateRule();
			comment.tokenType = "Comment";
			comment.expression = "^//.*";
			comment.onSucceed = [this](std::smatch&, Token&)->ScannActionResult
			{
				return (ScannActionResult)(DiscardThisToken | SkipCurrentLine);
			};

			auto& num = CreateRule();
			num.tokenType = "number";
			num.expression = R"(^\d+(\.\d+)?)";
			num.onSucceed = [this](std::smatch&, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::number;
				token.color = ConsoleForegroundColor::White;
				return SaveToken;
			};

			auto& id = CreateRule();
			id.priority = 1;
			id.tokenType = "identifier";
			id.expression = CommonRegex::Identifier;
			id.onSucceed = [this](std::smatch&, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::identifier;
				token.color = ConsoleForegroundColor::White;
				return SaveToken;
			};

			auto& arith_operator = CreateRule();
			arith_operator.tokenType = "arith_op";
			arith_operator.expression = R"(^(\+|\-|\*|/|\^))";
			arith_operator.onSucceed = [this](std::smatch&, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::arith_op;
				token.color = ConsoleForegroundColor::Red;
				return SaveToken;
			};

			auto& assign = CreateRule();
			assign.tokenType = "assign";
			assign.expression = "^=";
			assign.onSucceed = [this](std::smatch&, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::assign;
				token.color = ConsoleForegroundColor::Gray;
				return SaveToken;
			};

			auto& sep = CreateRule();
			sep.tokenType = "separator";
			sep.expression = R"(^(\.|\->|::|\{|\}|\(|\)|\[|\]|\,|;|:))";
			sep.onSucceed = [this](std::smatch&, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::separator;
				token.color = (ConsoleForegroundColor)14;
				return SaveToken;
			};


			auto& rel_op = CreateRule();
			rel_op.tokenType = "rel_op";
			rel_op.expression = "^(<|>|==|!=|<=|>=)";
			rel_op.onSucceed = [this](std::smatch&, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::rel_op;
				token.color = ConsoleForegroundColor::Cyan;
				return SaveToken;
			};

			auto& keyword = CreateRule();
			this->keyword = &keyword;
			keyword.tokenType = "keyword";
			keyword.expression = 
				"^(void|char|float|int|return|enum|struct|class|private|switch"
				"|case|break|default|if|else|while|do)";
			keyword.onSucceed = [this](std::smatch&, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::keyword;
				token.color = ConsoleForegroundColor::Blue;
				return SaveToken;
			};

			auto& raw_string = CreateRule();
			raw_string.tokenType = "raw_string";
			raw_string.expression = CommonRegex::RawString;
			raw_string.onSucceed = [this](std::smatch&, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::raw_string;
				token.color = (ConsoleForegroundColor)12;
				return SaveToken;
			};

			auto& logic_op = CreateRule();
			logic_op.tokenType = "log_op";
			//<< >> ! && || & | 
			logic_op.expression = "^(<<|>>|!|&&|\\|\\||&|\\|)";
			logic_op.onSucceed = [this](std::smatch&, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::log_op;
				token.color = ConsoleForegroundColor::Purple;
				return SaveToken;
			};
		}

		void SetKeyword(const std::regex& r) { keyword->expression = r; }
	};
}




