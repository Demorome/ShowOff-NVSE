#pragma once


// Everything here was ripped from JohnnyGuitar NVSE.

//not really useful for now...
bool(__cdecl* Cmd_GetAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BD8A0;
bool(__cdecl* Cmd_SetAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BDCD0;
bool(__cdecl* Cmd_ForceAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BE190;
bool(__cdecl* Cmd_ModAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BDE40;
bool(__cdecl* Cmd_DamageAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BDF20;
bool(__cdecl* Cmd_RestoreAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BE080;
bool(__cdecl* Cmd_GetBaseAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BE6E0;


bool AlchemyItem::IsPoison()
{
	EffectItem* effItem;
	EffectSetting* effSetting = NULL;
	ListNode<EffectItem>* iter = magicItem.list.list.Head();
	do
	{
		if (!(effItem = iter->data)) continue;
		effSetting = effItem->setting;
		if (effSetting && !(effSetting->effectFlags & 4)) return false;
	} while (iter = iter->next);
	return effSetting != NULL;
}

TESActorBase* Actor::GetActorBase()
{
	ExtraLeveledCreature* xLvlCre = GetExtraType(extraDataList, LeveledCreature);
	return (xLvlCre && xLvlCre->form) ? (TESActorBase*)xLvlCre->form : (TESActorBase*)baseForm;
}

static void PatchMemoryNop(ULONG_PTR Address, SIZE_T Size)
{
	DWORD d = 0;
	VirtualProtect((LPVOID)Address, Size, PAGE_EXECUTE_READWRITE, &d);

	for (SIZE_T i = 0; i < Size; i++)
		*(volatile BYTE*)(Address + i) = 0x90; //0x90 == opcode for NOP

	VirtualProtect((LPVOID)Address, Size, d, &d);

	FlushInstructionCache(GetCurrentProcess(), (LPVOID)Address, Size);
}

__declspec(naked) TESObjectCELL* TESObjectREFR::GetParentCell()
{
	__asm
	{
		mov		eax, [ecx + 0x40]
		test	eax, eax
		jnz		done
		push	kExtraData_PersistentCell
		add		ecx, 0x44
		call	BaseExtraList::GetByType
		test	eax, eax
		jz		done
		mov		eax, [eax + 0xC]
		done:
		retn
	}
}