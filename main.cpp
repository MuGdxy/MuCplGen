#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "MuCplGen/EasyScanner.h"
#include "MuCplGen/FileLoader.h"
#include "MuCplGen/DebugTool/Highlight.h"
//#include "Examples/ILGenerator/ILGenerator.h"
#include "MuCplGen/SyntaxDirected.h"
#include "MuCplGen/CFG.h"

using namespace MuCplGen;

struct MyTestCompiler : SyntaxDirected<LR1Parser<EasyToken,size_t>>
{
	using Term = Terminator<EasyToken, size_t>;
	MyTestCompiler(std::string cfg_path = "", std::ostream& log = std::cout) :SyntaxDirected("", log)
	{
		Term* t = new Term;
		t->name = "num";
		t->translation = [](const EasyToken& token)
		{
			if (token.type == EasyToken::TokenType::number) return true;
			else return false;
		};
		AddTerminator(t);

		ParseRule* p = new ParseRule;
		p->action_name = "GetNumber";
		p->expression = "Num -> num";
		p->SetAction<int, Empty>(
			[this](Empty)->int
			{
				auto token = CurrentToken();
				throw(SemanticError());
				return 1;
			});
		

		ParseRule* p0 = new ParseRule;
		p0->action_name = "GetNumberA";
		p0->expression = "NumA -> Num ;";
		p0->SetAction<Empty, int>(
			[this](int)->Empty
			{
				auto token = CurrentToken();
				return Empty{};
			});
		p0->SetSemanticErrorAction(
			[this](std::vector<std::any*> data)->std::any*
			{
				if (HasSemanticError(data)) std::cout << "GET Semantic Error";
				int i = NextSemanticError(data);
				int a = Get<int>(data[0]);
				return data[i];
			});
		AddParseRule(p0);
		AddParseRule(p);

		debug_option = Debug::DebugOption::AllDebugInfo;

		Initialize();
	}
};

int main()
{
	//auto input_text = FileLoader::Load("assignment_test.txt");
	//EasyScanner easyScanner;
	//auto token_set = easyScanner.Scann(input_text);
	//Debug::Highlight(input_text, token_set);
	//ILGenerator ILGen("complete_syntax.syn");
	
	//ILGen.Parse(input_text, token_set);
	//ILGen.HighlightIfHasError();
	//ILGen.ShowILCode();

	auto input_text = FileLoader::Load("easy.txt");
	EasyScanner easyScanner;
	auto tokens = easyScanner.Scann(input_text);
	MyTestCompiler test;
	test.Parse(tokens);
	//std::any o = 1;
	//std::any data = EasyToken{};
	//std::any e = Empty{};
	//std::vector<std::any*> as;
	//as.push_back(&e);
	//as.push_back(&o);
	//as.push_back(&data);
	//ParseRule prod;
	//prod.SetAction<PassOn, Empty, int, EasyToken&>(
	//	[](Empty, int b, EasyToken& token)
	//	{
	//		std::cout << token;
	//		return PassOn(2);
	//	});

	//auto res = prod.semantic_action(as);
}