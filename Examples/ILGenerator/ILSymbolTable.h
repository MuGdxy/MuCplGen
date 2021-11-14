#pragma once
#include <string>
#include <vector>
#include <stack>
#include <execution>
#include <MuCplGen/Token.h>
#include <MuCplGen/SealedValue.h>
struct ILType
{
	struct Width
	{
		static const size_t FLOAT = 8;
		static const size_t INT = 4;
		static const size_t CHAR = 1;
	};
	struct Range
	{

	};
};

struct ILEntry
{
	using ILSymbolTable = std::vector<ILEntry*>;
	bool independent = true;
	size_t width = 0;
	size_t offset = 0;
	std::string meta_type;
	std::vector<size_t> array_info;
	const MuCplGen::EasyToken* token = nullptr;
	ILSymbolTable* table_ptr = nullptr;
	std::string value;
	~ILEntry()
	{
		if (table_ptr&&independent)
			delete table_ptr;
	}
	

	friend class ILEnv;
private:
	ILEntry(){}
	friend std::ostream& operator<<(std::ostream& out, const ILEntry& entry);
	static void ShowRawEntry(std::ostream& out,const ILEntry* to_show);
	static void ShowAllEntry(std::ostream& out,const ILEntry* current);
	static size_t tabs;
};


std::ostream& operator << (std::ostream& out, const ILEntry& entry);

struct Address
{
	// if size() == 0 , address is a temp;
	std::vector<ILEntry*> chain;
	std::vector<Address*> array_index;
	std::string code;
	MuCplGen::EasyToken* token = nullptr;
	friend class ILEnv;
	friend std::ostream& operator << (std::ostream& out, const Address& addr);
private:
	size_t id;
	Address(){}
};

std::ostream& operator << (std::ostream& out, const Address& addr);

struct ILCode
{
	// res op left right
	Address* res;
	std::string op;
	Address* left;
	Address* right;
};

std::ostream& operator << (std::ostream& out, const ILCode& code);

class ILEnv
{
	using ILSymbolTable = std::vector<ILEntry*>;
	
	//std::stack<ILSymbolTable*> table_stack;
	std::stack<ILEntry*> entry_stack;
	std::stack<size_t> offset_stack;	
	std::vector<ILEntry*> ILEntry_heap;
	std::vector<Address*> address_heap;
	std::stack<Sealed*> sealed_value;
	size_t address_id = 0;
public:
	//ILSymbolTable* top;
	//ILSymbolTable* var_head;
	//ILSymbolTable* type_head;

	ILEntry* top;
	ILEntry* var_head;
	ILEntry* type_head;
	bool is_typedef = false;
	size_t offset;
	std::vector<ILCode> ILCodeStream;
	ILEnv():
		offset(0)
	{
		var_head = CreateILEntry();
		var_head->table_ptr = new ILSymbolTable();
		var_head->meta_type = "VAR";
		type_head = CreateILEntry();
		type_head->table_ptr = new ILSymbolTable();
		type_head->meta_type = "TYPE";
		top = var_head;
	}
	~ILEnv()
	{
		while (!sealed_value.empty())
		{
			delete sealed_value.top();
			sealed_value.pop();
		}
		for (auto entry : ILEntry_heap)
			delete entry;
		for (auto address : address_heap)
			delete address;
	}
	template<typename T>
	T* CreateValue(T obj)
	{
		auto temp = new SealedValue<T>(obj);
		sealed_value.push(temp);
		return temp->value;
	}
	ILEntry* CreateILEntry()
	{
		auto temp = new ILEntry();
		ILEntry_heap.push_back(temp);
		return temp;
	}
	Address* CreateAddress()
	{
		auto temp = new Address();
		temp->id = address_id++;
		address_heap.push_back(temp);
		return temp;
	}
	ILEntry* CreateVarFromCType(ILEntry* src);
	void SetVarTable()
	{
		top = var_head;
		is_typedef = false;
	}
	void SetTypeTable()
	{
		top = type_head;
		is_typedef = true;
	}
	// set entry.offset, and change the current offset with entry.width
	// then add the entry to var_head
	void AddSubEntryToCurrent(ILEntry* entry)
	{
		entry->offset = ceil((double)offset/(double)entry->width)* entry->width;
		offset += entry->width;
		if (!top->table_ptr)
			top->table_ptr = new ILSymbolTable();
		top->table_ptr->push_back(entry);
	}

	void PushEntry()
	{
		entry_stack.push(top);
		offset_stack.push(offset);
		offset = 0;
	}

	ILEntry* CreateILEntryWithTable()
	{
		auto temp = CreateILEntry();
		temp->table_ptr = new ILSymbolTable();
		return temp;
	}

	ILEntry* LastILEntryInCurTable()
	{
		return (*top->table_ptr)[top->table_ptr->size() - 1];
	}

	void PopEntry()
	{
		if (entry_stack.size())
		{
			top = entry_stack.top();
			entry_stack.pop();
			offset = offset_stack.top();
			offset_stack.pop();
		}
		else
			throw(std::logic_error("Empty Stack"));
	}
private:
	ILEntry* Copy(const ILEntry* current);
};

