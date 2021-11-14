#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <type_traits>
#include "LineContent.h"
#include "Token.h"


namespace MuCplGen::Debug
{
	template<typename Token = DebugToken>
	void Highlight(std::vector<LineContent>& input_text, std::vector<Token>& token_set, std::ostream& log = std::cout)
	{
		static_assert(std::is_base_of<DebugToken, Token>::value, "To Highlight, your token should derived from DebugToken");
		if ((&log) != (&std::cout)) { log << "[token highlight only validates with std::cout]"; }
		SetConsoleColor(enmCFC_White);
		size_t start = 0;
		if (input_text.size())
			start = input_text[0].line_no;
		size_t token_iter = 0;
		for (size_t i = 0; i < input_text.size(); i++)
		{
			SetConsoleColor(enmCFC_White);
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
						SetConsoleColor(token_set[token_iter].color);
						log << input_text[i].content[j];
						++j;
					}
					else if (i == token_set[token_iter].line - start && j < token_set[token_iter].start
						|| i < token_set[token_iter].line - start)
					{
						SetConsoleColor((ConsoleForegroundColor)0x2/*light green*/);
						log << input_text[i].content[j];
						++j;
					}
					else if (i == token_set[token_iter].line - start && j > token_set[token_iter].end
						|| i > token_set[token_iter].line - start)
					{
						++token_iter;
					}
				}
				else
				{
					SetConsoleColor(enmCFC_Green);
					log << input_text[i].content[j];
					++j;
				}
			}
			log << std::endl;
		}
		SetConsoleColor(enmCFC_White);
	}

	template<typename Token = DebugToken>
	void Highlight(std::vector<LineContent>& input_text, std::vector<Token>& token_set,
		size_t error_iter, std::string error_info, std::ostream& log = std::cout)

	{
		static_assert(std::is_base_of<DebugToken, Token>::value, "To Highlight, your token should derived from DebugToken");
		if ((&log) != (&std::cout)) { log << "[token highlight only validates with std::cout]"; }
		SetConsoleColor(enmCFC_White);
		auto error = false;
		size_t start = 0;
		if (input_text.size())
			start = input_text[0].line_no;
		size_t token_iter = 0;
		for (size_t i = 0; i < input_text.size(); i++)
		{
			SetConsoleColor(enmCFC_White);
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
						SetConsoleColor(token_set[token_iter].color);
						log << input_text[i].content[j];
						++j;
					}
					else if (i == token_set[token_iter].line - start && j < token_set[token_iter].start
						|| i < token_set[token_iter].line - start)
					{
						SetConsoleColor((ConsoleForegroundColor)0x2/*light green*/);
						log << input_text[i].content[j];
						++j;
					}
					else if (i == token_set[token_iter].line - start && j > token_set[token_iter].end
						|| i > token_set[token_iter].line - start)
					{
						++token_iter;
						if (token_iter == error_iter) error = true;
					}
				}
				else
				{
					SetConsoleColor(enmCFC_Green);
					log << input_text[i].content[j];
					++j;
				}
			}
			log << std::endl;
			if (error)
			{
				SetConsoleColor(ConsoleForegroundColor::enmCFC_Red);
				log << "[" << input_text[i].line_no << "\t]";
				for (size_t t = 0; t < token_set[error_iter].start; ++t)
					log << " ";
				log << "^" << error_info << std::endl;
				SetConsoleColor(enmCFC_White);
				error = false;
			}
		}
		SetConsoleColor(enmCFC_White);
	}

	template<typename Token = DebugToken>
	void Highlight(std::vector<LineContent>& input_text, std::vector<Token>& token_set,
		std::vector<std::pair<size_t, std::string>>& error_info_pair, std::ostream& log = std::cout)
	{
		static_assert(std::is_base_of<DebugToken, Token>::value, "To Highlight, your token should derived from DebugToken");
		if ((&log) != (&std::cout)) { log << "[token highlight only validates with std::cout]"; }
		SetConsoleColor(enmCFC_White);
		auto error = false;
		size_t error_iter = 0;
		size_t start = 0;
		if (input_text.size())
			start = input_text[0].line_no;
		size_t token_iter = 0;
		for (size_t i = 0; i < input_text.size(); i++)
		{
			SetConsoleColor(enmCFC_White);
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
						SetConsoleColor(token_set[token_iter].color);
						log << input_text[i].content[j];
						++j;
					}
					else if (i == token_set[token_iter].line - start && j < token_set[token_iter].start
						|| i < token_set[token_iter].line - start)
					{
						SetConsoleColor((ConsoleForegroundColor)0x2/*light green*/);
						log << input_text[i].content[j];
						++j;
					}
					else if (i == token_set[token_iter].line - start && j > token_set[token_iter].end
						|| i > token_set[token_iter].line - start)
					{
						++token_iter;
						if (error_iter < error_info_pair.size()
							&& token_iter == error_info_pair[error_iter].first) error = true;
					}
				}
				else
				{
					SetConsoleColor(enmCFC_Green);
					log << input_text[i].content[j];
					++j;
				}
			}
			log << std::endl;
			if (error)
			{
				SetConsoleColor(ConsoleForegroundColor::enmCFC_Red);
				log << "[" << input_text[i].line_no << "\t]";
				for (size_t t = 0; t < token_set[error_info_pair[error_iter].first].start; ++t)
					log << " ";
				log << "^" << error_info_pair[error_iter++].second << std::endl;
				SetConsoleColor(enmCFC_White);
				error = false;
			}
		}
		SetConsoleColor(enmCFC_White);
	}
}
