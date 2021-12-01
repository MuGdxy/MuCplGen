#pragma once
#include "../SyntaxDirected.h"

namespace MuCplGen
{
	template<class T>
	class Sequence : public BaseSubModule
	{
		bool has_sep = false;
		std::string separator;
	public:
		void SetSeparator(const std::string& val) { separator = val; has_sep = true; }
		Sequence(BaseSyntaxDirected* bsd, const std::string& scope) 
			:BaseSubModule(bsd, scope) { }

		//Input:[Scope.]Comp<T>
		//Output:[Scope.]out_nonterm<std::vector<T>>
		virtual void CreateRules(const std::string& out_nonterm) override
		{
			{
				auto& p = CreateParseRule();
				p.action_name = "Finish Sequence";
				p.head = out_nonterm;
				p.body = { "Comps" };
			}

			{
				auto& p = CreateParseRule();
				p.action_name = "Collect Comp";
				p.head = "Comps";
				if (has_sep) p.body = { "Comps", separator, "Comp" };
				else p.body = { "Comps", "Comp" };
				p.SetAction<PassOn, std::vector<T>&, T&>(
					[this](std::vector<T>& v,T& t)->PassOn
					{
						v.push_back(t);
						return PassOn(0);
					});
			}

			{
				auto& p = CreateParseRule();
				p.action_name = "Get First Comp";
				p.expression = "Comps -> Comp";
				p.SetAction<std::vector<T>, T&>(
					[this](T& t)->std::vector<T>
					{
						std::vector<T> ret;
						ret.push_back(t);
						return ret;
					});
			}
		}
	};
}
