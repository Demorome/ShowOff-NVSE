#pragma once

#include "decoding.h"

namespace PreventRepairs
{
	double __fastcall PreventRepairButton(ContChangesEntry* entry, int bPercent);
}

namespace GetLevelUpMenuUnspentPoints
{
	extern UInt32 g_LvlUpMenuUnspentPoints[2];
	void __fastcall CloseMenu_Hook();
	void WriteRetrievalHook();
}

// Used for SetNoEquipShowOff
namespace PreventInvItemActivation
{
	bool __fastcall CanActivateItemHook(TESForm* item, Actor* actor);
	void WriteOnActivateItemHook();
	void OnActivateInventoryItemHook();
}