#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "LineContent.h"
#include "Platform.h"

namespace MuCplGen
{
	struct FileLoader
	{		
		MU_NOINLINE
		static std::vector<LineContent> Load(const std::string& path)
		{
			std::vector<LineContent> lineContent;
			size_t line_no = 1;
			std::ifstream fs(path);
			std::string buf;
			while (std::getline(fs, buf));
				lineContent.push_back({ std::move(buf), line_no++ });
			fs.close();
			return lineContent;
		}

		MU_NOINLINE
		static std::vector<std::vector<LineContent>> Load(const std::string& path, std::string separator)
		{
			size_t iter = 0;
			std::vector<std::vector<LineContent>> inputs;
			inputs.push_back(std::vector<LineContent>());
			std::ifstream fs(path);
			size_t line_no = 1;
			std::string buf;
			while (std::getline(fs, buf))
			{
				LineContent line_content = { std::move(buf), line_no };
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
