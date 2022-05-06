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

	void WriteRetrievalHook() { WriteRelCall(0x785866, (UInt32)CloseMenu_Hook); }
}
