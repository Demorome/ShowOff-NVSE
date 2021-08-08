#pragma once

#include "decoding.h"

//====For non-event handler hooks====//

//Checks if the address has been changed, to take into account Stewie's Tweaks' implementation (bPickpocketWornItems).
//If the address was changed by something else, uh... Well I don't take that into account.
bool canPlayerPickpocketEqItems();

// From lStewieAl's Tweaks (bImprovedPickpocketing).
void __fastcall ContainerMenuHandleMouseoverAlt(ContainerMenu* menu, void* edx, int a2, int a3);

// From lStewieAl's Tweaks.
int CalculateCombatPickpocketAPCost(ContChangesEntry* item, Actor* target, signed int itemValue, signed int count, bool isItemOwnedByTarget);

// custom pickpocket code, for ripping items straight out of an opponent's hands/pockets.
// Code ripped from lStewieAl's Tweaks (TryPickpocket)
bool __fastcall TryCombatPickpocket(ContChangesEntry* selection, SInt32 count, Actor* actor, signed int itemValue);
void SetContainerSubtitleStringToPickpocketAPCost();
void ContainerHoverItemHook();
void resetPickpocketHooks();

//Replaces a IsInCombat check in the NPC activation code.
bool __fastcall PCCanPickpocketInCombatHOOK(Actor* actor, void* edx);

//Replaces a "GetIsCombatTarget w/ the player" check.
bool __fastcall ShowPickpocketStringInCombat(Actor* actor, void* edx, char a2);

//Still doesn't work even with this...
//Ripped code from somewhere, don't exactly remember - maybe JIP, Stewie or JG.
tList<PlayerCharacter::CompassTarget>* __fastcall ShowPickpocketStringInCombat2(PlayerCharacter* player, void* edx);

double __fastcall PreventRepairButton(ContChangesEntry* entry, int bPercent);

extern UInt32 g_LvlUpMenuUnspentPoints[2];  // Skill points, followed by Perks.


#if _DEBUG
// Below is reserved for messing around with IDA

bool GetCanSleepInOwnedBeds();
void SetCanSleepInOwnedBeds(bool bOn);


#if 0
// spreadArgs's value is really low, must've messed something up with SpreadArg...
void Actor_Spread_PerkModifier_Hook(PerkEntryPointID id, TESObjectREFR* refr, float* spreadArg, ...);
#endif

// End IDA debug stuff
#endif


void HandleGameHooks();