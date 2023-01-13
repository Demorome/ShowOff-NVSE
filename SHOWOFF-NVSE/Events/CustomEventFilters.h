#pragma once
#include "EventFilteringInterface.h"

class JohnnyEventFiltersOneFormOneInt : EventHandlerInterface
{
	typedef  std::unordered_set<unsigned int> RefUnorderedSet;

	RefUnorderedSet* Filters = 0;

	RefUnorderedSet* GetFilter(UInt32 filter)
	{
		if (filter >= numFilters) return NULL;
		return &(Filters[filter]);
	}
public:


	JohnnyEventFiltersOneFormOneInt(void** filters, UInt32 nuFilters)
	{
		numFilters = nuFilters;
		Filters = new RefUnorderedSet[numFilters];
		GenFilters = new GenericFilters[numFilters];
		for (int i = 0; i < nuFilters; i++) GenFilters[i].ptr = filters[i];

	}
	virtual ~JohnnyEventFiltersOneFormOneInt()
	{
		delete[] Filters;
		delete[] GenFilters;
	}

	virtual bool IsInFilter(UInt32 filterNum, GenericFilters toSearch)
	{
		RefUnorderedSet* FilterSet;
		if (!(FilterSet = GetFilter(filterNum))) return false;
		return  FilterSet->empty() || (FilterSet->find(toSearch.refID) != FilterSet->end());
	}


	virtual void InsertToFilter(UInt32 filterNum, GenericFilters toInsert)
	{
		RefUnorderedSet* FilterSet;
		if (!(FilterSet = GetFilter(filterNum))) return;
		FilterSet->insert(toInsert.refID);
	}
	virtual void DeleteFromFilter(UInt32 filterNum, GenericFilters toDelete)
	{
		RefUnorderedSet* FilterSet;
		if (!(FilterSet = GetFilter(filterNum))) return;
		FilterSet->erase(toDelete.refID);

	}
	virtual bool IsFilterEmpty(UInt32 filterNum)
	{
		RefUnorderedSet* FilterSet = GetFilter(filterNum);
		if (!FilterSet) return true;
		return FilterSet->empty();
	}
	virtual bool IsFilterEqual(GenericFilters Filter, UInt32 nuFilter)
	{
		return (Filter.ptr == GenFilters[nuFilter].ptr);
	}
	virtual bool IsAcceptedParameter(GenericFilters parameter)
	{
		return parameter.form->refID != 0x3B; // xMarker

	}

	virtual void SetUpFiltering()
	{
		if (GenFilters[1].intVal != -1) InsertToFilter(1, GenFilters[1].intVal);
		TESForm* currentFilter = GenFilters[0].form;
		if (!currentFilter) return;
		if (IS_TYPE(currentFilter, BGSListForm))
		{
			ListNode<TESForm>* iterator = ((BGSListForm*)currentFilter)->list.Head();
			do {
				TESForm* it = iterator->data;
				if (IsAcceptedParameter(it))
					InsertToFilter(0, it->refID);
			} while (iterator = iterator->next);

		}
		else if (IsAcceptedParameter(currentFilter)) InsertToFilter(0, currentFilter->refID);

	}

};
void* __fastcall CreateOneFormOneIntFilter(void** Filters, UInt32 numFilters) {
	return new JohnnyEventFiltersOneFormOneInt(Filters, numFilters);
}

struct EventFilterStructOneFormOneInt {
	TESForm* form;
	int intID;
};


// =====

struct EventFilterStructOneFormOneString {
	TESForm* form;
	char str[0x80];
};

// Filters are non-optional
struct JohnnyEventFiltersOneFormOneString : EventHandlerInterface
{
	static constexpr UInt8 NumFilters = 2;

	JohnnyEventFiltersOneFormOneString(void** filters)
	{
		const auto& filterStruct = *reinterpret_cast<EventFilterStructOneFormOneString*>(filters);
		numFilters = NumFilters;
		GenFilters = new GenericFilters[NumFilters];
		GenFilters[0].refID = filterStruct.form ? filterStruct.form->refID : 0;
		GenFilters[1].str = CopyString(filterStruct.str);
	}
	~JohnnyEventFiltersOneFormOneString() override
	{
		free(GenFilters[1].str);
		delete[] GenFilters;
		GenFilters = nullptr;
	}

	bool IsInFilter(UInt32 filterNum, GenericFilters toSearch) override
	{
		if (filterNum == 0)
			return toSearch.refID == GenFilters[0].refID;
		if (filterNum == 1)
			return StrEqualCI(toSearch.str, GenFilters[1].str);
		return false;
	}
	void InsertToFilter(UInt32 filterNum, GenericFilters toInsert) override
	{}
	void DeleteFromFilter(UInt32 filterNum, GenericFilters toDelete) override
	{}
	bool IsFilterEmpty(UInt32 filterNum) override
	{
		return false; // should never be empty (non-optional filters)
	}
	bool IsFilterEqual(GenericFilters filter, UInt32 nuFilter) override
	{
		return IsInFilter(nuFilter, filter);
	}
	bool IsAcceptedParameter(GenericFilters parameter) override
	{
		return false; // will check if param is accepted within event handler func.
	}
	void SetUpFiltering() override
	{}
};
void* __fastcall CreateOneFormOneStringFilter(void** Filters, UInt32 numFilters_unused) {
	return new JohnnyEventFiltersOneFormOneString(Filters);
}