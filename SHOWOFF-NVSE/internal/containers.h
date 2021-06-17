#pragma once

#include "utility.h"
#include "memory_pool.h"

// Some container code was ripped from JIP.

// Below definitions are used by JIP
#define MAP_DEFAULT_ALLOC			8
#define MAP_DEFAULT_BUCKET_COUNT	8
#define MAP_MAX_BUCKET_COUNT		0x40000
#define VECTOR_DEFAULT_ALLOC		8

void* __fastcall Pool_Alloc_Buckets(UInt32 numBuckets);
UInt32 __fastcall AlignBucketCount(UInt32 count);
UInt32 __fastcall StrHashCS(const char* inKey);
UInt32 __fastcall StrHashCI(const char* inKey);

enum
{
	kMapDefaultAlloc =		8,

	kMapDefaultBucketNum =	11,
	kMapBucketSizeInc =		2,

	kVectorDefaultAlloc =	2,
};

template <typename T_Data> __forceinline UInt32 AlignNumAlloc(UInt32 numAlloc)
{
	switch (sizeof(T_Data) & 0xF)
	{
	case 0:
		return numAlloc;
	case 2:
	case 6:
	case 0xA:
	case 0xE:
		if (numAlloc & 7)
		{
			numAlloc &= 0xFFFFFFF8;
			numAlloc += 8;
		}
		return numAlloc;
	case 4:
	case 0xC:
		if (numAlloc & 3)
		{
			numAlloc &= 0xFFFFFFFC;
			numAlloc += 4;
		}
		return numAlloc;
	case 8:
		if (numAlloc & 1)
			numAlloc++;
		return numAlloc;
	default:
		if (numAlloc & 0xF)
		{
			numAlloc &= 0xFFFFFFF0;
			numAlloc += 0x10;
		}
		return numAlloc;
	}
}

template <typename T_Key, typename T_Data> struct MappedPair
{
	T_Key		key;
	T_Data		value;
};

template <typename T_Key> class MappedKey
{
	T_Key		key;

public:
	T_Key Get() const {return key;}
	void Set(T_Key inKey) {key = inKey;}
	char Compare(T_Key inKey) const
	{
		if (inKey == key) return 0;
		return (inKey < key) ? -1 : 1;
	}
	void Clear() {}
};

template <> class MappedKey<const char*>
{
	const char	*key;

public:
	const char *Get() const {return key;}
	void Set(const char *inKey) {key = inKey;}
	char Compare(const char *inKey) const {return StrCompare(inKey, key);}
	void Clear() {}
};

template <> class MappedKey<char*>
{
	char		*key;

public:
	char *Get() const {return key;}
	void Set(char *inKey) {key = CopyString(inKey);}
	char Compare(char *inKey) const {return StrCompare(inKey, key);}
	void Clear() {free(key);}
};

#if 0
template <typename T_Key, typename T_Data> class Map
{
protected:
	typedef MappedKey<T_Key> M_Key;

	struct Entry
	{
		M_Key		key;
		T_Data		value;

		void Clear()
		{
			key.Clear();
			value.~T_Data();
		}
	};

	Entry		*entries;		// 00
	UInt32		numEntries;		// 04
	UInt32		alloc;			// 08

	__declspec(noinline) bool GetIndex(T_Key key, UInt32 &outIdx) const
	{
		UInt32 lBound = 0, uBound = numEntries, index;
		char cmpr;
		while (lBound != uBound)
		{
			index = (lBound + uBound) / 2;
			cmpr = entries[index].key.Compare(key);
			if (!cmpr)
			{
				outIdx = index;
				return true;
			}
			if (cmpr < 0) uBound = index;
			else lBound = index + 1;
		}
		outIdx = lBound;
		return false;
	}

	__declspec(noinline) bool InsertKey(T_Key key, T_Data *&outData)
	{
		UInt32 index;
		if (GetIndex(key, index))
		{
			outData = &entries[index].value;
			return false;
		}
		if (!entries) entries = (Entry*)malloc(sizeof(Entry) * alloc);
		else if (numEntries == alloc)
		{
			alloc *= 2;
			entries = (Entry*)realloc(entries, sizeof(Entry) * alloc);
		}
		Entry *entry = entries + index;
		index = numEntries - index;
		if (index) memmove(entry + 1, entry, sizeof(Entry) * index);
		numEntries++;
		entry->key.Set(key);
		outData = &entry->value;
		return true;
	}

public:
	Map(UInt32 _alloc = kMapDefaultAlloc) : entries(NULL), numEntries(0), alloc(_alloc) {}
	~Map()
	{
		if (!entries) return;
		while (numEntries)
			entries[--numEntries].Clear();
		free(entries);
		entries = NULL;
	}

	UInt32 Size() const {return numEntries;}
	bool Empty() const {return !numEntries;}

	bool Insert(T_Key key, T_Data *&outData)
	{
		if (!InsertKey(key, outData)) return false;
		new (outData) T_Data();
		return true;
	}

	T_Data& operator[](T_Key key)
	{
		T_Data *outData;
		if (InsertKey(key, outData))
			new (outData) T_Data();
		return *outData;
	}

	template <typename ...Args>
	T_Data* Emplace(T_Key key, Args&& ...args)
	{
		T_Data *outData;
		if (InsertKey(key, outData))
			new (outData) T_Data(std::forward<Args>(args)...);
		return outData;
	}

	bool HasKey(T_Key key) const
	{
		UInt32 index;
		return GetIndex(key, index);
	}

	T_Data Get(T_Key key) const
	{
		UInt32 index;
		return GetIndex(key, index) ? entries[index].value : NULL;
	}

	T_Data* GetPtr(T_Key key) const
	{
		UInt32 index;
		return GetIndex(key, index) ? &entries[index].value : NULL;
	}

	bool Erase(T_Key key)
	{
		UInt32 index;
		if (!GetIndex(key, index)) return false;
		Entry *entry = entries + index;
		entry->Clear();
		numEntries--;
		index = numEntries - index;
		if (index) memmove(entry, entry + 1, sizeof(Entry) * index);
		return true;
	}

	void Clear()
	{
		for (Entry *entry = entries; numEntries; numEntries--, entry++)
			entry->Clear();
	}

	class Iterator
	{
	protected:
		friend Map;

		Entry		*entry;		// 00
		UInt32		count;		// 04

	public:
		T_Key Key() const {return entry->key.Get();}
		T_Data& Get() const {return entry->value;}
		T_Data& operator*() const {return entry->value;}
		T_Data operator->() const {return entry->value;}
		bool End() const {return !count;}

		void operator++()
		{
			entry++;
			count--;
		}
		void operator--()
		{
			entry--;
			count--;
		}

		Iterator() {}
		Iterator(Map &source) : entry(source.entries), count(source.numEntries) {}
	};

	class OpIterator : public Iterator
	{
		Map			*table;		// 08

	public:
		Map* Table() const {return table;}

		void Remove(bool frwrd = true)
		{
			entry->Clear();
			Entry *pEntry = entry;
			UInt32 index;
			if (frwrd)
			{
				index = count - 1;
				entry--;
			}
			else index = table->numEntries - count;
			if (index) memmove(pEntry, pEntry + 1, sizeof(Entry) * index);
			table->numEntries--;
		}

		OpIterator(Map &source) : table(&source)
		{
			entry = source.entries;
			count = source.numEntries;
		}
		OpIterator(Map &source, T_Key key) : table(&source)
		{
			UInt32 index;
			if (source.GetIndex(key, index))
			{
				entry = source.entries + index;
				count = source.numEntries - index;
			}
			else count = 0;
		}
		OpIterator(Map &source, T_Key key, bool frwrd) : table(&source)
		{
			if (!source.numEntries)
			{
				count = 0;
				return;
			}
			UInt32 index;
			bool match = source.GetIndex(key, index);
			if (frwrd)
			{
				entry = source.entries + index;
				count = source.numEntries - index;
			}
			else
			{
				entry = source.entries + (index - !match);
				count = index + match;
			}
		}
	};
};
#endif

#if 0
template <typename T_Key> class Set
{
protected:
	typedef MappedKey<T_Key> M_Key;

	M_Key		*keys;		// 00
	UInt32		numKeys;	// 04
	UInt32		alloc;		// 08

	__declspec(noinline) bool GetIndex(T_Key key, UInt32 &outIdx) const
	{
		UInt32 lBound = 0, uBound = numKeys, index;
		char cmpr;
		while (lBound != uBound)
		{
			index = (lBound + uBound) / 2;
			cmpr = keys[index].Compare(key);
			if (!cmpr)
			{
				outIdx = index;
				return true;
			}
			if (cmpr < 0) uBound = index;
			else lBound = index + 1;
		}
		outIdx = lBound;
		return false;
	}

public:
	Set(UInt32 _alloc = kVectorDefaultAlloc) : keys(NULL), numKeys(0), alloc(_alloc) {}
	~Set()
	{
		if (!keys) return;
		while (numKeys)
			keys[--numKeys].Clear();
		free(keys);
		keys = NULL;
	}

	UInt32 Size() const {return numKeys;}
	bool Empty() const {return !numKeys;}

	__declspec(noinline) bool Insert(T_Key key)
	{
		UInt32 index;
		if (GetIndex(key, index)) return false;
		if (!keys) keys = (M_Key*)malloc(sizeof(M_Key) * alloc);
		else if (numKeys == alloc)
		{
			alloc *= 2;
			keys = (M_Key*)realloc(keys, sizeof(M_Key) * alloc);
		}
		M_Key *pKey = keys + index;
		index = numKeys - index;
		if (index) memmove(pKey + 1, pKey, sizeof(M_Key) * index);
		numKeys++;
		pKey->Set(key);
		return true;
	}

	bool HasKey(T_Key key) const
	{
		UInt32 index;
		return GetIndex(key, index);
	}

	bool Erase(T_Key key)
	{
		UInt32 index;
		if (!GetIndex(key, index)) return false;
		M_Key *pKey = keys + index;
		pKey->Clear();
		numKeys--;
		index = numKeys - index;
		if (index) memmove(pKey, pKey + 1, sizeof(M_Key) * index);
		return true;
	}

	void Clear(bool clrKeys = false)
	{
		if (clrKeys)
		{
			for (M_Key *pKey = keys; numKeys; numKeys--, pKey++)
				pKey->Clear();
		}
		else numKeys = 0;
	}

	__declspec(noinline) void CopyFrom(const Set &source)
	{
		numKeys = source.numKeys;
		if (!numKeys) return;
		if (!keys)
		{
			alloc = numKeys;
			keys = (M_Key*)malloc(sizeof(M_Key) * alloc);
		}
		else if (numKeys > alloc)
		{
			alloc = numKeys;
			free(keys);
			keys = (M_Key*)malloc(sizeof(M_Key) * alloc);
		}
		memcpy(keys, source.keys, sizeof(M_Key) * numKeys);
	}

	bool CompareTo(const Set &source) const
	{
		if (numKeys != source.numKeys) return false;
		return !numKeys || MemCmp(keys, source.keys, sizeof(M_Key) * numKeys);
	}

	class Iterator
	{
		friend Set;

		M_Key		*pKey;		// 00
		UInt32		count;		// 04

	public:
		T_Key operator*() const {return pKey->Get();}
		T_Key operator->() const {return pKey->Get();}
		bool End() const {return !count;}

		void operator++()
		{
			pKey++;
			count--;
		}

		Iterator() {}
		Iterator(Set &source) : pKey(source.keys), count(source.numKeys) {}
		Iterator(Set &source, T_Key key)
		{
			UInt32 index;
			if (source.GetIndex(key, index))
			{
				pKey = source.keys + index;
				count = source.numKeys - index;
			}
			else count = 0;
		}
	};

	class OpIterator : public Iterator
	{
		Set			*table;		// 08

	public:
		Set* Table() const {return table;}

		void Remove()
		{
			pKey->Clear();
			M_Key *_key = pKey;
			UInt32 index = count - 1;
			pKey--;
			if (index) memmove(_key, _key + 1, sizeof(M_Key) * index);
			table->numKeys--;
		}

		OpIterator(Set &source) : table(&source)
		{
			pKey = source.keys;
			count = source.numKeys;
		}
	};
};
#endif

template <typename T_Key> __forceinline UInt32 HashKey(T_Key inKey)
{
	if (std::is_same_v<T_Key, char*> || std::is_same_v<T_Key, const char*>)
		return StrHashCI(*(const char**)&inKey);
	UInt32 uKey;
	if (sizeof(T_Key) == 1)
		uKey = *(UInt8*)&inKey;
	else if (sizeof(T_Key) == 2)
		uKey = *(UInt16*)&inKey;
	else
	{
		uKey = *(UInt32*)&inKey;
		if (sizeof(T_Key) > 4)
			uKey += uKey ^ ((UInt32*)&inKey)[1];
	}
	return (uKey * 0xD) ^ (uKey >> 0xF);
}

template <typename T_Key> class HashedKey
{
	using Key_Arg = std::conditional_t<std::is_scalar_v<T_Key>, T_Key, const T_Key&>;

	T_Key		key;

public:
	__forceinline bool Match(Key_Arg inKey, UInt32) const { return key == inKey; }
	__forceinline Key_Arg Get() const { return key; }
	__forceinline void Set(Key_Arg inKey, UInt32) { key = inKey; }
	__forceinline UInt32 GetHash() const { return HashKey<T_Key>(key); }
	__forceinline void Clear() { key.~T_Key(); }
};

template <> class HashedKey<const char*>
{
	UInt32		hashVal;

public:
	__forceinline bool Match(const char*, UInt32 inHash) const { return hashVal == inHash; }
	__forceinline const char* Get() const { return ""; }
	__forceinline void Set(const char*, UInt32 inHash) { hashVal = inHash; }
	__forceinline UInt32 GetHash() const { return hashVal; }
	__forceinline void Clear() {}
};

template <> class HashedKey<char*>
{
	UInt32		hashVal;
	char* key;

public:
	__forceinline bool Match(char*, UInt32 inHash) const { return hashVal == inHash; }
	__forceinline char* Get() const { return key; }
	__forceinline void Set(char* inKey, UInt32 inHash)
	{
		hashVal = inHash;
		key = CopyString(inKey);
	}
	__forceinline UInt32 GetHash() const { return hashVal; }
	__forceinline void Clear() { free(key); }
};

template <typename T_Key, typename T_Data> class UnorderedMap  // Made by JIP
{
	using H_Key = HashedKey<T_Key>;
	using Key_Arg = std::conditional_t<std::is_scalar_v<T_Key>, T_Key, const T_Key&>;
	using Data_Arg = std::conditional_t<std::is_scalar_v<T_Data>, T_Data, T_Data&>;

	struct Entry
	{
		Entry		*next;
		H_Key		key;
		T_Data		value;

		void Clear()
		{
			key.Clear();
			value.~T_Data();
		}
	};

	struct Bucket
	{
		Entry* entries;

		void Insert(Entry* entry)
		{
			entry->next = entries;
			entries = entry;
		}

		void Remove(Entry* entry, Entry* prev)
		{
			if (prev) prev->next = entry->next;
			else entries = entry->next;
			entry->Clear();
			Pool_Free(entry, sizeof(Entry));
		}

		void Clear()
		{
			if (!entries) return;
			Entry* pEntry;
			do
			{
				pEntry = entries;
				entries = entries->next;
				pEntry->Clear();
				Pool_Free(pEntry, sizeof(Entry));
			}
			while (entries);
		}

		UInt32 Size() const
		{
			if (!entries) return 0;
			UInt32 size = 1;
			Entry* pEntry = entries;
			while (pEntry = pEntry->next)
				size++;
			return size;
		}
	};

	Bucket		*buckets;		// 00
	UInt32		numBuckets;		// 04
	UInt32		numEntries;		// 08

	Bucket* End() const { return buckets + numBuckets; }

	__declspec(noinline) void ResizeTable(UInt32 newCount)
	{
		Bucket* pBucket = buckets, * pEnd = End(), *newBuckets = (Bucket*)Pool_Alloc_Buckets(newCount);
		Entry* pEntry, * pTemp;
		newCount--;
		do
		{
			pEntry = pBucket->entries;
			while (pEntry)
			{
				pTemp = pEntry;
				pEntry = pEntry->next;
				newBuckets[pTemp->key.GetHash() & newCount].Insert(pTemp);
			}
			pBucket++;
		} 		while (pBucket != pEnd);
		Pool_Free(buckets, numBuckets * sizeof(Bucket));
		buckets = newBuckets;
		numBuckets = newCount + 1;
	}

	Entry* FindEntry(Key_Arg key) const
	{
		if (numEntries)
		{
			UInt32 hashVal = HashKey<T_Key>(key);
			for (Entry* pEntry = buckets[hashVal & (numBuckets - 1)].entries; pEntry; pEntry = pEntry->next)
				if (pEntry->key.Match(key, hashVal)) return pEntry;
		}
		return nullptr;
	}

	bool InsertKey(Key_Arg key, T_Data** outData)
	{
		if (!buckets)
		{
			numBuckets = AlignBucketCount(numBuckets);
			buckets = (Bucket*)Pool_Alloc_Buckets(numBuckets);
		}
		else if ((numEntries > numBuckets) && (numBuckets < MAP_MAX_BUCKET_COUNT))
			ResizeTable(numBuckets << 1);
		UInt32 hashVal = HashKey<T_Key>(key);
		Bucket* pBucket = &buckets[hashVal & (numBuckets - 1)];
		for (Entry* pEntry = pBucket->entries; pEntry; pEntry = pEntry->next)
		{
			if (!pEntry->key.Match(key, hashVal)) continue;
			*outData = &pEntry->value;
			return false;
		}
		numEntries++;
		Entry* newEntry = ALLOC_NODE(Entry);
		newEntry->key.Set(key, hashVal);
		pBucket->Insert(newEntry);
		*outData = &newEntry->value;
		return true;
	}

public:
	UnorderedMap(UInt32 _numBuckets = MAP_DEFAULT_BUCKET_COUNT) : buckets(nullptr), numBuckets(_numBuckets), numEntries(0) {}
	UnorderedMap(std::initializer_list<MappedPair<T_Key, T_Data>> inList) : buckets(nullptr), numBuckets(inList.size()), numEntries(0) { InsertList(inList); }
	~UnorderedMap()
	{
		if (!buckets) return;
		Clear();
		Pool_Free(buckets, numBuckets * sizeof(Bucket));
		buckets = nullptr;
	}

	UInt32 Size() const { return numEntries; }
	bool Empty() const { return !numEntries; }

	UInt32 BucketCount() const { return numBuckets; }

	void SetBucketCount(UInt32 newCount)
	{
		if (buckets)
		{
			newCount = AlignBucketCount(newCount);
			if ((numBuckets != newCount) && (numEntries <= newCount))
				ResizeTable(newCount);
		}
		else numBuckets = newCount;
	}

	float LoadFactor() const { return (float)numEntries / (float)numBuckets; }

	bool Insert(Key_Arg key, T_Data** outData)
	{
		if (!InsertKey(key, outData)) return false;
		new (*outData) T_Data();
		return true;
	}

	T_Data& operator[](Key_Arg key)
	{
		T_Data* outData;
		if (InsertKey(key, &outData))
			new (outData) T_Data();
		return *outData;
	}

	template <typename ...Args>
	T_Data* Emplace(Key_Arg key, Args&& ...args)
	{
		T_Data* outData;
		if (InsertKey(key, &outData))
			new (outData) T_Data(std::forward<Args>(args)...);
		return outData;
	}

	Data_Arg InsertNotIn(Key_Arg key, Data_Arg value)
	{
		T_Data* outData;
		if (InsertKey(key, &outData))
			RawAssign<T_Data>(*outData, value);
		return value;
	}

	void InsertList(std::initializer_list<MappedPair<T_Key, T_Data>> inList)
	{
		T_Data* outData;
		for (auto iter = inList.begin(); iter != inList.end(); ++iter)
		{
			InsertKey(iter->key, &outData);
			*outData = iter->value;
		}
	}

	bool HasKey(Key_Arg key) const { return FindEntry(key) ? true : false; }

	T_Data Get(Key_Arg key)
	{
		Entry* pEntry = FindEntry(key);
		return pEntry ? pEntry->value : NULL;
	}

	T_Data* GetPtr(Key_Arg key)
	{
		Entry* pEntry = FindEntry(key);
		return pEntry ? &pEntry->value : nullptr;
	}

	bool Erase(Key_Arg key)
	{
		if (numEntries)
		{
			UInt32 hashVal = HashKey<T_Key>(key);
			Bucket* pBucket = &buckets[hashVal & (numBuckets - 1)];
			Entry* pEntry = pBucket->entries, *prev = nullptr;
			while (pEntry)
			{
				if (pEntry->key.Match(key, hashVal))
				{
					numEntries--;
					pBucket->Remove(pEntry, prev);
					return true;
				}
				prev = pEntry;
				pEntry = pEntry->next;
			}
		}
		return false;
	}

	T_Data GetErase(Key_Arg key)
	{
		if (numEntries)
		{
			UInt32 hashVal = HashKey<T_Key>(key);
			Bucket* pBucket = &buckets[hashVal & (numBuckets - 1)];
			Entry* pEntry = pBucket->entries, * prev = nullptr;
			while (pEntry)
			{
				if (pEntry->key.Match(key, hashVal))
				{
					T_Data outVal = pEntry->value;
					numEntries--;
					pBucket->Remove(pEntry, prev);
					return outVal;
				}
				prev = pEntry;
				pEntry = pEntry->next;
			}
		}
		return NULL;
	}

	bool Clear()
	{
		if (!numEntries) return false;
		Bucket* pBucket = buckets, * pEnd = End();
		do
		{
			pBucket->Clear();
			pBucket++;
		} 		while (pBucket != pEnd);
		numEntries = 0;
		return true;
	}

	void DumpLoads()
	{
		UInt32 loadsArray[0x40];
		MemZero(loadsArray, sizeof(loadsArray));
		Bucket* pBucket = buckets;
		UInt32 maxLoad = 0, entryCount;
		for (Bucket* pEnd = End(); pBucket != pEnd; pBucket++)
		{
			entryCount = pBucket->Size();
			loadsArray[entryCount]++;
			if (maxLoad < entryCount)
				maxLoad = entryCount;
		}
		_MESSAGE("Size = %d\nBuckets = %d\n----------------\n", numEntries, numBuckets);
		for (UInt32 iter = 0; iter <= maxLoad; iter++)
			_MESSAGE("%d:\t%05d (%.4f%%)", iter, loadsArray[iter], 100.0 * (double)loadsArray[iter] / numEntries);
}

	class Iterator
	{
		friend UnorderedMap;

		UnorderedMap* table;
		Bucket* bucket;
		Entry* entry;

		void FindNonEmpty()
		{
			for (Bucket* pEnd = table->End(); bucket != pEnd; bucket++)
				if (entry = bucket->entries) return;
		}

	public:
		void Init(UnorderedMap& _table)
		{
			table = &_table;
			entry = nullptr;
			if (table->numEntries)
			{
				bucket = table->buckets;
				FindNonEmpty();
			}
		}

		void Find(Key_Arg key)
		{
			if (!table->numEntries)
			{
				entry = nullptr;
				return;
			}
			UInt32 hashVal = HashKey<T_Key>(key);
			bucket = &table->buckets[hashVal & (table->numBuckets - 1)];
			entry = bucket->entries;
			while (entry)
			{
				if (entry->key.Match(key, hashVal))
					break;
				entry = entry->next;
			}
		}

		UnorderedMap* Table() const { return table; }
		Key_Arg Key() const { return entry->key.Get(); }
		Data_Arg operator()() const { return entry->value; }
		T_Data& Ref() { return entry->value; }
		Data_Arg operator*() const { return entry->value; }
		Data_Arg operator->() const { return entry->value; }

		explicit operator bool() const { return entry != nullptr; }
		void operator++()
		{
			if (entry)
				entry = entry->next;
			else entry = bucket->entries;
			if (!entry && table->numEntries)
			{
				bucket++;
				FindNonEmpty();
			}
		}

		bool IsValid()
		{
			if (entry)
			{
				for (Entry* temp = bucket->entries; temp; temp = temp->next)
					if (temp == entry) return true;
				entry = nullptr;
			}
			return false;
		}

		void Remove()
		{
			Entry* curr = bucket->entries, * prev = nullptr;
			do
			{
				if (curr == entry) break;
				prev = curr;
			} 			while (curr = curr->next);
			table->numEntries--;
			bucket->Remove(entry, prev);
			entry = prev;
		}

		Iterator(UnorderedMap* _table = nullptr) : table(_table), entry(nullptr) {}
		Iterator(UnorderedMap& _table) { Init(_table); }
		Iterator(UnorderedMap& _table, Key_Arg key) : table(&_table) { Find(key); }
	};

	Iterator Begin() { return Iterator(*this); }
	Iterator Find(Key_Arg key) { return Iterator(*this, key); }
};

template <typename T_Key> class UnorderedSet
{
protected:
	typedef HashedKey<T_Key> H_Key;

	struct Bucket
	{
		H_Key		*entries;		// 00
		UInt8		numEntries;		// 04
		UInt8		alloc;			// 05
		UInt8		pad06[2];		// 06

		__declspec(noinline) H_Key *AddEntry()
		{
			if (alloc == numEntries)
			{
				alloc += kMapBucketSizeInc;
				entries = (H_Key*)realloc(entries, sizeof(H_Key) * alloc);
			}
			return entries + numEntries++;
		}

		__declspec(noinline) void Clear(bool doFree = true)
		{
			if (!entries) return;
			for (H_Key *entry = entries; numEntries; numEntries--, entry++)
				entry->Clear();
			if (doFree)
			{
				free(entries);
				entries = NULL;
			}
		}
	};

	Bucket		*buckets;		// 00
	UInt32		numBuckets;		// 04
	UInt32		numItems;		// 08

	__declspec(noinline) void ExpandTable()
	{
		UInt32 newCount = GetNextPrime(numBuckets * 2);
		Bucket *newBuckets = (Bucket*)calloc(newCount, sizeof(Bucket));
		H_Key *entry;
		UInt8 count;
		for (Bucket *currBucket = buckets; numBuckets; numBuckets--, currBucket++)
		{
			if (entry = currBucket->entries)
			{
				for (count = currBucket->numEntries; count; count--, entry++)
					*(newBuckets[entry->GetHash() % newCount].AddEntry()) = *entry;
				free(currBucket->entries);
			}
		}
		free(buckets);
		buckets = newBuckets;
		numBuckets = newCount;
	}

public:
	UnorderedSet(UInt32 _numBuckets = kMapDefaultBucketNum) : buckets(NULL), numBuckets(_numBuckets), numItems(0) {}
	~UnorderedSet()
	{
		if (!buckets) return;
		for (Bucket *bucket = buckets; numBuckets; numBuckets--, bucket++)
			bucket->Clear();
		free(buckets);
		buckets = NULL;
	}

	UInt32 Size() const {return numItems;}
	bool Empty() const {return !numItems;}

	__declspec(noinline) bool Insert(T_Key key)
	{
		if (!buckets) buckets = (Bucket*)calloc(numBuckets, sizeof(Bucket));
		else if (numItems == numBuckets) ExpandTable();
		UInt32 hashVal = H_Key::Hash(key);
		Bucket &bucket = buckets[hashVal % numBuckets];
		H_Key *entry = bucket.entries;
		for (UInt8 count = bucket.numEntries; count; count--, entry++)
			if (entry->GetHash() == hashVal) return false;
		entry = bucket.AddEntry();
		numItems++;
		entry->Set(key, hashVal);
		return true;
	}

	bool HasKey(T_Key key) const
	{
		if (!numItems) return false;
		UInt32 hashVal = H_Key::Hash(key);
		Bucket &bucket = buckets[hashVal % numBuckets];
		H_Key *entry = bucket.entries;
		for (UInt8 count = bucket.numEntries; count; count--, entry++)
			if (entry->GetHash() == hashVal) return true;
		return false;
	}

	bool Erase(T_Key key)
	{
		if (!numItems) return false;
		UInt32 hashVal = H_Key::Hash(key);
		Bucket &bucket = buckets[hashVal % numBuckets];
		H_Key *entry = bucket.entries;
		for (UInt8 count = bucket.numEntries; count; count--, entry++)
		{
			if (entry->GetHash() != hashVal) continue;
			entry->Clear();
			bucket.numEntries--;
			if (count > 1) *entry = bucket.entries[bucket.numEntries];
			numItems--;
			return true;
		}
		return false;
	}

	bool Clear(bool clrBkt = false)
	{
		if (!numItems) return false;
		numItems = 0;
		Bucket *bucket = buckets;
		for (UInt32 count = numBuckets; count; count--, bucket++)
		{
			if (clrBkt) bucket->Clear(false);
			else bucket->numEntries = 0;
		}
		return true;
	}

	class Iterator
	{
		friend UnorderedSet;

		UnorderedSet	*table;		// 00
		UInt32			bucketIdx;	// 04
		UInt8			entryIdx;	// 08
		UInt8			pad09[3];	// 09
		Bucket			*bucket;	// 0C
		H_Key			*entry;		// 10

		void FindValid()
		{
			if (table->numItems)
				for (; bucketIdx < table->numBuckets; bucketIdx++, bucket++)
					if (bucket->numEntries && (entry = bucket->entries)) return;
			entry = NULL;
		}

	public:
		void Init(UnorderedSet &_table)
		{
			table = &_table;
			if (table)
			{
				bucketIdx = 0;
				entryIdx = 0;
				bucket = table->buckets;
				FindValid();
			}
			else entry = NULL;
		}

		T_Key operator*() const {return entry->Get();}
		T_Key operator->() const {return entry->Get();}
		bool End() const {return !entry;}

		void operator++()
		{
			if (++entryIdx >= bucket->numEntries)
			{
				bucketIdx++;
				entryIdx = 0;
				bucket++;
				FindValid();
			}
			else entry++;
		}

		Iterator() : table(NULL), entry(NULL) {}
		Iterator(UnorderedSet &_table) {Init(_table);}
	};
};

template <typename T_Data> class Vector  // made by JIP.
{
	using Data_Arg = std::conditional_t<std::is_scalar_v<T_Data>, T_Data, T_Data&>;

	T_Data		*data;		// 00
	UInt32		numItems;	// 04
	UInt32		numAlloc;	// 08

	__declspec(noinline) T_Data* AllocateData()
	{
		if (!data)
		{
			numAlloc = AlignNumAlloc<T_Data>(numAlloc);
			data = POOL_ALLOC(numAlloc, T_Data); 
		}
		else if (numAlloc <= numItems)
		{
			UInt32 newAlloc = numAlloc << 1;
			POOL_REALLOC(data, numAlloc, newAlloc, T_Data);
			numAlloc = newAlloc;
		}
		return data + numItems++;
	}

	T_Data* End() const { return data + numItems; }

public:
	Vector(UInt32 _alloc = VECTOR_DEFAULT_ALLOC) : data(nullptr), numItems(0), numAlloc(_alloc) {}
	Vector(std::initializer_list<T_Data> inList) : data(nullptr), numItems(0), numAlloc(inList.size()) { AppendList(inList); }
	~Vector()
	{
		if (!data) return;
		Clear();
		POOL_FREE(data, numAlloc, T_Data);
		data = nullptr;
	}

	UInt32 Size() const { return numItems; }
	bool Empty() const { return !numItems; }

	T_Data* Data() const { return data; }

	T_Data& operator[](UInt32 index) const { return data[index]; }

	T_Data* GetPtr(UInt32 index) const { return (index < numItems) ? (data + index) : nullptr; }

	Data_Arg Top() const { return data[numItems - 1]; }

	T_Data* Append(Data_Arg item)
	{
		T_Data* pData = AllocateData();
		RawAssign<T_Data>(*pData, item);
		return pData;
	}

	template <typename ...Args>
	T_Data* Append(Args&& ...args)
	{
		T_Data* pData = AllocateData();
		new (pData) T_Data(std::forward<Args>(args)...);
		return pData;
	}

	void AppendList(std::initializer_list<T_Data> inList)
	{
		for (auto iter = inList.begin(); iter != inList.end(); ++iter)
			Append(*iter);
	}

	T_Data* Insert(UInt32 index, Data_Arg item)
	{
		if (index > numItems)
			index = numItems;
		UInt32 size = numItems - index;
		T_Data* pData = AllocateData();
		if (size)
		{
			pData = data + index;
			memmove(pData + 1, pData, sizeof(T_Data) * size);
		}
		RawAssign<T_Data>(*pData, item);
		return pData;
	}

	template <typename ...Args>
	T_Data* Insert(UInt32 index, Args&& ...args)
	{
		if (index > numItems)
			index = numItems;
		UInt32 size = numItems - index;
		T_Data* pData = AllocateData();
		if (size)
		{
			pData = data + index;
			memmove(pData + 1, pData, sizeof(T_Data) * size);
		}
		new (pData) T_Data(std::forward<Args>(args)...);
		return pData;
	}

	void Concatenate(const Vector& source)
	{
		if (!source.numItems) return;
		UInt32 newCount = numItems + source.numItems;
		if (!data)
		{
			if (numAlloc < newCount) numAlloc = newCount;
			numAlloc = AlignNumAlloc<T_Data>(numAlloc);
			data = POOL_ALLOC(numAlloc, T_Data);
		}
		else if (numAlloc < newCount)
		{
			newCount = AlignNumAlloc<T_Data>(newCount);
			POOL_REALLOC(data, numAlloc, newCount, T_Data);
			numAlloc = newCount;
		}
		memcpy(data + numItems, source.data, sizeof(T_Data) * source.numItems);
		numItems = newCount;
	}

	UInt32 InsertSorted(Data_Arg item, bool descending = false)
	{
		UInt32 lBound = 0, uBound = numItems, index;
		bool isLT;
		while (lBound != uBound)
		{
			index = (lBound + uBound) >> 1;
			isLT = item < data[index];
			if (descending) isLT = !isLT;
			if (isLT) uBound = index;
			else lBound = index + 1;
		}
		uBound = numItems - lBound;
		T_Data* pData = AllocateData();
		if (uBound)
		{
			pData = data + lBound;
			memmove(pData + 1, pData, sizeof(T_Data) * uBound);
		}
		RawAssign<T_Data>(*pData, item);
		return lBound;
	}

	typedef bool (*SortComperator)(const T_Data& lhs, const T_Data& rhs);
	UInt32 InsertSorted(Data_Arg item, SortComperator comperator)
	{
		UInt32 lBound = 0, uBound = numItems, index;
		while (lBound != uBound)
		{
			index = (lBound + uBound) >> 1;
			if (comperator(item, data[index]))
				uBound = index;
			else lBound = index + 1;
		}
		uBound = numItems - lBound;
		T_Data* pData = AllocateData();
		if (uBound)
		{
			pData = data + lBound;
			memmove(pData + 1, pData, sizeof(T_Data) * uBound);
		}
		RawAssign<T_Data>(*pData, item);
		return lBound;
	}

	void MoveToEnd(Data_Arg item)
	{
		if (numItems > 1)
		{
			T_Data* pData = data, * pEnd = End() - 1;
			do
			{
				if (*pData == item)
				{
					memcpy(pData, pData + 1, (UInt32)pEnd - (UInt32)pData);
					RawAssign<T_Data>(*pEnd, item);
					return;
				}
				pData++;
			} 			while (pData != pEnd);
		}
	}

	SInt32 GetIndexOf(Data_Arg item) const
	{
		if (numItems)
		{
			T_Data* pData = data, * pEnd = End();
			do
			{
				if (*pData == item)
					return pData - data;
				pData++;
			} 			while (pData != pEnd);
		}
		return -1;
	}

	template <class Matcher>
	SInt32 GetIndexOf(Matcher& matcher) const
	{
		if (numItems)
		{
			T_Data* pData = data, * pEnd = End();
			do
			{
				if (matcher(*pData))
					return pData - data;
				pData++;
			} 			while (pData != pEnd);
		}
		return -1;
	}

	template <class Matcher>
	T_Data* Find(Matcher& matcher) const
	{
		if (numItems)
		{
			T_Data* pData = data, * pEnd = End();
			do
			{
				if (matcher(*pData))
					return pData;
				pData++;
			} 			while (pData != pEnd);
		}
		return nullptr;
	}

	bool RemoveNth(UInt32 index)
	{
		if (index >= numItems) return false;
		T_Data* pData = data + index;
		pData->~T_Data();
		numItems--;
		index = numItems - index;
		if (index) memcpy(pData, pData + 1, sizeof(T_Data) * index);
		return true;
	}

	bool Remove(Data_Arg item)
	{
		if (numItems)
		{
			T_Data* pData = End(), * pEnd = data;
			do
			{
				pData--;
				if (*pData != item) continue;
				numItems--;
				pData->~T_Data();
				UInt32 size = (UInt32)End() - (UInt32)pData;
				if (size) memcpy(pData, pData + 1, size);
				return true;
			} 			while (pData != pEnd);
		}
		return false;
	}

	template <class Matcher>
	UInt32 Remove(Matcher& matcher)
	{
		if (numItems)
		{
			T_Data* pData = End(), * pEnd = data;
			UInt32 removed = 0, size;
			do
			{
				pData--;
				if (!matcher(*pData)) continue;
				numItems--;
				pData->~T_Data();
				size = (UInt32)End() - (UInt32)pData;
				if (size) memcpy(pData, pData + 1, size);
				removed++;
			} 			while (pData != pEnd);
			return removed;
		}
		return 0;
	}

	void RemoveRange(UInt32 beginIdx, UInt32 count)
	{
		if (!count || (beginIdx >= numItems)) return;
		if (count > (numItems - beginIdx))
			count = numItems - beginIdx;
		T_Data* pBgn = data + beginIdx, * pEnd = pBgn + count, * pData = pBgn;
		do
		{
			pData->~T_Data();
			pData++;
		} 		while (pData != pEnd);
		UInt32 size = (UInt32)End() - (UInt32)pData;
		if (size) memcpy(pBgn, pData, size);
		numItems -= count;
	}

	void Resize(UInt32 newSize)
	{
		if (numItems == newSize)
			return;
		T_Data* pData, * pEnd;
		if (numItems < newSize)
		{
			if (!data)
			{
				numAlloc = AlignNumAlloc<T_Data>(newSize);
				data = POOL_ALLOC(numAlloc, T_Data);
			}
			else if (numAlloc < newSize)
			{
				newSize = AlignNumAlloc<T_Data>(newSize);
				POOL_REALLOC(data, numAlloc, newSize, T_Data);
				numAlloc = newSize;
			}
			pData = data + numItems;
			pEnd = data + newSize;
			do
			{
				new (pData) T_Data();
				pData++;
			} 			while (pData != pEnd);
		}
		else
		{
			pData = data + newSize;
			pEnd = End();
			do
			{
				pData->~T_Data();
				pData++;
			} 			while (pData != pEnd);
		}
		numItems = newSize;
	}

	T_Data Pop()
	{
		if (!numItems) return NULL;
		numItems--;
		T_Data* pEnd = End();
		pEnd->~T_Data();
		return *pEnd;
	}

	void Clear()
	{
		if (numItems)
		{
			T_Data* pData = data, * pEnd = End();
			do
			{
				pData->~T_Data();
				pData++;
			} 			while (pData != pEnd);
		}
		numItems = 0;
	}

private:
	void QuickSortAscending(UInt32 p, UInt32 q)
	{
		if (p >= q) return;
		UInt32 i = p;
		for (UInt32 j = p + 1; j < q; j++)
		{
			if (data[p] < data[j])
				continue;
			i++;
			RawSwap<T_Data>(data[j], data[i]);
		}
		RawSwap<T_Data>(data[p], data[i]);
		QuickSortAscending(p, i);
		QuickSortAscending(i + 1, q);
	}

	void QuickSortDescending(UInt32 p, UInt32 q)
	{
		if (p >= q) return;
		UInt32 i = p;
		for (UInt32 j = p + 1; j < q; j++)
		{
			if (!(data[p] < data[j]))
				continue;
			i++;
			RawSwap<T_Data>(data[j], data[i]);
		}
		RawSwap<T_Data>(data[p], data[i]);
		QuickSortDescending(p, i);
		QuickSortDescending(i + 1, q);
	}

	void QuickSortCustom(UInt32 p, UInt32 q, SortComperator comperator)
	{
		if (p >= q) return;
		UInt32 i = p;
		for (UInt32 j = p + 1; j < q; j++)
		{
			if (comperator(data[p], data[j]))
				continue;
			i++;
			RawSwap<T_Data>(data[j], data[i]);
		}
		RawSwap<T_Data>(data[p], data[i]);
		QuickSortCustom(p, i, comperator);
		QuickSortCustom(i + 1, q, comperator);
	}
public:

	void Sort(bool descending = false)
	{
		if (descending)
			QuickSortDescending(0, numItems);
		else QuickSortAscending(0, numItems);
	}

	void Sort(SortComperator comperator)
	{
		QuickSortCustom(0, numItems, comperator);
	}

	void Shuffle()
	{
		UInt32 idx = numItems, rand;
		while (idx > 1)
		{
			rand = GetRandomInt(idx);
			idx--;
			if (rand != idx)
				RawSwap<T_Data>(data[rand], data[idx]);
		}
	}

	class Iterator
	{
		friend Vector;

		T_Data		*pData;
		UInt32		count;

		Iterator() {}

	public:
		explicit operator bool() const { return count != 0; }
		void operator++()
		{
			pData++;
			count--;
		}
		void operator+=(UInt32 _count)
		{
			if (_count >= count)
				count = 0;
			else
			{
				pData += _count;
				count -= _count;
			}
		}

		Data_Arg operator*() const { return *pData; }
		Data_Arg operator->() const { return *pData; }
		Data_Arg operator()() const { return *pData; }
		T_Data& Ref() { return *pData; }

		Iterator(Vector& source) : pData(source.data), count(source.numItems) {}
		Iterator(Vector& source, UInt32 index) : count(source.numItems)
		{
			if (count <= index)
				count = 0;
			else
			{
				pData = source.data + index;
				count -= index;
			}
		}
	};

	class RvIterator : public Iterator
	{
	public:
		void operator--()
		{
			pData--;
			count--;
		}
		void operator-=(UInt32 _count)
		{
			if (_count >= count)
				count = 0;
			else
			{
				pData -= _count;
				count -= _count;
			}
		}

		void Remove(Vector& source)
		{
			pData->~T_Data();
			UInt32 size = source.numItems - count;
			source.numItems--;
			if (size) memcpy(pData, pData + 1, size * sizeof(T_Data));
		}

		RvIterator(Vector& source)
		{
			if (count = source.numItems)
				pData = source.data + (count - 1);
		}
	};

	class CpIterator : public Iterator
	{
	public:
		CpIterator(Vector& source)
		{
			if (count = source.numItems)
			{
				UInt32 size = count * sizeof(T_Data);
				pData = (T_Data*)memcpy(GetAuxBuffer(s_auxBuffers[2], size), source.data, size);
			}
		}
	};

	Iterator Begin() { return Iterator(*this); }
	Iterator BeginAt(UInt32 index) { return Iterator(*this, index); }

	RvIterator BeginRv() { return RvIterator(*this); }

	CpIterator BeginCp() { return CpIterator(*this); }
};

template <typename T_Data, size_t N> class FixedTypeArray
{
protected:
	size_t		size;
	T_Data		data[N];

public:
	FixedTypeArray() : size(0) {}

	bool Empty() const {return size == 0;}
	size_t Size() const {return size;}
	T_Data *Data() {return data;}

	bool Append(T_Data item)
	{
		if (size >= N) return false;
		data[size++] = item;
		return true;
	}

	class Iterator
	{
	protected:
		friend FixedTypeArray;

		T_Data		*pData;
		UInt32		count;

	public:
		bool End() const {return !count;}
		void operator++()
		{
			count--;
			pData++;
		}

		T_Data& operator*() const {return *pData;}
		T_Data& operator->() const {return *pData;}
		T_Data& Get() const {return *pData;}

		Iterator(FixedTypeArray &source) : pData(source.data), count(source.size) {}
	};
};

