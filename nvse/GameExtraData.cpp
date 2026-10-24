#include "GameExtraData.h"
#include "GameBSExtraData.h"
#include "GameAPI.h"
#include "GameObjects.h"
#include "GameRTTI.h"
#include "GameScript.h"

#include "utility.h"


EquipDataSet FindEquippedItems(ExtraContainerChanges* contChanges, uint32_t const filterFlags)
{
	using namespace FindEquipped;  // make flag enum values visible.
	bool const skipUnplayable = (filterFlags & iFilter_NoUnplayable) || !filterFlags;
	bool const noQuestItems = filterFlags & iFilter_NoQuestItems;
	bool const noSlotlessItems = filterFlags & iFilter_NoSlotlessItems;

	EquipDataSet eqItems;  // Return value.
	
	if (contChanges->data && contChanges->data->objList) {
		for (auto itemIter = contChanges->data->objList->Begin(); !itemIter.End(); ++itemIter)  // see GetHotkeyItem from NVSE for similar loop example.
		{
			TESForm* itemForm = itemIter->type;
			if (!itemForm) continue;

			// Check if the item passes the flag filter checks.
			if (skipUnplayable && !IsEquipableItemPlayable(itemForm)) continue;
			if (noQuestItems && itemForm->IsQuestItem()) continue;

			// Slotless items automatically pass the equipSlot filter, unless noSlotlessItems is on.
			auto const formSlotMask = GetFormEquipSlotMask(itemForm);
			if (noSlotlessItems && !formSlotMask) continue;
			
			// Here, filterFlags is used purely for the first 19 bits it has (equip slot bits).
			// If it is null, then skip the check (by default, any equip slot forms are allowed).
			if (formSlotMask && filterFlags && !MatchAnyEquipSlots(formSlotMask, filterFlags)) 
				continue;

			// Form conditions passed, now check if the form is actually equipped.
			for (auto xDataIter = itemIter->extendData->Begin(); !xDataIter.End(); ++xDataIter)
			{
				if (xDataIter->HasType(kExtraData_Worn)) {
					// If so, add it to the list and go check the next item.
					EquipData m_found{itemForm, xDataIter.Get()};
					eqItems.emplace(m_found);
					break;
				}
			}
		}
	}
	return eqItems;
}

static_assert(sizeof(ExtraHealth) == 0x10);
static_assert(sizeof(ExtraLock) == 0x10);
static_assert(sizeof(ExtraCount) == 0x10);
static_assert(sizeof(ExtraTeleport) == 0x10);

static_assert(sizeof(ExtraWorn) == 0x0C);
static_assert(sizeof(ExtraWornLeft) == 0x0C);
static_assert(sizeof(ExtraCannotWear) == 0x0C);
static_assert(sizeof(ExtraContainerChanges::EntryData) == 0x0C);

#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
static const uint32_t s_ExtraContainerChangesVtbl					= 0x01015BB8;	//	0x0100fb78;
static const uint32_t s_ExtraWornVtbl								= 0x01015BDC;
//static const uint32_t s_ExtraWornLeftVtbl							= 0x01015BE8;
static const uint32_t s_ExtraCannotWearVtbl						= 0x01015BF4;

static const uint32_t s_ExtraOwnershipVtbl						= 0x010158B4;	//	0x0100f874;
static const uint32_t s_ExtraRankVtbl								= 0x010158CC;	//	0x0100f88c;
static const uint32_t s_ExtraActionVtbl							= 0x01015BAC;	
static const uint32_t s_ExtraFactionChangesVtbl					= 0x01015F30;
static const uint32_t s_ExtraScriptVtbl							= 0X1015914;

//static const uint32_t s_ExtraScript_init							= 0x0042C760;

static const uint32_t s_ExtraHealthVtbl = 0x010158E4;
static const uint32_t s_ExtraLockVtbl = 0x0101589C;
static const uint32_t s_ExtraCountVtbl = 0x010158D8;
static const uint32_t s_ExtraTeleportVtbl = 0x010158A8;
static const uint32_t s_ExtraWeaponModFlagsVtbl = 0x010159A4;

static const uint32_t s_ExtraHotkeyVtbl = 0x0101592C;

static const uint32_t s_ExtraSemaphore	= 0x011C3920;
static const uint32_t s_SemaphoreWait		= 0x0040FBF0;
static const uint32_t s_SemaphoreLeave	= 0x0040FBA0;
#else
#error
#endif

void ExtraContainerChanges::EntryData::Cleanup()
{
	if (!extendData) return;
	ListNode<ExtraDataList> *xdlIter = extendData->Head(), *prev = NULL;;
	ExtraDataList *xData;
	ExtraCount *xCount;
	do
	{
		xData = xdlIter->data;
		if (xData)
		{
			xCount = (ExtraCount*)xData->GetByType(kExtraData_Count);
			if (xCount && (xCount->count <= 1)) xData->RemoveByType(kExtraData_Count);
			if (xData->m_data)
			{
				prev = xdlIter;
				xdlIter = xdlIter->next;
				continue;
			}
		}
		GameHeapFree(xData);
		xdlIter = prev ? prev->RemoveNext() : xdlIter->RemoveMe();
	}
	while (xdlIter);
}

ExtraContainerChanges::Data *ExtraContainerChanges::Data::Create(TESObjectREFR *owner)
{
	Data *data = (Data*)GameHeapAlloc(sizeof(Data));
	MemZero(data, sizeof(Data));
	data->owner = owner;
	data->objList = (EntryDataList*)GameHeapAlloc(sizeof(EntryDataList));
	data->objList->Init();

	//additions from JIP
	data->totalWgCurrent = -1.0F;
	data->totalWgLast = -1.0F;
	data->byte10 = 0;
	
	return data;
}

void ExtraContainerChanges::Cleanup()
{
	if (data && data->objList)
	{
		ListNode<EntryData> *entryIter = data->objList->Head();
		do
		{
			if (entryIter->data)
				entryIter->data->Cleanup();
		}
		while ((entryIter = entryIter->next));
	}
}

BSExtraData *BSExtraData::Create(uint8_t xType, uint32_t size, uint32_t vtbl)
{
	BSExtraData *xData = (BSExtraData*)GameHeapAlloc(size);
	MemZero(xData, size);
	*(uint32_t*)xData = vtbl;
	xData->type = xType;
	return xData;
}

uint32_t GetCountForExtraDataList(ExtraDataList* list)
{
	if (!list)
		return 1;

	ExtraCount* xCount = (ExtraCount*)list->GetByType(kExtraData_Count);
	return xCount ? xCount->count : 1;
}

ExtraSecuritronFace* ExtraSecuritronFace::Create()
{
	ExtraSecuritronFace* pData = (ExtraSecuritronFace*)GameHeapAlloc(sizeof(ExtraSecuritronFace));
	ThisCall(0x42CCC0, pData);
	return pData;
}