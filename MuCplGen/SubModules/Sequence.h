#pragma once
#include "../SyntaxDirected.h"

namespace MuCplGen
{
	template<class T>
	class Sequence : public BaseSubModule
	{
		bool has_sep = false;
		std::string separator = "epsilon";
	public:
		void SetSeparator(const std::string& val) { separator = val; has_sep = true; }
		Sequence(BaseSyntaxDirected* bsd, const std::string& scope) 
			:BaseSubModule(bsd, scope) { }

		//Input:[Scope.]Comp<T or Empty>
		//Output:[Scope.]out_nonterm<std::vector<T> or size_t(count of comps)>
		virtual void CreateRules(const std::string& out_nonterm) override
		{
			bool empty = typeid(T) == typeid(Empty);
			{
				auto& p = CreateParseRule();
				p.action_name = "Finish Sequence";
				p.head = out_nonterm;
				p.body = { "Comps" };
			}
			
			{
				auto& p = CreateParseRule();
				p.head = "Sep";
				p.body = { separator };
			}

			{
				auto& p = CreateParseRule();
				p.action_name = "Collect Comp";
				p.expression = "Comps -> Comps Sep Comp";
				if (empty)
				{
					p.SetAction<PassOn, size_t&>(
						[this](size_t& v)->PassOn
						{
							++v;
							return PassOn(0);
						});
				}
				else
				{
					p.SetAction<PassOn, std::vector<T>&, Empty, T&>(
						[this](std::vector<T>& v, Empty, T& t)->PassOn
						{
							v.push_back(t);
							return PassOn(0);
						});
				}
			}

			{
				auto& p = CreateParseRule();
				p.action_name = "Get First Comp";
				p.expression = "Comps -> Comp";
				if (empty)
				{
					p.SetAction<size_t, Empty>(
						[this](Empty)->size_t
						{
							return size_t(1);
						});
				}
				else
				{
					p.SetAction<std::vector<T>, T&>(
						[this](T& t)->std::vector<T>
						{
							std::vector<T> ret;
							ret.push_back(t);
							return ret;
						});
				}
			}
		}
	};
}
