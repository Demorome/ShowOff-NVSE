#include "SafeWrite.h"

void __stdcall SafeWrite8(UInt32 addr, UInt32 data)
{
	UInt32 oldProtect;
	VirtualProtect((void*)addr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
	*(UInt8*)addr = data;
	VirtualProtect((void*)addr, 4, oldProtect, &oldProtect);
}

void __stdcall SafeWrite16(UInt32 addr, UInt32 data)
{
	UInt32 oldProtect;
	VirtualProtect((void*)addr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
	*(UInt16*)addr = data;
	VirtualProtect((void*)addr, 4, oldProtect, &oldProtect);
}

void __stdcall SafeWrite32(UInt32 addr, UInt32 data)
{
	UInt32 oldProtect;
	VirtualProtect((void*)addr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
	*(UInt32*)addr = data;
	VirtualProtect((void*)addr, 4, oldProtect, &oldProtect);
}

void __stdcall SafeWriteBuf(UInt32 addr, const void* data, UInt32 len)
{
	UInt32 oldProtect;
	VirtualProtect((void*)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy((void*)addr, data, len);
	VirtualProtect((void*)addr, len, oldProtect, &oldProtect);
}

// Use if it's not a 2 byte jz
void __stdcall WriteRelJump(UInt32 jumpSrc, UInt32 jumpTgt)  
{
	// ask to be able to modify the desired region of code (normally programs prevent code being modified by other code to prevent exploits)
	UInt32 oldProtect;
	VirtualProtect((void*)jumpSrc, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	
	*(UInt8*)jumpSrc = 0xE9;  // write the 'long jump' instruction
	*(UInt32*)(jumpSrc + 1) = jumpTgt - jumpSrc - 5;  // write the relative offset 

	// restore old protection of code
	VirtualProtect((void*)jumpSrc, 5, oldProtect, &oldProtect);
}

void __stdcall WriteRelCall(UInt32 jumpSrc, UInt32 jumpTgt)
{
	// ask to be able to modify the desired region of code (normally programs prevent code being modified by other code to prevent exploits)
	UInt32 oldProtect;
	VirtualProtect((void*)jumpSrc, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	
	*(UInt8*)jumpSrc = 0xE8;  // write the 'call' instruction
	*(UInt32*)(jumpSrc + 1) = jumpTgt - jumpSrc - 5;  // write the relative offset 

	// restore old protection of code
	VirtualProtect((void*)jumpSrc, 5, oldProtect, &oldProtect);
}

void WriteRelJnz(UInt32 jumpSrc, UInt32 jumpTgt)
{
	// jnz rel32, same as jne
	SafeWrite16(jumpSrc, 0x850F);
	SafeWrite32(jumpSrc + 2, jumpTgt - jumpSrc - 2 - 4);
}

__declspec(noinline) void WriteRelJe(UInt32 jumpSrc, UInt32 jumpTgt)
{
	// je rel32, same as jz
	SafeWrite16(jumpSrc, 0x840F);
	SafeWrite32(jumpSrc + 2, jumpTgt - jumpSrc - 2 - 4);
}

void WriteRelJle(UInt32 jumpSrc, UInt32 jumpTgt)
{
	// jle rel32
	SafeWrite16(jumpSrc, 0x8E0F);
	SafeWrite32(jumpSrc + 2, jumpTgt - jumpSrc - 2 - 4);
}

// numArgs does not factor in *this objects.
// Taken from lStewieAl.
void NopFunctionCall(UInt32 addr, UInt32 numArgs)
{
	if (numArgs == 0)
	{
		// write 5 byte nop instead of add esp, 0
		SafeWriteBuf(addr, "\x0F\x1F\x44\x00\x00", 5);
	}
	else
	{
		UInt32 oldProtect;
		VirtualProtect((void*)addr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
		*(UInt16*)addr = 0xC483; // add esp, X
		*(UInt8*)(addr + 2) = numArgs * 4;
		*(UInt16*)(addr + 3) = 0xFF89; // mov edi, edi (nop)
		VirtualProtect((void*)addr, 4, oldProtect, &oldProtect);
	}
}

// Taken from lStewieAl.
void NopFunctionCall(UInt32 addr)
{
	NopFunctionCall(addr, 0);
}

// Taken from lStewieAl.
void NopIndirectCall(UInt32 addr, UInt32 numArgs)
{
	NopFunctionCall(addr, numArgs);
	SafeWrite8(addr + 5, 0x90);
}

// Taken from lStewieAl.
void NopIndirectCall(UInt32 addr)
{
	NopIndirectCall(addr, 0);
}

// Taken from lStewieAl.
// Returns the address of the jump/called function, assuming there is one.
UInt32 GetRelJumpAddr(UInt32 jumpSrc)
{
	return *(UInt32*)(jumpSrc + 1) + jumpSrc + 5;
}