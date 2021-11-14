#pragma once

// DEBUG OPTIONS:
// Custom Syntax File Debug Options:
//#define CUSTOM_SYNTAX_FILE_DEBUG
//#define SEMANTIC_CHECK
//#define SHOW_CATCHED_VAR
//#define HIGH_LIGHT

// Input Parsing Debug Options:
#define SHOW_PARSE_PROCESS

#define AddAction(action)			AddSemanticAction(#action, action)

// CUSTOM CODING HELPER:
//#define LR1							LR1Parser<size_t>
//#define SLR							SLRParser<size_t>
//#define INPUT						std::vector<void*> input, size_t token_iter, Token_Set& token_set
//#define TokenSet					token_set
//#define TokenIter					token_iter
//#define CurrentToken				(token_set[token_iter])
//
//
//#define AddAction(action)			AddSemanticAction(#action,&Type::action)
//#define Initialization			    CompleteSemanticActionTable
//#define GetValue(Type,index)		(*(Type*)(input[index]))		
//#define GetPtr(Type,index)			((Type*)(input[index]))		
//#define PassOn(index)				input[index]
//#define Create(Type,...)			MakeStorage(new Type(__VA_ARGS__))
//#define CreateAs(Type,expr)			MakeStorage<Type>(new Type(expr))
//#define CreateFrom(expr)			MakeStorageFrom(expr)
//
//
//#define STOP_PARSING				(void*)1
//#define SEMANTIC_ERROR				(void*)2
//#define NIL							nullptr
//#define HasSemanticError			_hasSemanticError(input,token_iter,token_set)
//#define IfSemanticErrorThenPassOn	if(_hasSemanticError(input,token_iter,token_set)) return SEMANTIC_ERROR