#pragma once
#include <regex>
#include <functional>
#include "LineContent.h"
#include "Token.h"
#include "MuException.h"
namespace MuCplGen
{
	enum ScannActionResult
	{
		SaveToken = 0,
		SkipCurrentLine = 1,
		DiscardThisToken = 1 << 1,
	};

	struct CommonRegex
	{
		static constexpr const char* Blank = "^( |\n|\t)+";
		static constexpr const char* Number = R"(^(\-|\+)?\d+(\.\d+)?)";
		static constexpr const char* Identifier = R"(^([a-z]|[A-Z]|_)\w*)";
		static constexpr const char* RawString = R"(^((".*?[^\\]")|('(\\.|.)')))";
	};
	template<class Token = BaseToken>
	struct Scanner
	{
	public:
		using ScannAction = std::function<ScannActionResult(std::smatch, Token&)>;

		struct ScannRule
		{
			int priority = 0;
			std::string tokenType;
			std::regex expression;
			ScannAction onSucceed;
		};

		ScannRule& CreateRule()
		{
			auto tmp = new ScannRule;
			rules.push_back(tmp);
			return *tmp;
		}
		ScannAction defaultAction;

		std::vector<Token> Scann(const std::vector<LineContent>& input_text)
		{
			std::vector<Token> token_set;
			auto skip_line = false;
			for (size_t line = 0; line < input_text.size(); ++line)
			{
				const std::string& content = input_text[line].content;
				auto line_head = content.cbegin();
				auto begin = line_head;
				auto end = content.cend();
				while (!skip_line && begin != end)
				{
					Token buffer;
					auto discard_token = false;
					auto max_match = begin;
					auto rule_iter = rules.begin();
					auto rule_end = rules.end();
					auto candidate = rule_end;
					std::smatch candidate_match;
					for (; rule_iter != rule_end; ++rule_iter)
					{
						std::smatch m;
						if (std::regex_search(begin, end, m, (*rule_iter)->expression))
						{
							auto match_tail = m.suffix().first;
							auto last = std::distance(begin, max_match);
							auto current = std::distance(begin, match_tail);
							if (last <= current)
							{
								auto substitute = true;
								if (last == current 
									&& candidate != rule_end 
									&& (*candidate)->priority < (*rule_iter)->priority)
								{
									substitute = false;
								}
								if (substitute)
								{
									max_match = match_tail;
									candidate = rule_iter;
									candidate_match = m;
								}
							}
						}
					}
					if (candidate != rule_end)
					{
						buffer.line = input_text[line].line_no;
						buffer.start = std::distance(line_head, candidate_match.prefix().second);
						buffer.end = std::distance(line_head, candidate_match.suffix().first) - 1;
						buffer.name = candidate_match.str();
						buffer.Type((*candidate)->tokenType);
						ScannActionResult res = SaveToken;
						if ((*candidate)->onSucceed) res = (*candidate)->onSucceed(candidate_match, buffer);
						else if (defaultAction) res = defaultAction(candidate_match, buffer);
						if (res & ScannActionResult::SkipCurrentLine) skip_line = true;
						if (res & ScannActionResult::DiscardThisToken) discard_token = true;
					}
					if (!discard_token) token_set.push_back(buffer);
					if (begin == max_match) throw Exception("Oops...No match. Maybe check your Rules?");
					begin = max_match;
				}
				if (skip_line)
				{
					skip_line = false;
					continue;
				}
			}
			Token end_token;
			end_token.end_of_tokens = true;
			token_set.push_back(std::move(end_token));
			return token_set;
		}
		~Scanner() { for (auto rule : rules) delete rule; }
	private:
		std::vector<ScannRule*> rules;
	};
}