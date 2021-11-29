#include "../../MuCplGen/MuCplGen.h"
#include "Calculator.h"
using namespace MuCplGen;

int main()
{
	std::vector<LineContent> lines;
	LineContent line;
	line.line_no = 1;
	std::cout << "Calculate:";
	std::cin >> line.content;
	lines.push_back(line);

	EasyScanner easyScanner;
	std::vector<EasyToken> tokens = easyScanner.Scann(lines);
	Highlight(lines, tokens);

	Calculator calculator;
	calculator.Parse(lines, tokens);
}