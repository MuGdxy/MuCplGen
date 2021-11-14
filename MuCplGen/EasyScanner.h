#pragma once
#include <string>
#include <iostream>
#include "FileLoader.h"
#include "EasyToken.h"
#include "Scanner.h"

namespace MuCplGen
{
	class EasyScanner : public Scanner<EasyToken>
	{
		using Token = EasyToken;
	public:
		__declspec(noinline)
		EasyScanner()
		{
			Scanner::ScannRule blank;
			blank.tokenType = "Blank";
			blank.expression = "^( |\n|\t)+";
			blank.onSucceed = [this](std::smatch, Token&)->ScannActionResult
			{
				return DiscardThisToken;
			};

			Scanner::ScannRule comment;
			comment.tokenType = "Comment";
			comment.expression = "^//.*";
			comment.onSucceed = [this](std::smatch, Token&)->ScannActionResult
			{
				return (ScannActionResult)(DiscardThisToken | SkipCurrentLine);
			};

			Scanner::ScannRule num;
			num.tokenType = "number";
			num.expression = "^(\\-|\\+)?\\d+(\\.\\d+)?";
			num.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::number;
				token.color = ConsoleForegroundColor::enmCFC_White;
				return SaveToken;
			};

			Scanner::ScannRule id;
			id.priority = 1;
			id.tokenType = "identifier";
			id.expression = "^([a-z]|[A-Z]|_)\\w*";
			id.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::identifier;
				token.color = ConsoleForegroundColor::enmCFC_White;
				return SaveToken;
			};

			Scanner::ScannRule arith_operator;
			arith_operator.tokenType = "arith_op";
			arith_operator.expression = "^(\\+|\\-|\\*|/)";
			arith_operator.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::arith_op;
				token.color = ConsoleForegroundColor::enmCFC_Red;
				return SaveToken;
			};

			Scanner::ScannRule assign;
			assign.tokenType = "assign";
			assign.expression = "^=";
			assign.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::assign;
				token.color = ConsoleForegroundColor::enmCFC_Gray;
				return SaveToken;
			};

			Scanner::ScannRule sep;
			sep.tokenType = "separator";
			sep.expression = "^(\\.|\\->|::|\\{|\\}|\\(|\\)|\\[|\\]|\\,|;|:)";
			sep.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::separator;
				token.color = (ConsoleForegroundColor)14;
				return SaveToken;
			};


			Scanner::ScannRule rel_op;
			rel_op.tokenType = "rel_op";
			rel_op.expression = "^(<|>|==|!=|<=|>=)";
			rel_op.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::rel_op;
				token.color = ConsoleForegroundColor::enmCFC_Cyan;
				return SaveToken;
			};

			Scanner::ScannRule keyword;
			keyword.tokenType = "keyword";
			keyword.expression = 
				"^(void|char|float|int|return|enum|struct|class|private|switch"
				"|case|break|default|if|else|while|do)";
			keyword.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::keyword;
				token.color = ConsoleForegroundColor::enmCFC_Blue;
				return SaveToken;
			};

			Scanner::ScannRule raw_string;
			raw_string.tokenType = "raw_string";
			raw_string.expression ="^(\".*?\")|('.')";
			raw_string.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::raw_string;
				token.color = (ConsoleForegroundColor)12;
				return SaveToken;
			};

			Scanner::ScannRule logic_op;
			logic_op.tokenType = "log_op";
			//<< >> ! && || & | 
			logic_op.expression = "^(<<|>>|!|&&|\\|\\||&|\\|)";
			logic_op.onSucceed = [this](std::smatch, Token& token)->ScannActionResult
			{
				token.type = Token::TokenType::log_op;
				token.color = ConsoleForegroundColor::enmCFC_Purple;
				return SaveToken;
			};

			rules.push_back(blank);
			rules.push_back(comment);
			rules.push_back(num);
			rules.push_back(id);
			rules.push_back(arith_operator);
			rules.push_back(assign);
			rules.push_back(sep);
			rules.push_back(rel_op);
			rules.push_back(keyword);
			rules.push_back(raw_string);
			rules.push_back(logic_op);
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




