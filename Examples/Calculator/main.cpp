#include "../../MuCplGen/MuCplGen.h"
using namespace MuCplGen;

int main()
{
	LineContent line;
	line.line_no = 1;
	std::cin >> line.content;
	EasyScanner easyScanner;
	std::vector<EasyToken> tokens = easyScanner.Scann({ line });
}