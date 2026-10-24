#include "JohnnyEventPredefinitions.h"

std::shared_mutex EventsArrayMutex;
std::vector<EventInfo> EventsArray;

EventInfo __cdecl JGCreateEvent(const char* EventName, uint8_t maxArgs, uint8_t maxFilters, void* (__fastcall* CreatorFunction)(void**, uint32_t))
{
	std::lock_guard lock(EventsArrayMutex);
	EventInfo eventinfo = new EventInformation(EventName, maxArgs, maxFilters, CreatorFunction);
	EventsArray.push_back(eventinfo);
	return eventinfo;

}


void __cdecl JGFreeEvent(EventInfo& toRemove)
{
	std::lock_guard lock(EventsArrayMutex);
	if (!toRemove) return;
	auto it = std::find(std::begin(EventsArray), std::end(EventsArray), toRemove);
	if (it != EventsArray.end())
	{
		delete* it;
		it = EventsArray.erase(it);
	}
	toRemove = NULL;
}

