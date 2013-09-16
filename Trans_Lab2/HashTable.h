#pragma once
#include <functional> //C++11 feature: lambda expressions

template <typename TKEY, typename TVALUE>
class THashTable
{
public:
	static const int TABLE_SIZE = 50;
private:

	int st_statopcount;

	class Record
	{
	public:
		TKEY name;
		TVALUE value;
		Record *prev;
		Record *next;
	};

	Record *st_table[TABLE_SIZE];

public:
	typedef std::tr1::function<int (TKEY&)> HashFunc;
	typedef std::tr1::function<int (TKEY&, TKEY&)> CompareFunc;
	
	typedef std::tr1::function<bool (TVALUE&)> ItemCallbackFunc;

	THashTable(HashFunc _hash, CompareFunc _compare)
	{
		memset(st_table, 0, sizeof(Record *) * TABLE_SIZE);
		hash = _hash;
		compare = _compare;
	}

	~THashTable(void)
	{

	}

	int st_put(TKEY& name, TVALUE& value)
	{
		st_statopcount = 0;
		Record *rec = st_add(name);
		rec->value = value;
		return 1;
	}

	int st_set(TKEY& name, TVALUE& value)
	{
		st_statopcount = 0;
		Record *rec = st_find(name);
		if (!rec) return 0;
		rec->value = value;
		return 1;
	}

	bool st_exist(TKEY& name)
	{
		st_statopcount = 0;
		Record *rec = st_find(name);
		return (rec != nullptr);
	}

	TVALUE& st_get(TKEY& name)
	{
		st_statopcount = 0;
		Record *rec = st_find(name);
		if (!rec) throw std::string("Value not found");
		return rec->value;
	}

	int st_delete(TKEY& name)
	{
		st_statopcount = 0;
		Record *rec = st_find(name);
		if (!rec) return 0;
		if (rec->prev)
			rec->prev->next = rec->next;
		if (rec->next)
			rec->next->prev = rec->prev;
		delete (rec);
		return 1;
	}

	void ProcessHashTable(ItemCallbackFunc PrintItem)
	{
		for(int i = 0; i < TABLE_SIZE; i++)
		{
			for (Record *cur = st_table[i]; cur != nullptr; cur = cur->next)
			{
				if (!PrintItem(cur->value))
					return;
			}
		}
	}

private:
	HashFunc hash;
	CompareFunc compare;

	Record *st_find_in_list(Record *start, TKEY& name)
	{
		Record *cur = start;
		while(cur)
		{
			st_statopcount++;
			if (compare(cur->name, name) == 0)
				return cur;
			cur = cur->next;
		}
		return nullptr;
	}

	Record *st_create(TKEY& name)
	{
		int h = hash(name);

		Record *result = new Record();
		result->name = name;
		result->prev = NULL;
		result->next = NULL;
		if (!st_table[h])
		{
			st_table[h] = result;
		}
		else
		{
			result->next = st_table[h];
			result->next->prev = result;
			st_table[h] = result;
		}
		return result;
	}

	Record *st_add(TKEY& name)
	{
		Record *result = st_find(name);
		if (result) return result;
		else return st_create(name);
	}

	Record *st_find(TKEY& name)
	{
		int h = hash(name);
		return st_find_in_list(st_table[h], name);
	}
};

