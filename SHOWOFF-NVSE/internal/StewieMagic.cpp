#include "StewieMagic.h"
#include "SafeWrite.h"
#include "GameEffects.h"

void (*ApplyPerkModifiers)(uint32_t entryPointID, TESObjectREFR* perkOwner, void* arg3, ...) = (void(*)(uint32_t, TESObjectREFR*, void*, ...))0x5E58F0;
void(__cdecl* IncPCMiscStat)(MiscStatCode code) = (void(__cdecl*)(MiscStatCode))0x4D5C60;
void(__cdecl* DecPCMiscStat)(MiscStatCode code) = (void(__cdecl*)(MiscStatCode))0x60CAD0;
KarmaTier(__cdecl* GetKarmaTier)(float karma) = (KarmaTier(__cdecl*)(float))0x47E040;

bool __fastcall ContainerMenuCheckIsTargetChild(ContChangesEntry* entry, void* edx, int a1)
{
	TESObjectREFR* ref = ContainerMenu::GetSingleton()->containerRef;
	return ref->GetIsChildSize(true);
}

void Actor::UnequipItem(TESForm* objType, uint32_t unk1, ExtraDataList* itemExtraList, bool unk3, bool lockUnequip_unused, bool unk5)
{
	ThisCall(0x88C790, this, objType, unk1, itemExtraList, unk3, lockUnequip_unused, unk5);
}

void Tile::SetFloat(uint32_t id, float fltVal, bool bPropagate)
{
	ThisCall(0xA012D0, this, id, fltVal, bPropagate);
}

void Tile::SetString(uint32_t id, const char* strVal, bool bPropagate)
{
	ThisCall(0xA01350, this, id, strVal, bPropagate);
}


ExtraDataList* ExtraContainerChanges::EntryData::GetEquippedExtra()
{
	if (extendData)
	{
		ExtraDataList* xData;
		ListNode<ExtraDataList>* xdlIter = extendData->Head();
		do
		{
			xData = xdlIter->data;
			if (xData && xData->HasType(kExtraData_Worn))
				return xData;
		} while ((xdlIter = xdlIter->next));
	}
	return NULL;
}