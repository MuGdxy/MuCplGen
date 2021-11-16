#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "MuCplGen/EasyScanner.h"
#include "MuCplGen/FileLoader.h"
#include "MuCplGen/DebugTool/Highlight.h"
#include "Examples/ILGenerator/ILGenerator.h"
#include "MuCplGen/SyntaxDirected.h"
#include "MuCplGen/CFG.h"

using namespace MuCplGen;

struct MyTestParser : SyntaxDirected<LR1Parser<EasyToken,size_t>>
{
	using Term = Terminator<EasyToken, size_t>;
	MyTestParser(std::ostream& log = std::cout) :SyntaxDirected(log)
	{
		auto& t = CreateTerminator();
		t.name = "num";
		t.translation = [](const EasyToken& token)
		{
			if (token.type == EasyToken::TokenType::number) return true;
			else return false;
		};

		auto& p = CreateParseRule();
		p.action_name = "GetNumber";
		p.expression = "Num -> num";
		p.SetAction<int, Empty>(
			[this](Empty)->int
			{
				auto token = CurrentToken();
				throw(SemanticError());
				return 1;
			});
		

		auto& p0 = CreateParseRule();
		p0.action_name = "GetNumberA";
		p0.expression = "NumA -> Num ;";
		p0.SetAction<Empty, int>(
			[this](int)->Empty
			{
				auto token = CurrentToken();
				return Empty{};
			});
		p0.SetSemanticErrorAction(
			[this](std::vector<std::any*> data)->std::any*
			{
				if (HasSemanticError(data)) std::cout << "GET Semantic Error";
				int i = NextSemanticError(data);
				int a = Get<int>(data[0]);
				return data[i];
			});
		debug_option = Debug::DebugOption::AllDebugInfo;
		Initialize();
	}
};

int main()
{
	auto input_text = FileLoader::Load("assignment_test.txt");
	EasyScanner easyScanner;
	auto token_set = easyScanner.Scann(input_text);
	Debug::Highlight(input_text, token_set);
	ILGenerator ILGen;
	ILGen.Parse(input_text, token_set);
	ILGen.HighlightIfHasError();
	//ILGen.ShowILCode();
	//ILGen.ShowTables();

	//auto input_text = FileLoader::Load("easy.txt");
	//EasyScanner easyScanner;
	//auto tokens = easyScanner.Scann(input_text);
	//MyTestParser test;
	//test.Parse(tokens);
}

