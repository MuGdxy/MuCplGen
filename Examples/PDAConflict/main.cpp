#include <MuCplGen/MuCplGen.h>
using namespace MuCplGen;

class Reader : public SyntaxDirected<SLRParser<EasyToken>>
{
public:
	Reader(std::ostream& log = std::cout)
		: SyntaxDirected(log)
	{
		debug_option = DebugOption::ConciseInfo | DebugOption::ShowProductionTable | DebugOption::ShowReductionProcess;

		{
			//translate number token as terminator "num"
			auto& t = CreateTerminator("num");
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::number;
			};
		}

		//Entrance:
		{
			auto& p = CreateParseEntrance();
			p.expression = "S_ -> S";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "S -> F G";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "S -> G";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "G -> ( num )";
		}

		{
			auto& p = CreateParseRule();
			p.expression = "F -> epsilon";
		}
	}
};

int main()
{
	Reader reader;
	try { reader.Build(); } catch (Exception) { }
}