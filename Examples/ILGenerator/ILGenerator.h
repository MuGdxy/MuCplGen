#pragma once
#include <MuCplGen/SyntaxDirected.h>
#include <type_traits>
#include "ILSymbolTable.h"
#include "../../MuCplGen/Token.h"

using namespace MuCplGen;
class ILGenerator :public SyntaxDirected<LR1Parser<EasyToken, size_t>>
{
	
public:
	ILGenerator(std::string cfg_path) :SyntaxDirected(cfg_path)
	{
		debug_option = Debug::DebugOption::ConciseInfo;
		Initialize();
	}
	virtual void SetupSemanticActionTable() override
	{
		AddAction(passon_0);
		AddAction(passon_0);
		AddAction(passon_1);
		AddAction(begin_def);
		AddAction(begin_typedef);
		AddAction(end_typedef);
		AddAction(set_typeHead);
		AddAction(set_as_def);
		AddAction(set_def_with_val);
		AddAction(complete_arrayType);
		AddAction(building_array);
		AddAction(begin_array);
		AddAction(set_baseType);
		AddAction(set_customeType);
		AddAction(get_token);

		AddAction(assign);
		AddAction(do_add);
		AddAction(do_sub);
		AddAction(do_mul);
		AddAction(do_div);
		AddAction(do_negate);
		AddAction(create_addr);
		AddAction(complete_lVal);
		AddAction(start_lVal);
		AddAction(filling_addr);
		AddAction(start_Cmp);
		AddAction(create_id_addr);
		AddAction(complete_array);
		AddAction(create_array_dimension);
		AddAction(process_offset);
		AddAction(inc_array_dimension);
	}
	void ShowTables()
	{
		std::cout << "TypeTable:" << std::endl;
		for (auto entry : *Env.type_head->table_ptr)
			std::cout << *entry << std::endl;
		std::cout << "VarTable:" << std::endl;
		for (auto entry : *Env.var_head->table_ptr)
			std::cout << *entry << std::endl;
	}
	void ShowILCode()
	{
		for (size_t i = 0; i < Env.ILCodeStream.size(); ++i)
			std::cout << "[" << i << "\t]" << Env.ILCodeStream[i] << std::endl;
	}
	bool HighlightIfHasError()
	{
		if (error_info_pair.size())
		{
			Debug::Highlight(*input_text, *this->token_set, error_info_pair);
			return true;
		}
		return false;		
	}
private:
	ILEnv Env;
	size_t cur_dim = 0;
	Address* cur_addr = nullptr;

	std::stack<Address*> addr_stack;
	std::stack<size_t> dimension_stack;
	std::vector<std::pair<size_t, std::string>> error_info_pair;
	void PushAndCreateDim()
	{
		dimension_stack.push(cur_dim);
		cur_dim = 0;
	}
	void PopDim()
	{
		cur_dim = dimension_stack.top();
		dimension_stack.pop();
	}
#pragma region _Global
	SemanticAction passon_0 =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		return data[0];
	};

	SemanticAction passon_1 =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		return data[1];
	};

	SemanticAction get_token =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto token = Create<Token*>(&token_set[iter]);
		return token;
	};
#pragma endregion

#pragma region _Definition
	//M0		->	epsilon
	SemanticAction begin_def =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		Env.SetVarTable();
		return nullptr;
	};
	//M1		->	epsilon
	SemanticAction begin_typedef =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		Env.SetTypeTable();
		token_set[iter + 1].color = ConsoleForegroundColor::Cyan;
		return nullptr;
	};

	//TyDef		->	TyHead { M2 Defs }
	SemanticAction end_typedef =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto entry = Get<ILEntry*>(data[0]);
		for (size_t i = iter + 1; i < token_set.size(); ++i)
			if (token_set[i].name == entry->token->name)
			{
				token_set[i].Type("custom_type");
				token_set[i].type = EasyToken::TokenType::custom_type;
				token_set[i].color = ConsoleForegroundColor::Cyan;
			}
		entry->width = Env.offset;
		Env.PopEntry();
		return nullptr;
	};

	//TyHead	->	Record Id	
	SemanticAction set_typeHead =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto entry = Env.CreateILEntry();
		entry->meta_type = Get<Token*>(data[0])->name;
		entry->token = Get<Token*>(data[1]);
		Env.AddSubEntryToCurrent(entry);
		//head of type_table doesn't care about offset 
		entry->offset = 0;
		Env.PushEntry();
		Env.top = entry;
		auto o = Create<ILEntry*>(entry);
		return o;
	};

	//Def		->	Ty Id ;
	SemanticAction set_as_def =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto entry = Get<ILEntry*>(data[0]);
		auto token = Get<Token*>(data[1]);
		entry->token = token;
		Env.AddSubEntryToCurrent(entry);
		return nullptr;
	};

	//Def		->	Ty Id "=" Id

	SemanticAction set_def_with_val =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto entry = Get<ILEntry*>(data[0]);
		auto token = Get<Token*>(data[1]);
		entry->token = token;
		entry->value = Get<Token*>(data[3])->name;
		Env.AddSubEntryToCurrent(entry);
		return nullptr;
	};

	//Ty		->	Ty Arr
	SemanticAction complete_arrayType =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		constexpr size_t Ty = 0;
		constexpr size_t Arr = 1;
		auto entry = Get<ILEntry*>(data[Ty]);
		const auto& array_info = *std::any_cast<std::vector<size_t>>(data[Arr]);
		entry->array_info = array_info;
		size_t amount = 1;
		size_t width = entry->width;
		for (size_t size : array_info)
		{
			entry->width *= size;
			amount *= size;
		}
		Env.PushEntry();
		Env.top = entry;
		for (size_t i = 0; i < amount; ++i)
		{
			auto sub_entry = Env.CreateILEntry();
			sub_entry->meta_type = entry->meta_type;
			sub_entry->width = width;
			Env.AddSubEntryToCurrent(sub_entry);
		}
		Env.PopEntry();
		return data[Ty];
	};

	//Arr		->	Arr Cmp
	SemanticAction building_array =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto array_info = std::any_cast<std::vector<size_t>>(data[0]);
		auto token = Get<Token*>(data[1]);
		array_info->push_back(std::stoi(token->name));
		return data[0];
	};

	//Arr		->	Cmp
	SemanticAction begin_array =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto passon = Create<std::vector<size_t>>();
		auto array_info = std::any_cast<std::vector<size_t>>(passon);
		auto token = Get<Token*>(data[0]);
		array_info->push_back(std::stoi(token->name));
		return passon;
	};
	
	//CTy		->	id
	SemanticAction set_customeType =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		ILEntry* entry = nullptr;
		const auto& token = token_set[iter];
		for (const auto& type : *Env.type_head->table_ptr)
			if (type->token->name == token.name)
				entry = Env.CreateVarFromCType(type);

		return Create<ILEntry*>(entry);
	};

	//BTy		->	bTy
	SemanticAction set_baseType =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto entry = Env.CreateILEntry();
		const auto& token = token_set[iter];
		entry->meta_type = token.name;
		if (token.name == "int")
			entry->width = ILType::Width::INT;
		else if (token.name == "float")
			entry->width = ILType::Width::FLOAT;
		else if (token.name == "char")
			entry->width = ILType::Width::CHAR;
		return Create<ILEntry*>(entry);
	};
#pragma endregion

#pragma region _Assignment:
	//Asgn	->	LVal "=" RVal ";"
	SemanticAction assign =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data);
		if (index != -1) return data[index];
		Env.ILCodeStream.push_back(
			{ 
				Get<Address*>(data[0]),
				"",
				Get<Address*>(data[2]),
				nullptr 
			}
		);
		return nullptr;
	};

	//RVal	->	RVal "+" Tpl;
	SemanticAction do_add =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data);
		if (index != -1) return data[index];
		auto rval = Get<Address*>(data[0]);
		auto tpl = Get<Address*>(data[2]);
		auto new_addr = Env.CreateAddress();
		Env.ILCodeStream.push_back({ new_addr,"ADD",rval,tpl });
		return Create<Address*>(new_addr);
	};
		//RVal	->	RVal "-" Tpl;
	SemanticAction do_sub =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data);
		if (index != -1) return data[index];
		auto rval = Get<Address*>(data[0]);
		auto tpl = Get<Address*>(data[2]);
		auto new_addr = Env.CreateAddress();
		Env.ILCodeStream.push_back({ new_addr,"SUB",rval,tpl });
		return Create<Address*>(new_addr);
	};

	//Tpl		->	Tpl "*" Fn
	SemanticAction do_mul =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data);
		if (index != -1) return data[index];
		auto tpl = Get<Address*>(data[0]);
		auto fn = Get<Address*>(data[2]);
		auto new_addr = Env.CreateAddress();
		Env.ILCodeStream.push_back({ new_addr,"MUL",tpl,fn });
		return Create<Address*>(new_addr);
	};

	//Tpl		->	Tpl "/" Fn
	SemanticAction do_div =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data);
		if (index != -1) return data[index];
		auto tpl = Get<Address*>(data[0]);
		auto fn = Get<Address*>(data[2]);
		auto new_addr = Env.CreateAddress();
		Env.ILCodeStream.push_back({ new_addr,"DIV",tpl,fn });
		return Create<Address*>(new_addr);
	};

	//Fn		->	"-" Val
	SemanticAction do_negate =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data);
		if (index != -1) return data[index];
		auto addr = Get<Address*>(data[1]);
		auto new_addr = Env.CreateAddress();
		Env.ILCodeStream.push_back({ new_addr,"-", addr, nullptr });
		return Create<Address*>(new_addr);
	};

	//Val		->	ImVal
	SemanticAction create_addr =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data);
		if (index != -1) return data[index];
		auto token = Get<Token*>(data[0]);
		auto addr = Env.CreateAddress();
		addr->token = token;
		return Create<Address*>(addr);
	};
	

	//LVal		->	M0 LValCmp
	SemanticAction complete_lVal =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data); 
		if (index != -1) return data[index];
		Env.PopEntry();
		return data[1];
	};

	//M0		->	epsilon
	SemanticAction start_lVal =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data); 
		if(index != -1) return data[index];
		Env.PushEntry();
		Env.top = Env.var_head;
		return nullptr;
	};
	
	//LValCmp	->	LValCmp "." LValMem
	SemanticAction filling_addr =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data); 
		if (index != -1) return data[index];
		auto LValCmp =Get<Address*>(data[0]);
		auto LValMem = Get<Address*>(data[2]);
		LValCmp->chain.push_back(LValMem->chain[0]);
		if (LValMem->array_index.size())
			LValCmp->array_index.push_back(LValMem->array_index[0]);
		Env.top = LValMem->chain[0];
		return data[0];
	};
	//LValCmp	->	LValMem
	SemanticAction start_Cmp =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data);
		if (index != -1) return data[index];
		auto addr = Get<Address*>(data[0]);
		Env.top = addr->chain[addr->chain.size() - 1];
		return Create<Address*>(addr);
	};


	//LValMem	->	MemId
	SemanticAction create_id_addr =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data);
		if (index != -1) return data[index];
		auto token = Get<Token*>(data[0]);
		if (Env.top->table_ptr != nullptr)
		{
			for (auto entry : *Env.top->table_ptr)
				if (token->name == entry->token->name)
				{
					auto addr = Env.CreateAddress();
					addr->chain.push_back(entry);
					return Create<Address*>(addr);
				}
		}
		std::stringstream ss;
		if (Env.top->token)
			ss << "Error:\"" << Env.top->token->name << "\"has no such member \"" << token->name << "\"";
		else
			ss << "Error: undefined \"" << token->name << "\"";
		error_info_pair.push_back({ iter, ss.str() });
		return ParserError(ParserErrorCode::SemanticError);
	};
	//ArrVar	->	ArrId Arr
	SemanticAction complete_array =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data);
		if (index != -1) return data[index];
		auto ArrId = Get<Address*>(data[0]);
		auto Arr = Get<Address*>(data[1]);
		ArrId->array_index.push_back(Arr);
		cur_addr = addr_stack.top();
		addr_stack.pop();
		return data[0];
	};

	//ArrId  -> Id
	SemanticAction create_array_dimension =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data);
		if (index != -1) return data[index];
		auto token = Get<Token*>(data[0]);
		PushAndCreateDim();
		if (Env.top->table_ptr != nullptr)
		{
			for (auto entry : *Env.top->table_ptr)
				if (token->name == entry->token->name)
				{
					auto addr = Env.CreateAddress();
					addr->chain.push_back(entry);
					addr_stack.push(cur_addr);
					cur_addr = addr;
					return Create<Address*>(addr);
				}
		}
		error_info_pair.push_back({ iter,"ERROR: undefined " + token->name });
		return ParserError(ParserErrorCode::SemanticError);
	};
	//Arr		->  Arr Cmp
	SemanticAction process_offset =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto index = NextSemanticError(data);
		if (index != -1) return data[index];
		auto addr1 = Get<Address*>(data[0]);
		auto cur = cur_addr->chain[cur_addr->chain.size() - 1];
		if (cur_dim - 1 >= cur->array_info.size())
		{
			std::stringstream ss;
			ss << "Error: array \"" << cur->token->name << "\"'s dimension is "
				<< cur->array_info.size() << ", but yours " << cur_dim << ".";
			error_info_pair.push_back({ iter,ss.str() });
			return ParserError(ParserErrorCode::SemanticError);
		}
		auto cur_size = cur->array_info[cur_dim - 1];
		auto addr2 = Get<Address*>(data[1]);
		auto temp_mul = Env.CreateAddress();
		auto temp_offset = Env.CreateAddress();
		auto temp_size = Env.CreateAddress();
		temp_size->code = std::to_string(cur_size);
		Env.ILCodeStream.push_back({ temp_mul, "MUL", temp_size, addr1 });
		Env.ILCodeStream.push_back({ temp_offset, "ADD", temp_mul, addr2 });
		return Create<Address*>(temp_offset);
	};
	//Cmp	->	"[" RVal "]"
	SemanticAction inc_array_dimension =
		[this](std::vector<std::any*> data, size_t iter, TokenSet& token_set)->std::any*
	{
		auto addr = Get<Address*>(data[1]);
		++cur_dim;
		return data[1];
	};
#pragma endregion
};

