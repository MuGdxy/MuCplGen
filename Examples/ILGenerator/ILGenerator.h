#pragma once
#include <type_traits>
#include "MuCplGen/SyntaxDirected.h"
#include "ILSymbolTable.h"
#include <MuCplGen/Token.h>

using namespace MuCplGen;
class ILGenerator :public SyntaxDirected<SLRParser<EasyToken>>
{
public:
	ILGenerator(std::ostream& log = std::cout) :SyntaxDirected(log)
	{
		debug_option = Debug::DebugOption::ConciseInfo;
		generation_option = BuildOption::LoadAndSave;
		SetStorage("./storage/ILGen.bin");
		//bTy->keyword && "float";
		//bTy->keyword && "char";
		//bTy->keyword && "bool";
		//bTy->keyword && "int";
		{
			auto& t = CreateTerminator("bTy");
			t.translation = [this](const Token& token)
			{
				if (token.type == Token::TokenType::keyword)
				{
					if (token.name == "float"
						|| token.name == "int"
						|| token.name == "char"
						|| token.name == "bool") return true;
				}
				return false;
			};
		}

		//record->keyword && "struct";
		//record->keyword && "class";
		{
			auto& t = CreateTerminator("record");
			t.translation = [this](const Token& token)
			{
				if (token.type == Token::TokenType::keyword)
				{
					if (token.name == "struct"
						|| token.name == "class") return true;
				}
				return false;
			};
		}
		//str->raw_string;
		{
			auto& t = CreateTerminator("str");
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::raw_string;
			};
		}

		//num->number;
		{
			auto& t = CreateTerminator("num");
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::number;
			};
		}

		//id->identifier;
		{
			auto& t = CreateTerminator("id");
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::identifier;
			};
		}
		//cTy->custom_type;
		{
			auto& t = CreateTerminator("cTy");
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::custom_type;
			};
		}
		//rel->rel_op;
		{
			auto& t = CreateTerminator("rel");
			t.translation = [this](const Token& token)
			{
				return token.type == Token::TokenType::rel_op;
			};
		}
		//bl->keyword && "true";
		//bl->keyword && "false";
		{
			auto& t = CreateTerminator("bl");
			t.translation = [this](const Token& token)
			{
				if (token.type == Token::TokenType::keyword)
				{
					if (token.name == "false" || token.name == "true") return true;
				}
				return false;
			};
		}

		//_Global:
		Scope("_Global");

		//	Prgm_ -> Prgm
		{
			auto& p = CreateParseEntrance();
			p.expression = "Prgm_ -> Prgm";
		}

		//	Prgm -> Stmts
		{
			auto& p = CreateParseRule();
			p.expression = "Prgm -> Stmts";
		}
		//	Prgm -> epsilon
		{
			auto& p = CreateParseRule();
			p.expression = "Prgm -> epsilon";
		}

		//	Stmts -> Stmts Stmt
		{
			auto& p = CreateParseRule();
			p.expression = "Stmts -> Stmts Stmt";
		}

		//	Stmts -> Stmt
		{
			auto& p = CreateParseRule();
			p.expression = "Stmts -> Stmt";
		}

		//	Blck -> { Stmts }
		{
			auto& p = CreateParseRule();
			p.expression = "Blck -> { Stmts }";
		}

		//	Stmt -> Open
		{
			auto& p = CreateParseRule();
			p.expression = "Stmt -> Open";
		}

		//	Stmt -> Cls
		{
			auto& p = CreateParseRule();
			p.expression = "Stmt -> Cls";
		}

		//	Open -> IfHead Stmt
		{
			auto& p = CreateParseRule();
			p.expression = "Open -> IfHead Stmt";
		}

		//	Open -> IfHead Cls else Open
		{
			auto& p = CreateParseRule();
			p.expression = "Open -> IfHead Cls else Open";
		}

		//	Cls -> Stc
		{
			auto& p = CreateParseRule();
			p.expression = "Cls -> Stc";
		}

		//	Cls -> Blck
		{
			auto& p = CreateParseRule();
			p.expression = "Cls -> Blck";
		}

		//	Cls -> IfHead Cls else Cls
		{
			auto& p = CreateParseRule();
			p.expression = "Cls -> IfHead Cls else Cls";
		}

		// IfHead -> if ( BExpr )
		{
			auto& p = CreateParseRule();
			p.expression = "IfHead -> if ( BExpr )";
		}


		//	BExpr -> _Assignment.Expr
		{
			auto& p = CreateParseRule();
			p.expression = "BExpr -> _Assignment.Expr";
		}

		//	Stc -> _Definition.Stc
		{
			auto& p = CreateParseRule();
			p.expression = "Stc -> _Definition.Stc";
		}

		//	Stc -> _Assignment.Asgn
		{
			auto& p = CreateParseRule();
			p.expression = "Stc -> _Assignment.Asgn";
		}

		//_Definition
		Scope("_Definition");
		//	Stc -> M0 VarDef
		{
			auto& p = CreateParseRule();
			p.expression = "Stc -> M0 VarDef";
		}


		//	M0->epsilon; {begin_def};
		{
			auto& p = CreateParseRule();
			p.action_name = "begin_def";
			p.expression = "M0 -> epsilon";
			p.SetAction(
				[this](Empty)->Empty
				{
					Env.SetVarTable();
					return Empty{};
				});
		}
		//	VarDef -> Def
		{
			auto& p = CreateParseRule();
			p.expression = "VarDef -> Def";
		}

		//Stc -> M1 TyDef
		{
			auto& p = CreateParseRule();
			p.expression = "Stc -> M1 TyDef";
		}

		//M1 -> epsilon {begin_typedef}// top = type_head
		{
			auto& p = CreateParseRule();
			p.action_name = "begin_typedef";
			p.expression = "M1 -> epsilon";
			p.SetAction(
				[this](Empty)->Empty
				{
					Env.SetTypeTable();
					auto tokens = GetTokenSet();
					tokens[TokenIter() + 1].color = ConsoleForegroundColor::Cyan;
					return Empty{};
				});
		}

		//TyDef -> TyHead { Defs } ; {end_typedef} // pop table;get ILEntry from TyHead;TyHead.width = offset;
		{
			auto& p = CreateParseRule();
			p.action_name = "end_typedef";
			p.expression = "TyDef -> TyHead { Defs } ;";
			p.SetAction(
				[this](ILEntry* entry, Empty, Empty, Empty, Empty)->Empty
				{
					auto& token_set = GetTokenSet();
					auto iter = TokenIter();
					for (size_t i = iter; i < token_set.size(); ++i)
						if (token_set[i].name == entry->token->name)
						{
							token_set[i].Type("custom_type");
							token_set[i].type = EasyToken::TokenType::custom_type;
							token_set[i].color = ConsoleForegroundColor::Cyan;
						}
					entry->width = Env.offset;
					Env.PopEntry();
					return Empty{};
				});
		}

		//TyHead -> Record Id {set_typeHead}// new ILEntry;set type(struct/class/...);set Token(cTyname); AddEntry;
		{
			auto& p = CreateParseRule();
			p.action_name = "set_typeHead";
			p.expression = "TyHead -> Record Id";
			p.SetAction(
				[this](Token* type_token, Token* token)->ILEntry*
				{
					auto entry = Env.CreateILEntry();
					entry->meta_type = type_token->name;
					entry->token = token;
					token->type = EasyToken::TokenType::custom_type;
					token->color = ConsoleForegroundColor::Cyan;
					Env.AddSubEntryToCurrent(entry);
					//head of type_table doesn't care about offset 
					entry->offset = 0;
					Env.PushEntry();
					Env.top = entry;
					return entry;
				});
		}

		//Defs -> Defs Def {building_defs} 
		{
			auto& p = CreateParseRule();
			p.action_name = "building_defs";
			p.expression = "Defs -> Defs Def";
			p.SetAction(nullptr);
		}

		//Defs -> Def {begin_defs}
		{
			auto& p = CreateParseRule();
			p.action_name = "begin_defs";
			p.expression = "Defs -> Def";
			p.SetAction(nullptr);
		}

		//Def -> Ty Id ; {set_as_def}// get ILEntry from Ty;Get Token from Id; Ty.token = Id.token; AddEntry;
		{
			auto& p = CreateParseRule();
			p.action_name = "set_as_def";
			p.expression = "Def -> Ty Id ;";
			p.SetAction(
				[this](ILEntry* entry, Token* token, Empty)->Empty
				{
					entry->token = token;
					Env.AddSubEntryToCurrent(entry);
					return Empty{};
				});
		}

		//Def -> Ty Id = ImVal ; {set_def_with_val};// get ILEntry from Ty;Get Token from Id(1); Ty.token = Id(1).token; Ty.val = Id(2).token.name; AddEntry;
		{
			auto& p = CreateParseRule();
			p.action_name = "set_def_with_val";
			p.expression = "Def -> Ty Id = ImVal ;";
			p.SetAction(
				[this](ILEntry* entry, Token* token, Empty, Token* value_token, Empty)->Empty
				{
					entry->token = token;
					entry->value = value_token->name;
					Env.AddSubEntryToCurrent(entry);
					return Empty{};
				});
		}

		//Ty -> CTy {passon_0};
		{
			auto& p = CreateParseRule();
			p.expression = "Ty -> CTy";
		}

		auto complete_arrayType = [this](ILEntry* entry, std::vector<size_t>& array_info)->PassOn
		{
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
			return PassOn(0);
		};

		//Ty -> BTy Arr {complete_arrayType};// get ILEntry from Ty;set array_info;set entry.width = width * array_info;
		{
			auto& p = CreateParseRule();
			p.action_name = "complete_arrayType";
			p.expression = "Ty -> BTy Arr";
			p.SetAction(complete_arrayType);
		}

		//Ty -> CTy Arr {complete_arrayType}
		{
			auto& p = CreateParseRule();
			p.action_name = "complete_arrayType";
			p.expression = "Ty -> CTy Arr";
			p.SetAction(complete_arrayType);
		}

		//Ty -> BTy {passon_0}
		{
			auto& p = CreateParseRule();
			p.expression = "Ty -> BTy";
		}
		//Arr -> Arr Cmp {building_array};// array_info.push_back(stoi(Cmp.Token.name));
		{
			auto& p = CreateParseRule();
			p.action_name = "building_array";
			p.expression = "Arr -> Arr Cmp";
			p.SetAction(
				[this](std::vector<size_t>& array_info, Token* token)->PassOn
				{
					array_info.push_back(std::stoi(token->name));
					return PassOn(0);
				});
		}

		//Arr -> Cmp {begin_array}// pass array_info.push_back(stoi(Cmp.Token.name));
		{
			auto& p = CreateParseRule();
			p.action_name = "begin_array";
			p.expression = "Arr -> Cmp";
			p.SetAction(
				[this](Token* token)->std::vector<size_t>
				{
					std::vector<size_t> array_info;
					array_info.push_back(std::stoi(token->name));
					return array_info;
				});
		}

		//Cmp -> [ Num ] {passon_1}// PassOn Token;
		{
			auto& p = CreateParseRule();
			p.expression = "Cmp -> [ Num ]";
			p.SetAction(PassOn(1));
		}

		//CTy -> cTy {set_customeType}// search in type_head;copy;new ILEntry;set type;set width;
		{
			auto& p = CreateParseRule();
			p.action_name = "set_customeType";
			p.expression = "CTy -> cTy";
			p.SetAction(
				[this](Empty)
				{
					ILEntry* entry = nullptr;
					auto& token = CurrentToken();
					for (const auto& type : *Env.type_head->table_ptr)
						if (type->token->name == token.name)
							entry = Env.CreateVarFromCType(type);
					return entry;
				});
		}

		//BTy -> bTy {set_baseType}// new ILEntry;set type;set width;
		{
			auto& p = CreateParseRule();
			p.action_name = "set_baseType";
			p.expression = "BTy -> bTy";
			p.SetAction(
				[this](Empty)
				{
					auto entry = Env.CreateILEntry();
					auto& token = CurrentToken();
					entry->meta_type = token.name;
					if (token.name == "int") entry->width = ILType::Width::INT;
					else if (token.name == "float") entry->width = ILType::Width::FLOAT;
					else if (token.name == "char") entry->width = ILType::Width::CHAR;
					return entry;
				});
		}


		auto get_token = [this](Empty)->Token*
		{
			auto token = &CurrentToken();
			return token;
		};

		//Record -> record {get_token}// Get Token;return Token;
		{
			auto& p = CreateParseRule();
			p.expression = "Record -> record";
			p.SetAction(get_token);
		}

		//Id -> id {get_token};
		{
			auto& p = CreateParseRule();
			p.expression = "Id -> id";
			p.SetAction(get_token);
		}

		//ImVal -> Str {passon_0}
		{
			auto& p = CreateParseRule();
			p.expression = "ImVal -> Str";
		}

		//ImVal -> Num {passon_0}
		{
			auto& p = CreateParseRule();
			p.expression = "ImVal -> Num";
		}

		//Str -> str {get_token}
		{
			auto& p = CreateParseRule();
			p.expression = "Str -> str";
			p.SetAction(get_token);
		}

		//Num -> num {get_token}
		{
			auto& p = CreateParseRule();
			p.expression = "Num -> num";
			p.SetAction(get_token);
		}

		auto error_action = [this](const std::vector<std::any*>& data)
		{
			int next = -1;
			NextSemanticError(data, next);
			return PassOn(next);
		};

		//_Assignment:
		Scope("_Assignment");
		//	Asgn -> LVal = Expr ; {assign};
		{
			auto& p = CreateParseRule();
			p.action_name = "assign";
			p.expression = "Asgn -> LVal = Expr ;";
			p.SetAction(
				[this](Address* laddr, Empty, Address* raddr, Empty)->Empty
				{
					Env.ILCodeStream.push_back({ laddr, "", raddr, nullptr });
					return Empty{};
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}
		//	Expr -> BTpl || BTpl
		{
			auto& p = CreateParseRule();
			p.expression = "Expr -> BTpl || BTpl";
			p.SetAction(nullptr);
		}
		//	Expr -> BTpl {passon_0};
		{
			auto& p = CreateParseRule();
			p.expression = "Expr -> BTpl";
		}
		//	BTpl->BFn && BFn
		{
			auto& p = CreateParseRule();
			p.expression = "BTpl -> BFn && BFn";
			p.SetAction(nullptr);
		}

		//	BTpl -> BFn {passon_0}
		{
			auto& p = CreateParseRule();
			p.expression = "BTpl -> BFn";
		}
		//	BFn	-> ! BFn
		{
			auto& p = CreateParseRule();
			p.expression = "BFn -> ! BFn";
			p.SetAction(nullptr);
		}
		//	BFn	-> ( _Global.BExpr ) {passon_1}
		//{
		//	auto& p = CreateParseRule();
		//	p.expression = "BFn -> ( _Global.BExpr )";
		//	p.SetAction(PassOn(1));
		//	
		//}
		//	BFn -> RVal; {passon_0};
		{
			auto& p = CreateParseRule();
			p.expression = "BFn -> RVal";
		}

		//	RVal -> RVal + Tpl {do_add};
		{
			auto& p = CreateParseRule();
			p.action_name = "do_add";
			p.expression = "RVal -> RVal + Tpl";
			p.SetAction(
				[this](Address* rval, Empty, Address* tpl)->Address*
				{
					auto new_addr = Env.CreateAddress();
					Env.ILCodeStream.push_back({ new_addr,"ADD",rval,tpl });
					return new_addr;
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}
		//	RVal -> RVal - Tpl {do_sub};
		{
			auto& p = CreateParseRule();
			p.action_name = "do_sub";
			p.expression = "RVal -> RVal - Tpl";
			p.SetAction(
				[this](Address* rval, Empty, Address* tpl)->Address*
				{
					auto new_addr = Env.CreateAddress();
					Env.ILCodeStream.push_back({ new_addr,"SUB",rval,tpl });
					return new_addr;
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}
		//	RVal -> Tpl {passon_0};
		{
			auto& p = CreateParseRule();
			p.expression = "RVal -> Tpl";
		}
		//	Tpl -> Tpl * Fn {do_mul}
		{
			auto& p = CreateParseRule();
			p.action_name = "do_mul";
			p.expression = "Tpl -> Tpl * Fn";
			p.SetAction(
				[this](Address* rval, Empty, Address* tpl)->Address*
				{
					auto new_addr = Env.CreateAddress();
					Env.ILCodeStream.push_back({ new_addr,"MUL",rval,tpl });
					return new_addr;
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}
		//	Tpl -> Tpl / Fn; {do_div};
		{
			auto& p = CreateParseRule();
			p.action_name = "do_div";
			p.expression = "Tpl -> Tpl / Fn";
			p.SetAction(
				[this](Address* rval, Empty, Address* tpl)->Address*
				{
					auto new_addr = Env.CreateAddress();
					Env.ILCodeStream.push_back({ new_addr,"DIV",rval,tpl });
					return new_addr;
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
			
		}
		//	Tpl -> Fn {passon_0}
		{
			auto& p = CreateParseRule();
			p.expression = "Tpl -> Fn";
		}
		//	Fn -> ( RVal ) {passon_1};
		{
			auto& p = CreateParseRule();
			p.expression = "Fn	->	( RVal ) ";
			p.SetAction(PassOn(1));
		}
		//	Fn -> - Val {do_negate};// new Temp Entry
		{
			auto& p = CreateParseRule();
			p.action_name = "do_negate";
			p.expression = "Fn -> - Val";
			p.SetAction(
				[this](Empty, Address* addr)->Address*
				{
					auto new_addr = Env.CreateAddress();
					Env.ILCodeStream.push_back({ new_addr,"-", addr, nullptr });
					return new_addr;
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}
		//	Fn -> Val {passon_0}
		{
			auto& p = CreateParseRule();
			p.expression = "Fn -> Val";
		}

		//	Val -> LVal  {passon_0}
		{
			auto& p = CreateParseRule();
			p.expression = "Val -> LVal";
		}

		//	Val -> ImVal; {create_addr};
		{
			auto& p = CreateParseRule();
			p.action_name = "create_addr";
			p.expression = "Val -> ImVal";
			p.SetAction(
				[this](Token* token)->Address*
				{
					auto addr = Env.CreateAddress();
					addr->token = token;
					return addr;
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}

		// struct
		//	LVal -> M0 LValCmp {complete_lVal};//pop the Env.top;
		{
			auto& p = CreateParseRule();
			p.action_name = "complete_lVal";
			p.expression = "LVal -> M0 LValCmp";
			p.SetAction(
				[this](Empty, Address* addr)->PassOn
				{
					Env.PopEntry();
					return PassOn(1);
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}
		//	M0 -> epsilon; {start_lVal};//push Env.top;set Env.top as var_head;
		{
			auto& p = CreateParseRule();
			p.action_name = "start_lVal";
			p.expression = "M0 -> epsilon";
			p.SetAction(
				[this](Empty)->Empty
				{
					Env.PushEntry();
					Env.top = Env.var_head;
					return Empty{};
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}

		//	LValCmp -> LValCmp . LValMem {filling_addr};//connect LValMem<Address>.chain[0] to LValCmp<Address>.chain;
		{
			auto& p = CreateParseRule();
			p.action_name = "filling_addr";
			p.expression = "LValCmp -> LValCmp . LValMem";
			p.SetAction(
				[this](Address* LValCmp, Empty, Address* LValMem)
				{
					LValCmp->chain.push_back(LValMem->chain[0]);
					if (LValMem->array_index.size())
						LValCmp->array_index.push_back(LValMem->array_index[0]);
					Env.top = LValMem->chain[0];
					return PassOn(0);
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}
		//	LValCmp -> LValMem {start_Cmp};// set LValMem<Address>.lastEntry.tableptr to Env.top; return LValMem<Address>;
		{
			auto& p = CreateParseRule();
			p.action_name = "start_Cmp";
			p.expression = "LValCmp -> LValMem";
			p.SetAction(
				[this](Address* addr)
				{
					Env.top = addr->chain[addr->chain.size() - 1];
					return addr;
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}
		//	// baseType
		//	LValMem -> MemId {create_id_addr};//check MemId<Token>.name in table;create addr;return addr
		{
			auto& p = CreateParseRule();
			p.action_name = "create_id_addr";
			p.expression = "LValMem -> MemId";
			p.SetAction(
				[this](Token* token)
				{
					if (Env.top->table_ptr != nullptr)
					{
						for (auto entry : *Env.top->table_ptr)
							if (token->name == entry->token->name)
							{
								auto addr = Env.CreateAddress();
								addr->chain.push_back(entry);
								return addr;
							}
					}
					std::stringstream ss;
					if (Env.top->token)
						ss << "Error:\"" << Env.top->token->name << "\"has no such member \"" << token->name << "\"";
					else
						ss << "Error: undefined \"" << token->name << "\"";
					error_info_pair.push_back({ TokenIter(), ss.str() });
					throw(SemanticError());
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}
		//	MemId -> Id {passon_0};//passon_0
		{
			auto& p = CreateParseRule();
			p.expression = "MemId -> Id";
		}
		//	// arrayType
		//	LValMem -> ArrVar {passon_0};
		{
			auto& p = CreateParseRule();
			p.expression = "LValMem -> ArrVar";
		}
		//	ArrVar -> ArrId Arr {complete_array};//ArrId<Address>.array_index.push_back(Arr<Address>);pop Env.top; return AddrId<Address>;
		{
			auto& p = CreateParseRule();
			p.action_name = "complete_array";
			p.expression = "ArrVar -> ArrId Arr";
			p.SetAction(
				[this](Address* ArrId, Address* Arr) -> PassOn
				{
					ArrId->array_index.push_back(Arr);
					cur_addr = addr_stack.top();
					addr_stack.pop();
					return PassOn(1);
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}
		//	ArrId -> Id {create_array_dimension};//check Id<Token>.name in Env.top;create addr;push Env.top;set entry.tableptr to Env.top; return addr;
		{
			auto& p = CreateParseRule();
			p.action_name = "create_array_dimension";
			p.expression = "ArrId -> Id";
			p.SetAction(
				[this](Token* token) -> Address*
				{
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
								return addr;
							}
					}
					error_info_pair.push_back({ TokenIter(),"ERROR: undefined " + token->name });
					throw(SemanticError());
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}
		//	Arr -> Arr Cmp {process_offset};//create addr for [t_mul,t_offset], Generator Code: "t_mul = Addr1 * (ArrayInfo[dimension-1])","t_offset = t_mul + Addr2"; return t_offset;
		{
			auto& p = CreateParseRule();
			p.action_name = "process_offset";
			p.expression = "Arr -> Arr Cmp";
			p.SetAction(
				[this](Address* addr1, Address* addr2) -> Address*
				{
					auto cur = cur_addr->chain[cur_addr->chain.size() - 1];
					if (cur_dim - 1 >= cur->array_info.size())
					{
						std::stringstream ss;
						ss << "Error: array \"" << cur->token->name << "\"'s dimension is "
							<< cur->array_info.size() << ", but yours " << cur_dim << ".";
						error_info_pair.push_back({ TokenIter(),ss.str() });
						throw(SemanticError());
					}
					auto cur_size = cur->array_info[cur_dim - 1];
					auto temp_mul = Env.CreateAddress();
					auto temp_offset = Env.CreateAddress();
					auto temp_size = Env.CreateAddress();
					temp_size->code = std::to_string(cur_size);
					Env.ILCodeStream.push_back({ temp_mul, "MUL", temp_size, addr1 });
					Env.ILCodeStream.push_back({ temp_offset, "ADD", temp_mul, addr2 });
					return temp_offset;
				});
			p.SetSemanticErrorAction<PassOn>(error_action);
		}
		//	Arr -> Cmp {passon_0};
		{
			auto& p = CreateParseRule();
			p.expression = "Arr -> Cmp";
		}
		//	Cmp -> [ RVal ] {inc_array_dimension};//return RVal<Address>;
		{
			auto& p = CreateParseRule();
			p.action_name = "inc_array_dimension";
			p.expression = "Cmp -> [ RVal ]";
			p.SetAction(
				[this](Empty, Address* addr, Empty) -> PassOn
				{
					++cur_dim;
					return PassOn(1);
				});
		}
		//	ImVal -> Str {passon_0};
		{
			auto& p = CreateParseRule();
			p.expression = "ImVal -> Str";
		}
		//	ImVal -> Num {passon_0};
		{
			auto& p = CreateParseRule();
			p.expression = "ImVal -> Num";
		}

		//	Str -> str {get_token};
		{
			auto& p = CreateParseRule();
			p.expression = "Str -> str";
			p.SetAction(get_token);
		}
		//	Id -> id; {get_token};
		{
			auto& p = CreateParseRule();
			p.expression = "Id -> id";
			p.SetAction(get_token);
		}
		//	Num->num; {get_token};
		{
			auto& p = CreateParseRule();
			p.expression = "Num -> num";
			p.SetAction(get_token);
		}
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
			Debug::Highlight(GetInputText(), GetTokenSet(), error_info_pair);
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
};

