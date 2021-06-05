#pragma once
#include "GameData.h"
#include "SafeWrite.h"
#include "internal/StewieMagic.h"

//todo: Move non-extern global declarations to main.cpp, add externs here.

extern ICriticalSection g_Lock;

//NVSE Globals
extern bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);
extern bool (*ExtractFormatStringArgs)(UInt32 fmtStringPos, char* buffer, COMMAND_ARGS_EX, UInt32 maxParams, ...);  // From JIP_NVSE.H
extern NVSEArrayVarInterface* g_arrInterface;
extern NVSEArrayVar* (*CreateArray)(const NVSEArrayElement* data, UInt32 size, Script* callingScript);
extern NVSEArrayVar* (*CreateStringMap)(const char** keys, const NVSEArrayElement* values, UInt32 size, Script* callingScript);
extern bool (*AssignArrayResult)(NVSEArrayVar* arr, double* dest);
extern void (*SetElement)(NVSEArrayVar* arr, const NVSEArrayElement& key, const NVSEArrayElement& value);
extern void (*AppendElement)(NVSEArrayVar* arr, const NVSEArrayElement& value);
extern UInt32(*GetArraySize)(NVSEArrayVar* arr);
extern NVSEArrayVar* (*LookupArrayByID)(UInt32 id);
extern bool (*GetElement)(NVSEArrayVar* arr, const NVSEArrayElement& key, NVSEArrayElement& outElement);
extern bool (*GetArrayElements)(NVSEArrayVar* arr, NVSEArrayElement* elements, NVSEArrayElement* keys);
extern NVSEStringVarInterface* g_strInterface;
extern bool (*AssignString)(COMMAND_ARGS, const char* newValue);
extern const char* (*GetStringVar)(UInt32 stringID);
extern NVSEMessagingInterface* g_msg;
extern NVSEScriptInterface* g_scriptInterface;
extern NVSECommandTableInterface* g_commandInterface;

//Singletons
extern HUDMainMenu* g_HUDMainMenu;
extern TileMenu** g_tileMenuArray;
extern UInt32 g_screenWidth;
extern UInt32 g_screenHeight;
extern PlayerCharacter* g_thePlayer;
extern ActorValueOwner* g_playerAVOwner;
extern ProcessManager* g_processManager;
extern InterfaceManager* g_interfaceManager;
extern BSWin32Audio* g_bsWin32Audio;
extern DataHandler* g_dataHandler;
extern BSAudioManager* g_audioManager;
extern Sky** g_currentSky;

//-Hook Globals
extern std::atomic<bool> g_canPlayerPickpocketInCombat;

//-Force Pickpocketting INI globals (enabled via function)
extern std::atomic<float> g_fForcePickpocketBaseAPCost;
extern std::atomic<float> g_fForcePickpocketMinAPCost;
extern std::atomic<float> g_fForcePickpocketMaxAPCost;
extern std::atomic<float> g_fForcePickpocketPlayerAgilityMult;
extern std::atomic<float> g_fForcePickpocketPlayerSneakMult;
extern std::atomic<float> g_fForcePickpocketTargetPerceptionMult;
extern std::atomic<float> g_fForcePickpocketItemWeightMult;
extern std::atomic<float> g_fForcePickpocketItemValueMult;
extern std::atomic<float> g_fForcePickpocketPlayerStrengthMult;
extern std::atomic<float> g_fForcePickpocketTargetStrengthMult;
extern char* g_fForcePickpocketFailureMessage;

//-PreventBrokenItemRepairing (PBIR) INI globals 
extern std::atomic<bool> g_PBIR_On;
extern char* g_PBIR_FailMessage;


// Misc.
#define NUM_ARGS *((UInt8*)scriptData + *opcodeOffsetPtr)  //Probably breaks Compiler Override!! - Credits to Stewie.
#define REFR_RES *(UInt32*)result  //From JIPLN

//Below are already defined in GameForms.h - All from JIPLN
#define IS_TYPE(form, type) (*(UInt32*)form == kVtbl_##type)  
#define NOT_ID(form, type) (form->typeID != kFormType_##type) 
#define IS_ID(form, type) (form->typeID == kFormType_##type)

#define RegisterScriptCommand(name) nvse->RegisterCommand(&kCommandInfo_ ##name); //Default return type (return a number)
#define REG_CMD(name) nvse->RegisterCommand(&kCommandInfo_##name);  //Short version of RegisterScriptCommand, from JIP.
#define REG_TYPED_CMD(name, type) nvse->RegisterTypedCommand(&kCommandInfo_##name,kRetnType_##type);  //from JG
#define REG_CMD_STR(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_String); //From JIPLN
#define REG_CMD_ARR(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Array); //From JIPLN
#define REG_CMD_FORM(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Form); //From JIPLN
#define REG_CMD_AMB(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Ambiguous); //From JIPLN

typedef NVSEArrayVarInterface::Array NVSEArrayVar;
typedef NVSEArrayVarInterface::Element NVSEArrayElement;

//---Hooks and Hook Stuff

bool canPlayerPickpocketEqItems() { return *(UInt32*)0x75E87B != 0xFFD5F551; }
//Checks if the address has been changed, to take into account Stewie's Tweaks' implementation (bPickpocketWornItems).
//If the address was changed by something else, uh... Well I don't take that into account.


// From lStewieAl's Tweaks (bImprovedPickpocketing).
void __fastcall ContainerMenuHandleMouseoverAlt(ContainerMenu* menu, void* edx, int a2, int a3)
{
	// if not in pickpocket mode return
	if (menu->mode != ContainerMenu::Mode::kPickpocket || !IS_TYPE(menu->containerRef, Character)) return;

	TileMenu* tileMenu = menu->tile;
	Tile* subtitlesTile = CdeclCall<Tile*>(0xA08B20, tileMenu, "CM_SUBTITLE");
	if (subtitlesTile) subtitlesTile->SetString(kTileValue_string, "", 1);
	ThisStdCall(0x75CF70, menu, a2, a3);
}

// From lStewieAl's Tweaks.
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
// Code ripped from lStewieAl's Tweaks (TryPickpocket)
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
			ScopedLock lock(&g_Lock);
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
		call	SetContainerSubtitleStringTo
		APCost

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
	//Console_Print("Changes offline.");
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
#if _DEBUG
		Console_Print("I made a difference!"); //seems like we're failing before this even happens...
#endif
		return false;
	}
	return isInCombat;
}

//Still doesn't work even with this...
//Ripped code from somewhere, don't exactly remember - maybe JIP, Stewie or JG.
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

double __fastcall PreventRepairButton(ContChangesEntry* entry, int bPercent)
{
	auto const result = ThisStdCall<double>(0x4BCDB0, entry, bPercent);
	if (result == 0.0F)
	{
		//g_bRepairButtonPrevented_PBIR = true;
		return 100.0F;  //since you can't repair items at 100% health, I use that bit of code to prevent repairing items at 0% health.
		//super hacky
	}
	//g_bRepairButtonPrevented_PBIR = false;
	return result;
}


void HandleGameHooks()
{
	//Modify a "IsInCombat" check to allow NPC activation even if they are in combat.
	WriteRelCall(0x607E70, UINT32(PCCanPickpocketInCombatHOOK));
	WriteRelCall(0x5FA81F, UINT32(PCCanPickpocketInCombatHOOK));  //Same thing but for Creatures (like Super Mutants).

	// NOTE: When failing the AP cost check, it counts as if you were caught by the actor.
	// This could cause really weird behavior with the NPC just taking back everything that was previously stolen,
	// not to mention you would no longer be able to pickpocket them normally after combat.
	//
	// Possible solution: open and hook the companion loot exchange menu instead?

	//=== Event Handler Stuff

	
#if _DEBUG
	//Below isn't working currently...
	WriteRelCall(0x77738A, UINT32(ShowPickpocketStringInCombat));
	WriteRelCall(0x7772C9, UINT32(ShowPickpocketStringInCombat2));
	//WriteRelCall(0x770C0D, UINT32(ShowPickpocketStringInCombat2)); //breaks health target UI

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
}
