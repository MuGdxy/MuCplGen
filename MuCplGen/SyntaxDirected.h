#pragma once
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <typeinfo>
#include <functional>
#include <any>
#include "FileLoader.h"
#include "EasyScanner.h"
#include "Highlight.h"
#include "Parser.h"
#include "SealedValue.h"
#include "CustomCodeMacro.h"

namespace MuCplGen
{
	using namespace MuCplGen::Debug;
	template<typename Parser>
	class SyntaxDirected
	{
	public:
		using Token = typename Parser::Token;
		using TokenSet = std::vector<Token>;
	private:
		using Sym = typename Parser::SymbolType;
		// A -> ¦Á
		using Production = std::vector<Sym>;
		// A -> ¦Á0|¦Á1|...
		using Productions = std::vector<Production>;
		// A -> ¦Á0|¦Á1|...
		// B -> ¦Â0|¦Â2|...
		// ...
		using Production_Table = std::vector<Productions>;

#pragma region Syntax Rule of Syntax Productions
		enum SyntaxSymbol
		{
			ss_none = -1,
			WholeOrNone_,
			WholeOrNone,
			Whole,
			LabeledPro,
			Pro,
			Head,
			Body,
			IDs,
			ID,
			Statement,
			ActionLabel,
			ScopeLabel,

			ss_epsilon,
			ss_sym,
			axis,	//->
			le,		//{ 
			re,		//}
			semi,	//;
			colon,	//:
			point,	//.
			ss_end
		};
		EasyScanner easyScanner;
		SLRParser<EasyToken, SyntaxSymbol> slr_parser_for_syntax;
		std::vector<std::vector<std::vector<SyntaxSymbol>>> syntax_production_table;
		static SyntaxSymbol TransferForSyntax(const EasyToken& token)
		{
			if (token.type == EasyToken::TokenType::separator)
			{
				if (token.name == "->") return axis;
				else if (token.name == "{") return le;
				else if (token.name == "}") return re;
				else if (token.name == ";") return semi;
				else if (token.name == ":") return colon;
				else if (token.name == ".") return point;
				else return ss_sym;
			}
			else if (token.IsEndToken()) return ss_end;
			else return ss_sym;
		}
		static size_t syntax_unique_id;
		static size_t GetUniqueId()
		{
			return syntax_unique_id++;
		}
#pragma endregion
		Sym start = 0;
		Sym epsilon = 0;
		Sym end = 0;
		std::vector<EasyToken> token_set_for_production;
		std::vector<EasyToken> token_set_for_definition;
		Parser my_parser;
		std::map<std::string, Sym> record;
		std::set<std::string> heads;
		std::vector<std::string> sym_table;

		struct Production_Flag
		{
			Sym head;
			std::vector<std::string> names_in_body;
		};
		Production_Flag current_flag;
		std::string current_name;
		Production_Table production_table;
		std::string scope_label = "_Global";
		size_t part = 0;
		bool scope_on = true;
		std::string ScopedName(const EasyToken& token)
		{
			if (token.type == EasyToken::TokenType::raw_string)
			{
				std::string name = token.name.substr(1, token.name.size() - 2);
				return name;
			}
			if (!scope_on) return token.name;
			if (part == 0) return token.name;
			std::string name = scope_label + "." + token.name;
			if (part == 2 && !heads.count(name))
			{
#ifdef SEMANTIC_CHECK
				if (token.name[0] >= 'A' && token.name[0] <= 'Z')
					log << "[Semantic Check]: \n[" << token <<
					"] starts with big letter,but is recognized as term" << std::endl;
#endif // CHECK_ON
				return token.name;
			}
			return name;
			//return token.name;
		}
		struct InfoPair
		{
			Sym nonterm;
			size_t pro_index;
		};
		std::multimap<std::string, InfoPair> statements;
		// find statement with (nonterm,pro_index)
		//std::vector<void*> input, size_t token_iter, Token_Set& token_set
		//using SemanticAction = std::function<void* (std::vector<void*>, size_t, TokenSet&)>;
	protected:
		using SemanticAction = std::function<std::any* (std::vector<std::any*>, size_t, TokenSet&)>;
		struct NamedSemanticAction
		{
			std::string name;
			SemanticAction action;
		};

	private:
		std::ostream& log;
		std::map<std::string, NamedSemanticAction> initial_semantic_action_table;
		std::vector<std::vector<NamedSemanticAction>> quick_semantic_action_table;
		//std::vector<Sym> expects, size_t token_iter
		std::function<void(std::vector<Sym>, size_t)> error_action;

		// Candidate and Quick_Candidate are used to tell which Token will be transfered to which terminator
		// e.g. TokenType::identifier -> id || TokenType::digit -> id || Token.name == ")" -> )
		struct Candidate
		{
			std::string type;
			std::string name;
			std::string sym_name;
		};
		struct Quick_Candidate
		{
			EasyToken::TokenType type;
			std::string type_name;
			std::string name;
			Sym sym;
		};
		std::set<std::string> candidate_record;
		std::vector<Candidate> candidates;
		std::vector<Quick_Candidate> quick_candidates;
		Candidate candidate_flag;

		Sym			TransferForDefinition(const Token& token);
		void		SyntaxActionSetUpDefinition(size_t nonterm, size_t pro_index, size_t token_iter);
		void		SyntaxActionSetHead(size_t nonterm, size_t pro_index, size_t token_iter);
		void		SyntaxActionSetBody(size_t nonterm, size_t pro_index, size_t token_iter);
		std::any* SemanticActionDispatcher(std::vector<std::any*> input, size_t nonterm, size_t pro_index, size_t token_iter);
		void		ErrorActionDispatcher(std::vector<Sym> expects, size_t token_iter);
	public:
		SyntaxDirected(const std::string& path, std::ostream& log = std::cout);

		bool Parse(std::vector<LineContent>& input_text, TokenSet& token_set)
		{
			SetInput(input_text);
			return Parse(token_set);
		}

		bool Parse(TokenSet& token_set)
		{
			this->token_set = &token_set;
			return my_parser.Parse(token_set,
				[this](const Token& token) {return TransferForDefinition(token); },
				[this](std::vector<std::any*> input, size_t nonterm, size_t pro_index, size_t token_iter)
				{
					return SemanticActionDispatcher(input, nonterm, pro_index, token_iter);
				},
				[this](std::vector<Sym> expects, size_t token_iter)
				{
					ErrorActionDispatcher(expects, token_iter);
				}, log);
		}

		void SetInput(std::vector<LineContent>& input_text)
		{
			this->input_text = &input_text;
		}

		~SyntaxDirected()
		{
			while (!to_delete.empty())
			{
				delete to_delete.top();
				to_delete.pop();
			}
		}

#pragma region For Custom Code 
	protected:
		std::vector<LineContent>* input_text = nullptr;
		TokenSet* token_set;

		template<class T, class ...Args>
		std::any* Create(Args&&... args)
		{
			auto temp = new SealedValue<T>(args...);
			to_delete.push(temp);
			return &(temp->o);
		}

		template<class T>
		T& Get(std::any* a) { return std::any_cast<T&>(*a); }

		std::any* PassOn(std::vector<std::any*> data, int pos)
		{
			return data[pos];
		}

		std::any* ParserError(ParserErrorCode code = ParserErrorCode::Stop)
		{
			auto temp = new SealedValue<ParserErrorCode>(code);
			to_delete.push(temp);
			return &(temp->o);
		}

		// Semantic Actions Setup
		void CompleteSemanticActionTable()
		{
			my_parser.SetUp(production_table, end - 1, end, epsilon, start);
			SetupSemanticActionTable();
			//set the semantic action from initial_semantic_action_table to quick_semantic_action_table
			for (const auto& statement : statements)
			{
				auto iter = initial_semantic_action_table.find(statement.first);
				if (iter != initial_semantic_action_table.end())
					quick_semantic_action_table[statement.second.nonterm][statement.second.pro_index] = iter->second;
			}
		}

		virtual void SetupSemanticActionTable()
		{
			SetConsoleColor(ConsoleForegroundColor::enmCFC_Yellow, ConsoleBackGroundColor::enmCBC_Blue);
			log << "[Warning]: Yet Haven't Set Semantic Actions!!!" << std::endl;
			SetConsoleColor();
		}

		void AddSemanticAction(const char* statement_label, SemanticAction action)
		{
			NamedSemanticAction nsa;
			nsa.name = statement_label;
			nsa.action = action;
			initial_semantic_action_table[statement_label] = std::move(nsa);
		}

		void AddSemanticAction(NamedSemanticAction nsa)
		{
			initial_semantic_action_table[nsa.name] = nsa;
		}

		const std::string& GetSymbolName(Sym sym)
		{
			return sym_table[sym];
		}

		void AddParseErrorAction(std::function<void(std::vector<Sym> expects, size_t token_iter)> action)
		{
			error_action = action;
		}

		bool HasSemanticError(std::vector<std::any*> input)
		{
			return NextSemanticError(input) >= 0;
		}

		int NextSemanticError(std::vector<std::any*> input, int last = -1)
		{
			auto first = last + 1;
			auto size = input.size();
			for (int i = first; i < size; ++i)
			{
				if (input[i] == nullptr) continue;
				if (auto p = std::any_cast<ParserErrorCode>(input[i]))
					if (*p == ParserErrorCode::SemanticError) return i;
			}
			return -1;
		}

		void Initialize()
		{
			CompleteSemanticActionTable();
			my_parser.debug_option = debug_option;
		}

		size_t debug_option = 0;
	private:
		std::stack<Sealed*> to_delete;
#pragma endregion
	};

	class TestCompiler :public SyntaxDirected<SLRParser<size_t>>
	{
	public:
		TestCompiler(std::string cfg_path) :SyntaxDirected(cfg_path)
		{

		}
	};

	template<typename Parser>
	size_t SyntaxDirected<Parser>::syntax_unique_id = 0;

	template<typename Parser>
	SyntaxDirected<Parser>::SyntaxDirected(const std::string& path, std::ostream& log) :
		log(log),
		syntax_production_table(
			{
				//Whole_ -> WholeOrNone
				{{WholeOrNone}},
				//WholeOrNone -> Whole | epsilon
				{{Whole},{ss_epsilon}},
				//Whole -> Whole LabeledPro | LabeledPro
				{{Whole,LabeledPro},{LabeledPro}},
				//LabeledPro -> ScopeLabel Pro
				{{ScopeLabel,Pro},{Pro}},
				//Pro -> Head "->" Body | Head "->" Body Statement
				{{Head,axis,Body},{Head,axis,Body,Statement}},
				//Head -> ss_sym
				{{ID}},
				//Body -> IDs;
				{{IDs,semi}},
				//IDs -> IDs ID | ID
				{{IDs,ID},{ID}},
				//ID -> ss_sym | ss_sym.ss_sym 
				{{ss_sym},{ss_sym,point,ss_sym}},
				//Statement -> { ss_sym };
				{{le,ActionLabel,re,semi}},
				//ActionLabel
				{{ss_sym}},
				//ScopeLabel -> ss_sym:
				{{ss_sym,colon}}
			})
	{
		//static_assert(std::is_same_v<Parser, SLRParser<Token, Sym>>::value
		//	|| std::is_same_v<Parser, LR1Parser<Token, Sym>>::value,
		//	"Only SLRParser & LR1Parser supported");

		slr_parser_for_syntax.SetUp(syntax_production_table, (SyntaxSymbol)(ss_end - 1), ss_end, ss_epsilon, WholeOrNone_);
		//slr_parser_for_definition.SetUp(definition_production_table,(Syntax_Symbol)(ss_end-1),ss_end,ss_epsilon,Whole_);
		auto inputs = FileLoader::Load(path, "$$");
		auto& definition_input = inputs[0];
		auto& production_input = inputs[1];
		token_set_for_definition = easyScanner.Scann(definition_input);
		token_set_for_production = easyScanner.Scann(production_input);
		if (debug_option & DebugOption::HighlightSyntaxFile)
		{
			log << "SyntaxLoader:" << std::endl;
			log << "Definition:" << std::endl;
			Highlight(definition_input, token_set_for_definition, log);
			log << "Production:" << std::endl;
			Highlight(production_input, token_set_for_production, log);
		}
		if (debug_option & DebugOption::SyntaxFileProcessDetail)
		{
			log << "[Syntax Parse] START" << std::endl;
			slr_parser_for_syntax.information = "[First Part Parse]";
		}
		part = 0;
		// Setup Candidates
		if (!slr_parser_for_syntax.Parse(
			token_set_for_definition,
			[](const EasyToken& token) {return TransferForSyntax(token); },
			[this](std::vector<std::any*> input, size_t nonterm, size_t pro_index, size_t token_iter)->std::any*
			{
				SyntaxActionSetUpDefinition(nonterm, pro_index, token_iter);
				return nullptr;
			},nullptr))
			throw std::exception("Syntax Config: Syntax Mistakes, check the first part of your syntax config text");
			
		slr_parser_for_syntax.Reset();

		if (debug_option & DebugOption::SyntaxFileCatchedVariables)
		{

			log << "Catched Candidates(term):" << std::endl;
			for (const auto& candidate : candidates)
				log << candidate.sym_name << " -> "
				<< "[type_name:" << candidate.type << "]"
				"[name:" << candidate.name << "]" << std::endl;
		}

		if (debug_option & DebugOption::SyntaxFileProcessDetail)
			slr_parser_for_syntax.information = "[Second Part Parse: Head]";
		
		part = 1;
		// Get nonterm
		scope_label = "_Global";
		if (!slr_parser_for_syntax.Parse(token_set_for_production,
			[](const EasyToken& token)->SyntaxSymbol {return TransferForSyntax(token); },
			[this](std::vector<std::any*> input, size_t nonterm, size_t pro_index, size_t token_iter)->std::any*
			{
				SyntaxActionSetHead(nonterm, pro_index, token_iter);
				return nullptr;
			},
			nullptr))
			throw std::exception("Syntax Config: Syntax Mistakes, check the second part of your syntax config text");
			slr_parser_for_syntax.Reset();
			// set the size of production_table,cause all nonterms have been recognized 
			production_table.resize(sym_table.size());
			quick_semantic_action_table.resize(sym_table.size());
			epsilon = sym_table.size();
			record.insert({ "epsilon",sym_table.size() });
			sym_table.push_back("epsilon");

			if (debug_option & DebugOption::SyntaxFileCatchedVariables)
			{
				log << "Catched Heads(nonterm):" << std::endl;
				for (size_t i = 0; i < sym_table.size() - 1; ++i)
				{
					log << i << " : " << sym_table[i] << std::endl;
				}
			}

			if (debug_option & DebugOption::SyntaxFileProcessDetail)
				slr_parser_for_syntax.information = "[Second Part Parse: Body]";
			part = 2;
			// Get term
			scope_label = "_Global";
			slr_parser_for_syntax.Parse(
				token_set_for_production,
				[](const EasyToken& token)->SyntaxSymbol {return TransferForSyntax(token); },
				[this](std::vector<std::any*> input, size_t nonterm, size_t pro_index, size_t token_iter)->std::any*
				{
					SyntaxActionSetBody(nonterm, pro_index, token_iter);
					return nullptr;
				},
				nullptr);
			end = sym_table.size();
			sym_table.push_back("$");

			if (debug_option & DebugOption::SyntaxFileCatchedVariables)
			{
				log << "Generated Symbols:" << std::endl;
				for (size_t i = 0; i < sym_table.size(); ++i)
					log << i << " : " << sym_table[i] << std::endl;
				log << "Catched ActionNames:" << std::endl;
				for (const auto& item : statements)
					log << item.first << " : " << sym_table[item.second.nonterm]
					<< " -> Index[" << item.second.pro_index << "]" << std::endl;
			}

			if (debug_option & DebugOption::SyntaxFileProcessDetail)
				log << "[Syntax Parse] SUCCEED" << std::endl;
			// Set Quick Candidates with the help of Candidates

			for (const auto& candidate : candidates)
			{
				Quick_Candidate q;
				//q.type = StringToTokenType(candidate.type);
				q.type_name = candidate.type;
				q.name = candidate.name;
				q.sym = record[candidate.sym_name];
				quick_candidates.push_back(std::move(q));
			}

			// Set rest Candidates
			for (const auto& sym : record)
				if (sym.second > epsilon && !candidate_record.count(sym.first))
				{
					Quick_Candidate q;
					q.type = EasyToken::TokenType::none;
					q.type_name = "";
					q.name = sym.first;
					q.sym = sym.second;
					quick_candidates.push_back(std::move(q));
				}

			if (debug_option & DebugOption::SyntaxFileCatchedCandidates)
			{
				log << "Generated Candidates(nonterm):" << std::endl;
				log << "Info: if your nonterm has been catched as candidate" << std::endl;
				log << "that maybe because your nonterm doesn't have any ProductionBody" << std::endl;
				for (const auto& candidate : quick_candidates)
					log << "\"" << sym_table[candidate.sym] << "\""
					<< "(" << candidate.sym << ")" << " -> "
					"[name:" << candidate.name << "]" << std::endl;
			}
	}

	template<typename Parser>
	typename SyntaxDirected<Parser>::Sym SyntaxDirected<Parser>::TransferForDefinition(const Token& token)
	{
		for (const auto& candidate : quick_candidates)
		{
			bool null_name = false;
			if (candidate.name != "")
			{
				if (token.Type() == candidate.type_name && token.name == candidate.name)
					return candidate.sym;
				if (token.name == candidate.name)
					return candidate.sym;
			}
			else if (token.Type() == candidate.type_name) return candidate.sym;
		}
		if (token.IsEndToken()) return end;
		throw(std::logic_error("TransferForDefinition Failed"));
	}

	template<typename Parser>
	void SyntaxDirected<Parser>::SyntaxActionSetUpDefinition(size_t nonterm, size_t pro_index, size_t token_iter)
	{
		switch (nonterm)
		{
		case Head:
		{
			auto& token = token_set_for_definition[token_iter];
			if (debug_option & DebugOption::SyntaxFileSemanticCheck)
			{
				if (token.name[0] >= 'A'&& token.name[0] <= 'Z')
					log << "[Semantic Check]:\n[" << token <<
					"] starts with big letter,but is recognized as term" << std::endl;
			}
			if (token.type == EasyToken::TokenType::raw_string)
				candidate_flag.sym_name = token.name.substr(1, token.name.size() - 2);
			else candidate_flag.sym_name = token.name;
		}
		break;
		case IDs:

			// if identifier, regard as definition via "TokenType -> terminator"
			if (token_set_for_definition[token_iter].type == EasyToken::TokenType::identifier)
			{
				candidate_flag.type = token_set_for_definition[token_iter].name;
			}
			// if raw_string, regard as definition via "TokenName -> terminator"
			else if (token_set_for_definition[token_iter].type == EasyToken::TokenType::raw_string)
			{

				const auto& name = token_set_for_definition[token_iter].name;
				candidate_flag.name = name.substr(1, name.size() - 2);
			}
			if (debug_option & DebugOption::SyntaxFileProcessDetail)
				log << "[IDs -> ss_sym]\n" << token_set_for_production[token_iter] << std::endl;
			break;
		case Body:
			candidate_record.insert(candidate_flag.sym_name);
			candidates.push_back(std::move(candidate_flag));
			break;
		case ScopeLabel:
			scope_label = token_set_for_definition[token_iter - 1].name;
			if (debug_option & DebugOption::SyntaxFileProcessDetail)
				log << "[ScopeLabel -> ss_sym:]\n" << token_set_for_definition[token_iter - 1] << std::endl;
			break;
		default:
			break;
		}
	}

	template<typename Parser>
	void SyntaxDirected<Parser>::SyntaxActionSetHead(size_t nonterm, size_t pro_index, size_t token_iter)
	{
		switch (nonterm)
		{
		case Head:// Head -> ID
		{
			if (debug_option & DebugOption::SyntaxFileSemanticCheck)
			{
				if (token_set_for_production[token_iter].name[0] >= 'a' && token_set_for_production[token_iter].name[0] <= 'z')
					log << "[Semantic Check]:\n[" << token_set_for_production[token_iter] <<
					"] starts with small letter,but is recognized as nonterm" << std::endl;
			}

			std::string name = ScopedName(token_set_for_production[token_iter]);
			if (!record.count(name))
			{
				sym_table.push_back(name);
				heads.insert(name);
				record.insert({ name,sym_table.size() - 1 });
			}
			if (debug_option & DebugOption::SyntaxFileProcessDetail)
				log << "[Head]\n" << token_set_for_production[token_iter] << std::endl;
		}
		break;
		case Pro:
			if (debug_option & DebugOption::SyntaxFileProcessDetail)
			{
				if (pro_index == 0)
					log << "[Pro -> Head axis Body]\n" << token_set_for_production[token_iter] << std::endl;
				else log << "[Pro -> Head axis Body Statement]\n" << token_set_for_production[token_iter] << std::endl;
			}
			break;
		case ID:
			if (pro_index == 0)
				//ID -> ss_sym
				scope_on = true;
			else
				//ID -> ss_sym.ss_sym
				scope_on = false;
			break;
		case ScopeLabel:
			scope_label = token_set_for_production[token_iter - 1].name;
			if (debug_option & DebugOption::SyntaxFileProcessDetail)
				log << "[ScopeLabel -> ss_sym:]\n" << token_set_for_production[token_iter - 1] << std::endl;
			break;
		default:
			break;
	}
}

	template<typename Parser>
	void SyntaxDirected<Parser>::SyntaxActionSetBody(size_t nonterm, size_t pro_index, size_t token_iter)
	{
		switch (nonterm)
		{
		case Head:
		{
			std::string name = ScopedName(token_set_for_production[token_iter]);
			current_flag.head = record[name];
		}
		break;
		case ID:
			if (pro_index == 0)
				//ID -> ss_sym
			{
				scope_on = true;
				current_name = ScopedName(token_set_for_production[token_iter]);
			}
			else
				//ID -> ss_sym.ss_sym
			{
				scope_on = false;
				current_name = token_set_for_production[token_iter - 2].name + "."
					+ token_set_for_production[token_iter].name;
			}
			break;
		case IDs:
		{
			current_flag.names_in_body.push_back(current_name);
			if (!record.count(current_name))
			{
				sym_table.push_back(current_name);
				record.insert({ current_name,sym_table.size() - 1 });
			}
		}
		break;
		case Body:
		{
			Production production;
			//for (size_t i = current_flag.start;
			//	i < current_flag.start + current_flag.production_length; ++i)
			//	production.push_back(record[ScopedName(token_set_for_production[i])]);
			for (const auto& name : current_flag.names_in_body)
				production.push_back(record[name]);
			current_flag.names_in_body.clear();
			production_table[current_flag.head].push_back(std::move(production));
			quick_semantic_action_table[current_flag.head].push_back({ "", nullptr });
			if (debug_option & DebugOption::SyntaxFileProcessDetail)
				log << "[Body -> IDs ;]\n" << token_set_for_production[token_iter] << std::endl;
		}
		break;
		case ActionLabel:
		{
			std::string name = token_set_for_production[token_iter].name;
			statements.insert(
				{
					name,
					{
						current_flag.head,
						production_table[current_flag.head].size() - 1
					}
				});

		}
		if (debug_option & DebugOption::SyntaxFileProcessDetail)
			log << "[ActionLabel]:\n" << token_set_for_production[token_iter] << std::endl;
		break;
		case ScopeLabel:
			scope_label = token_set_for_production[token_iter - 1].name;
			if (debug_option & DebugOption::SyntaxFileProcessDetail)
				log << "[ScopeLabel -> ss_sym:]\n" << token_set_for_production[token_iter - 1] << std::endl;
			break;
		default:
			break;
		}
	}

	template<typename Parser>
	std::any* SyntaxDirected<Parser>::SemanticActionDispatcher(std::vector<std::any*> input, size_t nonterm, size_t pro_index, size_t token_iter)
	{
		auto& nsa = quick_semantic_action_table[nonterm][pro_index];
		if (debug_option & DebugOption::ShowReductionProcess)
		{
			log << sym_table[nonterm] << " -> ";
			for (const auto& sym : production_table[nonterm][pro_index])
				log << sym_table[sym] << " ";
			log << "(" << (*token_set)[token_iter].name << ")" << std::endl;
			if (nsa.action != nullptr) log << "Semantic Action:" << nsa.name << std::endl;
		}
		if (nsa.action == nullptr) return nullptr;
		else return nsa.action(input, token_iter, *token_set);
	}

	template<typename Parser>
	void SyntaxDirected<Parser>::ErrorActionDispatcher(std::vector<Sym> expects, size_t token_iter)
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
						ss << " \'" << sym_table[item] << "\' ";
					Highlight(*(input_text), *(token_set), token_iter, ss.str(), log);
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
}