#include "JohnnyEventPredefinitions.h"

std::mutex EventsArrayMutex;
std::vector<EventInfo> EventsArray;

#if 0
void* __fastcall GenericCreateFilter(void** Filters, uint32_t numFilters) {
	return new JohnnyEventFiltersForm(Filters, numFilters);
}
#endif

EventInfo FindHandlerInfoByChar(const char* nameToFind)
{
	auto it = EventsArray.begin();
	while (it != EventsArray.end())
	{
		if (!(_stricmp((*it)->EventName, nameToFind)))
			return *it;
		it++;
	}
	return NULL;
}

EventInfo __cdecl JGCreateEvent(const char* EventName, uint8_t maxArgs, uint8_t maxFilters, void* (__fastcall* CreatorFunction)(void**, uint32_t))
{
	std::lock_guard<std::mutex> lock(EventsArrayMutex);
	EventInfo eventinfo = new EventInformation(EventName, maxArgs, maxFilters, CreatorFunction);
	EventsArray.push_back(eventinfo);
	return eventinfo;

}


void __cdecl JGFreeEvent(EventInfo& toRemove)
{
	std::lock_guard<std::mutex> lock(EventsArrayMutex);
	if (!toRemove) return;
	auto it = std::find(std::begin(EventsArray), std::end(EventsArray), toRemove);
	if (it != EventsArray.end())
	{
		delete* it;
		it = EventsArray.erase(it);
	}
	toRemove = NULL;
}

