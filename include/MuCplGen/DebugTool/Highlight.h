#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <type_traits>
#include "ColoredText.h"
#include "../LineContent.h"
#include "DebugToken.h"


namespace MuCplGen::Debug
{
	template<typename Token = DebugToken>
	void Highlight(std::vector<LineContent>& input_text, std::vector<Token>& token_set,
		const std::vector<std::pair<size_t, std::string>>& error_info_pair, std::ostream& log = std::cout)
	{
		static_assert(std::is_base_of<DebugToken, Token>::value, "To Highlight, your token should derived from DebugToken");
		if ((&log) != (&std::cout)) { log << "[token highlight only validates with std::cout]"; }
		SetConsoleColor(log, ConsoleForegroundColor::White);
		auto error = false;
		size_t error_iter = 0;
		size_t start = 0;
		if (input_text.size())
			start = input_text[0].line_no;
		size_t token_iter = 0;
		for (size_t i = 0; i < input_text.size(); i++)
		{
			SetConsoleColor(log, ConsoleForegroundColor::White);
			log << "[" << input_text[i].line_no << "\t]";
			size_t j = 0;
			bool over = false;
			while (j < input_text[i].content.size())
			{
				if (token_iter >= token_set.size())
					over = true;
				if (!over)
				{
					if (token_set[token_iter].line - start == i
						&& j <= token_set[token_iter].end
						&& token_set[token_iter].start <= j)
					{
						SetConsoleColor(log, token_set[token_iter].color);
						log << input_text[i].content[j];
						++j;
					}
					else if (i == token_set[token_iter].line - start && j < token_set[token_iter].start
						|| i < token_set[token_iter].line - start)
					{
						SetConsoleColor(log, ConsoleForegroundColor::Green);
						log << input_text[i].content[j];
						++j;
					}
					else if (i == token_set[token_iter].line - start && j > token_set[token_iter].end
						|| i > token_set[token_iter].line - start)
					{
						if (error_iter < error_info_pair.size()
							&& token_iter == error_info_pair[error_iter].first) error = true;
						++token_iter;
					}
				}
				else
				{
					SetConsoleColor(log, ConsoleForegroundColor::Green);
					log << input_text[i].content[j];
					++j;
				}
			}
			log << std::endl;
			if (error)
			{
				SetConsoleColor(log, ConsoleForegroundColor::Red);
				log << "[" << input_text[i].line_no << "\t]";
				for (size_t t = 0; t < token_set[error_info_pair[error_iter].first].start; ++t)
					log << " ";
				log << "^" << error_info_pair[error_iter++].second << std::endl;
				SetConsoleColor(log);
				error = false;
			}
		}
		SetConsoleColor(log);
	}

	template<typename Token = DebugToken>
	void Highlight(std::vector<LineContent>& input_text, std::vector<Token>& token_set, std::ostream& log = std::cout)
	{
		Highlight(input_text, token_set, {}, log);
	}

	template<typename Token = DebugToken>
	void Highlight(std::vector<LineContent>& input_text, std::vector<Token>& token_set,
		size_t error_iter,const std::string& error_info, std::ostream& log = std::cout)
	{
		Highlight(input_text, token_set, { {error_iter, error_info} }, log);
	}
}
