#pragma once
#include <vector>
#include <string>
#include <any>
#include <functional>
#include <stack>
#include <type_traits>
namespace MuCplGen
{
    struct Empty {};

    struct PassOn
    {
        PassOn(int i) : index(i){}
        int index;
    };
	template<class UserToken>
	struct Production
	{
		using Token = UserToken;
		using TokenSet = std::vector<Token>;
		std::string action_name;
		std::string expression;
		size_t token_iter;
		TokenSet* token_set;
		std::function<std::any* (std::vector<std::any*>)> semantic_action;


		template<class Arg>
		static Arg GetArg(const std::vector<std::any*>& data, int index)
		{
			return std::any_cast<Arg>(*data[index]);
		}

		template<class Ret>
		std::any* RecordRet(Ret&& ret, std::vector<std::any*> data)
		{
            if (std::is_null_pointer<Ret>::value) return nullptr;
            if (typeid(PassOn) == typeid(Ret)) return data[dynamic_cast<PassOn*>(&ret)->index];
			auto* o = new std::any;
			o->emplace<Ret>(std::forward<Ret>(ret));
			gc.push(o);
            return o;
		}

		std::stack<std::any*> gc;
		~Production()
		{
			while (!gc.empty())
			{
				delete gc.top();
				gc.pop();
			}
		}
        
        //--{
//CodeGen by Python
//date: 2021-11-15
template<class Ret, class Arg0>
void SetAction(std::function<Ret(Arg0)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);

        return RecordRet(action(arg0), data);
    };
}
template<class Ret, class Arg0, class Arg1>
void SetAction(std::function<Ret(Arg0, Arg1)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);

        return RecordRet(action(arg0, arg1), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);

        return RecordRet(action(arg0, arg1, arg2), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2, class Arg3>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2, Arg3)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);
auto&& arg3 = GetArg<Arg3>(data, 3);

        return RecordRet(action(arg0, arg1, arg2, arg3), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2, Arg3, Arg4)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);
auto&& arg3 = GetArg<Arg3>(data, 3);
auto&& arg4 = GetArg<Arg4>(data, 4);

        return RecordRet(action(arg0, arg1, arg2, arg3, arg4), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);
auto&& arg3 = GetArg<Arg3>(data, 3);
auto&& arg4 = GetArg<Arg4>(data, 4);
auto&& arg5 = GetArg<Arg5>(data, 5);

        return RecordRet(action(arg0, arg1, arg2, arg3, arg4, arg5), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);
auto&& arg3 = GetArg<Arg3>(data, 3);
auto&& arg4 = GetArg<Arg4>(data, 4);
auto&& arg5 = GetArg<Arg5>(data, 5);
auto&& arg6 = GetArg<Arg6>(data, 6);

        return RecordRet(action(arg0, arg1, arg2, arg3, arg4, arg5, arg6), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);
auto&& arg3 = GetArg<Arg3>(data, 3);
auto&& arg4 = GetArg<Arg4>(data, 4);
auto&& arg5 = GetArg<Arg5>(data, 5);
auto&& arg6 = GetArg<Arg6>(data, 6);
auto&& arg7 = GetArg<Arg7>(data, 7);

        return RecordRet(action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);
auto&& arg3 = GetArg<Arg3>(data, 3);
auto&& arg4 = GetArg<Arg4>(data, 4);
auto&& arg5 = GetArg<Arg5>(data, 5);
auto&& arg6 = GetArg<Arg6>(data, 6);
auto&& arg7 = GetArg<Arg7>(data, 7);
auto&& arg8 = GetArg<Arg8>(data, 8);

        return RecordRet(action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);
auto&& arg3 = GetArg<Arg3>(data, 3);
auto&& arg4 = GetArg<Arg4>(data, 4);
auto&& arg5 = GetArg<Arg5>(data, 5);
auto&& arg6 = GetArg<Arg6>(data, 6);
auto&& arg7 = GetArg<Arg7>(data, 7);
auto&& arg8 = GetArg<Arg8>(data, 8);
auto&& arg9 = GetArg<Arg9>(data, 9);

        return RecordRet(action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);
auto&& arg3 = GetArg<Arg3>(data, 3);
auto&& arg4 = GetArg<Arg4>(data, 4);
auto&& arg5 = GetArg<Arg5>(data, 5);
auto&& arg6 = GetArg<Arg6>(data, 6);
auto&& arg7 = GetArg<Arg7>(data, 7);
auto&& arg8 = GetArg<Arg8>(data, 8);
auto&& arg9 = GetArg<Arg9>(data, 9);
auto&& arg10 = GetArg<Arg10>(data, 10);

        return RecordRet(action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);
auto&& arg3 = GetArg<Arg3>(data, 3);
auto&& arg4 = GetArg<Arg4>(data, 4);
auto&& arg5 = GetArg<Arg5>(data, 5);
auto&& arg6 = GetArg<Arg6>(data, 6);
auto&& arg7 = GetArg<Arg7>(data, 7);
auto&& arg8 = GetArg<Arg8>(data, 8);
auto&& arg9 = GetArg<Arg9>(data, 9);
auto&& arg10 = GetArg<Arg10>(data, 10);
auto&& arg11 = GetArg<Arg11>(data, 11);

        return RecordRet(action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);
auto&& arg3 = GetArg<Arg3>(data, 3);
auto&& arg4 = GetArg<Arg4>(data, 4);
auto&& arg5 = GetArg<Arg5>(data, 5);
auto&& arg6 = GetArg<Arg6>(data, 6);
auto&& arg7 = GetArg<Arg7>(data, 7);
auto&& arg8 = GetArg<Arg8>(data, 8);
auto&& arg9 = GetArg<Arg9>(data, 9);
auto&& arg10 = GetArg<Arg10>(data, 10);
auto&& arg11 = GetArg<Arg11>(data, 11);
auto&& arg12 = GetArg<Arg12>(data, 12);

        return RecordRet(action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);
auto&& arg3 = GetArg<Arg3>(data, 3);
auto&& arg4 = GetArg<Arg4>(data, 4);
auto&& arg5 = GetArg<Arg5>(data, 5);
auto&& arg6 = GetArg<Arg6>(data, 6);
auto&& arg7 = GetArg<Arg7>(data, 7);
auto&& arg8 = GetArg<Arg8>(data, 8);
auto&& arg9 = GetArg<Arg9>(data, 9);
auto&& arg10 = GetArg<Arg10>(data, 10);
auto&& arg11 = GetArg<Arg11>(data, 11);
auto&& arg12 = GetArg<Arg12>(data, 12);
auto&& arg13 = GetArg<Arg13>(data, 13);

        return RecordRet(action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);
auto&& arg3 = GetArg<Arg3>(data, 3);
auto&& arg4 = GetArg<Arg4>(data, 4);
auto&& arg5 = GetArg<Arg5>(data, 5);
auto&& arg6 = GetArg<Arg6>(data, 6);
auto&& arg7 = GetArg<Arg7>(data, 7);
auto&& arg8 = GetArg<Arg8>(data, 8);
auto&& arg9 = GetArg<Arg9>(data, 9);
auto&& arg10 = GetArg<Arg10>(data, 10);
auto&& arg11 = GetArg<Arg11>(data, 11);
auto&& arg12 = GetArg<Arg12>(data, 12);
auto&& arg13 = GetArg<Arg13>(data, 13);
auto&& arg14 = GetArg<Arg14>(data, 14);

        return RecordRet(action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14), data);
    };
}
template<class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15>
void SetAction(std::function<Ret(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        auto&& arg0 = GetArg<Arg0>(data, 0);
auto&& arg1 = GetArg<Arg1>(data, 1);
auto&& arg2 = GetArg<Arg2>(data, 2);
auto&& arg3 = GetArg<Arg3>(data, 3);
auto&& arg4 = GetArg<Arg4>(data, 4);
auto&& arg5 = GetArg<Arg5>(data, 5);
auto&& arg6 = GetArg<Arg6>(data, 6);
auto&& arg7 = GetArg<Arg7>(data, 7);
auto&& arg8 = GetArg<Arg8>(data, 8);
auto&& arg9 = GetArg<Arg9>(data, 9);
auto&& arg10 = GetArg<Arg10>(data, 10);
auto&& arg11 = GetArg<Arg11>(data, 11);
auto&& arg12 = GetArg<Arg12>(data, 12);
auto&& arg13 = GetArg<Arg13>(data, 13);
auto&& arg14 = GetArg<Arg14>(data, 14);
auto&& arg15 = GetArg<Arg15>(data, 15);

        return RecordRet(action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15), data);
    };
}

//--}

	};
}