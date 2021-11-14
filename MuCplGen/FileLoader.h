#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "LineContent.h"

namespace MuCplGen
{
	struct FileLoader
	{
		static const size_t LINE_LENGTH = 4048;
		
		_declspec(noinline) 
		static std::vector<LineContent> Load(const std::string& path)
		{
			std::vector<LineContent> lineContent;
			size_t line_no = 1;
			std::ifstream fs(path);
			char container[LINE_LENGTH];
			while (fs.getline(container, LINE_LENGTH))
				lineContent.push_back({ container, line_no++ });
			fs.close();
			return lineContent;
		}

		_declspec(noinline)
		static std::vector<std::vector<LineContent>> Load(const std::string& path, std::string separator)
		{
			size_t iter = 0;
			std::vector<std::vector<LineContent>> inputs;
			inputs.push_back(std::vector<LineContent>());
			std::ifstream fs(path);
			char container[LINE_LENGTH];
			size_t line_no = 1;
			while (fs.getline(container, LINE_LENGTH))
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
	};	
}
