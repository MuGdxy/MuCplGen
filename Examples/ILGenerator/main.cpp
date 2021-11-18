#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../../MuCplGen/MuCplGen.h"
#include "Examples/ILGenerator/ILGenerator.h"
#include "../../MuCplGen/DebugTool/StopWatch.h"
using namespace MuCplGen;

int main()
{
	StopWatch sw;
	sw.Start();
	auto input_text = FileLoader::Load("assignment_test.txt");
	EasyScanner easyScanner;
	auto token_set = easyScanner.Scann(input_text);
	Debug::Highlight(input_text, token_set);
	
	ILGenerator ILGen;
	ILGen.Parse(input_text, token_set);
	ILGen.HighlightIfHasError();

	//ILGen.ShowILCode();
	//ILGen.ShowTables();
	sw.Stop();
	std::cout << sw << std::endl;
}

