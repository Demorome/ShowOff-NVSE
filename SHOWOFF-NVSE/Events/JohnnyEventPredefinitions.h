#pragma once
#include "EventFilteringInterface.h"
#include <memory>
#include <shared_mutex>

#include "GameObjects.h"

class EventInformation;
void* __fastcall GenericCreateFilter(void** maxFilters, uint32_t numFilters);

class EventInformation
{
private:
	void* (__fastcall* CreateFilter)(void**, uint32_t); // supposed to be passing itself
	std::vector<BaseEventClass> EventQueueAdd;
	std::shared_mutex QueueRWLock; //need a readers writer lock to protect from multiple users registering an event in the same frame (very rare, but can happen)
public:
	const char* EventName;
	uint8_t numMaxArgs; // dispatched args #
	uint8_t numMaxFilters;
	std::vector<BaseEventClass> EventCallbacks;
	EventInformation(const char* EventName, uint8_t& numMaxArgs, uint8_t& numMaxFilters, void* (__fastcall* CreatorFunction)(void**, uint32_t) = GenericCreateFilter)
	{
		this->EventName = EventName;
		this->numMaxArgs = numMaxArgs;
		this->numMaxFilters = numMaxFilters;
		this->CreateFilter = CreatorFunction;
	}
	virtual ~EventInformation()
	{
		FlushEventCallbacks();
	}
	void FlushEventCallbacks()
	{
		auto it = EventCallbacks.begin();
		while (it != EventCallbacks.end())
		{
			delete it->eventFilter;
			++it;
		}
		EventCallbacks.clear();
	}


	void virtual RegisterEvent(Script* script, void** filters)
	{
		// workaround karut's hackiness
		const auto* filtersArr = reinterpret_cast<GenericFilters*>(filters);

		uint32_t maxFilters = this->numMaxFilters;
		for (auto it = this->EventCallbacks.begin(); it != this->EventCallbacks.end(); ++it)
		{
			if (script == it->ScriptForEvent)
			{
				if (!maxFilters) return;
				if (!it->eventFilter->GetNumFilters()) continue;
				uint32_t i = 0; // filter iterator
				for (; i < maxFilters; i++)
				{
					if (!(it->eventFilter->IsFilterEqual(filtersArr[i], i))) break;
				}
				if (i >= maxFilters) return;
			}

		}
		std::shared_lock rLock(QueueRWLock);
		for (auto it = this->EventQueueAdd.begin(); it != this->EventQueueAdd.end(); ++it)
		{

			if (script == it->ScriptForEvent)
			{
				if (!maxFilters) return;
				if (!it->eventFilter->GetNumFilters()) continue;
				uint32_t i = 0; // filter iterator
				for (; i < maxFilters; i++)
				{
					if (!(it->eventFilter->IsFilterEqual(filtersArr[i], i))) break;
				}
				if (i >= maxFilters) return;
			}

		}
		rLock.unlock();
		BaseEventClass NewEvent;
		NewEvent.ScriptForEvent = script;
		NewEvent.capturedLambdaVars = LambdaVarContext(script);
		if (maxFilters)
		{

			*(void**)&(NewEvent.eventFilter) = this->CreateFilter(filters, maxFilters);
			NewEvent.eventFilter->SetUpFiltering();
		}
		std::unique_lock wLock(QueueRWLock);
		this->EventQueueAdd.push_back(std::move(NewEvent));
	}
	void virtual RemoveEvent(Script* script, void** filters)
	{
		auto it = EventCallbacks.begin();
		while (it != EventCallbacks.end())
		{
			if (script == it->ScriptForEvent)
			{
				if (auto eventFilters = it->eventFilter)
				{
					uint32_t maxFilters = eventFilters->GetNumFilters();
					for (uint32_t i = 0; i < maxFilters; i++)
					{
						if (!(it->eventFilter->IsFilterEqual(filters[i], i))) goto NotFound;

					}
				}
				it->SetDeleted(true);
			}
		NotFound:
			it++;

		}
	}
	void virtual AddQueuedEvents()
	{
		EventCallbacks.insert(EventCallbacks.end(), std::make_move_iterator(EventQueueAdd.begin()), std::make_move_iterator(EventQueueAdd.end()));
		EventQueueAdd.clear();
	}
	void virtual DeleteEvents()
	{
		auto it = EventCallbacks.begin();
		while (it != EventCallbacks.end())
		{
			if (it->GetDeleted())
			{

				if (it->eventFilter)
				{
					delete it->eventFilter;
				}

				it = EventCallbacks.erase(it);
				continue;
			}
			it++;

		}
	}
};
typedef EventInformation* EventInfo;
extern std::shared_mutex EventsArrayMutex;
extern std::vector<EventInfo> EventsArray;

EventInfo __cdecl JGCreateEvent(const char* EventName, uint8_t maxArgs, uint8_t maxFilters, void* (__fastcall* CreatorFunction)(void**, uint32_t));
void __cdecl JGFreeEvent(EventInfo& toRemove);