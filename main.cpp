#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "MuCplGen/EasyScanner.h"
#include "MuCplGen/FileLoader.h"
#include "MuCplGen/DebugTool/Highlight.h"
#include "Examples/ILGenerator/ILGenerator.h"


using namespace MuCplGen;

struct MyTestCompiler : SyntaxDirected<SLRParser<size_t>>
{
	std::ostream& o;
	MyTestCompiler(std::string cfg_path, std::ostream& log = std::cout) :SyntaxDirected(cfg_path), o(log)
	{
		Initialize();
	}
};

int main()
{
	auto input_text = FileLoader::Load("assignment_test.txt");
	EasyScanner easyScanner;
	auto token_set = easyScanner.Scann(input_text);
	Debug::Highlight(input_text, token_set);
	ILGenerator ILGen("complete_syntax.syn");
	//MyTestCompiler test("complete_syntax.syn");
	ILGen.Parse(input_text, token_set);
	ILGen.HighlightIfHasError();
	ILGen.ShowILCode();
}