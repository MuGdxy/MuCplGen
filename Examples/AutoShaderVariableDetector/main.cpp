#include "VariableReader.h"
using namespace MuCplGen;

int main()
{
	EasyScanner easyScanner;
	easyScanner.SetKeyword(std::regex("^(int|float|color|sampler|vec2|vec3|vec4|bool|true|false)"));
	VariableReader reader;
	reader.Build();

	auto lines = FileLoader::Load("./Auto.shader");
	std::vector<EasyToken> tokens = easyScanner.Scann(lines);
	reader.Parse(lines, tokens);
	Highlight(lines, tokens);
}