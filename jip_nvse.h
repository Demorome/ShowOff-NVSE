#pragma once


/*
__declspec(naked) bool IsConsoleOpen()
{
	__asm
	{
		mov		al, byte ptr ds : [0x11DEA2E]
		test	al, al
		jz		done
		mov		eax, ds : [0x126FD98]
		mov		edx, fs : [0x2C]
		mov		eax, [edx + eax * 4]
		test	eax, eax
		jz		done
		mov		al, [eax + 0x268]
		done :
		retn
	}
}

__declspec(naked) void __fastcall DoConsolePrint(double* result)
{
	__asm
	{
		call	IsConsoleOpen
		test	al, al
		jnz		proceed
		retn
		proceed :
		mov		edx, [ebp]
			mov		edx, [edx - 0x30]
			mov		edx, [edx]
			movsd	xmm0, qword ptr[ecx]
			push	ebp
			mov		ebp, esp
			sub		esp, 0x50
			lea		ecx, [ebp - 0x50]
			call	StrCopy
			mov		ecx, eax
			mov		dword ptr[ecx], ' >> '
			add		ecx, 4
			call	FltToStr
			lea		eax, [ebp - 0x50]
			push	eax
			push	eax
			mov		ecx, g_consoleManager
			mov		eax, 0x71D0A0
			call	eax
			mov		esp, ebp
			pop		ebp
			retn
	}
}

__declspec(naked) void __fastcall DoConsolePrintID(double* result)
{
	__asm
	{
		call	IsConsoleOpen
		test	al, al
		jnz		proceed
		retn
		proceed :
		mov		edx, [ebp]
			mov		edx, [edx - 0x30]
			mov		edx, [edx]
			push	ebp
			mov		ebp, esp
			sub		esp, 0x60
			push	esi
			mov		esi, [ecx]
			lea		ecx, [ebp - 0x60]
			call	StrCopy
			mov		ecx, eax
			mov		dword ptr[ecx], ' >> '
			add		ecx, 4
			test	esi, esi
			jnz		haveID
			mov		word ptr[ecx], '0'
			jmp		noEDID
			haveID :
		mov		edx, esi
			call	UIntToHex
			push	esi
			mov		esi, eax
			call	LookupFormByRefID
			test	eax, eax
			jz		noEDID
			mov		ecx, eax
			mov		eax, [ecx]
			call	dword ptr[eax + 0x130]
			test	eax, eax
			jz		noEDID
			cmp[eax], 0
			jz		noEDID
			mov		ecx, esi
			mov		word ptr[ecx], '\" '
			add		ecx, 2
			mov		edx, eax
			call	StrCopy
			mov		word ptr[eax], '\"'
			noEDID:
		pop		esi
			lea		eax, [ebp - 0x60]
			push	eax
			push	eax
			mov		ecx, g_consoleManager
			mov		eax, 0x71D0A0
			call	eax
			mov		esp, ebp
			pop		ebp
			retn
	}
}
*/