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
namespace MuCplGen
{
    struct Empty {};

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

    template<class UserToken, class T>
    struct Terminator
    {
        template<class Parser>
        friend class SyntaxDirected;
        friend class ParseRule;
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
    public:
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
                    << "Check your Semantic Action parameters!";
                throw(Exception(ss.str()));
            }
            auto arg = data[index];
            if (arg == nullptr)
            {
                //to fool the compiler
                auto e = Empty{};
                if (typeid(typename std::remove_reference<Arg>::type) == typeid(Empty))
                    return *reinterpret_cast<typename std::remove_reference<Arg>::type*>(&e);
                else
                {
                    std::string name = typeid(typename std::remove_reference<Arg>::type).name();
                    std::stringstream ss;
                    ss << "Unmatched Type!"
                        << "Parser Rule=" << this->action_name << std::endl
                        << "Production=" << this->expression << std::endl
                        << "Type of income data is <Empty>, your parameter=" << name << std::endl
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
                << "Production=" << this->expression << std::endl
                << "your para type=" << your_parameter_type_name
                << " income data type=" << data_type_name;
            throw(Exception(ss.str()));
        }
    private:
        static std::string scoped_on;

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

        //--{
//CodeGen by Python
//date: 2021-11-15
        template<class Ret, class Arg0>
        void SetAction(std::function<Ret(Arg0)> action)
        {
            semantic_action = [action, this](std::vector<std::any*> data)->std::any*
            {
                auto arg0 = GetArg<Arg0>(data, 0);

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
            };
        }
        template<class Ret, class Arg0, class Arg1>
        void SetAction(std::function<Ret(Arg0, Arg1)> action)
        {
            semantic_action = [action, this](std::vector<std::any*> data)->std::any*
            {
                auto&& arg0 = GetArg<Arg0>(data, 0);
                auto&& arg1 = GetArg<Arg1>(data, 1);

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2, arg3);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2, arg3, arg4);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2, arg3, arg4, arg5);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
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

                Ret ret;
                bool error = false;
                SemanticError se;
                try
                {
                    ret = action(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15);
                }
                catch (SemanticError e)
                {
                    error = true;
                    se = e;
                }
                if (!error) return RecordRet(ret, data);
                else return RecordRet(se.error_data, data);
            };
        }

        //--}



    };

    std::string ParseRule::scoped_on;
}