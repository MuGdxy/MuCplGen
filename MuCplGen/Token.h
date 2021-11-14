#pragma once
#include <string>
#include "../ColoredText.h"

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
			register size_t hash = 5381;
			while (size_t ch = (size_t)*str++)
			{
				hash += (hash << 5) + ch;
				return hash;
			}
		}
		
		void Type(const std::string& type)
		{
			this->type = type;
			hash = GetHash(type.c_str());
		}

		const std::string& Type() const
		{
			return type;
		}

		virtual bool SameTypeAs(const BaseToken& r) const
		{
			if (hash == r.hash) return type == r.type;
			else return false;
		}

		bool IsEndToken() const { return end_of_tokens; }
	protected:
		std::string type;	
	private:
		bool end_of_tokens = false;
		size_t hash = 0;
	};

	struct EasyToken : public BaseToken
	{
		enum class TokenType
		{
			none,
			rel_op,
			arith_op,
			log_op,
			number,
			identifier,
			assign,
			keyword,
			separator,
			raw_string,
			custom_type
		};
		TokenType type = TokenType::none;

		ConsoleForegroundColor color = ConsoleForegroundColor::enmCFC_White;
	};
}
