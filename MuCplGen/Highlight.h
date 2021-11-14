#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
#include <vector>
#include "LineContent.h"
#include "Token/Token.h"



template<typename T = int>
void Highlight(std::vector<LineContent> input_text, std::vector<MuCplGen::EasyToken>& token_set)
{
	SetConsoleColor(enmCFC_White);
	size_t start = 0;
	if (input_text.size())
		start = input_text[0].line_no;
	size_t token_iter = 0;
	for (size_t i = 0; i < input_text.size(); i++)
	{
		SetConsoleColor(enmCFC_White);
		std::cout << "[" << input_text[i].line_no << "\t]";
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
					std::cout << input_text[i].content[j];
					++j;
				}
				else if (i == token_set[token_iter].line - start && j < token_set[token_iter].start
					|| i < token_set[token_iter].line - start)
				{
					SetConsoleColor((ConsoleForegroundColor)0x2/*light green*/);
					std::cout << input_text[i].content[j];
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
				std::cout << input_text[i].content[j];
				++j;
			}
		}
		std::cout << std::endl;
	}
	SetConsoleColor(enmCFC_White);
}

template<typename Input,typename Token_Set>
void Highlight(Input&& input_text, Token_Set&& token_set,
	size_t error_iter, std::string error_info)

{
	SetConsoleColor(enmCFC_White);
	auto error = false;
	size_t start = 0;
	if (input_text.size())
		start = input_text[0].line_no;
	size_t token_iter = 0;
	for (size_t i = 0; i < input_text.size(); i++)
	{
		SetConsoleColor(enmCFC_White);
		std::cout << "[" << input_text[i].line_no << "\t]";
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
					std::cout << input_text[i].content[j];
					++j;
				}
				else if (i == token_set[token_iter].line - start && j < token_set[token_iter].start
					|| i < token_set[token_iter].line - start)
				{
					SetConsoleColor((ConsoleForegroundColor)0x2/*light green*/);
					std::cout << input_text[i].content[j];
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
				std::cout << input_text[i].content[j];
				++j;
			}
		}
		std::cout << std::endl;
		if (error)
		{
			SetConsoleColor(ConsoleForegroundColor::enmCFC_Red);
			std::cout << "[" << input_text[i].line_no << "\t]";
			for (size_t t = 0; t < token_set[error_iter].start; ++t)
				std::cout << " ";
			std::cout << "^" << error_info << std::endl;
			SetConsoleColor(enmCFC_White);
			error = false;
		}
	}
	SetConsoleColor(enmCFC_White);
}

template<typename Input,typename Token_Set>
void Highlight(Input&& input_text, Token_Set&& token_set,
	std::vector<std::pair<size_t, std::string>>& error_info_pair)
{
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
		std::cout << "[" << input_text[i].line_no << "\t]";
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
					std::cout << input_text[i].content[j];
					++j;
				}
				else if (i == token_set[token_iter].line - start && j < token_set[token_iter].start
					|| i < token_set[token_iter].line - start)
				{
					SetConsoleColor((ConsoleForegroundColor)0x2/*light green*/);
					std::cout << input_text[i].content[j];
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
				std::cout << input_text[i].content[j];
				++j;
			}
		}
		std::cout << std::endl;
		if (error)
		{
			SetConsoleColor(ConsoleForegroundColor::enmCFC_Red);
			std::cout << "[" << input_text[i].line_no << "\t]";
			for (size_t t = 0; t < token_set[error_info_pair[error_iter].first].start; ++t)
				std::cout << " ";
			std::cout << "^" << error_info_pair[error_iter++].second << std::endl;
			SetConsoleColor(enmCFC_White);
			error = false;
		}
	}
	SetConsoleColor(enmCFC_White);
}