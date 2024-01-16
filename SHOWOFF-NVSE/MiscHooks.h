#pragma once
#include "decoding.h"
#include "SOTypes.h"

enum IDAFunctionNames
{
};

namespace HandleHooks
{
	void HandleGameHooks();
	void HandleDelayedGameHooks();
	void HandleSuperDelayedGameHooks();

	//see ShowOffEvents.h
	void HandleEventHooks(); 
	void HandleDelayedEventHooks();
	void HandleSuperDelayedEventHooks();
}

namespace PickpocketEquippedItems
{
	bool CanPlayerPickpocketEqItems();
}

namespace PickpocketInCombat
{
	void __fastcall ContainerMenuHandleMouseoverAlt(ContainerMenu* menu, void* edx, int a2, int a3);
	int CalculateCombatPickpocketAPCost(ContChangesEntry* item, Actor* target, signed int itemValue, signed int count, bool isItemOwnedByTarget);
	bool __fastcall TryCombatPickpocket(ContChangesEntry* selection, SInt32 count, Actor* actor, signed int itemValue);
	void SetContainerSubtitleStringToPickpocketAPCost();
	void ContainerHoverItemHook();
	void ResetPickpocketHooks();
	bool __fastcall PCCanPickpocketInCombatHOOK(Actor* actor, void* edx);
	bool __fastcall ShowPickpocketStringInCombat(Actor* actor, void* edx, char a2);
	tList<PlayerCharacter::CompassTarget>* __fastcall ShowPickpocketStringInCombat2(PlayerCharacter* player, void* edx);
}

namespace GetCompassTargets
{
	using CompassTarget = PlayerCharacter::CompassTarget;

	// Caches CompassTargets that are actually shown in compass.
	// Should be refreshed every frame.
	extern std::vector<CompassTarget> g_TargetsInCompass;
}

namespace SetItemHotkeyIconPath
{
	extern std::unordered_map<RefID, std::string> g_hotkeyIconOverrides;
}

namespace FreezeAmmoRegen
{
	extern bool g_freezeAmmoRegen;
}

namespace SetForceDrawHitscanProjectiles
{
	extern bool g_isForcingProjectilesToDraw;
}
