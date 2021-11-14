#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "LineContent.h"

template<typename T = int>
std::vector<LineContent> FileLoader(std::string path)
{
	std::vector<LineContent> lineContent;
	size_t line_no = 1;
	std::ifstream fs(path);
	char container[1024];
	while (fs.getline(container, 1024))
	{
		lineContent.push_back({ std::string(container),line_no++ });
	}
	fs.close();
	return lineContent;
}

template<typename T = int>
std::vector<std::vector<LineContent>> FileLoader(std::string path, std::string separator)
{
	size_t iter = 0;
	std::vector<std::vector<LineContent>> inputs;
	inputs.push_back(std::vector<LineContent>());
	std::ifstream fs(path);
	char container[1024];
	size_t line_no = 1;
	while (fs.getline(container, 1024))
	{
		LineContent line_content = { container,line_no };
		if (line_content.content == separator)
		{
			inputs.push_back(std::vector<LineContent>());
			++iter;
		}
		else
			inputs[iter].push_back(line_content);
		++line_no;
	}
	fs.close();
	return inputs;
}