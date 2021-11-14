#pragma once
namespace MuCplGen::Debug
{
	enum DebugOption
	{
		None = 0,
		HighlightSyntaxFile = 1,
		SyntaxFileProcessStage = 1 << 1,
		SyntaxFileProcessDetail = 1 << 2,
		SyntaxFileCatchedVariables = 1 << 3,
		SyntaxFileCatchedCandidates = 1 << 4,
		SyntaxFileSemanticCheck = 1 << 5,
		ShowReductionProcess = 1 << 6,
		
		ParserDetail = 1 << 7,
		ParserError = 1 << 8,
		SyntaxError = 1 << 9,

		AllDebugInfo = HighlightSyntaxFile | SyntaxFileProcessStage | SyntaxFileProcessDetail
		| SyntaxFileCatchedVariables | SyntaxFileCatchedCandidates | SyntaxFileSemanticCheck
		| ShowReductionProcess | ParserDetail | ParserError,

		ConciseInfo = SyntaxFileProcessStage | ParserError | SyntaxError
	};
}