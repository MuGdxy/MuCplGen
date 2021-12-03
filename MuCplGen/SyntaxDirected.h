#pragma once
#include <map>
#include <unordered_map>
#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <sstream>
#include <typeinfo>
#include <functional>
#include <any>
#include "FileLoader.h"
#include "EasyScanner.h"
#include "DebugTool/Highlight.h"
#include "Parser.h"
#include "SealedValue.h"
#include "CFG.h"

namespace MuCplGen
{
	using namespace MuCplGen::Debug;
	enum class BuildOption
	{
		Runtime = 0,
		Save = 1,
		Load = 1 << 1,
		LoadAndSave = Load | Save
	};

	class BaseSyntaxDirected
	{
	protected:
		std::list<ParseRule*> parse_rules;
		std::string current_scope;
	public:
		void Scope(const std::string& scope) { current_scope = scope; }
		const std::string& Scope() { return current_scope; }

		ParseRule& CreateParseRule()
		{
			auto tmp = new ParseRule;
			tmp->scope = current_scope;
			parse_rules.push_back(tmp);
			return *tmp;
		}

		~BaseSyntaxDirected()
		{
			for (auto rule : parse_rules) delete rule;
		}
	};

	template<typename Parser>
	class SyntaxDirected : public BaseSyntaxDirected
	{
	public:
		using Token = typename Parser::Token;
		using TokenSet = std::vector<Token>;
		using Sym = typename Parser::SymbolType;
		using Term = Terminator<Token, Sym>;
	private:
		using Production = std::vector<Sym>;
		using Productions = std::vector<Production>;
		using ProductionTable = std::vector<Productions>;


		Sym start = 0;
		Sym epsilon = 0;
		Sym end = 0;
		Parser my_parser;

		std::unordered_map<std::string, Sym> name_to_sym;
		std::vector<std::string> sym_to_name;


		ProductionTable production_table;

	protected:
		using SemanticAction = std::function<std::any* (std::vector<std::any*>, size_t, TokenSet&)>;
	private:
		std::ostream& log;
		std::vector<Term*> terminator_rules;
		std::vector<std::vector<ParseRule*>> quick_parse_rule_table;
		//std::vector<Sym> expects, size_t token_iter
		std::function<void(std::vector<Sym>, size_t)> error_action;

		MU_NOINLINE
			Sym	TokenToTerminator(const Token& token)
		{
			Sym sym = -1;
			size_t priority = -1;
			for (auto term : terminator_rules)
			{
				bool success = term->translation(token);
				if (success && term->priority == 0)
				{
					sym = term->sym;
					break;
				}
				else if (success)
				{
					if (term->priority < priority) priority = term->priority;
					sym = term->sym;
				}
			}
			if (token.IsEndToken()) return end;
			if (sym == (Sym)-1)
				throw(std::logic_error("TokenToTerminator Failed"));
			else return sym;
		}

		MU_NOINLINE
			std::any* SemanticActionDispatcher(std::vector<std::any*> input, size_t nonterm, size_t pro_index, size_t token_iter)
		{
			auto rule = quick_parse_rule_table[nonterm][pro_index];
			this->token_iter = token_iter;
			auto error_pos = -1;
			auto error_data = NextSemanticError(input, error_pos);
			if (error_data)
			{
				if (debug_option & DebugOption::ShowReductionProcess)
				{
					log << rule->fullname_expression << " Action{" << rule->action_name << "} => Semantic Error Occurs" << std::endl;
				}
				if (rule->semantic_error) return rule->semantic_error(input);
				else return input[error_pos];
			}
			else
			{
				if (debug_option & DebugOption::ShowReductionProcess)
				{
					log << rule->fullname_expression << " Action{" << rule->action_name << "}" << std::endl;
				}
				if (rule->semantic_action == nullptr) return nullptr;
				else return rule->semantic_action(input);
			}
		}

		MU_NOINLINE
			void ErrorReductionActionDispatcher(std::vector<Sym> expects, size_t token_iter)
		{
			if (error_action == nullptr)
			{
				if (debug_option & DebugOption::SyntaxError)
				{
					if (input_text)
					{
						std::stringstream ss;
						ss << "Expected Symbol:";
						for (const auto& item : expects)
							ss << " \'" << sym_to_name[item] << "\' ";
						Highlight(*(input_text), *(token_set), { { token_iter, ss.str() } }, log);
					}
					if ((*token_set)[token_iter].IsEndToken())
					{
						if (token_iter > 0) log << "Missing Token After:\n" << (*token_set)[token_iter - 1] << std::endl;
						else log << "No Symbol At All!" << std::endl;
					}
					else log << "Error Token:\n" << (*token_set)[token_iter] << std::endl;
				}
			}
			else error_action(std::move(expects), token_iter);
		}

		std::string ScopedName(const std::string& scope, const std::string& name)
		{
			if (scope.empty()) return name;
			else if (name.find(".") == -1) return scope + "." + name;
			else return name;
		}

		void CheckAndAddSymbol(const std::string& name)
		{
			if (name_to_sym.find(name) == name_to_sym.end())
			{
				name_to_sym.insert({ name, sym_to_name.size() });
				sym_to_name.push_back(name);
			}
		}

		MU_NOINLINE
		void SetupSymbols()
		{
			//start = 0 nonterm epsilon term end
			//terminator
			for (auto& rule : parse_rules)
			{
				rule->ParseExpression();
				CheckAndAddSymbol(rule->ScopedName(rule->head));
			}

			production_table.resize(sym_to_name.size());
			quick_parse_rule_table.resize(sym_to_name.size());
			epsilon = sym_to_name.size();
			name_to_sym.insert({ "epsilon",sym_to_name.size() });
			sym_to_name.push_back("epsilon");


			// ruled terminator
			for (auto rule : terminator_rules)
			{
				std::string name = rule->ScopedName(rule->name);
				rule->sym = sym_to_name.size();
				CheckAndAddSymbol(name);
			}

			//wild terminator
			for (auto rule : parse_rules)
			{
				Production production;
				for (auto& body : rule->body)
				{
					auto name = rule->ScopedName(body);
					if (name_to_sym.find(name) == name_to_sym.end())//not a defined nonterm
					{
						if (name_to_sym.find(body) == name_to_sym.end()) //new term
						{
							auto sym = sym_to_name.size();
							name_to_sym.insert({ body, sym });
							sym_to_name.push_back(body);
							production.push_back(sym);

							auto& t = CreateTerminator();
							t.name = body;
							t.translation = [&t](const Token& token)
							{
								return t.name == token.name;
							};
							t.sym = sym;
						}
						else production.push_back(name_to_sym[body]);
					}
					else production.push_back(name_to_sym[name]);
				}
				auto head = rule->ScopedName(rule->head);
				std::stringstream ss;
				ss << head;
				ss << " -> ";
				for (auto& p : production) ss << sym_to_name[p] << " ";
				rule->fullname_expression = ss.str();
				production_table[name_to_sym[head]].push_back(std::move(production));
				quick_parse_rule_table[name_to_sym[head]].push_back(rule);
			}

			end = sym_to_name.size();
			sym_to_name.push_back("$");

			if (debug_option & DebugOption::ShowCatchedVariables)
			{
				log << "Catched Variables:" << std::endl;
				for (auto& name : sym_to_name) log << name << std::endl;
			}
			if (debug_option & DebugOption::ShowProductionTable)
			{
				log << "Production Table:" << std::endl;
				for (size_t i = 0; i < production_table.size(); ++i)
					for (auto& production : production_table[i])
					{
						log << sym_to_name[i] << " -> ";
						for (auto b : production) log << sym_to_name[b] << " ";
						log << std::endl;
					}
			}
		}

	size_t token_iter = 0;
	TokenSet* token_set = nullptr;
	std::vector<LineContent>* input_text = nullptr;
	std::string storage = "";
public:
	SyntaxDirected(std::ostream& log = std::cout) : log(log) {}

	MU_NOINLINE
	void Build()
	{
		SetupSymbols();
		if (generation_option == BuildOption::Runtime) 
			my_parser.SetUp(production_table, end - 1, end, epsilon, start);
		else
		{
			if ((int)generation_option & (int)BuildOption::Load && !Load(storage))
			{
				my_parser.SetUp(production_table, end - 1, end, epsilon, start);
				if ((int)generation_option & (int)BuildOption::Save) Save(storage);
			}
		}
		my_parser.Reset();
		my_parser.debug_option = debug_option;
	}

	void Save(const std::string& path) { my_parser.Save(path); }

	bool Load(const std::string& path) { return my_parser.Load(path); }

	bool Parse(std::vector<LineContent>& input_text, TokenSet& token_set)
	{
		this->input_text = &input_text;
		this->token_set = &token_set;
		if (!production_table.size()) Build();
		else my_parser.Reset();
		return my_parser.Parse(token_set,
			[this](const Token& token) {return TokenToTerminator(token); },
			[this](std::vector<std::any*> input, size_t nonterm, size_t pro_index, size_t token_iter)
			{
				return SemanticActionDispatcher(input, nonterm, pro_index, token_iter);
			},
			[this](std::vector<Sym> expects, size_t token_iter)
			{
				ErrorReductionActionDispatcher(expects, token_iter);
			}, log);
	}

	TokenSet& GetTokenSet() { return *token_set; }

	size_t TokenIter() { return token_iter; }

	Token& CurrentToken() { return (*token_set)[token_iter]; }

	std::vector<LineContent>& GetInputText() { return *input_text; }

	template<class T>
	T GetErrorData(ParserErrorData* error) { return std::any_cast<T>(error->data); }
	
	bool HasSemanticError(std::vector<std::any*> input)
	{
		auto next = -1;
		return NextSemanticError(input, next);
	}

	/*
	usage:
	int next = -1;
	while(true) 
	{ auto data = NextSemanticError(input, next); ... if(!data) break;}
	*/
	MU_NOINLINE ParserErrorData* NextSemanticError(std::vector<std::any*> input, int& next)
	{
		auto first = next + 1;
		auto size = input.size();
		for (int i = first; i < size; ++i)
		{
			if (input[i] == nullptr) continue;
			if (auto p = std::any_cast<ParserErrorData>(input[i]))
				if (p && p->code == ParserErrorCode::SemanticError)
				{
					next = i;
					return p;
				}
		}
		next = -1;
		return nullptr;
	}

	const std::string& GetSymbolName(Sym sym)
	{
		return sym_to_name[sym];
	}

	~SyntaxDirected()
	{
		for (auto rule : terminator_rules) delete rule;
	}

#pragma region For Custom Code 
protected:
	BuildOption generation_option = BuildOption::Runtime;

	void SetStorage(const std::string& storage) { this->storage = storage; }

	Term& CreateTerminator()
	{
		auto tmp = new Term;
		terminator_rules.push_back(tmp);
		return *tmp;
	};

	void AddParseErrorAction(std::function<void(std::vector<Sym> expects, size_t token_iter)> action)
	{
		error_action = action;
	}

	size_t debug_option = 0;
#pragma endregion
};

	class BaseSubModule
	{
	protected:
		std::string scope;
		BaseSyntaxDirected& bsd;
		ParseRule& CreateParseRule()
		{
			auto& tmp = bsd.CreateParseRule();
			tmp.scope = scope;
			return tmp;
		}
	public:
		BaseSubModule(BaseSyntaxDirected* bsd, const std::string& scope):bsd(*bsd), scope(scope){}
		virtual void CreateRules(const std::string& out_nonterm) = 0;
	};
}