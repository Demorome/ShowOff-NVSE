#pragma once

#include "Serialization.h"
#include "common/ICriticalSection.h"

// simple template class used to support NVSE custom data types (strings, arrays, etc)

struct _VarIDs : Set<uint32_t>
{
	uint32_t PopFirst()
	{
		uint32_t* keys = Keys(), first = *keys;
		if (--numKeys)
			memmove(keys, keys + 1, numKeys * 4);
		return first;
	}

	uint32_t LastKey() { return Keys()[numKeys - 1]; }
};

template <class Var>
class VarMap
{
protected:
#if _DEBUG
	typedef Map<uint32_t, Var> _VarMap;
#else
	typedef UnorderedMap<uint32_t, Var> _VarMap;
#endif
	class VarCache
	{
		// if desired this can be replaced with an impl that caches more than one var without changing client code
		uint32_t		varID;
		Var* var;

	public:
		VarCache() : varID(0), var(NULL) {}

		~VarCache()
		{
			Reset();
		}

		void Insert(uint32_t id, Var* v)
		{
			varID = id;
			var = v;
		}

		// clear all cached vars (only one in current impl)
		void Reset()
		{
			varID = 0;
			var = NULL;
		}

		void Remove(uint32_t id)
		{
			if (id == varID)
			{
				Reset();
			}
		}

		Var* Get(uint32_t id)
		{
			return (varID == id) ? var : NULL;
		}
	};

	_VarMap				vars;
	_VarIDs				usedIDs;
	_VarIDs				tempIDs;		// set of IDs of unreferenced vars, makes for easy cleanup
	_VarIDs				availableIDs;	// IDs < greatest used ID available as IDs for new vars
	VarCache			cache;
	ICriticalSection	cs;				// trying to avoid what looks like concurrency issues
	ICriticalSection    tempIdsCs;

	void SetIDAvailable(uint32_t id)
	{
		if (id) availableIDs.Insert(id);
	}

	uint32_t GetUnusedID()
	{
		uint32_t id = 1;
		cs.Enter();

		if (!availableIDs.Empty())
			id = availableIDs.PopFirst();
		else if (!usedIDs.Empty())
			id = usedIDs.LastKey() + 1;
		cs.Leave();
		return id;
	}

public:
	VarMap()
	{
	}

	~VarMap()
	{
		Reset();
	}

	Var* Get(uint32_t varID)
	{
		if (!varID) return NULL;
		Var* var = cache.Get(varID);
		if (!var)
		{
			var = vars.GetPtr(varID);
			if (var)
				cache.Insert(varID, var);
		}
		return var;
	}

	bool VarExists(uint32_t varID)
	{
		return Get(varID) ? true : false;
	}

	template <typename ...Args>
	Var* Insert(uint32_t varID, Args&& ...args)
	{
		cs.Enter();
		usedIDs.Insert(varID);
		Var* var = vars.Emplace(varID, std::forward<Args>(args)...);
		cs.Leave();
		return var;
	}

	void Delete(uint32_t varID)
	{
		cs.Enter();
		cache.Remove(varID);
		vars.Erase(varID);
		usedIDs.Erase(varID);
		tempIDs.Erase(varID);
		SetIDAvailable(varID);
		cs.Leave();
	}

	static void DeleteBySelf(VarMap* self, uint32_t varID)
	{
		if (self)
			self->Delete(varID);
	}

	void Reset()
	{
		cache.Reset();

		typename _VarMap::Iterator iter;
		while (true)
		{
			iter.Init(vars);
			if (iter.End()) break;
			iter.Remove();
		}

		usedIDs.Clear();
		tempIDs.Clear();
		availableIDs.Clear();
	}

	void MarkTemporary(uint32_t varID, bool bTemporary)
	{
		ScopedLock lock(tempIdsCs);
		if (bTemporary)
		{
			tempIDs.Insert(varID);
		}
		else
		{
			tempIDs.Erase(varID);
		}
	}

	bool IsTemporary(uint32_t varID)
	{
		return tempIDs.HasKey(varID);
	}
};