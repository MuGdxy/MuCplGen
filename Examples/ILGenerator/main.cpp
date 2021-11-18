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

	auto input_text = FileLoader::Load("assignment_test.txt");
	
	StopWatch total;
	total.Start();
	StopWatch scanner_build_time;
	scanner_build_time.Start();
	EasyScanner easyScanner;
	scanner_build_time.Stop();

	StopWatch scann_time;
	scann_time.Start();
	auto token_set = easyScanner.Scann(input_text);
	Debug::Highlight(input_text, token_set);
	scann_time.Stop();
	
	StopWatch parser_build_time;
	ILGenerator ILGen;
	parser_build_time.Start();
	ILGen.Build();
	parser_build_time.Stop();

	StopWatch parse_time;
	parse_time.Start();
	ILGen.Parse(input_text, token_set);
	ILGen.HighlightIfHasError();
	//ILGen.ShowILCode();
	//ILGen.ShowTables();
	parse_time.Stop();
	total.Stop();
	
	std::cout << "scanner_build=" << scanner_build_time << std::endl;
	std::cout << "scann_time=" << scann_time << std::endl;
	std::cout << "parser_build=" << parser_build_time << std::endl;
	std::cout << "parse_time=" << parse_time << std::endl;
	std::cout << "total=" << total << std::endl;
}

