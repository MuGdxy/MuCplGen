#include "../../MuCplGen/MuCplGen.h"
#include "../../MuCplGen/SubModules/Sequence.h"
using namespace MuCplGen;

class SequenceReader : public SyntaxDirected<SLRParser<EasyToken>>
{
	Sequence<float> sq;
public:
	SequenceReader(std::ostream& log = std::cout)
		: SyntaxDirected(log), sq(this, "FloatVec")
	{
		debug_option = DebugOption::ConciseInfo | DebugOption::ShowProductionTable | DebugOption::ShowReductionProcess;

		{
			//translate number token as terminator Num
			auto& t = CreateTerminator();
			t.name = "Num";
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::number;
			};
		}

		{
			auto& p = CreateParseRule();
			p.expression = "Vec -> FloatVec.Vec";
			p.SetAction<Empty, std::vector<float>&>(
				[this](std::vector<float>& v)->Empty
				{
					std::cout << "Vector = (";
					for (size_t i = 0; i < v.size(); ++i)
					{
						if (i != v.size() - 1) std::cout << v[i] << ", ";
						else std::cout << v[i];
					}
					std::cout << ")" << std::endl;
					return Empty{};
				});
		}

		sq.CreateRules("Vec");

		{
			auto& p = CreateParseRule();
			p.expression = "FloatVec.Comp -> Num";
			p.SetAction<float, Empty>(
				[this](Empty)->float
				{
					auto& token = CurrentToken();
					return std::stof(token.name);
				});
		}
	}
};

int main()
{
	SequenceReader sq_reader;
	sq_reader.Build();

	std::vector<LineContent> lines;
	LineContent line;
	line.line_no = 1;
	std::cout << "Input Vector, Sep by ' ':";
	std::getline(std::cin, line.content);
	lines.push_back(line);

	EasyScanner easyScanner;
	std::vector<EasyToken> tokens = easyScanner.Scann(lines);
	Highlight(lines, tokens);


	sq_reader.Parse(lines, tokens);
}