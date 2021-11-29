#pragma once
#include <vector>
#include <unordered_map>
#include <set>
#include <cassert>
#include "MuException.h"

enum class ActionType
{
	error = -1,
	accept,
	move_in,
	move_epsilon,
	reduce
};

template <typename T>
class PushDownAutomaton
{
public:
	// A -> ��
	using Production = std::vector<T>;

	// A -> ��0|��1|...
	using Productions = std::vector<Production>;

	// A -> ��0|��1|...
	// B -> ��0|��2|...
	// ...
	using ProductionTable = std::vector<Productions>;

	// A: a0,a1,...
	// B: b0,b1,...
	// ...
	using FirstTable = std::vector<std::set<T>>;
	using FollowTable = std::vector<std::set<T>>;

	// SELECT(A -> ��) = {a1,a2,...}
	// Select_Table[nonterm][production] => set of term;
	using SelectTable = std::vector<std::map<Production&, std::set<T>>>;

	//	|nt/t	|a		|b		|c		|
	//	|A		|A->��	|none	|none	|
	//	|B		|none	|none	|B->��	|
	//  |...	|		|		|		|
	using LL1Table = std::vector<std::vector<Production>>;

	// e.g. [S' -> .S]
	struct Item
	{
		// to get Productions in Production_Table
		T sym;
		// to get certain production
		int production_index;
		// A->��.B��
		int point_pos;
		bool operator == (const Item& obj) const
		{
			return sym == obj.sym
				&& production_index == obj.production_index
				&& point_pos == obj.point_pos;
		}

		bool operator < (const Item& rhs) const
		{
			if (sym < rhs.sym) return true;
			if (sym == rhs.sym
				&& production_index < rhs.production_index)
				return true;
			if (sym == rhs.sym
				&& production_index == rhs.production_index
				&& point_pos < rhs.point_pos)
				return true;
			return false;
		}
	};

	struct SimplifiedSetOfItems
	{
		SimplifiedSetOfItems(const std::set<Item>& cores, size_t non_core_size) :
			cores(cores), non_cores(non_core_size, false)
		{}

		// e.g. {[E'->E.],[E->E. + T]} 
		std::set<Item> cores;

		// Look ahead term
		T term;

		// e.g.	include		false	true	false	...
		//		nonterm		A		B		C		...
		std::vector<bool> non_cores;

		bool operator == (const SimplifiedSetOfItems& set) const
		{
			if (set.cores.size() != cores.size()) return false;
			bool eq = false;
			auto it1 = set.cores.begin();
			auto it2 = cores.begin();
			for (; it1 != set.cores.end();)
			{
				eq = (*it2 == *it1);
				if (!eq)
					break;
				++it1;
				++it2;
			}
			return eq;
		}
	};

	struct LR1Item
	{
		// to get Productions in Production_Table
		T nonterm;
		// to get certain production
		size_t production_index;
		// A->��.B��
		size_t point_pos;

		T LAterm;

		bool operator == (const Item& obj)
		{
			return nonterm == obj.nonterm
				&& production_index == obj.production_index
				&& point_pos == obj.point_pos;
		}

		bool operator < (const LR1Item& rhs) const
		{
			if (nonterm < rhs.nonterm) return true;
			if (nonterm == rhs.nonterm
				&& production_index < rhs.production_index)
				return true;
			if (nonterm == rhs.nonterm
				&& production_index == rhs.production_index
				&& point_pos < rhs.point_pos)
				return true;
			return false;
		}

		bool operator == (const LR1Item& rhs) const
		{
			return nonterm == rhs.nonterm
				&& production_index == rhs.production_index
				&& point_pos == rhs.point_pos
				&& LAterm == rhs.LAterm;
		}
	};

	// for LR0
	using CollectionOfItemSets = std::vector<SimplifiedSetOfItems>;

	using LR1Collection = std::vector<std::vector<LR1Item>>;

	struct GotoHash
	{
		std::size_t operator()(const std::pair<size_t, T>& k) const
		{
			return std::hash<size_t>()(k.first) ^
				(std::hash<T>()(k.second) << sizeof(size_t) * 4);
		}
	};

	struct GotoEqual
	{
		bool operator()(const std::pair<size_t, T>& lhs, const std::pair<size_t, T>& rhs) const
		{
			return lhs.first == rhs.first && lhs.second == rhs.second;
		}
	};

private:
	template<class Ty>
	static std::ostream& Save(std::ostream& o, Ty& t)
	{
		size_t pos = o.tellp();
		o.write((char*)&t, sizeof(Ty));
		o.seekp(pos + sizeof(Ty));
		return o;
	}

	template<class Ty>
	static std::istream& Load(std::istream& i, Ty& t)
	{
		size_t pos = i.tellg();
		i.read((char*)&t, sizeof(Ty));
		i.seekg(pos + sizeof(Ty));
		return i;
	}
public:


	// GOTO[state,symbol] -> next state
	using GotoTable = std::unordered_map<std::pair<size_t, T>, size_t, GotoHash, GotoEqual>;

	friend
	std::ostream& operator << (std::ostream& o, GotoTable& gt)
	{
		size_t size = gt.size();
		Save(o, size);
		for (auto& [k, val] : gt)
		{
			std::pair<size_t, T> first = k;
			Save(o, first);
			Save(o, val);
		}
		return o;
	}

	friend
	std::istream& operator >> (std::istream& i, GotoTable& gt)
	{
		size_t size;
		Load(i, size);
		gt.clear();
		for (size_t t = 0; t < size; ++t)
		{
			size_t val;
			std::pair<size_t, T> first;
			Load(i, first);
			Load(i, val);
			gt.insert({ first, val });
		}
		return i;
	}

	// type == ActionType::move_in		aim_state available
	// type == ActionType::reduce		nonterm,production_length available 
	// type == ActionType::accept		--
	// type == ActionType::error		--
	struct Action
	{
		ActionType type = ActionType::error;
		size_t aim_state = 0;
		T sym;
		size_t production_index = 0;
		size_t production_length = 0;
	};

	// Action_Table for SLR
	using ActionTable = std::unordered_map<std::pair<size_t, T>, Action, GotoHash, GotoEqual>;

	friend
	std::ostream& operator << (std::ostream& o, ActionTable& at)
	{
		size_t size = at.size();
		Save(o, size);
		for (auto& [k,val] : at)
		{
			std::pair<size_t, T> first = k;
			Save(o, first);
			Save(o, val);
		}
		return o;
	}

	friend
	std::istream& operator >> (std::istream& i, ActionTable& at)
	{
		size_t size;
		Load(i, size);
		at.clear();
		for (size_t t = 0; t < size; ++t)
		{
			Action a;
			std::pair<size_t, T> first;
			Load(i, first);
			Load(i, a);
			at.insert({ first, a });
		}
		return i;
	}

	static void Show(const std::vector<std::set<T>>& table)
	{
		for (size_t i = 0; i < table.size(); i++)
		{
			std::cout << "nt[" << i << "]\t:";
			for (auto item : table[i])
			{
				std::cout << "t[" << item << "]\t";
			}
			std::cout << std::endl;
		}
	}

	static void Show(const LL1Table& table)
	{
		std::cout << "nt/t\t\t";
		for (size_t j = 0; j < table[0].size(); ++j)
		{
			std::cout << "t[" << j + table[0].size() << "]\t\t";
		}
		std::cout << std::endl;
		for (size_t i = 0; i < table.size(); ++i)
		{
			std::cout << "nt[" << i << "]\t\t";
			size_t flag = 0;
			for (size_t j = 0; j < table[i].size(); ++j)
			{
				for (size_t C = flag; C < j; ++C)
				{
					std::cout << "\t\t";
				}
				flag = j;
				for (auto item : table[i][j])
				{
					std::cout << item << " ";
				}
			}
			std::cout << std::endl;
		}
	}

	static FirstTable FIRST(
		const ProductionTable& production_table,
		const T epsilon, const T last_term, const T first = (T)0)
	{
		auto int_last = (int)last_term;
		auto int_first = (int)first;
		FirstTable ret(int_last + 1);
		bool changed = true;
		while (changed)
		{
			changed = false;
			//do until First_Table no longer changes;
			for (int i = int_last; i >= int_first; --i)
				setup_first_table(changed, ret, production_table, epsilon, (T)i);
		}
		return ret;
	}

	static FollowTable FOLLOW(
		const FirstTable& first_table,
		const ProductionTable& production_table,
		const T epsilon, const T end_symbol, const T first_nonterm = (T)0);

	static SelectTable SELECT(
		const FirstTable& first_table,
		const FollowTable& follow_table,
		const T epsilon, const T end_symbol, const T first_nonterm = (T)0)
	{
		//TODO:
		//auto int_first_of_nonterm = (int)first_nonterm;
		//auto int_end_of_nonterm = (int)epsilon - 1;
		//Select_Table ret(int_end_of_nonterm + 1);
	}

	static LL1Table Preanalysis(
		const FirstTable& first_table,
		const FollowTable& follow_table,
		const ProductionTable& production_table,
		const T epsilon, const T end_symbol, const T first_nonterm = (T)0);

	static SimplifiedSetOfItems CLOSURE(
		const ProductionTable& production_table,
		const std::set<Item> cores,
		const T& epsilon, const T& start = (T)0);


	static SimplifiedSetOfItems CLOSURE(
		const ProductionTable& production_table,
		const SimplifiedSetOfItems& I,
		const T& epsilon, const T& start = (T)0)
	{
		return CLOSURE(production_table, I.cores, epsilon, start);
	}

	static void CLOSURE(
		std::vector<LR1Item>& I,
		const ProductionTable& production_table,
		const FirstTable& first_table,
		const T& epsilon, const T& start = (T)0);

	static SimplifiedSetOfItems GOTO(
		const ProductionTable& production_table,
		const SimplifiedSetOfItems& I,
		const T& symbol, const T& epsilon, const T& start = (T)0);

	static std::vector<LR1Item> GOTO(
		const ProductionTable& production_table,
		const FirstTable& first_table,
		const std::vector<LR1Item>& I,
		const T& symbol, const T& epsilon, const T& start = (T)0);

	static std::tuple<LR1Collection, GotoTable> COLLECTION_LR(
		const ProductionTable& production_table,
		const FirstTable& first_table,
		const T& epsilon, const T& end, const T& start = (T)0);

	static std::tuple<CollectionOfItemSets, GotoTable> COLLECTION(
		const ProductionTable& production_table,
		const T& epsilon, const T& end, const T& start = (T)0);

	static ActionTable SetActionTable(
		const ProductionTable& production_table,
		const LR1Collection& collection,
		const GotoTable& goto_table,
		const FollowTable& follow_table,
		const T& epsilon, const T& end, const T& start = (T)0);

	static ActionTable SetActionTable(
		const ProductionTable& production_table,
		const CollectionOfItemSets& collection,
		const GotoTable& goto_table,
		const FollowTable& follow_table,
		const T& epsilon, const T& end, const T& start = (T)0);
private:
	// term > epsilon > nonterm
	static void setup_first_table(
		bool& changed,
		FirstTable& first_table,
		const ProductionTable& production_table,
		const T epsilon, const T Sym);

	// for certain A -> ��B��
	// beta_start_index>=length of production X->��B��,means X -> ��B
	static void set_follow_of_X_with_certain_beta(
		bool& changed,
		const FirstTable& first_table,
		FollowTable& follow_table,
		const ProductionTable& production_table,
		const T epsilon, const T sym_A,
		const int production_index, const int pos_B);
};

/*		ActionType type = ActionType::error;
		size_t aim_state = 0;
		T sym;
		size_t production_index = 0;
		size_t production_length = 0;*/
template<typename T>
std::istream& operator >> (std::istream& in, typename PushDownAutomaton<T>::Action& action)
{
	int temp;
	in >> temp;
	action.type = (ActionType)temp;
	size_t sym;
	in >> sym;
	action.sym = (T)sym;
	in >> action.production_index;
	in >> action.production_length;
	return in;
}

template<typename T>
std::ostream& operator << (std::ostream& out, typename PushDownAutomaton<T>::Action& action)
{
	int temp;
	temp = (int)action.type;
	out << temp;
	size_t sym;
	sym = (size_t)action.sym;
	out << sym;
	out << action.production_index;
	out << action.production_length;
	return out;
}

template<typename T>
typename PushDownAutomaton<T>::FollowTable PushDownAutomaton<T>::FOLLOW(
	const FirstTable& first_table,
	const ProductionTable& production_table,
	const T epsilon, const T end_symbol, const T first_nonterm)
{
	auto int_first_of_nonterm = (int)first_nonterm;
	auto int_end_of_nonterm = (int)epsilon - 1;
	FollowTable ret(int_end_of_nonterm + 1);
	ret[int_first_of_nonterm].insert(end_symbol);
	bool changed = true;
	while (changed)
	{
		changed = false;
		//do until Follow_Table no longer changes;
		for (int non_term = int_first_of_nonterm; non_term <= int_end_of_nonterm; ++non_term)
		{
			for (size_t production_index = 0; production_index < production_table[non_term].size(); ++production_index)
			{
				for (size_t pos_B = 0; pos_B < production_table[non_term][production_index].size(); ++pos_B)
				{
					//if(ret[0].size()==2)
					//	__debugbreak();
					set_follow_of_X_with_certain_beta(
						changed, first_table, ret, production_table,
						epsilon, (T)non_term,
						production_index, pos_B);
				}
			}
		}
	}
	return ret;
}

template<typename T>
typename PushDownAutomaton<T>::LL1Table PushDownAutomaton<T>::Preanalysis(
	const FirstTable& first_table,
	const FollowTable& follow_table,
	const ProductionTable& production_table,
	const T epsilon, const T end_symbol, const T first_nonterm)
{
	auto int_first_nonterm = (int)first_nonterm;
	auto int_end_nonterm = (int)epsilon - 1;
	auto int_epsilon = (int)epsilon;
	LL1Table ret(int_end_nonterm + 1, std::vector<Production>(end_symbol - epsilon));
	for (size_t sym = int_first_nonterm; sym < production_table.size(); ++sym)
	{
		//A
		for (const auto& production : production_table[sym])
		{
			bool has_epsilon = false;
			//A -> ��
			//�� -> Y1Y2...Yk
			//FIRST(��)
			for (size_t sym_index = 0; sym_index < production.size(); ++sym_index)
			{
				//for every symbol Yj
				auto Yj = production[sym_index];
				auto firstset_of_Yj = first_table[Yj];
				for (auto term : firstset_of_Yj)
				{
					has_epsilon = false;
					if (term == int_epsilon)
					{
						has_epsilon = true;
						continue;
					}
					auto column = term - epsilon - 1;
					ret[sym][column] = production;
				}
				//once a Yj doesn't have int_epsilon , stop.  
				if (!has_epsilon)
					break;
			}
			//if �� belongs to FIRST(��)
			if (has_epsilon)
				for (auto term : follow_table[sym])
				{
					auto column = term - epsilon - 1;
					ret[sym][column] = production;
				}
		}
	}
	return ret;
}

template<typename T>
typename PushDownAutomaton<T>::SimplifiedSetOfItems PushDownAutomaton<T>::CLOSURE(
	const ProductionTable& production_table,
	const std::set<Item> cores,
	const T& epsilon, const T& start)
{
	SimplifiedSetOfItems ret(cores, epsilon - start);
	for (const auto& core_item : ret.cores)
	{
		// for every core_item
		const auto& production =
			production_table[core_item.sym][core_item.production_index];
		// if A -> ��.a��, discard.
		if (core_item.point_pos >= production.size()
			|| production[core_item.point_pos] >= epsilon)
			continue;
		// else:
		// A->��.B��
		auto B = production[core_item.point_pos];
		// B->��
		ret.non_cores[B] = true;
	}

	auto changed = true;
	while (changed)
	{
		changed = false;
		// A->��.B��
		for (size_t B = 0; B < ret.non_cores.size(); ++B)
			if (ret.non_cores[B])// if has CLOSURE(B)
				for (const auto& production : production_table[B])
					if (//if B-> C��, where C is nonterm (e.g. E -> T)
						production[0] < (int)epsilon
						//And doesn't contain CLOSURE(C)
						&& !ret.non_cores[production[0]])
					{
						ret.non_cores[production[0]] = true;
						changed = true;
					}
	}
	return ret;
}

template<typename T>
void PushDownAutomaton<T>::CLOSURE(
	std::vector<LR1Item>& I, 
	const ProductionTable& production_table,
	const FirstTable& first_table, 
	const T& epsilon, const T& start)
{
	bool changed = true;
	while (changed)
	{
		changed = false;
		for (size_t i = 0; i < I.size(); ++i)
		{
			auto pro_length = production_table[I[i].nonterm][I[i].production_index].size();
			//if A -> X .
			if (I[i].point_pos >= pro_length) continue;

			// else: A -> ��.B��
			auto B = production_table[I[i].nonterm][I[i].production_index][I[i].point_pos];
			if (B < epsilon)
			{
				for (size_t B_pro_index = 0; B_pro_index < production_table[B].size(); ++B_pro_index)
					// for each [B->��]
				{
					// Get First(��a)
					// if �� = ��,First(��a) = a, put [B->.��,a]
					if (I[i].point_pos + 1 >= pro_length)
					{
						auto contain = false;
						LR1Item newitem = { B,B_pro_index,0,I[i].LAterm };
						//for(size_t j = i;j<I.size();++j)
						for (const auto& containedItem : I)
							if (containedItem == newitem)
							{
								contain = true;
								break;
							}
						if (!contain)
						{
							I.push_back(std::move(newitem));
							changed = true;
						}
					}
					else // �� != ��
					{
						// for every sym in ��
						for (size_t iter = I[i].point_pos + 1;
							iter < production_table[I[i].nonterm][I[i].production_index].size();
							++iter)
						{
							const auto& sym = production_table[I[i].nonterm][I[i].production_index][iter];
							if (sym > epsilon) //if sym is term
							{
								const auto& term = sym;
								auto contain = false;
								LR1Item newitem = { B,B_pro_index,0, term };
								for (const auto& containedItem : I)
									if (containedItem == newitem)
									{
										contain = true;
										break;
									}
								if (!contain)
								{
									I.push_back(std::move(newitem));
									changed = true;
								}
								break;
							}
							else // sym is nonterm
							{
								const auto& firsts = first_table[sym];
								bool has_epsilon = false;
								for (const auto& first : firsts)
								{
									if (first == epsilon) has_epsilon = true;
									else
									{
										auto contain = false;
										LR1Item newitem = { B,B_pro_index,0,first };
										for (const auto& containedItem : I)
											if (containedItem == newitem)
											{
												contain = true;
												break;
											}
										if (!contain)
										{
											I.push_back(std::move(newitem));
											changed = true;
										}
									}
								}

								// if first(��) has epsilon
								if (has_epsilon)
								{
									auto contain = false;
									LR1Item newitem = { B,B_pro_index,0,I[i].LAterm };
									for (const auto& containedItem : I)
										if (containedItem == newitem)
										{
											contain = true;
											break;
										}
									if (!contain)
									{
										I.push_back(std::move(newitem));
										changed = true;
									}
								}

							}
						}
					}

				}
			}
			// [A->��.B��,a]
			for (size_t nonterm = 0; nonterm < production_table.size(); ++nonterm)
				for (size_t pro_index = 0; pro_index < production_table[nonterm].size(); ++pro_index)
				{
					auto pro_length = production_table[nonterm][pro_index].size();
					if (I[i].point_pos >= pro_length)
						// [A->��.,a]
						continue;
					if (B < epsilon)
						// if B is nonterm
					{

					}
				}
		}
	}
}

template<typename T>
typename PushDownAutomaton<T>::SimplifiedSetOfItems PushDownAutomaton<T>::GOTO(
	const ProductionTable& production_table,
	const SimplifiedSetOfItems& I,
	const T& symbol, const T& epsilon, const T& start)
{
	std::set<Item> cores;
	//for core_item
	for (const auto& core_item : I.cores)
	{
		// for every core_item
		const auto& production =
			production_table[core_item.sym][core_item.production_index];
		if (core_item.point_pos < production.size()
			// e.g. E->E.+T symbol = +
			&& production[core_item.point_pos] == symbol
			// e.g A->.��
			&& symbol != epsilon)
		{
			//e.g.  insert E->E+.T
			//insert Item(initialized by list)
			cores.insert({ (T)core_item.sym,core_item.production_index,core_item.point_pos + 1 });
		}

	}
	//for non_core_item
	for (size_t sym = 0; sym < I.non_cores.size(); ++sym)
	{
		if (I.non_cores[sym])// if has CLOSURE(nonterm)
			for (int production_index = 0;
				production_index < production_table[sym].size();
				++production_index)
				if (// e.g. E->.T  A->.��
					production_table[sym][production_index][0] == symbol)
				{
					// e.g.  insert E->T.
					// insert Item(initialized by list)
					cores.insert({ (T)sym,production_index,1 });
				}
	}
	SimplifiedSetOfItems ret = CLOSURE(production_table, cores, epsilon, start);
	return ret;
}

template<typename T>
std::vector<typename PushDownAutomaton<T>::LR1Item> PushDownAutomaton<T>::GOTO(const ProductionTable& production_table, const FirstTable& first_table, const std::vector<LR1Item>& I, const T& symbol, const T& epsilon, const T& start)
{
	std::vector<LR1Item> J;
	// for every item
	for (const auto& item : I)
	{
		const auto& production =
			production_table[item.nonterm][item.production_index];
		if (item.point_pos < production.size()
			// e.g. E->E.+T symbol = + A->.��
			&& production[item.point_pos] == symbol)
		{
			J.push_back({ item.nonterm ,item.production_index,item.point_pos + 1,item.LAterm });
		}
	}
	CLOSURE(J, production_table, first_table, epsilon, start);
	return J;
}

template<typename T>
std::tuple<typename PushDownAutomaton<T>::LR1Collection, typename PushDownAutomaton<T>::GotoTable> 
PushDownAutomaton<T>::COLLECTION_LR(
	const ProductionTable& production_table, 
	const FirstTable& first_table, 
	const T& epsilon, const T& end, const T& start)
{
	std::tuple<LR1Collection, GotoTable> ret;
	auto& collection = std::get<0>(ret);
	auto& table = std::get<1>(ret);
	std::vector<LR1Item> I0 = { {start,0,0,end} };
	CLOSURE(I0, production_table, first_table, epsilon, start);
	collection.push_back(I0);

	bool changed = true;
	size_t iter = 0;
	size_t start_pos = 0;
	while (changed)
	{
		// in order to record where to start in next loop 
		iter = collection.size();
		changed = false;
		for (size_t i = start_pos; i < collection.size(); ++i)
			for (int Sym = 0; Sym <= (int)end; ++Sym)
			{
				// for every symbol X
				std::vector<LR1Item> temp_I =
					GOTO(production_table, first_table, collection[i], (T)Sym, epsilon, start);
				bool inexist = true;
				if (temp_I.size())
					for (int j = 0; j < collection.size(); ++j)
					{
						if (temp_I == collection[j])
						{
							inexist = false;
							// GOTO[state_i,symbol] -> state_j
							table[{i, (T)Sym}] = j;
							break;
						}
					}
				else
				{
					continue;
				}

				if (inexist)
				{
					// generate a new state which is now the last elem of collection
					table[{i, (T)Sym}] = collection.size();
					collection.push_back(temp_I);
					changed = true;
				}
			}
		start_pos = iter;
	}
	return ret;
}

template<typename T>
std::tuple<typename PushDownAutomaton<T>::CollectionOfItemSets, typename PushDownAutomaton<T>::GotoTable>
PushDownAutomaton<T>::COLLECTION(
	const ProductionTable& production_table,
	const T& epsilon, const T& end, const T& start)
{
	std::tuple<CollectionOfItemSets, GotoTable> ret;
	CollectionOfItemSets& collection = std::get<0>(ret);
	GotoTable& table = std::get<1>(ret);

	SimplifiedSetOfItems I0 = CLOSURE(
		production_table,
		std::set<Item>({ {start,0,0} }),
		epsilon, start);
	collection.push_back(I0);

	// e.g. Process:
	// generation	1st		2nd		3rd	
	//				I0		I1		I4
	//						I2		I5
	//						I3
	// certain Ii must be producted by Im, who is exactly
	// one generation early than Ii. 
	// and every thing producted by Ii should be checked,
	// whether this state already exists in all privous
	// generations. i.e. whether we need to generate a new
	// Item in the bottom of the Collection. 
	bool changed = true;
	size_t iter = 0;
	size_t start_pos = 0;
	while (changed)
	{
		// in order to record where to start in next loop 
		iter = collection.size();
		changed = false;
		for (size_t i = start_pos; i < collection.size(); ++i)
			for (int Sym = 0; Sym <= (int)end; ++Sym)
			{
				// for every symbol X
				SimplifiedSetOfItems temp_I =
					GOTO(production_table, collection[i], (T)Sym, epsilon, start);
				bool inexist = true;
				if (temp_I.cores.size())
					for (int j = 0; j < collection.size(); ++j)
					{
						if (temp_I == collection[j])
						{
							inexist = false;
							// GOTO[state_i,symbol] -> state_j
							table[{i, (T)Sym}] = j;
							break;
						}
					}
				else
				{
					continue;
				}

				if (inexist)
				{
					// generate a new state which is now the last elem of collection
					table[{i, (T)Sym}] = collection.size();
					collection.push_back(temp_I);
					changed = true;
				}
			}
		start_pos = iter;
	}
	return ret;
}

template<typename T>
typename PushDownAutomaton<T>::ActionTable PushDownAutomaton<T>::SetActionTable(
	const ProductionTable& production_table, 
	const LR1Collection& collection, 
	const GotoTable& goto_table,
	const FollowTable& follow_table, 
	const T& epsilon, const T& end, const T& start)
{
	ActionTable ret;
	auto int_end = (int)end;
	auto int_epsilon = (int)epsilon;
	auto int_start = (int)start;
	for (const auto& item : goto_table)
	{
		auto Sym = std::get<1>(std::get<0>(item));
		auto int_sym = (int)Sym;
		auto cur_state = std::get<0>(std::get<0>(item));
		auto aim_state = std::get<1>(item);
		// every certain goto in goto_table
		if (int_sym > int_epsilon && int_sym < int_end)
			// if the state is A->��.a��, a is a term
		{
			// then ACTION[i,a] = move in a
			Action a;
			a.type = ActionType::move_in;
			// also record the next state to be moved into the stack
			a.aim_state = aim_state;
			a.sym = (T)int_sym;
			a.production_length = 0;
			auto iter = ret.find({ cur_state, Sym });
			if (iter != ret.end())
				if (a.type != iter->second.type
					|| a.sym != iter->second.sym)
					throw std::logic_error("Confliction");
			ret[{cur_state, Sym}] = std::move(a);
		}
		else if (int_sym == int_epsilon)
		{
			for (const auto& item : collection[cur_state])
				if (production_table[item.nonterm][item.production_index][0] == epsilon)
				{
					Action a;
					a.type = ActionType::move_epsilon;
					// also record the next state to be moved into the stack
					a.aim_state = aim_state;
					a.sym = (T)int_sym;
					a.production_length = 0;
					auto iter = ret.find({ cur_state, item.LAterm });
					if (iter != ret.end())
						if (a.type != iter->second.type
							|| a.sym != iter->second.sym
							|| a.production_index != iter->second.production_index)
							throw std::logic_error("Confliction");
					ret[{cur_state, item.LAterm}] = std::move(a);
				}
		}
	}
	//for every certain Ii
	for (size_t i = 0; i < collection.size(); ++i)
	{
		//for every certain item in Ii
		for (const auto& item : collection[i])
		{
			auto production_length = production_table[item.nonterm][item.production_index].size();
			if (item.point_pos >= production_length)
				// if the state is A->��.
			{
				if (item.nonterm == start)
					// if S'-> S.
				{
					// then ACTION[i,$] = accept
					Action a;
					a.type = ActionType::accept;
					a.aim_state = 0;
					a.sym = start;
					a.production_index = item.production_index;
					a.production_length = production_length;
					ret[{i, end}] = std::move(a);
				}
				else
				{
					// [A->��.,a]
					// ACTION[i,a] = reduce A->��
					Action a;
					a.type = ActionType::reduce;
					// also record the head of the production
					a.sym = item.nonterm;
					a.production_index = item.production_index;
					a.production_length = production_length;

					if (ret.count({ i, item.LAterm }))
						throw std::logic_error("Confliction");
					ret[{i, item.LAterm}] = std::move(a);
				}
			}
		}
	}
	return ret;
}

template<typename T>
typename PushDownAutomaton<T>::ActionTable PushDownAutomaton<T>::SetActionTable(
	const ProductionTable& production_table,
	const CollectionOfItemSets& collection,
	const GotoTable& goto_table,
	const FollowTable& follow_table,
	const T& epsilon, const T& end, const T& start)
{
	ActionTable ret;
	auto int_end = (int)end;
	auto int_epsilon = (int)epsilon;
	auto int_start = (int)start;
	for (const auto& item : goto_table)
	{
		auto Sym = std::get<1>(std::get<0>(item));
		auto int_sym = (int)Sym;
		auto cur_state = std::get<0>(std::get<0>(item));
		auto aim_state = std::get<1>(item);
		// every certain goto in goto_table
		if (int_sym > int_epsilon && int_sym < int_end)
			// if the state is A->��.a��, a is a term
		{
			// then ACTION[i,a] = move in a
			Action a;
			a.type = ActionType::move_in;
			// also record the next state to be moved into the stack
			a.aim_state = aim_state;
			a.sym = (T)int_sym;
			a.production_length = 0;
			auto iter = ret.find({ cur_state, Sym });
			if (iter != ret.end())
				if (a.type != iter->second.type
					|| a.sym != iter->second.sym)
					throw std::logic_error("Confliction");
			ret[{cur_state, Sym}] = std::move(a);
		}
		else if (int_sym == int_epsilon)
		{
			for (size_t i = 0; i < collection[cur_state].non_cores.size(); ++i)
				if (collection[cur_state].non_cores[i])
					for (const auto& pro : production_table[i])
						if (pro[0] == epsilon)
							for (const auto& term : follow_table[i])
							{
								Action a;
								a.type = ActionType::move_epsilon;
								// also record the next state to be moved into the stack
								a.aim_state = aim_state;
								a.sym = (T)int_sym;
								a.production_length = 0;
								auto iter = ret.find({ cur_state, term });
								if (iter != ret.end())
									if (a.type != iter->second.type
										|| a.sym != iter->second.sym
										|| a.production_index != iter->second.production_index)
										throw std::logic_error("Confliction");
								ret[{cur_state, term}] = std::move(a);
							}
		}
	}
	for (size_t i = 0; i < collection.size(); ++i)
	{
		// every certain Ii
		for (const Item& core : collection[i].cores)
		{
			// every certain core
			auto production_length = production_table[(int)core.sym][core.production_index].size();
			if (core.point_pos >= production_length)
				// if the state is A->��.
			{
				if (core.sym == start)
					// if S'-> S.
				{
					// then ACTION[i,$] = accept
					Action a;
					a.type = ActionType::accept;
					a.aim_state = 0;
					a.sym = start;
					a.production_index = core.production_index;
					a.production_length = production_length;
					ret[{i, end}] = std::move(a);
				}
				else
					for (const auto& term : follow_table[(int)core.sym])
					{
						// then for every term a, in FOLLOW(A)
						// ACTION[i,a] = reduce A->��
						Action a;
						a.type = ActionType::reduce;
						// also record the head of the production
						a.sym = core.sym;
						a.production_index = core.production_index;
						a.production_length = production_length;
						if (ret.count({ i, term }))
							throw std::logic_error("Confliction");
						ret[{i, term}] = std::move(a);
					}
			}
		}
	}
	return ret;
}

template<typename T>
void PushDownAutomaton<T>::setup_first_table(
	bool& changed,
	FirstTable& first_table,
	const ProductionTable& production_table,
	const T epsilon, const T Sym)
{
	auto int_sym = (int)Sym;
	auto int_epsilon = (int)epsilon;
	//for X -> a
	if (int_sym >= int_epsilon)
	{
		if (first_table[Sym].insert(Sym).second)
			changed = true;
	}
	else
	{
		//productions of X:
		//X -> alpha | beta | ...
		for (size_t production_index = 0; production_index < production_table[int_sym].size(); ++production_index)
		{
			// a certain production: X -> Y1Y2...Yk
			if (production_table[int_sym][production_index][0] != int_epsilon)
				//if the production is not X -> epsilon
			{
				bool has_epsilon = false;
				for (size_t nonterm_index = 0; nonterm_index < production_table[int_sym][production_index].size(); ++nonterm_index)
				{
					//for every symbol Yj
					auto Yj = production_table[int_sym][production_index][nonterm_index];
					auto firstset_of_Yj = first_table[Yj];
					for (auto term : firstset_of_Yj)
					{
						has_epsilon = false;
						if (term == int_epsilon)
						{
							has_epsilon = true;
							continue;
						}
						if (first_table[int_sym].insert(term).second) changed = true;
					}
					//once a Yj doesn't have int_epsilon , stop.  
					if (!has_epsilon)
						break;
				}
				//if all Yj has int_epsilon , then put int_epsilon into FIRST(X);
				if (has_epsilon && first_table[int_sym].insert(epsilon).second)
					changed = true;
			}
			//when the production is X -> epsilon
			else if (first_table[int_sym].insert(epsilon).second)
				changed = true;
		}
	}
}

template<typename T>
void PushDownAutomaton<T>::set_follow_of_X_with_certain_beta(
	bool& changed,
	const FirstTable& first_table,
	FollowTable& follow_table,
	const ProductionTable& production_table,
	const T epsilon, const T sym_A,
	const int production_index, const int pos_B)
{
	auto int_sym_A = (int)sym_A;
	auto int_sym_B = (int)production_table[int_sym_A][production_index][pos_B];
	const size_t beta_start_index = pos_B + 1;
	// if(int_sym_A == 1 && beta_start_index == 2)
	//	__debugbreak();

	// if B is not non-term 
	if (int_sym_B >= (int)epsilon)
		return;

	// if A -> ��B
	// then all FOLLOW(A) belongs to FOLLOW(B)
	if (beta_start_index >= production_table[int_sym_A][production_index].size())
	{
		// then all FOLLOW(A) belongs to FOLLOW(B)
		const auto& follow_of_A = follow_table[int_sym_A];
		for (auto term : follow_of_A)
		{
			if (follow_table[int_sym_B].insert(term).second)
				changed = true;
		}
		return;
	}

	// A -> ��B��		
	// all FIRST(��) (excepts ��) belongs to FIRST(B)
	bool has_epsilon = false;
	const size_t length_of_production = production_table[int_sym_A][production_index].size();
	// �� = X1X2...Xm
	for (size_t index_in_beta = beta_start_index; index_in_beta < length_of_production; ++index_in_beta)
	{
		// for certain Xi
		T sym_X = production_table[int_sym_A][production_index][index_in_beta];
		int int_sym_X = (int)sym_X;
		for (auto term : first_table[int_sym_X])
		{
			if (term == epsilon)
			{
				has_epsilon = true;
				continue;
			}
			if (follow_table[int_sym_B].insert(term).second)
				changed = true;
		}
		if (!has_epsilon)
			break;
	}

	// X -> ��B��	
	// if FIRST(��) has ��
	// then all FOLLOW(A) belongs to FOLLOW(B)
	if (has_epsilon)
	{
		const auto& follow_of_A = follow_table[int_sym_A];
		for (auto term : follow_of_A)
		{
			if (follow_table[int_sym_B].insert(term).second)
				changed = true;
		}
	}
}

template<typename T>
std::ostream& operator << (std::ostream& out, std::tuple<size_t, T> tup)
{
	out << std::get<0>(tup);
	out << (size_t)std::get<1>(tup);
	return out;
}