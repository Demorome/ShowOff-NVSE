#include "MenuHooks.h"

#include "SafeWrite.h"
#include "ShowOffNVSE.h"
#include "StewieMagic.h"

namespace PreventRepairs
{
	__declspec(naked) void PreventRepairingBrokenItemsInPipboy()
	{
		static UInt32 const GetHealthPerc = 0x4BCDB0,
			retnAddr = 0x781919,
			retnFalse = 0x781AF1;
		_asm
		{
			call GetHealthPerc
			FLDZ	// st(0) = 0.0, st(1) = healthPerc
			fcomp	// st(0) = healthPerc
			fnstsw ax	
			test ah, 1	//if (0 >= healthPerc) <-> (healthPerc <= 0)
			jne doNormal	//jump if false (healthPerc > 0)

			fstp st(0)	//pop st(0) off
			mov al, 0	//result = false
			jmp retnFalse

			doNormal:
			jmp retnAddr	
		}
	}

	__declspec(naked) void PreventRepairingBrokenItemsByVendor()
	{
		static UInt32 const rtnAddr = 0x7B7BD9,
			GameSettings_GetFloatValueAddr = 0x403E20,
			rtnFalse = 0x7B7C3B;
		enum
		{
			healthPerc = -0x14
		};
		_asm
		{
			FLDZ
			fcomp qword ptr ss : [esp+healthPerc]		//check if health == 0
			fnstsw ax	
			test ah, 0x44	//is ST(0) == [esp]?
			//if health == 0, return
			jp doNormal
			add esp, 4		//undo push at 0x7B7BC2, normally GameSettings_GetFloatValueAddr would clean it.
			mov al, 0
			jmp rtnFalse

			doNormal:
			call GameSettings_GetFloatValueAddr
			jmp rtnAddr
		}
	}
}

namespace GetLevelUpMenuUnspentPoints
{
	UInt32 g_LvlUpMenuUnspentPoints[2] = { 0, 0 }; // Skill points, followed by Perks.

	void __fastcall CloseMenu_Hook()
	{
		if (auto const menu = LevelUpMenu::GetSingleton())
		{
			g_LvlUpMenuUnspentPoints[0] = menu->numSkillPointsToAssign - menu->numAssignedSkillPoints;
			g_LvlUpMenuUnspentPoints[1] = menu->numPerksToAssign - menu->numAssignedPerks;
		}
		CdeclCall(0x7851D0);  // LevelUpMenu::Close()
	}

	void WriteRetrievalHook() { WriteRelCall(0x785866, (UInt32)CloseMenu_Hook); }
}

namespace PreventInvItemActivation
{
	/*
	* Item activation and equipping is conflated here.
	* Certain funcs will prevent item activation early, such as if you're at max health while activating a stimpak (0x780E97).
	* The above would result in this code not running at all.
	* Todo: maybe do something about that? ^
	* NOTE: Can prevent script functions like EquipItem from working.
	* If the NPC's best weapon can no longer be equipped, it can no longer wield a weapon, even via `EquipItem SomeOtherWeap`.
	* ^ TODO: check 0x6047C0 for a way to fix this.
	*/
	bool __fastcall CanActivateItemHook(TESForm* item, Actor* actor/*, int count, ExtraDataList* extraData*/)
	{
		auto CheckNoEquipFilters = [&actor, &item](RefID const actorID, RefID const itemID)
		{
			bool canActivate;

			// Null filters means that the event is unfiltered for that filter type.
			if (!actorID)
			{
				canActivate = actorID == actor->refID;
			}
			else if (!itemID)
			{
				canActivate = itemID == item->refID;
			}
			else
			{
				canActivate = actorID == actor->refID && itemID == item->refID;
			}
			return canActivate;
		};

		bool canActivate = true;
		for (auto const& iter : g_noEquipMap)
		{
			if (!(canActivate = CheckNoEquipFilters(iter.first, iter.second)))
				break;
		}

		if (canActivate)
		{
			for (auto const& iter : g_NoEquipFunctions)
			{
				// todo: try passing an inventory ref instead. Would require extracting extra data from the function args.
				//InventoryRefCreateEntry(actor, item, count, extraData);
				ArrayElementR elem;
				FunctionCallScript(iter.second.Get(), actor, actor, &elem, 1, item);
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
		enum Offsets
		{
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

		doNormal :
			mov ecx, [ebp + itemOffset]
			movzx eax, byte ptr[ecx + 4] // TESForm->typeID
			ret

		noActivate :
			pop ecx // pop the pushed return address from call
			jmp endFuncAddr
		}
	}

	void WriteOnActivateItemHook()
	{
		// Replace "call TESForm__DoGetTypeID"
		WriteRelCall(0x88C87A, (UInt32)OnActivateInventoryItemHook);
	}
}
