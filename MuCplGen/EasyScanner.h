#pragma once
#include <string>
#include <iostream>
#include "FileLoader.h"
#include "EasyToken.h"
#include "Scanner.h"

namespace MuCplGen
{
	using namespace MuCplGen::Debug;
	class EasyScanner : public Scanner<EasyToken>
	{
		using Token = EasyToken;
	public:
		__declspec(noinline)
		EasyScanner()
		{
			auto& blank = CreateRule();
			blank.tokenType = "Blank";
			blank.expression = CommonRegex::Blank;
			blank.onSucceed = [this](std::smatch, Token&)->ScannActionResult
			{
				return DiscardThisToken;
			};

			auto& comment = CreateRule();
			comment.tokenType = "Comment";
			comment.expression = "^//.*";
			comment.onSucceed = [this](std::smatch, Token&)->ScannActionResult
			{
				return (ScannActionResult)(DiscardThisToken | SkipCurrentLine);
			};

			auto& num = CreateRule();
			num.tokenType = "number";
			num.expression = "^(\\-|\\+)?\\d+(\\.\\d+)?";
			num.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::number;
				token.color = ConsoleForegroundColor::White;
				return SaveToken;
			};

			auto& id = CreateRule();
			id.priority = 1;
			id.tokenType = "identifier";
			id.expression = CommonRegex::Identifier;
			id.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::identifier;
				token.color = ConsoleForegroundColor::White;
				return SaveToken;
			};

			auto& arith_operator = CreateRule();
			arith_operator.tokenType = "arith_op";
			arith_operator.expression = R"(^(\+|\-|\*|/))";
			arith_operator.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::arith_op;
				token.color = ConsoleForegroundColor::Red;
				return SaveToken;
			};

			auto& assign = CreateRule();
			assign.tokenType = "assign";
			assign.expression = "^=";
			assign.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::assign;
				token.color = ConsoleForegroundColor::Gray;
				return SaveToken;
			};

			auto& sep = CreateRule();
			sep.tokenType = "separator";
			sep.expression = R"(^(\.|\->|::|\{|\}|\(|\)|\[|\]|\,|;|:))";
			sep.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::separator;
				token.color = (ConsoleForegroundColor)14;
				return SaveToken;
			};


			auto& rel_op = CreateRule();
			rel_op.tokenType = "rel_op";
			rel_op.expression = "^(<|>|==|!=|<=|>=)";
			rel_op.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::rel_op;
				token.color = ConsoleForegroundColor::Cyan;
				return SaveToken;
			};

			auto& keyword = CreateRule();
			keyword.tokenType = "keyword";
			keyword.expression = 
				"^(void|char|float|int|return|enum|struct|class|private|switch"
				"|case|break|default|if|else|while|do)";
			keyword.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::keyword;
				token.color = ConsoleForegroundColor::Blue;
				return SaveToken;
			};

			auto& raw_string = CreateRule();
			raw_string.tokenType = "raw_string";
			raw_string.expression = CommonRegex::RawString;
			raw_string.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::raw_string;
				token.color = (ConsoleForegroundColor)12;
				return SaveToken;
			};

			auto& logic_op = CreateRule();
			logic_op.tokenType = "log_op";
			//<< >> ! && || & | 
			logic_op.expression = "^(<<|>>|!|&&|\\|\\||&|\\|)";
			logic_op.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::log_op;
				token.color = ConsoleForegroundColor::Purple;
				return SaveToken;
			};
		}
	};

	////std::vector<Scanner::Token> EasyScanner(const std::vector<LineContent>& input_text)
	//template<typename T = int>
	//std::vector<EasyToken> EasyScann(const std::vector<LineContent>& input_text)
	//{
	//	std::vector<EasyToken> token_set;
	//	RelationalOperator RelOp;
	//	LogicalOperator LogOp;
	//	ArithmeticOperator arithOp;
	//	Keyword keyword;
	//	Identifier identifier;
	//	Number number;
	//	Assignment assign;
	//	Separator separator;
	//	RawString raw_string;
	//	bool omit = false;
	//	for (size_t line = 0; line < input_text.size(); ++line)
	//	{
	//		auto str = input_text[line].content.c_str();
	//		size_t iter = 0;
	//		char c;
	//		bool skip = false;
	//		do
	//		{
	//			c = str[iter];
	//			auto next = c == '\0' ? '\0' : str[iter + 1];
	//			if (c == '/')
	//			{
	//				// for "//" comment
	//				if (str[iter + 1] == '/')
	//				{
	//					skip = true;
	//					break;
	//				}
	//				// for "/*"
	//				else if (str[iter + 1] == '*')
	//				{
	//					omit = true;
	//					++iter;
	//				}
	//			}
	//			else if (c == '*' && str[iter + 1] == '/')
	//			{
	//				omit = false;
	//				iter += 2;
	//				if (str[iter] == '\0')
	//					break;
	//				else
	//					continue;
	//			}
	//			// omit words between /**/
	//			if (omit)
	//			{
	//				++iter;
	//				continue;
	//			}
	//			if (raw_string.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(raw_string.current_token);
	//			if (!raw_string.YetNotStart()) c = '\0';
	//			if (number.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(number.current_token);
	//			if (assign.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(assign.current_token);
	//			if (keyword.Scann(c, input_text[line].line_no, iter, next))
	//			{
	//				//before Keyword shouldn't be [a...z][A...Z]["_"]
	//				int before = iter - keyword.current_token.length() - 1;
	//				if (before < 0 || !(isalpha(str[before]) || str[before] == '_'))
	//					token_set.push_back(keyword.current_token);
	//			}
	//			if (identifier.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(identifier.current_token);
	//			if (RelOp.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(RelOp.current_token);
	//			if (LogOp.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(LogOp.current_token);
	//			if (arithOp.Scann(c, input_text[line].line_no, iter, next))token_set.push_back(arithOp.current_token);
	//			// last to do: sparator 
	//			if (separator.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(separator.current_token);
	//			// recover the char.
	//			c = str[iter++];
	//		} while (c != '\0');

	//		if (skip)
	//			continue;
	//	}
	//	EasyToken t;
	//	t.type = TokenType::end_symbol;
	//	token_set.push_back(std::move(t));
	//	return token_set;
	//}
}




