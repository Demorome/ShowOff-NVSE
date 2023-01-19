#pragma once
//#include "GameBSExtraData.h"
#include "GameObjects.h"

// Copied from JIP
struct InventoryRef
{
	struct Data
	{
		TESForm* type;	// 00
		ExtraContainerChanges::EntryData* entry;	// 04
		ExtraDataList* xData;	// 08
	} data;
	TESObjectREFR* containerRef;	// 0C
	TESObjectREFR* tempRef;		// 10
	UInt8				pad14[24];		// 14
	bool				doValidation;	// 2C
	bool				removed;		// 2D
	UInt8				pad2E[2];		// 2E

	SInt32 GetCount() const { return data.entry->countDelta; }
	ExtraDataList* CreateExtraData();
	// Returns the copied-to ContChangesEntry*, if it exists.
	ContChangesEntry* CopyWithNewExtraData(ExtraDataList* newDataList);
};
