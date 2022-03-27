#include "MiscHooks.h"

#include "MenuHooks.h"
#include "SafeWrite.h"
#include "ShowOffNVSE.h"
#include "StewieMagic.h"

namespace PickpocketEquippedItems
{
	//Checks if the address has been changed, to take into account Stewie's Tweaks' implementation (bPickpocketWornItems).
	//If the address was changed by something else, uh... Well I don't take that into account.
	bool CanPlayerPickpocketEqItems()
	{
		return *(UInt32*)0x75E87B != 0xFFD5F551;
	}
}

namespace PickpocketInCombat
{
	// From lStewieAl's Tweaks (bImprovedPickpocketing).
	void __fastcall ContainerMenuHandleMouseoverAlt(ContainerMenu* menu, void* edx, int a2, int a3)
	{
		if (menu->mode != ContainerMenu::Mode::kPickpocket || NOT_TYPE(menu->containerRef, Character))
			return;

		if (auto subtitlesTile = CdeclCall<Tile*>(0xA08B20, menu->tile, "CM_SUBTITLE"))
			subtitlesTile->SetString(kTileValue_string, "", true);

		menu->HandleMouseoverAlt(a2, a3);
	}

	// From lStewieAl's Tweaks.
	int CalculateCombatPickpocketAPCost(ContChangesEntry* item, Actor* target, int itemValue, int count, bool isItemOwnedByTarget)
	{
		// Average player (5 AGL) will have ~80 action points
		// Meaning, to balance this out, AP costs should refrain from going over 80 (save for some exceptions).

		// Detection value won't be taken into account - this is a brute-force kind of approach, anyone will see it coming, unless they're unconscious.
		// Still, it's a Sleight of Hand trick, so the target's Perception will be important, as will the player's Sneak + Agility (used as an intermediary for Sleight of Hand).
		// If the enemy is perceptive, it'll be trickier for the player to do their trick, so also take targetPerception into account.

		int const playerAgility = g_playerAVOwner->GetThresholdedActorValue(kAVCode_Agility);
		int const playerSneak = g_playerAVOwner->GetThresholdedActorValue(kAVCode_Sneak);
		int const targetPerception = target->avOwner.GetThresholdedActorValue(kAVCode_Perception);
		float const itemWeight = item->type->GetWeight() * count;

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

				int const targetStrength = target->avOwner.GetThresholdedActorValue(kAVCode_Strength);
				int const playerStrength = g_playerAVOwner->GetThresholdedActorValue(kAVCode_Strength);

				cost += std::max<float>(0, (g_fForcePickpocketTargetStrengthMult * targetStrength
					- g_fForcePickpocketPlayerStrengthMult * playerStrength));
				//0 or greater.
			}
		}

		cost = std::min<float>(cost, g_fForcePickpocketMaxAPCost); //yes, the choice of global setting is intentional.
		cost = std::max<float>(cost, g_fForcePickpocketMinAPCost);

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

	void ResetPickpocketHooks()
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
				ResetPickpocketHooks();
			return true;
		}
		if (IsPickpocketHookSet)
			ResetPickpocketHooks();
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
}






void PatchShowRaceMenu()
{
	// Prevent ShowRaceMenu from resetting active temp effects.
	NopFunctionCall(0x7ADDC7, 1);

	// Prevent ShowRaceMenu from resetting abilities.
	PatchMemoryNop(0x7ADDD2, 5);
}

namespace PatchResetCell	
{
	__declspec(naked) void GetShouldRespawnHook()
	{
		static const UInt32 retnAddr = 0x54E1CD,
			getShouldRespawnAddr = 0x881C90;
		
		enum {
			cellDetachTime = -0x4 //-0x8
		};
		_asm
		{
			//cmp [ebp + cellDetachTime], -1		//did not work!
			cmp dword ptr ss : [ebp + cellDetachTime], -1
			jg doNormal
			jnz skipIniCheck //jump if -2, essentially
			cmp g_bResetInteriorResetsActors, 0
			jz doNormal
			skipIniCheck:
			mov al, 1
			jmp retnAddr

			doNormal:
			call getShouldRespawnAddr
			jmp retnAddr
		}
	}

	void WriteHook()
	{
		// replace jnz -> JG	(compares with -1, add support for -2)
		SafeWrite8(0x54E09A, 0x7F);

		//Replace Actor::GetShouldRespawn call in Cell::HandleResets
		//...Cuz it checks for time even though ResetInterior was called.
		WriteRelJump(0x54E1C8, (UInt32)GetShouldRespawnHook);
	}
}


namespace HandleHooks
{
	void HandleFunctionHooks()
	{
		PreventInvItemActivation::WriteOnActivateItemHook();
		GetLevelUpMenuUnspentPoints::WriteRetrievalHook();

#if 0
		WriteRelCall(0x8B0FF0, UInt32(Actor_Spread_PerkModifier_Hook));


		// Modify a "IsInCombat" check to allow NPC activation even if they are in combat.
		WriteRelCall(0x607E70, UINT32(PickpocketInCombat::PCCanPickpocketInCombatHOOK));
		WriteRelCall(0x5FA81F, UINT32(PickpocketInCombat::PCCanPickpocketInCombatHOOK));  //Same thing but for Creatures (like Super Mutants).

		// NOTE: When failing the AP cost check, it counts as if you were caught by the actor.
		// This could cause really weird behavior with the NPC just taking back everything that was previously stolen,
		// not to mention you would no longer be able to pickpocket them normally after combat.

		// Below isn't working currently...
		WriteRelCall(0x77738A, UINT32(PickpocketInCombat::ShowPickpocketStringInCombat));
		WriteRelCall(0x7772C9, UINT32(PickpocketInCombat::ShowPickpocketStringInCombat2));
		//WriteRelCall(0x770C0D, UINT32(ShowPickpocketStringInCombat2)); //breaks health target UI
		// Possible solution: open and hook the companion loot exchange menu instead?
#endif

	}

	void HandleGameTweaks()
	{
		if (g_bNoRepairingBrokenItems)
		{
			WriteRelJump(0x7818C0, (UINT32)PreventRepairs::PreventRepairingBrokenItems);
		}
	}

	void HandleGameFixes()
	{
		PatchShowRaceMenu();
		PatchResetCell::WriteHook();
	}

	void HandleGameHooks()
	{
		HandleFunctionHooks();
		HandleGameFixes();
		HandleGameTweaks();
	}

	void HandleDelayedGameHooks()
	{

	}
}