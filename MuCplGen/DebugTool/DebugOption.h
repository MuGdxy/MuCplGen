#pragma once
namespace MuCplGen::Debug
{
	enum DebugOption
	{
		None = 0,
		HighlightSyntaxFile = 1,
		SyntaxFileProcessStage = 1 << 1,
		ShowProductionTable = 1 << 2,
		ShowCatchedVariables = 1 << 3,
		ShowCatchedWildTerminator = 1 << 4,
		SyntaxFileSemanticCheck = 1 << 5,
		ShowReductionProcess = 1 << 6,
		
		ParserDetail = 1 << 7,
		ParserError = 1 << 8,
		SyntaxError = 1 << 9,

		AllDebugInfo = HighlightSyntaxFile | SyntaxFileProcessStage | ShowProductionTable
		| ShowCatchedVariables | ShowCatchedWildTerminator | SyntaxFileSemanticCheck
		| ShowReductionProcess | ParserDetail | ParserError | SyntaxError,

		ConciseInfo = SyntaxFileProcessStage | ParserError | SyntaxError
	};
}