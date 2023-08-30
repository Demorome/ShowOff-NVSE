#include "CustomEventFilters.h"

void* __fastcall CreateOneFormOneStringFilter(void** Filters, UInt32 numFilters_unused) {
	return new JohnnyEventFiltersOneFormOneString(Filters);
}