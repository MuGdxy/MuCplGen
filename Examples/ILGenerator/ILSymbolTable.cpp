#include <iostream>
#include "ILSymbolTable.h"

size_t ILEntry::tabs = 0;

std::ostream& operator<<(std::ostream& out, const ILEntry& entry)
{
	ILEntry::ShowAllEntry(out,&entry);
	return out;
}

std::ostream& operator<<(std::ostream& out, const Address& addr)
{
	//if is temp addr
	if (addr.chain.size() == 0)
	{
		if (addr.token)//ImVal
		{
			out << "#" << addr.token->name;
		}
		else// Tmp
		{
			if (addr.code.size())
				out << "#" << addr.code;
			else
				out << "t" << addr.id;
		}
	}
	else
	{
		bool first = true;
		size_t i = 0;
		for (auto entry : addr.chain)
		{
			if (!first)
				out << ".";
			else
				first = false;
			out << entry->token->name;
			if (entry->array_info.size())
				out << "[" << *addr.array_index[i++]<<"]";
		}
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const ILCode& code)
{
	if (code.res)
		out << *code.res << " <= ";
	out << code.op << " ";
	if (code.left)
		out << *code.left;
	if (code.right)
		out <<", "<<*code.right;
	return out;
}

ILEntry* ILEnv::CreateVarFromCType(ILEntry* src)
{
	ILEntry* ret = CreateILEntry();
	ret->meta_type = src->token->name;
	ret->width = src->width;
	if (is_typedef)
	{
		ret->table_ptr = src->table_ptr;
		ret->independent = false;
		return ret;
	}
	if (src->table_ptr)
	{
		ret->table_ptr = new ILSymbolTable();
		for (auto entry : *src->table_ptr)
			ret->table_ptr->push_back(Copy(entry));
	}
	return ret;
}

ILEntry* ILEnv::Copy(const ILEntry* current)
{
	ILEntry* ret = CreateILEntry();
	ret->meta_type = current->meta_type;
	ret->token = current->token;
	ret->value = current->value;
	ret->offset = current->offset;	
	ret->width = current->width;
	ret->array_info = current->array_info;
	if (current->table_ptr)
	{
		ret->table_ptr = new ILSymbolTable();
		for (auto entry : *current->table_ptr)
			ret->table_ptr->push_back(Copy(entry));
	}
	return ret;
}

void ILEntry::ShowRawEntry(std::ostream& out,const ILEntry* to_show)
{
	out << "MetaType<" << to_show->meta_type;
	for (const auto& item : to_show->array_info)
		out << "[" << item << "]";
	out << "> ";
	if(to_show->token)
		out << "NameValPair<" << to_show->token->name << ",";
	else
		out << "Val<";
	if (to_show->value.size())
		out << to_show->value << ">";
	else
		out << "Null>";
	out << "Width[" << to_show->width << "] ";
	out << "Offset[" << to_show->offset << "] ";
}

void ILEntry::ShowAllEntry(std::ostream& out,const ILEntry* current)
{
	ShowRawEntry(out,current);
	if (current->table_ptr && current->independent)
	{
		++tabs;
		for (size_t i = 0; i < (*current->table_ptr).size(); i++)
		{
			out << std::endl;
			for (size_t j = 0; j < tabs; ++j)
				out << "\t";
			if (i < (*current->table_ptr).size() - 1)
				out << "й└йд ";
			else
				out << "й╕йд ";
			ShowAllEntry(out,(*current->table_ptr)[i]);
		}
		--tabs;
	}
}
