#pragma once
#include <MuParser/SyntaxDirected.h>

class ExampleCompiler :public SyntaxDirected<ExampleCompiler>
{
public:
	ExampleCompiler(std::string cfg_path) :SyntaxDirected(cfg_path)
	{
		Initialization();
	}

	virtual void SetupSemanticActionTable() override
	{
		AddAction(ExampleAction1);
		AddAction(ExampleAction2);
		AddAction(ExampleAction3);
		AddAction(ExampleAction4);
	}

	struct MyData
	{
		// data you want to pass
	};
	std::vector<MyData> myData;

	void* ExampleAction1(INPUT)
	{
		// Get the data struct from the first Symbol of the Production Body  
		auto index = 0;
		auto value1 = GetValue(MyData, index);
		auto value2 = GetValue(int, 1);
		auto value3 = GetValue(int, 2);
		
		// Use default ctor to build a new Custome Data Struct
		// There is no need to delete the ptr,causes SyntaxDirected will do the GC,when
		// you use Create() to construct a new object
		auto create = Create(MyData);

		// to create a new object from expression
		auto create_from = CreateFrom(value2+value3);
		int a = 1;
		auto create_from1 = CreateFrom(a);

		// to create a new object copy from expression but you can do
		// some convert if allowed
		auto create_as = CreateAs(double, value2 + value3);

		return create_as;
	}

	void* ExampleAction2(INPUT)
	{
		// if you need nothing to modify,use PassOn(index) to pass on the value
		return PassOn(0);
	}

	void* ExampleAction3(INPUT)
	{
		// if you need to get something from the tokens
		// when reduce, token_iter will be the last recognized token
		// e.g. F -> digit token_set[token_iter] will be 
		// the token info of the number
		auto name = Create(std::string);
		*name = TokenSet[TokenIter].name;
		// equals to
		*name = CurrentToken.name;
		return name;
	}

	void* ExampleAction4(INPUT)
	{
		auto data = CreateFrom(myData.size());
		return data;
	}
};

