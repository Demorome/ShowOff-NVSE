#pragma once

#include "decoding.h"

namespace PreventRepairs
{
	void PreventRepairingBrokenItemsInPipboy();
	void PreventRepairingBrokenItemsByVendor();
}

namespace GetLevelUpMenuUnspentPoints
{
	extern UInt32 g_LvlUpMenuUnspentPoints[2];
	void __fastcall CloseMenu_Hook();
	void WriteRetrievalHook();
}