#include "nvse/GameUI.h"

//All code in here copied from JIP

__declspec(naked) UInt32 InterfaceManager::GetTopVisibleMenuID()
{
	__asm
	{
		cmp		byte ptr[ecx + 0xC], 2
		jb		retn0
		mov		eax, [ecx + 0xD0]
		add		ecx, 0x114
		test	eax, eax
		jz		stackIter
		mov		eax, [eax + 0x20]
		retn
		ALIGN 16
		stackIter:
		add		ecx, 4
			cmp[ecx], 0
			jnz		stackIter
			mov		eax, [ecx - 4]
			cmp		eax, 1
			jnz		done
			mov		ecx, 0x11F3479
			mov		eax, 0x3EA
			cmp[ecx], 0
			jnz		done
			mov		al, 0xFF
			cmp[ecx + 0x15], 0
			jnz		done
			mov		al, 0xEB
			cmp[ecx + 1], 0
			jnz		done
			mov		eax, 0x40B
			cmp[ecx + 0x21], 0
			jnz		done
			mov		al, 0x25
			cmp[ecx + 0x3B], 0
			jnz		done
			retn0 :
		xor eax, eax
			done :
		retn
	}
}

Tile* InterfaceManager::GetActiveTile()
{
	return activeTile ? activeTile : activeTileAlt;
}