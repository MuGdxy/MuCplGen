#pragma once
#include <functional>
#include <stack>
#include <vector>
#include <type_traits>
#include <any>
#include "PushDownAutomaton.h"
#include "Token.h"
#include "DebugOption.h"

namespace MuCplGen
{
	using namespace Debug;
	enum class ParserErrorCode
	{
		Stop = 1,
		SemanticError = 2
	};

	template<class UserToken, typename T = size_t>
	class SLRParser
	{
		template<class Parser>
		friend class SyntaxDirected;
	public:
		using Token = UserToken;
		using SymbolType = T;
	private:
		using TokenSet = std::vector<Token>;
		using GotoTable = typename PushDownAutomaton<T>::GotoTable;
		using ActionTable = typename PushDownAutomaton<T>::ActionTable;
		using ProductionTable = typename PushDownAutomaton<T>::ProductionTable;
		using FollowTable = typename PushDownAutomaton<T>::FollowTable;
		using CollectionOfItemSets = typename PushDownAutomaton<T>::CollectionOfItemSets;
		using State = size_t;

		std::stack<State> state_stack;
		//std::stack<void*> semantic_stack;
		//std::stack<void*> help_stack;
		//std::vector<void*> pass;
		std::stack<std::any*> semantic_stack;
		std::stack<std::any*> help_stack;
		std::vector<std::any*> pass;

		CollectionOfItemSets item_collection;
		ProductionTable production_table;
		FollowTable follow_table;
		GotoTable goto_table;
		ActionTable action_table;
		size_t token_pointer;
		size_t debug_option;
	public:
		std::string information;

		SLRParser() : token_pointer(0) {}

		SLRParser(
			const ProductionTable& production_table,
			const T last_term, const T end_symbol,
			const T epsilon, const T first = (T)0);

		void Reset()
		{
			while (!state_stack.empty())
				state_stack.pop();
			state_stack.push(0);
		}

		void SetUp(const ProductionTable& production_table,
			const T last_term, const T end_symbol,
			const T epsilon, const T first)
		{
			this->production_table = production_table;
			auto first_table = PushDownAutomaton<T>::FIRST(
				production_table, epsilon, last_term, first);
			follow_table = PushDownAutomaton<T>::FOLLOW(
				first_table, production_table, epsilon, end_symbol, first);
			auto back = PushDownAutomaton<T>::COLLECTION(
				production_table, epsilon, end_symbol, first);
			item_collection = std::get<0>(back);
			goto_table = std::get<1>(back);
			action_table = PushDownAutomaton<T>::SetActionTable(
				production_table, item_collection, goto_table, follow_table,
				epsilon, end_symbol, first);
			state_stack.push(0);
		}
		using TransferFunc = std::function<T(const Token&)>;
		//using SemanticAction = std::function<void*(std::vector<void*>, size_t, size_t, size_t)>;
		using SemanticAction = std::function<std::any* (std::vector<std::any*>, size_t, size_t, size_t)>;
		using ErrorFunc = std::function<void(std::vector<T>, size_t)>;

		bool Parse(const TokenSet& token_set, TransferFunc transferFunc,
			SemanticAction semanticFunc, ErrorFunc errorFunc = nullptr, std::ostream& log = std::cout);
	};

	template<class UserToken, typename T>
	SLRParser<UserToken, T>::SLRParser(
		const ProductionTable& production_table,
		const T last_term, const T end_symbol,
		const T epsilon, const T first) :
		token_pointer(0),
		production_table(production_table)
	{
		static_assert(std::is_base_of_v<BaseToken, UserToken>::value, "Your Token should be drived from BaseToken");

		auto first_table = PushDownAutomaton<T>::FIRST(
			production_table, epsilon, last_term, first);
		follow_table = PushDownAutomaton<T>::FOLLOW(
			first_table, production_table, epsilon, end_symbol, first);
		auto back = PushDownAutomaton<T>::COLLECTION(
			production_table, epsilon, end_symbol, first);
		item_collection = std::get<0>(back);
		goto_table = std::get<1>(back);
		action_table = PushDownAutomaton<T>::SetActionTable(
			production_table, item_collection, goto_table, follow_table,
			epsilon, end_symbol, first);
		state_stack.push(0);
	}

	template<class UserToken, typename T>
	bool SLRParser<UserToken, T>::Parse(
		const TokenSet& token_set, TransferFunc transfer_func,
		SemanticAction semantic_action, ErrorFunc error_func, std::ostream& log)
	{
		auto len = token_set.size();
		size_t top_token_iter = 0;
		size_t iter = 0;
		bool on = true;
		bool acc = false;
		while (on)
		{
			if (iter >= token_set.size())
			{
				if (debug_option & DebugOption::ParserDetail)
					log << "tokens run out without an EndToken" << std::endl;
				break;
			}
			T input_term = transfer_func(token_set[iter]);
			if (state_stack.size() == 0) throw std::exception("Check if you call Initialize() in your constructor.");
			auto action_iter = action_table.find({ state_stack.top(),input_term });
			if (action_table.find({ state_stack.top(),input_term }) != action_table.end())
			{
				auto& action = std::get<1>(*action_iter);
				switch (action.type)
				{
				case ActionType::move_in:
					state_stack.push(action.aim_state);
					semantic_stack.push(nullptr);
					top_token_iter = iter;
					++iter;
					if (debug_option & DebugOption::ParserDetail)
						log << "move_in state:" << action.aim_state<< " term: " << action.sym << std::endl;
					break;
				case ActionType::move_epsilon:
					state_stack.push(action.aim_state);
					semantic_stack.push(nullptr);
					top_token_iter = iter;
					if (debug_option & DebugOption::ParserDetail)
					log << "move_epsilon state:" << action.aim_state << " term: " << action.sym << std::endl;
					break;
				case ActionType::reduce:
				{
					pass.clear();
					for (size_t i = 0; i < action.production_length; i++)
					{
						state_stack.pop();
						help_stack.push(semantic_stack.top());
						semantic_stack.pop();
					}
					for (size_t i = 0; i < action.production_length; i++)
					{
						pass.push_back(help_stack.top());
						help_stack.pop();
					}
					auto goto_state = goto_table[{state_stack.top(), action.sym}];
					state_stack.push(goto_state);
					if (debug_option & DebugOption::ParserDetail)
						log << "reduce: nonterm" << action.sym << "\n"
							<< "\tpop amount: " << action.production_length
							<< " push state: " << goto_state << std::endl;
					auto back = semantic_action(std::move(pass), (size_t)action.sym, action.production_index, top_token_iter);
					if (auto cast = std::any_cast<ParserErrorCode>(back))
					{
						switch (*cast)
						{
						case ParserErrorCode::Stop:
							if (debug_option & DebugOption::ParserError)
								log << information << ":SLRParse STOP for semantic Error" << std::endl;
							on = false;
							break;
						default:
							if (debug_option & DebugOption::ParserDetail)
								log << ":Parser Going On with Error Code" << std::endl;
							semantic_stack.push(back);
							break;
						}
					}
					else semantic_stack.push(back);
					break;
				}
				case ActionType::accept:
					pass.clear();
					pass.push_back(semantic_stack.top());
					semantic_stack.pop();
					on = false;
					semantic_stack.push(
						semantic_action(std::move(pass), (size_t)action.sym, action.production_index, top_token_iter)
					);
					if (debug_option & DebugOption::ParserDetail)
					{
						SetConsoleColor(ConsoleForegroundColor::enmCFC_Yellow, ConsoleBackGroundColor::enmCBC_Blue);
						log << information << ":SLRParser Accept" << std::endl;
						SetConsoleColor();
					}
					acc = true;
					break;
				default:
					assert("Fatal Error!");
					break;
				}
			}
			else if (error_func)
			{
				if (debug_option & DebugOption::ParserError)
					log << information << ":SLRParser Error" << std::endl;
				std::vector<T> expects;
				for (const auto& item : action_table)
					if (std::get<0>(item.first) == state_stack.top())
						expects.push_back(std::get<1>(item.first));
				error_func(std::move(expects), iter);
				break;
			}
		}
		return acc;
	}

	template<class UserToken, typename T = size_t>
	class LR1Parser
	{
		template<class Parser>
		friend class SyntaxDirected;
	public:
		using Token = UserToken;
		using SymbolType = T;
	private:
		using TokenSet = std::vector<Token>;
		using GotoTable = typename PushDownAutomaton<T>::GotoTable;
		using ActionTable = typename PushDownAutomaton<T>::ActionTable;
		using ProductionTable = typename PushDownAutomaton<T>::ProductionTable;
		using FollowTable = typename PushDownAutomaton<T>::FollowTable;
		using LR1Collection = typename PushDownAutomaton<T>::LR1Collection;
		using State = size_t;

		std::stack<State> state_stack;
		std::stack<std::any*> semantic_stack;
		std::stack<std::any*> help_stack;
		std::vector<std::any*> pass;

		ProductionTable production_table;
		FollowTable follow_table;
		LR1Collection item_collection;
		GotoTable goto_table;
		ActionTable action_table;
		size_t token_pointer;
		size_t debug_option;
	public:
		std::string information;

		LR1Parser() : token_pointer(0) {}

		LR1Parser(
			const ProductionTable& production_table,
			const T last_term, const T end_symbol,
			const T epsilon, const T first = (T)0);

		~LR1Parser() {}

		void Reset()
		{
			while (!state_stack.empty())
				state_stack.pop();
			state_stack.push(0);
		}

		void SetUp(
			const ProductionTable& production_table,
			const T last_term, const T end_symbol,
			const T epsilon, const T first)
		{
			this->production_table = production_table;
			auto first_table = PushDownAutomaton<T>::FIRST(
				production_table, epsilon, last_term, first);
			follow_table = PushDownAutomaton<T>::FOLLOW(
				first_table, production_table, epsilon, end_symbol, first);
			auto back = PushDownAutomaton<T>::COLLECTION_LR(
				production_table, first_table, epsilon, end_symbol, first);
			item_collection = std::get<0>(back);
			goto_table = std::get<1>(back);
			action_table = PushDownAutomaton<T>::SetActionTable(
				production_table, item_collection, goto_table, follow_table,
				epsilon, end_symbol, first);
			state_stack.push(0);
		}

		using TransferFunc = std::function<T(const Token&)>;
		using SemanticAction = std::function<std::any* (std::vector<std::any*>, size_t, size_t, size_t)>;
		using ErrorFunc = std::function<void(std::vector<T>, size_t)>;

		bool Parse(
			const TokenSet& token_set, TransferFunc transfer_func,
			SemanticAction semantic_action, ErrorFunc error_func, std::ostream& log = std::cout);
	};

	template<class UserToken, typename T>
	LR1Parser<UserToken, T>::LR1Parser(
		const ProductionTable& production_table,
		const T last_term, const T end_symbol,
		const T epsilon, const T first) :
		token_pointer(0),
		production_table(production_table)
	{
		static_assert(std::is_base_of_v<BaseToken, UserToken>::value, "Your Token should be drived from BaseToken");
		auto first_table = PushDownAutomaton<T>::FIRST(
			production_table, epsilon, last_term, first);
		follow_table = PushDownAutomaton<T>::FOLLOW(
			first_table, production_table, epsilon, end_symbol, first);
		auto back = PushDownAutomaton<T>::COLLECTION_LR(
			production_table, first_table, epsilon, end_symbol, first);
		item_collection = std::get<0>(back);
		goto_table = std::get<1>(back);
		action_table = PushDownAutomaton<T>::SetActionTable(
			production_table, item_collection, goto_table, follow_table,
			epsilon, end_symbol, first);
		state_stack.push(0);
	}

	template<class UserToken, typename T>
	bool LR1Parser<UserToken, T>::Parse(
		const TokenSet& token_set, TransferFunc transfer_func,
		SemanticAction semantic_action, ErrorFunc error_func, std::ostream& log)
	{
		auto len = token_set.size();
		size_t top_token_iter = 0;
		size_t iter = 0;
		bool on = true;
		bool acc = false;
		while (on)
		{
			if (iter >= token_set.size())
			{
				if (debug_option & DebugOption::ParserDetail)
					log << "tokens run out without an EndToken" << std::endl;
				break;
			}
			T input_term = transfer_func(token_set[iter]);
			if (state_stack.size() == 0) throw std::exception("Check if you call Initialize() in your constructor.");
			auto action_iter = action_table.find({ state_stack.top(),input_term });
			if (action_table.find({ state_stack.top(),input_term }) != action_table.end())
			{
				auto& action = std::get<1>(*action_iter);
				switch (action.type)
				{
				case ActionType::move_in:
					state_stack.push(action.aim_state);
					semantic_stack.push(nullptr);
					top_token_iter = iter;
					++iter;
					if (debug_option & DebugOption::ParserDetail)
						log << "move_in state:" << action.aim_state
							<< " term: " << action.sym << std::endl;
					break;
				case ActionType::move_epsilon:
					state_stack.push(action.aim_state);
					semantic_stack.push(nullptr);
					top_token_iter = iter;
					if (debug_option & DebugOption::ParserDetail)
						log << "move_epsilon state:" << action.aim_state
							<< " term: " << action.sym << std::endl;
					break;
				case ActionType::reduce:
				{
					pass.clear();
					for (size_t i = 0; i < action.production_length; i++)
					{
						state_stack.pop();
						help_stack.push(semantic_stack.top());
						semantic_stack.pop();
					}
					for (size_t i = 0; i < action.production_length; i++)
					{
						pass.push_back(help_stack.top());
						help_stack.pop();
					}
					auto goto_state = goto_table[{state_stack.top(), action.sym}];
					state_stack.push(goto_state);
					if (debug_option & DebugOption::ParserDetail)
						log << "reduce: nonterm" << action.sym << "\n"
							<< "\tpop amount: " << action.production_length
							<< " push state: " << goto_state << std::endl;
					auto back = semantic_action(std::move(pass), (size_t)action.sym, action.production_index, top_token_iter);
					if (auto cast = std::any_cast<ParserErrorCode>(back))
					{
						switch (*cast)
						{
						case ParserErrorCode::Stop:
							if (debug_option & DebugOption::ParserError)
								log << information << ":LR1Parse STOP for semantic Error" << std::endl;
							on = false;
							break;
						default:
							if (debug_option & DebugOption::ParserDetail)
								log << ":Parser Going On with Error Code" << std::endl;
							semantic_stack.push(back);
							break;
						}
					}
					else semantic_stack.push(back);
					break;
				}
				case ActionType::accept:
					pass.clear();
					pass.push_back(semantic_stack.top());
					semantic_stack.pop();
					on = false;
					semantic_stack.push(
						semantic_action(std::move(pass), (size_t)action.sym, action.production_index, top_token_iter)
					);
					if (debug_option & DebugOption::ParserError)
					{
						SetConsoleColor(ConsoleForegroundColor::enmCFC_Yellow, ConsoleBackGroundColor::enmCBC_Blue);
						log << information << ":LR1Parser Accept" << std::endl;
						SetConsoleColor();
					}
					acc = true;
					break;
				default:
					assert("Fatal Error!");
					break;
				}
			}
			else if (error_func)
			{
				if (debug_option & DebugOption::ParserError)
					log << information << ":LR1Parser Error" << std::endl;
				std::vector<T> expects;
				for (const auto& item : action_table)
					if (std::get<0>(item.first) == state_stack.top())
						expects.push_back(std::get<1>(item.first));
				error_func(std::move(expects), iter);
				break;
			}
		}
		return acc;
	}
}
