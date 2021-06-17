#pragma once
#include "GameProcess.h"
#include "jip_nvse.h"
#include "decoding.h"
#include "SafeWrite.h"

// Everything here was ripped from lStewieAl.


enum
{
	kAddr_GetItemHealthPerc = 0x4BCDB0,
	kAddr_GetItemValue = 0x4BD400,
	kAddr_ReadXML = 0xA01B00,
	kAddr_TileFromTemplate = 0xA1DDB0,
	kAddr_EvaluateConditions = 0x680C60,
	kAddr_EvaluateCondition = 0x681600,
};

bool __fastcall ContainerMenuCheckIsTargetChild(ContChangesEntry* entry, void* edx, int a1)
{
	TESObjectREFR* ref = ContainerMenu::GetSingleton()->containerRef;
	return ref->GetIsChildSize(true);
}

__declspec(naked) void Actor::UnequipItem(TESForm* objType, UInt32 unk1, ExtraDataList* itemExtraList, UInt32 unk3, bool lockUnequip, UInt32 unk5)
{
	static const UInt32 procAddr = kAddr_UnequipItem;
	__asm	jmp		procAddr
}

__declspec(naked) float ExtraContainerChanges::EntryData::GetItemHealthPerc(bool arg1)
{
	static const UInt32 procAddr = kAddr_GetItemHealthPerc;
	__asm	jmp		procAddr
}

/*
__declspec(naked) float ExtraContainerChanges::EntryData::GetValue()
{
	static const UInt32 procAddr = kAddr_GetItemValue;
	__asm jmp procAddr
}
*/

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
TESForm* (__cdecl* LookupFormByName)(char*) = (TESForm * (__cdecl*)(char*))(0x8D5910);
void* (*NiAllocator)(UInt32 size) = (void* (*)(UInt32))0xAA13E0;
void(__thiscall* NiReleaseThenAdd)(void* address, void* object) = (void(__thiscall*)(void*, void*))(0x66B0D0);
void(__thiscall* HandleStealing)(Actor* thief, TESObjectREFR* target, TESForm* form, int quantity, int value, TESObjectREFR* owner) = (void(__thiscall*)(Actor*, TESObjectREFR*, TESForm*, int, int, TESObjectREFR*))(0x8BFA40);
enum MiscStatCode;
void(__cdecl* IncPCMiscStat)(MiscStatCode code) = (void(__cdecl*)(MiscStatCode))0x4D5C60;
void(__cdecl* DecPCMiscStat)(MiscStatCode code) = (void(__cdecl*)(MiscStatCode))0x60CAD0;

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


enum KarmaTier
{
	Good = 0x0,
	NeutralKarma = 0x1,
	Evil = 0x2,
	VeryGood = 0x3,
	VeryEvil = 0x4,
};

KarmaTier(__cdecl* GetKarmaTier)(float karma) = (KarmaTier(__cdecl*)(float))0x47E040;

// can be passed to QueueUIMessage to determine Vaultboy icon displayed
enum eEmotion {
	neutral = 0,
	happy = 1,
	sad = 2,
	pain = 3
};

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

enum QueuedMenuRequestType
{
	kContainerMenu = 1,
	kHackingMenu = 2,
	kComputerMenu = 3,
	kDialogMenu = 4,
	kRaceSexMenu = 5,
	kStartMenu = 6,
	kMessageMenu = 7,
	kCompanionWheelMenu = 8,
	kPlayerNameMenu = 9
};

enum ContainerMode
{
	kNormal = 1,
	kPickpocket = 2,
	kTeammate = 3,
	kRockItLauncher = 4
};

/*
void _usercall HandleActivationMenuQueueing(QueuedMenuRequestType menu, int interactionObject, int a3, int a4, ContainerMode containerType)
{
	
}
*/