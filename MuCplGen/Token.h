#pragma once
#include <ostream>
#include <string>

namespace MuCplGen
{
	struct BaseToken
	{
		template<class T>
		friend class Scanner;

		std::string name;
		size_t line = 0;
		size_t start = 0;
		size_t end = 0;
		size_t length() { return end - start + 1; }
		constexpr size_t GetHash(const char* str) const
		{
			if (!*str)
				return 0;
			size_t hash = 5381;
			while (size_t ch = (size_t)*str++)
			{
				hash += (hash << 5) + ch;
				return hash;
			}
			return 0;
		}
		
		void Type(const std::string& type)
		{
			this->type_name = type;
			hash = GetHash(type.c_str());
		}

		const std::string& Type() const
		{
			return type_name;
		}

		virtual bool SameTypeAs(const BaseToken& r) const
		{
			if (hash == r.hash) return type_name == r.type_name;
			else return false;
		}

		bool IsEndToken() const { return end_of_tokens; }

		friend std::ostream& operator << (std::ostream& os, const BaseToken& token)
		{
			os << "basic_token:" << std::endl;
			os << "type_name:" << token.type_name << std::endl;
			os << "name:" << token.name << std::endl;
			os << "line:" << token.line << std::endl;
			os << "Start:" << token.start << std::endl;
			os << "end:" << token.end << std::endl;
			os << "is_end_token:" << token.end_of_tokens;
			return os;
		}
	protected:
		std::string type_name;
	private:
		bool end_of_tokens = false;
		size_t hash = 0;
	};
}
