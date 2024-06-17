#include "StewieMagic.h"
#include "SafeWrite.h"
#include "GameEffects.h"


void (*ToggleMenus)(bool toggleON) = ((void(__cdecl*)(bool))(0x703810));
void (*CreateScreenshot)(char* name) = ((void(__cdecl*)(char*))(0x878860));
void (*RefreshItemListBox)(void) = (void (*)(void))0x704AF0;
void (*TransferContainerItem)(int amount) = (void (*)(int))0x75DC80;
void(__cdecl* ApplyIMOD)(TESImageSpaceModifier* imod, float magnitude, NiNode* node) = (void(__cdecl*)(TESImageSpaceModifier*, float, NiNode*))0x5299A0;
void (*ApplyPerkModifiers)(UInt32 entryPointID, TESObjectREFR* perkOwner, void* arg3, ...) = (void(*)(UInt32, TESObjectREFR*, void*, ...))0x5E58F0;
TESImageSpaceModifier* (*GetGetHitIMOD)(void) = (TESImageSpaceModifier * (*)(void))0x5D2860;
float (*GetItemWeight)(TESForm* baseItem, bool isHardcore) = (float(*)(TESForm*, bool))0x48EBC0;
unsigned int(__cdecl* GetRandomNumberInRange)(int a1, int a2) = (unsigned int(__cdecl*)(int, int))(0x944460);
Tile* (__cdecl* GetMenuTile)(UInt32 id) = (Tile * (__cdecl*)(UInt32))0xA09030;
Menu* (__thiscall* TileList__GetMenu)(Tile*) = (Menu * (__thiscall*)(Tile*))0xA03C90;
void(__thiscall* Menu__RegisterTile)(Menu*, Tile*, bool) = (void(__thiscall*)(Menu*, Tile*, bool))0xA1DC70;
void(__thiscall* Menu__HideTitle)(Menu*, bool) = (void(__thiscall*)(Menu*, bool))0xA1DC20;
float (*GetWeaponDPS)(ActorValueOwner* avOwner, TESObjectWEAP* weapon, float condition, UInt8 arg4, ContChangesEntry* entry, UInt8 arg6, UInt8 arg7, int arg8, float arg9, float arg10, UInt8 arg11, UInt8 arg12, TESForm* ammo) =
(float (*)(ActorValueOwner*, TESObjectWEAP*, float, UInt8, ContChangesEntry*, UInt8, UInt8, int, float, float, UInt8, UInt8, TESForm*))0x645380;
float (*GetWeaponDamage)(TESObjectWEAP* weapon, float condition, ContChangesEntry* entry, TESForm* ammo) = (float (*)(TESObjectWEAP*, float, ContChangesEntry*, TESForm*))0x6450F0;
float(__cdecl* ScaleArmorDTDRByCondition)(UInt16 dtDR, float condition) = (float(__cdecl*)(UInt16, float))0x646360;
void(__cdecl* HandleActorValueChange)(ActorValueOwner* avOwner, int avCode, float oldVal, float newVal, ActorValueOwner* avOwner2) =
(void(__cdecl*)(ActorValueOwner*, int, float, float, ActorValueOwner*))0x66EE50;
void(__cdecl* HUDMainMenu_UpdateVisibilityState)(signed int) = (void(__cdecl*)(signed int))(0x771700);
void(__cdecl* PipBoy_SwitchToMapTab)(char, char) = (void(__cdecl*)(char, char))0x704170;
NiNode* (__cdecl* GetSubNodeByName)(NiAVObject*, char*) = (NiNode * (__cdecl*)(NiAVObject*, char*))(0x4AAE30);
TESForm* (__cdecl* LookupFormByName)(const char*) = (TESForm * (__cdecl*)(const char*))(0x483A00);
void* (*NiAllocator)(UInt32 size) = (void* (*)(UInt32))0xAA13E0;
void(__thiscall* NiReleaseThenAdd)(void* address, void* object) = (void(__thiscall*)(void*, void*))(0x66B0D0);
void(__thiscall* HandleStealing)(Actor* thief, TESObjectREFR* target, TESForm* form, int quantity, int value, TESObjectREFR* owner) = (void(__thiscall*)(Actor*, TESObjectREFR*, TESForm*, int, int, TESObjectREFR*))(0x8BFA40);
void(__cdecl* IncPCMiscStat)(MiscStatCode code) = (void(__cdecl*)(MiscStatCode))0x4D5C60;
void(__cdecl* DecPCMiscStat)(MiscStatCode code) = (void(__cdecl*)(MiscStatCode))0x60CAD0;
KarmaTier(__cdecl* GetKarmaTier)(float karma) = (KarmaTier(__cdecl*)(float))0x47E040;

bool __fastcall ContainerMenuCheckIsTargetChild(ContChangesEntry* entry, void* edx, int a1)
{
	TESObjectREFR* ref = ContainerMenu::GetSingleton()->containerRef;
	return ref->GetIsChildSize(true);
}

__declspec(naked) void Actor::UnequipItem(TESForm* objType, UInt32 unk1, ExtraDataList* itemExtraList, UInt32 unk3, bool lockUnequip_unused, UInt32 unk5)
{
	static const UInt32 procAddr = kAddr_UnequipItem;
	__asm	jmp		procAddr
}

__declspec(naked) float ExtraContainerChanges::EntryData::GetItemHealthPerc(bool arg1)
{
	static const UInt32 procAddr = kAddr_GetItemHealthPerc;
	__asm	jmp		procAddr
}

// Detection value is between -100 and 500, < 0 being undetected.
// Checks how well the calling actor can see the "detected" actor.
int Actor::GetDetectionValue(Actor* detected)
{
	if (baseProcess && !baseProcess->processLevel)
	{
		DetectionData* data = baseProcess->GetDetectionData(detected, 0);
		if (data) return data->detectionValue;
	}
	return -100;
}


__declspec(naked) void Tile::SetFloat(UInt32 id, float fltVal, bool bPropagate)
{
	static const UInt32 procAddr = kAddr_TileSetFloat;
	__asm	jmp		procAddr
}

__declspec(naked) void Tile::SetString(UInt32 id, const char* strVal, bool bPropagate)
{
	static const UInt32 procAddr = kAddr_TileSetString;
	__asm	jmp		procAddr
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
		} while (xdlIter = xdlIter->next);
	}
	return NULL;
}

signed int EffectItem::GetSkillCode()
{
	return ThisStdCall<signed int>(0x403EA0, this);
}

// Credits: 99% ripped off from Stewie's GetPlayerHealthEffectsSum() function.
float Actor::GetHealthEffectsSum()
{
	auto const effects = this->magicTarget.GetEffectList();
	float sum = 0;
	for (auto iter = effects->Begin(); !iter.End(); ++iter)
	{
		auto effect = iter.Get();
		EffectItem* effectItem = effect->effectItem;
		if (!effect->bApplied || effect->duration <= 0 || !effectItem || effectItem->GetSkillCode() != kAVCode_Health || !effectItem->setting) continue;

		sum += effectItem->magnitude * (effect->duration - effect->timeElapsed);
	}
	float const medicineSkillMult = this->cvOwner.GetMedicineSkillMult(); //todo: check if this only applies to player.
	sum *= medicineSkillMult;
	//todo: apply Survival skill mult too?

	ApplyPerkModifiers(kPerkEntry_ModifyRecoveredHealth, this, &sum);  //todo: check if this only applies to player.
	//todo: look into 0x406A84, and try to find other places where healing is applied.
	// 0x81660B is also relevant.

	return sum;
}

namespace Radio
{
	TESObjectREFR* GetCurrentStation()
	{
		TESObjectREFR* station = nullptr;
		if (RadioEntry::GetSingleton())
		{
			station = RadioEntry::GetSingleton()->radioRef;
		}

		return station;
	}
	
	void (*SetEnabled)(bool toggleON) = (void(__cdecl*)(bool))0x8324E0;
	void (*SetStation)(TESObjectREFR* station, bool toggleON) = (void(__cdecl*)(TESObjectREFR*, bool))0x832240;
	bool GetEnabled() { return *(UInt8*)0x11DD434; }
	tList<TESObjectREFR>* GetFoundStations() { return (tList<TESObjectREFR>*)0x11DD59C; };
	void GetNearbyStations(tList<TESObjectREFR>* dst)
	{
		CdeclCall(0x4FF1A0, g_thePlayer, dst, nullptr);
	}

	void SetActiveStation(TESObjectREFR* station)
	{
		if (GetEnabled())
		{
			// stops the current playing dialogue line
			SetEnabled(false);
		}

		SetEnabled(true);

		SetStation(station, true);
	}
}