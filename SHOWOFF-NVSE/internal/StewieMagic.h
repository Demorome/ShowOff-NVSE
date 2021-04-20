#pragma once
#include "internal/decoding.h"

bool __fastcall ContainerMenuCheckIsTargetChild(ContChangesEntry* entry, void* edx, int a1)
{
	TESObjectREFR* ref = ContainerMenu::GetSingleton()->containerRef;
	return ref->GetIsChildSize(true);
}
