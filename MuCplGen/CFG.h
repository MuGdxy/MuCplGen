#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <any>
#include <functional>
#include <stack>
#include <type_traits>
#include "Parser.h"
#include "MuException.h"
#include "FunctionDeduce.h"

namespace MuCplGen
{
	struct Empty {};

	template<class T = void>
	class StaticVar
	{
		friend struct ParseRule;
		static Empty empty;
	};

	template<class T>
	Empty StaticVar<T>::empty;

	struct PassOn
	{
		PassOn(int i = 0) : index(i) {}
		int index;
	};

	struct SemanticError : public Exception
	{
		SemanticError() : Exception("SemanticError") {};
		SemanticError(ParserErrorCode code) :Exception("SemanticError")
		{
			error_data.code = code;
		}
		ParserErrorData error_data;
		virtual ~SemanticError() override {}
	};

	template<class UserToken, class T = size_t>
	struct Terminator
	{
		template<class Parser>
		friend class SyntaxDirected;
		friend struct ParseRule;
		using Token = UserToken;
		std::string scope;
		std::string name;
		int priority = 0;
		std::function<bool(const Token&)> translation;
		std::string ScopedName(const std::string& name)
		{
			if (scope.empty()) return name;
			else if (name.find(".") == -1) return scope + "." + name;
			else return name;
		}
	private:
		T sym;
	};

	struct ParseRule
	{
		template<class Parser>
		friend class SyntaxDirected;
		std::string action_name;
		std::string expression;
		std::string scope;
		//std::vector<std::string> scopes;
		using SemanticAction = std::function<std::any* (std::vector<std::any*>)>;
		std::string head;
		std::vector<std::string> body;
		std::string fullname_expression;

		std::string ScopedName(const std::string& name)
		{
			if (scope.empty()) return name;
			else if (name.find(".") == -1) return scope + "." + name;
			else return name;
		}

		//std::string ScopedName(const std::string& name)
		//{
		//    if(scopes.empty()) return name;
		//    else if (name.find(".") == -1)
		//    {
		//        std::stringstream ss;
		//        for (auto& scope : scopes) ss << scope << ".";
		//        ss << name;
		//        return ss.str();
		//    }
		//    else return name;
		//}

		//void PushScope(const std::string& scope) { scopes.push_back(scope); }
		void ParseExpression()
		{
			if (!head.empty())
			{
				std::stringstream ss;
				ss << head << " ";
				ss << "-> ";
				for (auto& b : body) ss << b << " ";
				expression = ss.str();
				return;
			}
			std::stringstream ss(expression);
			std::string buf;
			ss >> buf;
			head = ScopedName(buf);
			ss >> buf;
			if (buf != "->") throw(Exception("Error Syntax"));
			buf.clear();
			while (true)
			{
				buf.clear();
				ss >> buf;
				if (buf.empty()) break;
				body.push_back(buf);
			}
		}

		template<class Ret>
		void SetSemanticErrorAction(std::function<Ret(const std::vector<std::any*>&)> on_error)
		{
			semantic_error = [on_error, this](std::vector<std::any*> data)->std::any*
			{
				return RecordRet(on_error(data), data);
			};
		}

		template<class Arg>
		Arg GetArg(const std::vector<std::any*>& data, int index)
		{
			if (index >= data.size())
			{
				std::stringstream ss;
				ss << "Out of data range!"
					<< "Parser Rule=" << this->action_name << std::endl
					<< "Production=" << this->expression << std::endl
					<< "Check your Semantic ActionSetter parameters!";
				throw(Exception(ss.str()));
			}
			auto arg = data[index];
			if (arg == nullptr)
			{
				//to fool the compiler
				if (typeid(typename std::remove_reference<Arg>::type) == typeid(Empty))
					return *reinterpret_cast<typename std::remove_reference<Arg>::type*>(&StaticVar<void>::empty);
				else
				{
					std::string name = typeid(typename std::remove_reference<Arg>::type).name();
					std::stringstream ss;
					ss << "Unmatched Type!"
						<< "Parser Rule=" << this->action_name << std::endl
						<< "Production=" << this->fullname_expression << std::endl
						<< "Type of income data is <Empty>, your parameter=" << name << "(Arg" << index << ")" << std::endl
						<< "Check your Semantic Action parameters!";
					throw(Exception(ss.str()));
				}
			}
			else
			{
				if (typeid(Arg) == data[index]->type()) return std::any_cast<Arg>(*data[index]);
			}
			std::string your_parameter_type_name = typeid(Arg).name();
			std::string data_type_name = data[index]->type().name();
			std::stringstream ss;
			ss << "Unmatched Type! Check your Semantic Action of this Parser Rule!" << std::endl
				<< "Parser Rule=" << this->action_name << std::endl
				<< "Production=" << this->fullname_expression << std::endl
				<< "your para type=" << your_parameter_type_name << "(Arg" << index << ")" << std::endl
				<< " income data type=" << data_type_name << std::endl
				<< "Check your Semantic Action parameters!";
			throw(Exception(ss.str()));
		}
	private:
		int current_arg = -1;
		void BeginArgs(const std::vector<std::any*>& data) { current_arg = data.size(); }

		template<class Arg>
		Arg GetArg(const std::vector<std::any*>& data)
		{
			return GetArg<Arg>(data, --current_arg);
		}
		template<class Ret>
		std::any* RecordRet(Ret&& ret, const std::vector<std::any*>& data)
		{
			if (std::is_null_pointer<Ret>::value) return nullptr;
			if (typeid(PassOn) == typeid(Ret)) return data[reinterpret_cast<PassOn*>(&ret)->index];
			auto* o = new std::any;
			o->emplace<Ret>(std::forward<Ret>(ret));
			gc.push(o);
			return o;
		}

		std::stack<std::any*> gc;
		SemanticAction semantic_action = [this](std::vector<std::any*> data) { return data[0]; };
		SemanticAction semantic_error;

	public:
		~ParseRule()
		{
			while (!gc.empty())
			{
				delete gc.top();
				gc.pop();
			}
		}

		template <class F, class Op = decltype(&F::operator())>
		void SetAction(F action)
		{
			ActionSetter<FxDeduce::deduce_function_t<Op>>::Set(this, action);
		}

		void SetAction(void* ptr)
		{
			if (ptr != nullptr) throw(Exception("ptr must be nullptr"));
			semantic_action = nullptr;
		}

		void SetAction(const PassOn& passon)
		{
			int index = passon.index;
			semantic_action = [this, index = passon.index](std::vector<std::any*> data) { return data[index]; };
		}

		template<class T>
		class ActionSetter;

		template<class Ret, class... Args>
		class ActionSetter<Ret(Args...)>
		{
		public:
			template<class T>
			static void Set(ParseRule* pr, T&& action)
			{
				pr->semantic_action = [action, pr](std::vector<std::any*> data)->std::any*
				{
					pr->BeginArgs(data);
					static_assert(!std::is_same_v<Ret,void>, 
						"Your Semantic Action has no return, "
						"return Empty{},if you have nothing to pass on");
					Ret ret;
					bool error = false;
					SemanticError se;
					try
					{
						ret = action(pr->GetArg<Args>(data)...);
					}
					catch (SemanticError e)
					{
						error = true;
						se = e;
					}
					if (!error) return pr->RecordRet(ret, data);
					else return pr->RecordRet(se.error_data, data);
				};
			}
		};
	};
}