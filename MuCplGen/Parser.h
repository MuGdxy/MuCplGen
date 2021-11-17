#pragma once
#include <functional>
#include <stack>
#include <vector>
#include <type_traits>
#include <any>
#include "Platform.h"
#include "FileSystem.h"
#include "PushDownAutomaton.h"
#include "Token.h"
#include "DebugTool/DebugOption.h"
#include "MuException.h"

namespace MuCplGen
{
	using namespace Debug;
	enum class ParserErrorCode
	{
		Stop = 1,
		SemanticError = 2
	};

	template<class UserToken, typename T = size_t>
	class BaseParser
	{
		template<class Parser>
		friend class SyntaxDirected;
	public:
		using Token = UserToken;
		using SymbolType = T;
		using TokenSet = std::vector<Token>;
	protected:
		using GotoTable = typename PushDownAutomaton<T>::GotoTable;
		using ActionTable = typename PushDownAutomaton<T>::ActionTable;
		using Action = typename PushDownAutomaton<T>::Action;
		using ProductionTable = typename PushDownAutomaton<T>::ProductionTable;
		using FollowTable = typename PushDownAutomaton<T>::FollowTable;
		using State = size_t;
		GotoTable goto_table;
		ActionTable action_table;
		
		
		size_t token_pointer = 0;
		size_t debug_option = 0;
		std::stack<State> state_stack;
		std::stack<std::any*> semantic_stack;
		std::stack<std::any*> help_stack;
		std::vector<std::any*> pass;
	public:
		std::string information;
		std::string parser_name;

		virtual void SetUp(const ProductionTable& production_table,
			const T last_term, const T end_symbol,
			const T epsilon, const T first) = 0;
		
		void Reset()
		{
			token_pointer = 0;
			while (!state_stack.empty())
				state_stack.pop();
			state_stack.push(0);
		}

		using TransferFunc = std::function<T(const Token&)>;
		using SemanticAction = std::function<std::any* (std::vector<std::any*>, size_t, size_t, size_t)>;
		using ErrorFunc = std::function<void(std::vector<T>, size_t)>;

		virtual bool Parse(const BaseParser::TokenSet& token_set, BaseParser::TransferFunc transfer_func,
			SemanticAction semantic_action, ErrorFunc error_func = nullptr, std::ostream& log = std::cout)
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
				if (state_stack.size() == 0) throw Exception("Check if you call Initialize() in your constructor.");
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
							log << "move_in state:" << action.aim_state << " term: " << action.sym << std::endl;
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
						SortStack(action);
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
									log << information << ":Parse STOP for semantic Error" << std::endl;
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
						SortStack(action);
						on = false;
						semantic_stack.push(
							semantic_action(std::move(pass), (size_t)action.sym, action.production_index, top_token_iter)
						);
						if (debug_option & DebugOption::ParserDetail)
						{
							SetConsoleColor(ConsoleForegroundColor::Yellow, ConsoleBackgroundColor::Blue);
							log << information << ":Parser Accept";
							SetConsoleColor();
							log << std::endl;
							
						}
						acc = true;
						break;
					default:
						assert(0);
						break;
					}
				}
				else if (error_func)
				{
					if (debug_option & DebugOption::ParserError)
						log << information << ":Parser Error" << std::endl;
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

		MU_NOINLINE
		void SortStack(Action& action)
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
		}

		virtual void Save(const std::string& path)
		{
			FileSystem::path p(path);
			auto dir = p.parent_path();
			if(!FileSystem::exists(dir)) FileSystem::create_directory(dir);
			std::ofstream o(path, std::ios::binary);
			o << action_table << goto_table;
		}

		virtual bool Load(const std::string& path)
		{
			auto error = true;
			if(FileSystem::exists(path))
			{
				try
				{
					std::ifstream i(path, std::ios::binary);
					i >> action_table >> goto_table;
					error = false;
				}
				catch (std::exception e)
				{
					error = true;
				}
			}			
			return !error;
		}
	};


	template<class UserToken, typename T = size_t>
	class SLRParser : public BaseParser<UserToken, T>
	{
	private:
		using Base = BaseParser<UserToken, T>;
		using CollectionOfItemSets = typename PushDownAutomaton<T>::CollectionOfItemSets;
	public:
		SLRParser() { this->parser_name = "SLR"; }

		SLRParser(
			const typename Base::ProductionTable& production_table,
			const T last_term, const T end_symbol,
			const T epsilon, const T first = (T)0)
			: SLRParser()
		{
			static_assert(std::is_base_of_v<BaseToken, UserToken>, "Your Token should be drived from BaseToken");
			SetUp(production_table, last_term, end_symbol, epsilon, first);
		}

		MU_NOINLINE
		void SetUp(const typename Base::ProductionTable& production_table,
			const T last_term, const T end_symbol,
			const T epsilon, const T first) override
		{
			auto first_table = PushDownAutomaton<T>::FIRST(
				production_table, epsilon, last_term, first);
			auto follow_table = PushDownAutomaton<T>::FOLLOW(
				first_table, production_table, epsilon, end_symbol, first);
			auto back = PushDownAutomaton<T>::COLLECTION(
				production_table, epsilon, end_symbol, first);
			auto item_collection = std::get<0>(back);
			this->goto_table = std::get<1>(back);
			this->action_table = PushDownAutomaton<T>::SetActionTable(
				production_table, item_collection, this->goto_table, follow_table,
				epsilon, end_symbol, first);
		}
	};

	template<class UserToken, typename T = size_t>
	class LR1Parser : public BaseParser<UserToken, T>
	{
	private:
		using Base = BaseParser<UserToken, T>;
		using LR1Collection = typename PushDownAutomaton<T>::LR1Collection;
	public:
		LR1Parser() { this->parser_name = "LR1"; }

		LR1Parser(
			const typename Base::ProductionTable& production_table,
			const T last_term, const T end_symbol,
			const T epsilon, const T first = (T)0)
			:LR1Parser()
		{
			static_assert(std::is_base_of_v<BaseToken, UserToken>, "Your Token should be drived from BaseToken");
			SetUp(production_table, last_term, end_symbol, epsilon, first);
		}

		~LR1Parser() {}

		MU_NOINLINE
		void SetUp(
			const typename Base::ProductionTable& production_table,
			const T last_term, const T end_symbol,
			const T epsilon, const T first) override
		{
			auto first_table = PushDownAutomaton<T>::FIRST(
				production_table, epsilon, last_term, first);
			auto follow_table = PushDownAutomaton<T>::FOLLOW(
				first_table, production_table, epsilon, end_symbol, first);
			auto back = PushDownAutomaton<T>::COLLECTION_LR(
				production_table, first_table, epsilon, end_symbol, first);
			auto item_collection = std::get<0>(back);
			this->goto_table = std::get<1>(back);
			this->action_table = PushDownAutomaton<T>::SetActionTable(
				production_table, item_collection, this->goto_table, follow_table,
				epsilon, end_symbol, first);
		}
	};
}
