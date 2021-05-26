#pragma once
#include "GameData.h"
#include "SafeWrite.h"
#include "internal/StewieMagic.h"
#include "GameProcess.h"
#include "settings.h"


// Globals - for INI globals, see settings.h

UInt32 g_ShowOffVersion = 100;

bool g_canPlayerPickpocketInCombat = false;

extern Vector<ArrayElementL> s_tempElements;

// Misc.
bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);
#define NUM_ARGS *((UInt8*)scriptData + *opcodeOffsetPtr)  //Probably breaks Compiler Override!!
#define REFR_RES *(UInt32*)result  //From JIP

#define IS_TYPE(form, type) (*(UInt32*)form == kVtbl_##type)  //already defined in GameForms.h
#define NOT_ID(form, type) (form->typeID != kFormType_##type) //already defined in GameForms.h
#define IS_ID(form, type) (form->typeID == kFormType_##type) //already defined in GameForms.h

#define RegisterScriptCommand(name) 	nvse->RegisterCommand(&kCommandInfo_ ##name);
#define REG_CMD(name) nvse->RegisterCommand(&kCommandInfo_##name);  //Short version of RegisterScriptCommand.
#define REG_TYPED_CMD(name, type) nvse->RegisterTypedCommand(&kCommandInfo_##name,kRetnType_##type);
#define REG_CMD_STR(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_String) // From JIP_NVSE.H
#define REG_CMD_ARR(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Array) // From JIP_NVSE.H

#define VarNameSize 64

typedef NVSEArrayVarInterface::Array NVSEArrayVar;
typedef NVSEArrayVarInterface::Element NVSEArrayElement;
bool (*ExtractFormatStringArgs)(UInt32 fmtStringPos, char* buffer, COMMAND_ARGS_EX, UInt32 maxParams, ...);  // From JIP_NVSE.H


//Imports from JG
class EventInformation;
typedef EventInformation* EventInfo;
typedef EventInfo (*JGCreateEvent)(const char* EventName, UInt8 maxArgs, UInt8 maxFilters, void* (__fastcall* CreatorFunction)(void**, UInt32));



// Singletons and shortcuts for those singletons.
NVSEArrayVarInterface* g_arrInterface = NULL;
NVSEArrayVar* (*CreateArray)(const NVSEArrayElement* data, UInt32 size, Script* callingScript);
NVSEArrayVar* (*CreateStringMap)(const char** keys, const NVSEArrayElement* values, UInt32 size, Script* callingScript);
bool (*AssignArrayResult)(NVSEArrayVar* arr, double* dest);
void (*SetElement)(NVSEArrayVar* arr, const NVSEArrayElement& key, const NVSEArrayElement& value);
void (*AppendElement)(NVSEArrayVar* arr, const NVSEArrayElement& value);
UInt32(*GetArraySize)(NVSEArrayVar* arr);
NVSEArrayVar* (*LookupArrayByID)(UInt32 id);
bool (*GetElement)(NVSEArrayVar* arr, const NVSEArrayElement& key, NVSEArrayElement& outElement);
bool (*GetElements)(NVSEArrayVar* arr, NVSEArrayElement* elements, NVSEArrayElement* keys);

NVSEStringVarInterface* g_strInterface = NULL;
bool (*AssignString)(COMMAND_ARGS, const char* newValue);
const char* (*GetStringVar)(UInt32 stringID);

NVSEMessagingInterface* g_msg = NULL;
NVSEScriptInterface* g_script = NULL;
NVSECommandTableInterface* CmdIfc = NULL;




HUDMainMenu* g_HUDMainMenu = NULL;
TileMenu** g_tileMenuArray = NULL;
UInt32 g_screenWidth = 0;
UInt32 g_screenHeight = 0;

PlayerCharacter* g_thePlayer = nullptr;
ActorValueOwner* g_playerAVOwner = NULL;
ProcessManager* g_processManager = nullptr;
InterfaceManager* g_interfaceManager = nullptr;
BSWin32Audio* g_bsWin32Audio = nullptr;
DataHandler* g_dataHandler = nullptr;
BSAudioManager* g_audioManager = nullptr;
Sky** g_currentSky = nullptr;


//---Hooks and Hook Stuff

bool canPlayerPickpocketEqItems() { return *(UInt32*)0x75E87B != 0xFFD5F551; }
//Checks if the address has been changed, to take into account Stewie's Tweaks' implementation.
//If the address was changed by something else, uh... Well I don't take that into account.


void __fastcall ContainerMenuHandleMouseoverAlt(ContainerMenu* menu, void* edx, int a2, int a3)
{
	// if not in pickpocket mode return
	if (menu->mode != ContainerMenu::Mode::kPickpocket || !IS_TYPE(menu->containerRef, Character)) return;

	TileMenu* tileMenu = menu->tile;
	Tile* subtitlesTile = CdeclCall<Tile*>(0xA08B20, tileMenu, "CM_SUBTITLE");
	if (subtitlesTile) subtitlesTile->SetString(kTileValue_string, "", 1);
	ThisStdCall(0x75CF70, menu, a2, a3);
}

int CalculateCombatPickpocketAPCost(ContChangesEntry* item, Actor* target, signed int itemValue, signed int count, bool isItemOwnedByTarget)
{
	// Average player (5 AGL) will have ~80 action points
	// Meaning, to balance this out, AP costs should refrain from going over 80 (save for some exceptions).

	// Detection value won't be taken into account - this is a brute-force kind of approach, anyone will see it coming, unless they're unconscious.
	// Still, it's a Sleight of Hand trick, so the target's Perception will be important, as will the player's Sneak + Agility (used as an intermediary for Sleight of Hand).
	// If the enemy is perceptive, it'll be trickier for the player to do their trick, so also take targetPerception into account.

	int playerAgility = g_playerAVOwner->GetThresholdedActorValue(kAVCode_Agility);
	int playerSneak = g_playerAVOwner->GetThresholdedActorValue(kAVCode_Sneak);
	int targetPerception = target->avOwner.GetThresholdedActorValue(kAVCode_Perception);
	float itemWeight = item->type->GetWeight() * count;

	float cost = g_fForcePickpocketBaseAPCost
		- playerAgility * g_fForcePickpocketPlayerAgilityMult
		- playerSneak * g_fForcePickpocketPlayerSneakMult
		+ targetPerception * g_fForcePickpocketTargetPerceptionMult
		+ itemWeight * g_fForcePickpocketItemWeightMult
		;

	if (isItemOwnedByTarget)
	{
		cost += itemValue * g_fForcePickpocketItemValueMult;  //Value only matters to the opponent; it determines how much they want to hold onto something.
		if (item->GetEquippedExtra())
		{
			// If the item is equipped ON THE TARGET, the target's Strength will be taken into account to apply a penalty.
			// This penalty can be minimized with the player's own Strength, which will also only be taken into account if the item being stolen is equipped ON THE TARGET.
			
			int targetStrength = target->avOwner.GetThresholdedActorValue(kAVCode_Strength);
			int playerStrength = g_playerAVOwner->GetThresholdedActorValue(kAVCode_Strength);

			cost += max(0, (g_fForcePickpocketTargetStrengthMult * targetStrength
					- g_fForcePickpocketPlayerStrengthMult * playerStrength));
					//0 or greater.
		}
	}

	cost = min(cost, g_fForcePickpocketMaxAPCost); //yes, the choice of global setting is intentional.
	cost = max(cost, g_fForcePickpocketMinAPCost);
	
	return cost;
}

// custom pickpocket code, for ripping items straight out of an opponent's hands/pockets.
bool __fastcall TryCombatPickpocket(ContChangesEntry* selection, SInt32 count, Actor* actor, signed int itemValue)
{
	bool wasSuccessful = true;
	ContainerMenu* menu = ContainerMenu::GetSingleton();

	if (actor && /*itemValue > 0 &&*/ !actor->GetHasKnockedState())
	{
		bool isItemOwnedByTarget = menu->currentItems == &menu->rightItems;
		int actionPointCost = CalculateCombatPickpocketAPCost(selection, actor, itemValue, count, isItemOwnedByTarget);
		int playerActionPoints = g_playerAVOwner->GetActorValue(kAVCode_ActionPoints);
		if (actionPointCost > playerActionPoints)
		{
			ThisStdCall(0x8C00E0, g_thePlayer, actor, 0, 0);
			char buf[260];
			sprintf(buf, "%s", g_fForcePickpocketFailureMessage);
			QueueUIMessage(buf, eEmotion::sad, NULL, NULL, 2.0, 0);

			//menu->hasFailedPickpocket= true; //not sure what the point of this is.
			wasSuccessful = false;
		}
		else
		{
			g_thePlayer->DamageActionPoints(actionPointCost);

			/*
			if (menu->currentItems == &menu->rightItems && !menu->hasPickedPocket)
			{
				IncPCMiscStat(kMiscStat_PocketsPicked);
				menu->hasPickedPocket = true;

				if (g_bPickpocketRewardXP)
				{
					float itemWeight = selection->type->GetWeight() * count;
					int targetPerception = actor->avOwner.GetThresholdedActorValue(kAVCode_Perception);
					g_thePlayer->RewardXP(itemWeight * g_fPickpocketItemWeightMult + (itemValue * g_fPickpocketItemValueMult) + targetPerception);
				}

			}*/
		}

		// decrease the player's karma if they didn't try to steal from someone evil or very evil
		float stolenActorKarma = actor->avOwner.GetActorValue(kAVCode_Karma);
		KarmaTier stolenKarmaTier = GetKarmaTier(stolenActorKarma);
		if (stolenKarmaTier != KarmaTier::Evil && stolenKarmaTier != KarmaTier::VeryEvil)
		{
			int karmaMod = *(float*)0x11CDE24; // fKarmaModStealing
			ThisStdCall(0x94FD30, g_thePlayer, karmaMod);
		}
	}
	return wasSuccessful;
}

void SetContainerSubtitleStringToPickpocketAPCost()
{
	ContainerMenu* container = ContainerMenu::GetSingleton();

	// if not in pickpocket mode return
	if (container->mode != ContainerMenu::Mode::kPickpocket || !IS_TYPE(container->containerRef, Character)) return;

	// get the subtitles tile
	TileMenu* tileMenu = container->tile;

	Tile* subtitlesTile = CdeclCall<Tile*>(0xA08B20, tileMenu, "CM_SUBTITLE");
	if (!subtitlesTile) return;

	bool isStealNotPlace = (container->currentItems == &container->rightItems);
	int itemValue = isStealNotPlace ? ThisStdCall<int>(0x75E240, container, ContainerMenu::GetSelection(), 1, 1) : 1;

	bool isItemOwnedByTarget = container->currentItems == &container->rightItems;
	int actionPointCost = CalculateCombatPickpocketAPCost(ContainerMenu::GetSelection(), (Actor*)container->containerRef, itemValue, 1, isItemOwnedByTarget);
	int currentAP = g_playerAVOwner->GetActorValue(kAVCode_ActionPoints);
	char buf[260];
	sprintf(buf, "Action Point Cost: %d / %d", actionPointCost, currentAP);
	subtitlesTile->SetString(kTileValue_string, buf, 1);
}

__declspec(naked) void ContainerHoverItemHook()
{
	static const UInt32 retnAddr = 0x75CEDA;
	_asm
	{
		call	SetContainerSubtitleStringToPickpocketAPCost

		originalCode :
			mov		ecx, [ebp - 4]
			mov		edx, [ecx + 0x5C]
			jmp		retnAddr
	}
}

bool IsPickpocketHookSet = false;
UINT32 PickpocketFuncAddr1 = 0;
UINT32 PickpocketFuncAddr2 = 0;
UINT32 PickpocketFuncAddr3 = 0;

void resetPickpocketHooks()
{	
	//Undo function hooks.
	WriteRelCall(0x75DBDA, UINT32(PickpocketFuncAddr1));
	WriteRelCall(0x75DFA7, UINT32(PickpocketFuncAddr2));
	WriteRelJump(0x75CED4, UINT32(PickpocketFuncAddr3));
	SafeWrite8(0x5FA8E4, 0x74);
	SafeWrite8(0x608200, 0x74);

	IsPickpocketHookSet = false;
	Console_Print("Changes offline.");
} 


//Replaces a IsInCombat check in the NPC activation code.
bool __fastcall PCCanPickpocketInCombatHOOK(Actor* actor, void* edx)
{
	bool const isInCombat = ThisStdCall<bool>(0x493BB0, actor);
	if (isInCombat)
	{
		if (g_canPlayerPickpocketInCombat && g_thePlayer->IsSneaking())
		{
			if (!IsPickpocketHookSet)
			{				
				//Replace some function calls, store the OPCodes of the functions we're replacing, set a flag to prevent repeating work.
				IsPickpocketHookSet = true;
				PickpocketFuncAddr1 = GetRelJumpAddr(0x75DBDA);
				PickpocketFuncAddr2 = GetRelJumpAddr(0x75DFA7);
				PickpocketFuncAddr3 = GetRelJumpAddr(0x75CED4);
				//PickpocketFuncAddr4

				//Replace the pickpocket calculation code with an AP cost system.
				WriteRelCall(0x75DBDA, UInt32(TryCombatPickpocket));
				WriteRelCall(0x75DFA7, UInt32(TryCombatPickpocket));

				// Set the pickpocket AP Cost when hovered
				WriteRelJump(0x75CED4, UInt32(ContainerHoverItemHook));

				// Clear the string when unhovered
				SafeWrite32(0x10721C0, UInt32(ContainerMenuHandleMouseoverAlt)); //don't bother changing this back, Stewie does the exact same thing anyways.
																						//Could make this a default hook
				// Jump over the "was player caught by this NPC" check
				SafeWrite8(0x608200, 0xEB);
				SafeWrite8(0x5FA8E4, 0xEB); //for creatures

				Console_Print("Changes online.");

			}
			return false;  // so the pickpocket menu is allowed to open despite the actor being in combat.
		}
		if (!g_canPlayerPickpocketInCombat && IsPickpocketHookSet)
			resetPickpocketHooks();
		return true;
	}
	if (IsPickpocketHookSet)
		resetPickpocketHooks();
	return false;
}

//Replaces a "GetIsCombatTarget w/ the player" check.
bool __fastcall ShowPickpocketStringInCombat(Actor* actor, void* edx, char a2)
{
	bool isInCombat = ThisStdCall<bool>(0x8BC700, actor, g_thePlayer);
	if (isInCombat && g_canPlayerPickpocketInCombat && g_thePlayer->IsSneaking())
	{
		Console_Print("I made a difference!"); //seems like we're failing before this even happens...
		return false;
	}
	return isInCombat;
}

//Still doesn't work even with this...
tList<PlayerCharacter::CompassTarget>* __fastcall ShowPickpocketStringInCombat2(PlayerCharacter* player, void* edx)
{
	if (g_canPlayerPickpocketInCombat)
	{
		//Console_Print("I made a difference 2!"); 
		tList<PlayerCharacter::CompassTarget>* nope = NULL;
		return nope;
	}
	return g_thePlayer->compassTargets;
}

bool __fastcall QueueUIMessageHook(HUDMainMenu* menu, void* edx, char* msgText, eEmotion IconType, char* iconPath, char* soundPath, float displayTime, bool instantEndCurrentMessage)
{
	//dispatch event by looping thru all registered handlers

	//Console_Print("==Testing hook==\n -msgText: %s\n -IconType: %d\n -iconPath: %s\n -soundPath: %s\n -displayTime: %f\n -instantEndCurrentMessage: %d", msgText, IconType, iconPath, soundPath, displayTime, instantEndCurrentMessage);
	
	//Call original function.
	return ThisStdCall_B(0x775380, menu, msgText, IconType, iconPath, soundPath, displayTime, instantEndCurrentMessage);
}


#if 0
Actor* g_backwardsRangedAttackActor = nullptr;

double __fastcall GetHeadingAngle(TESObjectREFR* actor, TESObjectREFR* target)
{
	double headingAngle = 0;
	CdeclCall(0x5A0410, actor, target, 0, &headingAngle);
	return headingAngle;
}

Actor* __fastcall GetBackwardsRangedAttackActor(void* combatProcedure, void* edx)
{
	g_backwardsRangedAttackActor = ThisStdCall<Actor*>(0x996DE0, combatProcedure);
	return g_backwardsRangedAttackActor;
}

bool __fastcall PreventBackwardsRangedAttacks(void* combatState, void* edx)
{
	if (g_backwardsRangedAttackActor)
	{
		Actor* target = ThisStdCall<Actor*>(0x8A0360, g_backwardsRangedAttackActor);
		double const headingAngle = GetHeadingAngle(g_backwardsRangedAttackActor, target);

		if ( (headingAngle <= g_PBRA_MaxAbsHeadingAngle) && (headingAngle >= -g_PBRA_MaxAbsHeadingAngle) )
		{
#if _DEBUG
			Console_Print("PreventBackwardsRangedAttacks -> Actor is facing target.");
#endif
			return ThisStdCall<UInt8>(0x97FB20, combatState);
		}
#if _DEBUG
		Console_Print("PreventBackwardsRangedAttacks -> Actor is NOT facing target.");
#endif
	}
	return false;
}
#endif

//bool g_bRepairButtonPrevented_PBIR;

double __fastcall PreventRepairButton(ContChangesEntry* entry, int bPercent)
{
	auto const result = ThisStdCall<double>(0x4BCDB0, entry, bPercent);
	if (result == 0.0F)
	{
		//g_bRepairButtonPrevented_PBIR = true;
		return 100.0F;  //since you can't repair items at 100% health, I use that bit of code to prevent repairing items at 0% health.
	}
	//g_bRepairButtonPrevented_PBIR = false;
	return result;
}


void HandleGameHooks()
{
	//Modify a "IsInCombat" check to allow NPC activation even if they are in combat.
	WriteRelCall(0x607E70, UINT32(PCCanPickpocketInCombatHOOK));
	//Same thing but for Creatures (like Super Mutants).
	WriteRelCall(0x5FA81F, UINT32(PCCanPickpocketInCombatHOOK));

	// NOTE: When failing the AP cost check, it counts as if you were caught by the actor.
	// This could cause really weird behavior with the NPC just taking back everything that was previously stolen,
	// not to mention you would no longer be able to pickpocket them normally after combat.
	//
	// Possible solution: open and hook the companion loot exchange menu instead?

#if _DEBUG
	//Below isn't working currently...
	WriteRelCall(0x77738A, UINT32(ShowPickpocketStringInCombat));
	WriteRelCall(0x7772C9, UINT32(ShowPickpocketStringInCombat2));
	//WriteRelCall(0x770C0D, UINT32(ShowPickpocketStringInCombat2)); //breaks health target UI
	
	WriteRelCall(0x705379, UINT32(QueueUIMessageHook));
	WriteRelCall(0x7EE74D, UINT32(QueueUIMessageHook));
	WriteRelCall(0x7EE87D, UINT32(QueueUIMessageHook));
	WriteRelCall(0x7EEA6C, UINT32(QueueUIMessageHook));
	WriteRelCall(0x833303, UINT32(QueueUIMessageHook));
	WriteRelCall(0x8B959B, UINT32(QueueUIMessageHook));

	//SafeWrite8(0x9D0D40, 0x0);  //test the fix that prevents actors from shooting behind themselves.
	if (g_PBIR_On)
	{
		WriteRelCall(0x7818B7, UINT32(PreventRepairButton));
		/*
		char buf[260];
		sprintf(buf, "%s", g_PBIR_FailMessage);
		QueueUIMessage(buf, eEmotion::sad, NULL, NULL, 2.0, 0);
		 */
	}
	
#endif

#if 0  //remnants of the past, beaten by Stewie/c6
	if (g_PBRA_On)
	{
		WriteRelCall(0x9D0A46, UINT32(GetBackwardsRangedAttackActor));
		//WriteRelCall(0x9D0D6D, UINT32(PreventBackwardsRangedAttacks));
		PatchMemoryNop(0x9D13B2, 8);
	}
#endif
}
