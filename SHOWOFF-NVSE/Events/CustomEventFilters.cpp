#include "CustomEventFilters.h"

void* __fastcall CreateOneFormOneStringFilter(void** Filters, uint32_t numFilters_unused) {
	return new JohnnyEventFiltersOneFormOneString(Filters);
}