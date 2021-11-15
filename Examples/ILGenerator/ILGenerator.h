#pragma once
#include <MuCplGen/SyntaxDirected.h>
#include <type_traits>
#include "ILSymbolTable.h"
#include "../../MuCplGen/Token.h"

using namespace MuCplGen;
class ILGenerator :public SyntaxDirected<SLRParser<EasyToken, size_t>>
{
public:
	ILGenerator(std::ostream& log = std::cout) :SyntaxDirected(log)
	{
		debug_option = Debug::DebugOption::AllDebugInfo;

		//bTy->keyword && "float";
		//bTy->keyword && "char";
		//bTy->keyword && "bool";
		//bTy->keyword && "int";
		{
			auto t = new Term;
			t->name = "bTy";
			t->translation = [this](const Token& token)
			{
				if (token.type == Token::TokenType::keyword)
				{
					if (token.name == "float"
						|| token.name == "int"
						|| token.name == "char"
						|| token.name == "bool") return true;
				}
				else return false;
			};
			AddTerminator(t);
		}

		//record->keyword && "struct";
		//record->keyword && "class";
		{
			auto t = new Term;
			t->name = "record";
			t->translation = [this](const Token& token)
			{
				if (token.type == Token::TokenType::keyword)
				{
					if (token.name == "struct"
						|| token.name == "class") return true;
				}
				else return false;
			};
			AddTerminator(t);
		}
		//str->raw_string;
		{
			auto t = new Term;
			t->name = "str";
			t->translation = [this](const Token& token)
			{
				if (token.type == Token::TokenType::raw_string) return true;
				else return false;
			};
			AddTerminator(t);
		}

		//num->number;
		{
			auto t = new Term;
			t->name = "num";
			t->translation = [this](const Token& token)
			{
				if (token.type == Token::TokenType::number) return true;
				else return false;
			};
			AddTerminator(t);
		}

		//id->identifier;
		{
			auto t = new Term;
			t->name = "id";
			t->translation = [this](const Token& token)
			{
				if (token.type == Token::TokenType::identifier) return true;
				else return false;
			};
			AddTerminator(t);
		}
		//cTy->custom_type;
		{
			auto t = new Term;
			t->name = "cTy";
			t->translation = [this](const Token& token)
			{
				if (token.type == Token::TokenType::custom_type) return true;
				else return false;
			};
			AddTerminator(t);
		}
		//rel->rel_op;
		{
			auto t = new Term;
			t->name = "rel";
			t->translation = [this](const Token& token)
			{
				if (token.type == Token::TokenType::rel_op) return true;
				else return false;
			};
			AddTerminator(t);
		}
		//bl->keyword && "true";
		//bl->keyword && "false";
		{
			auto t = new Term;
			t->name = "bl";
			t->translation = [this](const Token& token)
			{
				if (token.type == Token::TokenType::keyword)
				{
					if (token.name == "false" || token.name == "true") return true;
				}
				else return false;
			};
			AddTerminator(t);
		}

		//_Global:
		ParseRule::SetCurrentScope("_Global");

		//	Prgm_ -> Prgm
		{
			auto p = new ParseRule;
			p->expression = "Prgm_ -> Prgm";
			AddParseRule(p);
		}

		//	Prgm -> Stmts
		{
			auto p = new ParseRule;
			p->expression = "Prgm -> Stmts";
			AddParseRule(p);
		}
		//	Prgm -> epsilon
		{
			auto p = new ParseRule;
			p->expression = "Prgm -> epsilon";
			AddParseRule(p);
		}

		//	Stmts -> Stmts Stmt
		{
			auto p = new ParseRule;
			p->expression = "Stmts -> Stmts Stmt";
			AddParseRule(p);
		}

		//	Stmts -> Stmt
		{
			auto p = new ParseRule;
			p->expression = "Stmts -> Stmt";
			AddParseRule(p);
		}

		//	Blck -> { Stmts }
		{
			auto p = new ParseRule;
			p->expression = "Blck -> { Stmts }";
			AddParseRule(p);
		}

		//	Stmt -> Open
		{
			auto p = new ParseRule;
			p->expression = "Stmt -> Open";
			AddParseRule(p);
		}

		//	Stmt -> Cls
		{
			auto p = new ParseRule;
			p->expression = "Stmt -> Cls";
			AddParseRule(p);
		}

		//	Open -> IfHead Stmt
		{
			auto p = new ParseRule;
			p->expression = "Open -> IfHead Stmt";
			AddParseRule(p);
		}

		//	Open -> IfHead Cls else Open
		{
			auto p = new ParseRule;
			p->expression = "Open -> IfHead Cls else Open";
			AddParseRule(p);
		}

		//	Cls -> Stc
		{
			auto p = new ParseRule;
			p->expression = "Cls -> Stc";
			AddParseRule(p);
		}

		//	Cls -> Blck
		{
			auto p = new ParseRule;
			p->expression = "Cls -> Blck";
			AddParseRule(p);
		}

		//	Cls -> IfHead Cls else Cls
		{
			auto p = new ParseRule;
			p->expression = "Cls -> IfHead Cls else Cls";
			AddParseRule(p);
		}

		// IfHead -> if ( BExpr )
		{
			auto p = new ParseRule;
			p->expression = "IfHead -> if ( BExpr )";
			AddParseRule(p);
		}


		//	BExpr -> _Assignment.Expr
		{
			auto p = new ParseRule;
			p->expression = "BExpr -> _Assignment.Expr";
			AddParseRule(p);
		}

		//	Stc -> _Definition.Stc
		{
			auto p = new ParseRule;
			p->expression = "Stc -> _Definition.Stc";
			AddParseRule(p);
		}

		//	Stc -> _Assignment.Asgn
		{
			auto p = new ParseRule;
			p->expression = "Stc -> _Assignment.Asgn";
			AddParseRule(p);
		}

		//_Definition
		ParseRule::SetCurrentScope("_Definition");
		//	Stc -> M0 VarDef
		{
			auto p = new ParseRule;
			p->expression = "Stc -> M0 VarDef";
			AddParseRule(p);
		}


		//	M0->epsilon; {begin_def};
		{
			auto p = new ParseRule;
			p->action_name = "begin_def";
			p->expression = "M0 -> epsilon";
			p->SetAction<Empty, Empty>(
				[this](Empty)->Empty
				{
					Env.SetVarTable();
					return Empty{};
				});
			AddParseRule(p);
		}
		//	VarDef -> Def
		{
			auto p = new ParseRule;
			p->expression = "VarDef -> Def";
			AddParseRule(p);
		}

		//Stc -> M1 TyDef
		{
			auto p = new ParseRule;
			p->expression = "Stc -> M1 TyDef";
			AddParseRule(p);
		}

		//M1 -> epsilon {begin_typedef}// top = type_head
		{
			auto p = new ParseRule;
			p->action_name = "begin_typedef";
			p->expression = "M1 -> epsilon";
			p->SetAction<Empty, Empty>(
				[this](Empty)->Empty
				{
					Env.SetTypeTable();
					auto tokens = GetTokenSet();
					tokens[TokenIter() + 1].color = ConsoleForegroundColor::Cyan;
					return Empty{};
				});
			AddParseRule(p);
		}

		//TyDef -> TyHead { Defs } ; {end_typedef} // pop table;get ILEntry from TyHead;TyHead.width = offset;
		{
			auto p = new ParseRule;
			p->action_name = "end_typedef";
			p->expression = "TyDef -> TyHead { Defs } ;";
			p->SetAction<Empty, ILEntry*>(
				[this](ILEntry* entry)->Empty
				{
					auto token_set = GetTokenSet();
					auto iter = TokenIter();
					for (size_t i = iter + 1; i < token_set.size(); ++i)
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
			AddParseRule(p);
		}

		//TyHead -> Record Id {set_typeHead}// new ILEntry;set type(struct/class/...);set Token(cTyname); AddEntry;
		{
			auto p = new ParseRule;
			p->action_name = "set_typeHead";
			p->expression = "TyHead -> Record Id";
			p->SetAction<ILEntry*, Token*, Token*>(
				[this](Token* type_token, Token* token)->ILEntry*
				{
					auto entry = Env.CreateILEntry();
					entry->meta_type = type_token->name;
					entry->token = token;
					Env.AddSubEntryToCurrent(entry);
					//head of type_table doesn't care about offset 
					entry->offset = 0;
					Env.PushEntry();
					Env.top = entry;
					return entry;
				});
			AddParseRule(p);
		}

		//Defs -> Defs Def {building_defs} 
		{
			auto p = new ParseRule;
			p->action_name = "building_defs";
			p->expression = "Defs -> Defs Def";
			p->SetAction(nullptr);
			AddParseRule(p);
		}

		//Defs -> Def {begin_defs}
		{
			auto p = new ParseRule;
			p->action_name = "begin_defs";
			p->expression = "Defs -> Def";
			p->SetAction(nullptr);
			AddParseRule(p);
		}

		//Def -> Ty Id ; {set_as_def}// get ILEntry from Ty;Get Token from Id; Ty.token = Id.token; AddEntry;
		{
			auto p = new ParseRule;
			p->action_name = "set_as_def";
			p->expression = "Def -> Ty Id ;";
			p->SetAction<Empty, ILEntry*, Token*>(
				[this](ILEntry* entry, Token* token)->Empty
				{
					entry->token = token;
					Env.AddSubEntryToCurrent(entry);
					return Empty{};
				});
			AddParseRule(p);
		}

		//Def -> Ty Id = ImVal ; {set_def_with_val};// get ILEntry from Ty;Get Token from Id(1); Ty.token = Id(1).token; Ty.val = Id(2).token.name; AddEntry;
		{
			auto p = new ParseRule;
			p->action_name = "set_def_with_val";
			p->expression = "Def -> Ty Id = ImVal ;";
			p->SetAction<Empty, ILEntry*, Token*, Empty, Token*>(
				[this](ILEntry* entry, Token* token, Empty, Token* value_token)->Empty
				{
					entry->token = token;
					entry->value = value_token->name;
					Env.AddSubEntryToCurrent(entry);
					return Empty{};
				});
			AddParseRule(p);
		}

		//Ty -> CTy {passon_0};
		{
			auto p = new ParseRule;
			p->expression = "Ty -> CTy";
			AddParseRule(p);
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
			auto p = new ParseRule;
			p->action_name = "complete_arrayType";
			p->expression = "Ty -> BTy Arr";
			p->SetAction<PassOn, ILEntry*, std::vector<size_t>&>(complete_arrayType);
			AddParseRule(p);
		}

		//Ty -> CTy Arr {complete_arrayType}
		{
			auto p = new ParseRule;
			p->action_name = "complete_arrayType";
			p->expression = "Ty -> CTy Arr";
			p->SetAction<PassOn, ILEntry*, std::vector<size_t>&>(complete_arrayType);
			AddParseRule(p);
		}

		//Ty -> BTy {passon_0}
		{
			auto p = new ParseRule;
			p->expression = "Ty -> BTy";
			AddParseRule(p);
		}
		//Arr -> Arr Cmp {building_array};// array_info.push_back(stoi(Cmp.Token.name));
		{
			auto p = new ParseRule;
			p->action_name = "building_array";
			p->expression = "Arr -> Arr Cmp";
			p->SetAction<PassOn, std::vector<size_t>&, Token*>(
				[this](std::vector<size_t>& array_info, Token* token)->PassOn
				{
					array_info.push_back(std::stoi(token->name));
					return PassOn(0);
				});
			AddParseRule(p);
		}

		//Arr -> Cmp {begin_array}// pass array_info.push_back(stoi(Cmp.Token.name));
		{
			auto p = new ParseRule;
			p->action_name = "begin_array";
			p->expression = "Arr -> Cmp";
			p->SetAction<std::vector<size_t>, Token*>(
				[this](Token* token)->std::vector<size_t>
				{
					std::vector<size_t> array_info;
					array_info.push_back(std::stoi(token->name));
					return array_info;
				});
			AddParseRule(p);
		}

		//Cmp -> [ Num ] {passon_1}// PassOn Token;
		{
			auto p = new ParseRule;
			p->expression = "Cmp -> [ Num ]";
			p->SetAction(PassOn(1));
			AddParseRule(p);
		}

		//CTy -> cTy {set_customeType}// search in type_head;copy;new ILEntry;set type;set width;
		{
			auto p = new ParseRule;
			p->action_name = "set_customeType";
			p->expression = "CTy -> cTy";
			p->SetAction<ILEntry*, Empty>(
				[this](Empty)
				{
					ILEntry* entry = nullptr;
					auto& token = CurrentToken();
					for (const auto& type : *Env.type_head->table_ptr)
						if (type->token->name == token.name)
							entry = Env.CreateVarFromCType(type);
					return entry;
				});
			AddParseRule(p);
		}

		//BTy -> bTy {set_baseType}// new ILEntry;set type;set width;
		{
			auto p = new ParseRule;
			p->action_name = "set_baseType";
			p->expression = "BTy -> bTy";
			p->SetAction<ILEntry*, Empty>(
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
			AddParseRule(p);
		}


		auto get_token = [this](Empty)->Token*
		{
			auto token = &CurrentToken();
			return token;
		};

		//Record -> record {get_token}// Get Token;return Token;
		{
			auto p = new ParseRule;
			p->expression = "Record -> record";
			p->SetAction<Token*, Empty>(get_token);
			AddParseRule(p);
		}

		//Id -> id {get_token};
		{
			auto p = new ParseRule;
			p->expression = "Id -> id";
			p->SetAction<Token*, Empty>(get_token);
			AddParseRule(p);
		}

		//ImVal -> Str {passon_0}
		{
			auto p = new ParseRule;
			p->expression = "ImVal -> Str";
			AddParseRule(p);
		}

		//ImVal -> Num {passon_0}
		{
			auto p = new ParseRule;
			p->expression = "ImVal -> Num";
			AddParseRule(p);
		}

		//Str -> str {get_token}
		{
			auto p = new ParseRule;
			p->expression = "Str -> str";
			p->SetAction<Token*, Empty>(get_token);
			AddParseRule(p);
		}

		//Num -> num {get_token}
		{
			auto p = new ParseRule;
			p->expression = "Num -> num";
			p->SetAction<Token*, Empty>(get_token);
			AddParseRule(p);
		}

		//_Assignment:
		ParseRule::SetCurrentScope("_Assignment");
		//	Asgn -> LVal = Expr ; {assign};
		{
			auto p = new ParseRule;
			p->action_name = "assign";
			p->expression = "Asgn -> LVal = Expr ;";
			p->SetAction<Empty, Address*, Address*>(
				[this](Address* laddr, Address* raddr)->Empty
				{
					Env.ILCodeStream.push_back({ laddr, "", raddr, nullptr });
					return Empty{};
				});
			p->SetSemanticErrorAction([this](std::vector<std::any*> data)
				{
					auto index = NextSemanticError(data);
					return data[index];
				});
			AddParseRule(p);
		}
		//	Expr -> BTpl || BTpl
		{
			auto p = new ParseRule;
			p->expression = "Expr -> BTpl || BTpl";
			p->SetAction(nullptr);
			AddParseRule(p);
		}
		//	Expr -> BTpl {passon_0};
		{
			auto p = new ParseRule;
			p->expression = "Expr -> BTpl";
			AddParseRule(p);
		}
		//	BTpl->BFn && BFn
		{
			auto p = new ParseRule;
			p->expression = "BTpl -> BFn && BFn";
			p->SetAction(nullptr);
			AddParseRule(p);
		}

		//	BTpl -> BFn {passon_0}
		{
			auto p = new ParseRule;
			p->expression = "BTpl -> BFn";
			AddParseRule(p);
		}
		//	BFn	-> ! BFn
		{
			auto p = new ParseRule;
			p->expression = "BFn -> ! BFn";
			p->SetAction(nullptr);
			AddParseRule(p);
		}
		//	BFn	-> ( _Global.BExpr ) {passon_1}
		{
			auto p = new ParseRule;
			p->expression = "BFn -> ( _Global.BExpr )";
			p->SetAction(PassOn(1));
			AddParseRule(p);
		}
		//	BFn -> RVal; {passon_0};
		{
			auto p = new ParseRule;
			p->expression = "BFn -> RVal";
			AddParseRule(p);
		}

		auto error_action = [this](std::vector<std::any*> data)
		{
			auto index = NextSemanticError(data);
			return data[index];
		};

		//	RVal -> RVal + Tpl {do_add};
		{
			auto p = new ParseRule;
			p->action_name = "do_add";
			p->expression = "RVal -> RVal + Tpl";
			p->SetAction<Address*, Address*, Empty, Address*>(
				[this](Address* rval, Empty, Address* tpl)->Address*
				{
					auto new_addr = Env.CreateAddress();
					Env.ILCodeStream.push_back({ new_addr,"ADD",rval,tpl });
					return new_addr;
				});
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}
		//	RVal -> RVal - Tpl {do_sub};
		{
			auto p = new ParseRule;
			p->action_name = "do_sub";
			p->expression = "RVal -> RVal - Tpl";
			p->SetAction<Address*, Address*, Empty, Address*>(
				[this](Address* rval, Empty, Address* tpl)->Address*
				{
					auto new_addr = Env.CreateAddress();
					Env.ILCodeStream.push_back({ new_addr,"SUB",rval,tpl });
					return new_addr;
				});
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}
		//	RVal -> Tpl {passon_0};
		{
			auto p = new ParseRule;
			p->expression = "RVal -> Tpl";
			AddParseRule(p);
		}
		//	Tpl -> Tpl * Fn {do_mul}
		{
			auto p = new ParseRule;
			p->action_name = "do_mul";
			p->expression = "Tpl -> Tpl * Fn";
			p->SetAction<Address*, Address*, Empty, Address*>(
				[this](Address* rval, Empty, Address* tpl)->Address*
				{
					auto new_addr = Env.CreateAddress();
					Env.ILCodeStream.push_back({ new_addr,"MUL",rval,tpl });
					return new_addr;
				});
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}
		//	Tpl -> Tpl / Fn; {do_div};
		{
			auto p = new ParseRule;
			p->action_name = "do_div";
			p->expression = "Tpl -> Tpl / Fn";
			p->SetAction<Address*, Address*, Empty, Address*>(
				[this](Address* rval, Empty, Address* tpl)->Address*
				{
					auto new_addr = Env.CreateAddress();
					Env.ILCodeStream.push_back({ new_addr,"DIV",rval,tpl });
					return new_addr;
				});
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}
		//	Tpl -> Fn {passon_0}
		{
			auto p = new ParseRule;
			p->expression = "Tpl -> Fn";
			AddParseRule(p);
		}
		//	Fn -> ( RVal ) {passon_1};
		{
			auto p = new ParseRule;
			p->expression = "Fn	->	( RVal ) ";
			p->SetAction(PassOn(1));
			AddParseRule(p);
		}
		//	Fn -> - Val {do_negate};// new Temp Entry
		{
			auto p = new ParseRule;
			p->action_name = "do_negate";
			p->expression = "Fn -> - Val";
			p->SetAction<Address*, Empty, Address*>(
				[this](Empty, Address* addr)->Address*
				{
					auto new_addr = Env.CreateAddress();
					Env.ILCodeStream.push_back({ new_addr,"-", addr, nullptr });
					return new_addr;
				});
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}
		//	Fn -> Val {passon_0}
		{
			auto p = new ParseRule;
			p->expression = "Fn -> Val";
			AddParseRule(p);
		}

		//	Val -> LVal  {passon_0}
		{
			auto p = new ParseRule;
			p->expression = "Val -> LVal";
			AddParseRule(p);
		}

		//	Val -> ImVal; {create_addr};
		{
			auto p = new ParseRule;
			p->action_name = "create_addr";
			p->expression = "Val -> ImVal";
			p->SetAction<Address*, Token*>(
				[this](Token* token)->Address*
				{
					auto addr = Env.CreateAddress();
					addr->token = token;
					return addr;
				});
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}

		// struct
		//	LVal -> M0 LValCmp {complete_lVal};//pop the Env.top;
		{
			auto p = new ParseRule;
			p->action_name = "complete_lVal";
			p->expression = "LVal -> M0 LValCmp";
			p->SetAction<PassOn, Empty, Address*>(
				[this](Empty, Address* addr)->PassOn
				{
					Env.PopEntry();
					return PassOn(1);
				});
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}
		//	M0 -> epsilon; {start_lVal};//push Env.top;set Env.top as var_head;
		{
			auto p = new ParseRule;
			p->action_name = "start_lVal";
			p->expression = "M0 -> epsilon";
			p->SetAction<Empty, Empty>(
				[this](Empty)->Empty
				{
					Env.PushEntry();
					Env.top = Env.var_head;
					return Empty{};
				});
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}

		//	LValCmp -> LValCmp . LValMem {filling_addr};//connect LValMem<Address>.chain[0] to LValCmp<Address>.chain;
		{
			auto p = new ParseRule;
			p->action_name = "filling_addr";
			p->expression = "LValCmp -> LValCmp . LValMem";
			p->SetAction<PassOn, Address*, Address*>(
				[this](Address* LValCmp, Address* LValMem)
				{
					LValCmp->chain.push_back(LValMem->chain[0]);
					if (LValMem->array_index.size())
						LValCmp->array_index.push_back(LValMem->array_index[0]);
					Env.top = LValMem->chain[0];
					return PassOn(0);
				});
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}
		//	LValCmp -> LValMem {start_Cmp};// set LValMem<Address>.lastEntry.tableptr to Env.top; return LValMem<Address>;
		{
			auto p = new ParseRule;
			p->action_name = "start_Cmp";
			p->expression = "LValCmp -> LValMem";
			p->SetAction<Address*, Address*>(
				[this](Address* addr)
				{
					Env.top = addr->chain[addr->chain.size() - 1];
					return addr;
				});
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}
		//	// baseType
		//	LValMem -> MemId {create_id_addr};//check MemId<Token>.name in table;create addr;return addr
		{
			auto p = new ParseRule;
			p->action_name = "create_id_addr";
			p->expression = "LValMem -> MemId";
			p->SetAction<Address*, Token*>(
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
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}
		//	MemId -> Id {passon_0};//passon_0
		{
			auto p = new ParseRule;
			p->expression = "MemId -> Id";
			AddParseRule(p);
		}
		//	// arrayType
		//	LValMem -> ArrVar {passon_0};
		{
			auto p = new ParseRule;
			p->expression = "LValMem -> ArrVar";
			AddParseRule(p);
		}
		//	ArrVar -> ArrId Arr {complete_array};//ArrId<Address>.array_index.push_back(Arr<Address>);pop Env.top; return AddrId<Address>;
		{
			auto p = new ParseRule;
			p->action_name = "complete_array";
			p->expression = "ArrVar -> ArrId Arr";
			p->SetAction<PassOn, Address*, Address*>(
				[this](Address* ArrId, Address* Arr) -> PassOn
				{
					ArrId->array_index.push_back(Arr);
					cur_addr = addr_stack.top();
					addr_stack.pop();
					return PassOn(1);
				});
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}
		//	ArrId -> Id {create_array_dimension};//check Id<Token>.name in Env.top;create addr;push Env.top;set entry.tableptr to Env.top; return addr;
		{
			auto p = new ParseRule;
			p->action_name = "create_array_dimension";
			p->expression = "ArrId -> Id";
			p->SetAction<Address*, Token*, Address*>(
				[this](Token* token, Address* Arr) -> Address*
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
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}
		//	Arr -> Arr Cmp {process_offset};//create addr for [t_mul,t_offset], Generator Code: "t_mul = Addr1 * (ArrayInfo[dimension-1])","t_offset = t_mul + Addr2"; return t_offset;
		{
			auto p = new ParseRule;
			p->action_name = "process_offset";
			p->expression = "Arr -> Arr Cmp";
			p->SetAction<Address*, Address*, Address*>(
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
			p->SetSemanticErrorAction(error_action);
			AddParseRule(p);
		}
		//	Arr -> Cmp {passon_0};
		{
			auto p = new ParseRule;
			p->expression = "Arr -> Cmp";
			AddParseRule(p);
		}
		//	Cmp -> [ RVal ] {inc_array_dimension};//return RVal<Address>;
		{
			auto p = new ParseRule;
			p->action_name = "inc_array_dimension";
			p->expression = "Cmp -> [ RVal ]";
			p->SetAction<PassOn, Empty, Address*>(
				[this](Empty, Address* addr) -> PassOn
				{
					++cur_dim;
					return PassOn(1);
				});
			AddParseRule(p);
		}
		//	ImVal -> Str {passon_0};
		{
			auto p = new ParseRule;
			p->expression = "ImVal -> Str";
			AddParseRule(p);
		}
		//	ImVal -> Num {passon_0};
		{
			auto p = new ParseRule;
			p->expression = "ImVal -> Num";
			AddParseRule(p);
		}

		//	Str -> str {get_token};
		{
			auto p = new ParseRule;
			p->expression = "Str -> str";
			p->SetAction<Token*, Empty>(get_token);
			AddParseRule(p);
		}
		//	Id -> id; {get_token};
		{
			auto p = new ParseRule;
			p->expression = "Id -> id";
			p->SetAction<Token*, Empty>(get_token);
			AddParseRule(p);
		}
		//	Num->num; {get_token};
		{
			auto p = new ParseRule;
			p->expression = "Num -> num";
			p->SetAction<Token*, Empty>(get_token);
			AddParseRule(p);
		}
		Initialize();
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
};

