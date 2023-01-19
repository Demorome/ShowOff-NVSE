#include "InventoryRef.h"

// Copied from JIP
ExtraDataList* InventoryRef::CreateExtraData()
{
	ContChangesEntry* pEntry = containerRef->GetContainerChangesEntry(data.type);
	if (!pEntry) return nullptr;
	data.xData = ExtraDataList::Create();
	if (pEntry->extendData)
		pEntry->extendData->Prepend(data.xData);
	else
	{
		pEntry->extendData = (ExtraContainerChanges::ExtendDataList*)GameHeapAlloc(8);
		pEntry->extendData->Init(data.xData);
	}
	containerRef->MarkAsModified(0x20);
	return data.xData;
}

// Precondition: newDataList is not nullptr.
ContChangesEntry* InventoryRef::CopyWithNewExtraData(ExtraDataList* newDataList)
{
	ContChangesEntry* pEntry = containerRef->GetContainerChangesEntry(data.type);
	if (!pEntry) return nullptr;
	data.xData = newDataList;
	if (pEntry->extendData)
		pEntry->extendData->Prepend(data.xData);
	else
	{
		pEntry->extendData = (ExtraContainerChanges::ExtendDataList*)GameHeapAlloc(8);
		pEntry->extendData->Init(data.xData);
	}
	containerRef->MarkAsModified(0x20);
	return pEntry;
}