#include "Hooks.h"

//#include "GameAPI.h"
//#include "Johnnnny Guitarrrrr.h"
#include "SafeWrite.h"
#include "ShowOffNVSE.h"
#include "StewieMagic.h"
//#include "GameRTTI.h"


//====For non-event handler hooks.====//

//Checks if the address has been changed, to take into account Stewie's Tweaks' implementation (bPickpocketWornItems).
//If the address was changed by something else, uh... Well I don't take that into account.
bool canPlayerPickpocketEqItems() { return *(UInt32*)0x75E87B != 0xFFD5F551; }


// From lStewieAl's Tweaks (bImprovedPickpocketing).
void __fastcall ContainerMenuHandleMouseoverAlt(ContainerMenu* menu, void* edx, int a2, int a3)
{
	// if not in pickpocket mode return
	if (menu->mode != ContainerMenu::Mode::kPickpocket || !IS_TYPE(menu->containerRef, Character)) return;

	TileMenu* tileMenu = menu->tile;
	Tile* subtitlesTile = CdeclCall<Tile*>(0xA08B20, tileMenu, "CM_SUBTITLE");
	if (subtitlesTile) subtitlesTile->SetString(kTileValue_string, "", true);
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
			ScopedLock lock(g_Lock);
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

UInt32 g_LvlUpMenuUnspentPoints[2] = { 0, 0 }; // Skill points, followed by Perks.

namespace GetLevelUpMenuUnspentPoints
{
	void __fastcall CloseMenu_Hook()
	{
		if (LevelUpMenu* const menu = *(LevelUpMenu**)0x11D9FDC)
		{
			g_LvlUpMenuUnspentPoints[0] = menu->numSkillPointsToAssign - menu->numAssignedSkillPoints;
			g_LvlUpMenuUnspentPoints[1] = menu->numPerksToAssign - menu->numAssignedPerks;
		}
		CdeclCall(0x7851D0);  // LevelUpMenu::Close()
	}

	void WriteRetrievalHook() { WriteRelCall(0x785866, (UInt32)CloseMenu_Hook); }
}

/*
 * Item activation and equipping is conflated here.
 * Certain funcs will prevent item activation early, such as if you're at max health while activating a stimpak (0x780E97).
 * The above would result in this code not running at all.
 * Todo: maybe do something about that? ^
 * NOTE: Can prevent script functions like EquipItem from working.
 * If the NPC's best weapon can no longer be equipped, it can no longer wield a weapon, even via `EquipItem SomeOtherWeap`.
 * ^ TODO: check 0x6047C0 for a way to fix this.
*/
bool __fastcall CanActivateItemHook(TESForm* item, Actor* actor)
{
	bool canActivate = true;

	// Spaghetti to account for pairs which have null members (generic filters).
	ActorAndItemPair const actorAndItem = { actor->refID, item->refID };
	ActorAndItemPair const nullAndItem = { NULL, item->refID };
	ActorAndItemPair const actorAndNull = { actor->refID, NULL };
	for (auto const &iter : g_noEquipMap)
	{
		if (actorAndItem == iter || nullAndItem == iter || actorAndNull == iter)
		{
			canActivate = false;
			break;
		}
	}
	
	if (canActivate)
	{
		for (auto const &iter : g_NoEquipFunctions)
		{
			NVSEArrayElement elem;
			FunctionCallScript(iter.second.Get(), actor, nullptr, &elem, 1, item);	// todo: try passing an inventory ref instead. Would require extracting extra data from the function args.
			if (!elem.Bool())	// todo: TEST
			{
				canActivate = false;
				break;
			}
		}
	}

	if (g_ShowFuncDebug)
		Console_Print("CanActivateItemHook: CanActivate: %i, Item: [%08X], %s, type: %u, Actor: [%08X], %s, type: %u", canActivate, item->refID, item->GetName(), item->typeID, actor->refID, actor->GetName(), actor->typeID);
	
	return canActivate;
}

// Refactored thanks to lStewieAl!
// Kormakur also helped fix a bug with "cmp, al 1" -> "test al, al"
__declspec(naked) void OnActivateInventoryItemHook()
{
	static const UInt32 endFuncAddr = 0x88D27A;

	// Global variables do not work for "[ebp - someVal]"-style statements; use enum instead.
	enum {
		actorOffset = -0x80,
		itemOffset = 0x8,
	};

	_asm
	{
		// ignore the line prior (mov ecx, [ebp+item])
		mov ecx, [ebp + itemOffset]
		mov edx, [ebp + actorOffset]
		call CanActivateItemHook
		test al, al
		je noActivate
		
	doNormal:
		mov ecx, [ebp + itemOffset]
		movzx eax, byte ptr[ecx + 4] // TESForm->typeID
		ret
		
	noActivate:
		pop ecx // pop the pushed return address from call
		jmp endFuncAddr
	}
}


#if _DEBUG

bool GetCanSleepInOwnedBeds()
{
	//if (GetRelJumpAddr(0x509690) == 0x32EB) return true;  //flawed since it's not an actual jump
	return false;
}

// Copies after lStewieAl's patchSleepInOwnedBeds()
void SetCanSleepInOwnedBeds(bool bOn)
{
	if (bOn && !GetCanSleepInOwnedBeds())
	{
		// jump to allow sleep if the bed is owned but the player is activating it
		SafeWrite16(0x509690, 0x32EB);

		// allow sleeping while trespassing
		SafeWrite8(0x509737, 0xEB);
	}
	else if (GetCanSleepInOwnedBeds())
	{
		// Restore original data.
		//SafeWrite16(0x509690, );
		//SafeWrite8(0x509737, );
	}

}


#if 0
// spreadArgs's value is really low, must've messed something up with SpreadArg...
void Actor_Spread_PerkModifier_Hook(PerkEntryPointID id, TESObjectREFR* refr, float* spreadArg, ...)
{
	ApplyPerkModifiers(id, refr, spreadArg);
	float spread = *spreadArg;
	//float spread = *(float*)&spreadArg;
	//UInt32* argAddr = &spreadArg;
	Console_Print("Spread: %f", spread);
	//modify spread by custom stuff.

}
#endif

// End IDA debug stuff
#endif

void HandleGameHooks()
{
	NopFunctionCall(0x7ADDC7, 1); // For preventing ShowRaceMenu from resetting active temp effects.
	PatchMemoryNop(0x7ADDD2, 5); // For preventing ShowRaceMenu from resetting abilities.

	GetLevelUpMenuUnspentPoints::WriteRetrievalHook();
	

	//==For functions.

	// replace "call TESForm__DoGetTypeID"
	WriteRelCall(0x88C87A, (UInt32)OnActivateInventoryItemHook); // for SetNoEquipShowOff

	

	
	

#if _DEBUG

	
	

#if 0
	WriteRelCall(0x8B0FF0, UInt32(Actor_Spread_PerkModifier_Hook));
#endif

	// Modify a "IsInCombat" check to allow NPC activation even if they are in combat.
	WriteRelCall(0x607E70, UINT32(PCCanPickpocketInCombatHOOK));
	WriteRelCall(0x5FA81F, UINT32(PCCanPickpocketInCombatHOOK));  //Same thing but for Creatures (like Super Mutants).

	// NOTE: When failing the AP cost check, it counts as if you were caught by the actor.
	// This could cause really weird behavior with the NPC just taking back everything that was previously stolen,
	// not to mention you would no longer be able to pickpocket them normally after combat.

	// Below isn't working currently...
	WriteRelCall(0x77738A, UINT32(ShowPickpocketStringInCombat));
	WriteRelCall(0x7772C9, UINT32(ShowPickpocketStringInCombat2));
	//WriteRelCall(0x770C0D, UINT32(ShowPickpocketStringInCombat2)); //breaks health target UI
	// Possible solution: open and hook the companion loot exchange menu instead?


	if (g_PBIR_On)
	{
		WriteRelCall(0x7818B7, UINT32(PreventRepairButton));
		/*
		char buf[260];
		sprintf(buf, "%s", g_PBIR_FailMessage);
		QueueUIMessage(buf, eEmotion::sad, NULL, NULL, 2.0, 0);
		 */
	}



	//----Below is reserved for messing around with IDA

#endif
}
