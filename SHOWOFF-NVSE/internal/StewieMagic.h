#pragma once
#include "GameProcess.h"
#include "jip_nvse.h"
#include "decoding.h"

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

bool __fastcall ContainerMenuCheckIsTargetChild(ContChangesEntry* entry, void* edx, int a1);

extern void (*ToggleMenus)(bool toggleON);
extern void (*CreateScreenshot)(char* name);
extern void (*RefreshItemListBox)(void);
extern void (*TransferContainerItem)(int amount);
extern void(__cdecl* ApplyIMOD)(TESImageSpaceModifier* imod, float magnitude, NiNode* node);
extern void (*ApplyPerkModifiers)(UInt32 entryPointID, TESObjectREFR* perkOwner, void* arg3, ...);
extern TESImageSpaceModifier* (*GetGetHitIMOD)(void);
extern float (*GetItemWeight)(TESForm* baseItem, bool isHardcore);
extern unsigned int(__cdecl* GetRandomNumberInRange)(int a1, int a2);
extern Tile* (__cdecl* GetMenuTile)(UInt32 id);
extern Menu* (__thiscall* TileList__GetMenu)(Tile*);
extern void(__thiscall* Menu__RegisterTile)(Menu*, Tile*, bool);
extern void(__thiscall* Menu__HideTitle)(Menu*, bool);
extern float (*GetWeaponDPS)(ActorValueOwner* avOwner, TESObjectWEAP* weapon, float condition, UInt8 arg4, ContChangesEntry* entry, UInt8 arg6, UInt8 arg7, int arg8, float arg9, float arg10, UInt8 arg11, UInt8 arg12, TESForm* ammo);
extern float (*GetWeaponDamage)(TESObjectWEAP* weapon, float condition, ContChangesEntry* entry, TESForm* ammo);
extern float(__cdecl* ScaleArmorDTDRByCondition)(UInt16 dtDR, float condition);
extern void(__cdecl* HandleActorValueChange)(ActorValueOwner* avOwner, int avCode, float oldVal, float newVal, ActorValueOwner* avOwner2);
extern void(__cdecl* HUDMainMenu_UpdateVisibilityState)(signed int);
extern void(__cdecl* PipBoy_SwitchToMapTab)(char, char);
extern NiNode* (__cdecl* GetSubNodeByName)(NiAVObject*, char*);
extern TESForm* (__cdecl* LookupFormByName)(char*);
extern void* (*NiAllocator)(UInt32 size);
extern void(__thiscall* NiReleaseThenAdd)(void* address, void* object);
extern void(__thiscall* HandleStealing)(Actor* thief, TESObjectREFR* target, TESForm* form, int quantity, int value, TESObjectREFR* owner);
extern void(__cdecl* IncPCMiscStat)(MiscStatCode code);
extern void(__cdecl* DecPCMiscStat)(MiscStatCode code);

enum KarmaTier
{
	Good = 0x0,
	NeutralKarma = 0x1,
	Evil = 0x2,
	VeryGood = 0x3,
	VeryEvil = 0x4,
};

extern KarmaTier(__cdecl* GetKarmaTier)(float karma);

// can be passed to QueueUIMessage to determine Vaultboy icon displayed
enum eEmotion {
	neutral = 0,
	happy = 1,
	sad = 2,
	pain = 3
};

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