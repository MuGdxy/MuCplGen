#pragma once
#include "SyntaxDirected.h"

class DefinitionCompiler :public SyntaxDirected<DefinitionCompiler>
{
public:
	DefinitionCompiler(std::string cfg_path) :SyntaxDirected(cfg_path)
	{
		Initialization();
	}
	virtual void SetupSemanticActionTable() override
	{
		
	}
};