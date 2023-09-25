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
			call GetHealthPerc	//	st(0) = healthPerc
			fcom kDblZero
			fnstsw ax	
			test ah, 0x41	//if (healthPerc <= 0)
			jp doNormal	//jump if false (healthPerc > 0)

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
			rtnFalse = 0x7B7C35;
		enum
		{
			healthPerc = -0x14
		};
		_asm
		{
			fld qword ptr ss : [ebp + healthPerc]
			fcomp kDblZero
			fnstsw ax	
			test ah, 0x41	//if (healthPerc <= 0)
			jp doNormal	//jump if false (healthPerc > 0)

			mov al, 1	//shouldHide = true
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

	void WriteRetrievalHook() { ReplaceCall(0x785866, (UInt32)CloseMenu_Hook); }
}

namespace LevelUpMenuHooks
{
	// Taken from Tweaks
	__declspec(naked) void LevelUpMenuClickPerkHook()
	{
		_asm
		{
			mov ecx, dword ptr ss : [ebp - 0x10] // LevelUpMenu
			mov eax, [ecx + 0x60] // numPerksToAssign
			test eax, eax
			jne clickPerk
			mov edx, [ecx + 0x58] // numAssignedPerks
			test edx, edx
			je noClick

			clickPerk :
			mov eax, 0x785AD3
			push dword ptr ss : [ebp + 0xC]
			jmp eax

			noClick :
			mov eax, 0x785DE8
			jmp eax
		}
	}

	// Taken from Tweaks.
	UInt32 IsPlayerAtPerkLevel()
	{
		UInt32 iLevelsPerPerk = *(UInt32*)0x11CD078;
		return (g_thePlayer->GetLevel() % iLevelsPerPerk) == 0;
	}

	// Taken from Tweaks.
	double __fastcall PerkMenuCheckNumSelectedPerks(Tile* tile, void* edx, int key)
	{
		auto menu = LevelUpMenu::GetSingleton();
		if (menu->numPerksToAssign != 1)
		{
			if (menu->numAssignedPerks >= menu->numPerksToAssign)
			{
				// will prevent the tile from being clicked
				return 0;
			}
		}
		else
		{
			// Reset selected perks first, to emulate vanilla behavior,
			// ..which is to allow swapping the sole selected perk for another by clicking on another perk.
			ThisStdCall(0x787850, &menu->perkListBox, 1);
		}
		return tile->GetValueFloat(key);
	}

	namespace ShowPerkMenu
	{
		void WriteDelayedHooks()
		{
			// == Credits to lStewieAl for this all this code, taken from patchShowPerksMenuEvenIfNoneToAssign.

			// Prevent clicking on perks if there are none to assign.
			// Needed to prevent jank for ShowPerkMenu.
			// While this does overwrite a hook for Tweaks, it should make no difference who wrote this hook.
			WriteRelJump(0x785ACC, UInt32(LevelUpMenuClickPerkHook));

			// init number of perks to assign to be 0 if not on a perk level
			SafeWriteBuf(0x78508C, "\x8B\x0D\xDC\x9F\x1D\x01\x89\x41\x60\x0F\x1F\x40", 12); // fine if Tweaks wrote this instead.
			WriteRelCall(0x785087, UInt32(IsPlayerAtPerkLevel)); // it's 100% OK if Tweaks writes this hook for us instead.

			// == Hooks below taken from Tweaks, from CustomPerksPerLevel::InitHooks

			// remove call resetting selected Perks when one is selected
			// In PerkMenuCheckNumSelectedPerks, we will reset selected perks if there was just 1 perk to allot at the start.
			NopFunctionCall(0x785B2C, 1);

			// wrap call checking tile alpha, prevent clicking on the tile if numPerksToAssign perks are already selected
			// If there was only 1 perk to assign to start with, won't prevent vanilla behavior,
			//	..which is to allow clicking on another perk and just have the selection swap.
			// Note: This hook must be delayed so it can overwrite Tweak's.
			ReplaceCall(0x785B0F, UInt32(PerkMenuCheckNumSelectedPerks));
		}
	}

}
