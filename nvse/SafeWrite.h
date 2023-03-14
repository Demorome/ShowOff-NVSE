#pragma once

#include <intrin.h>

#define __HOOK __declspec(naked) void
// Credits to Kormakur for the idea.

void __stdcall SafeWrite8(UInt32 addr, UInt32 data);
void __stdcall SafeWrite16(UInt32 addr, UInt32 data);
void __stdcall SafeWrite32(UInt32 addr, UInt32 data);
void __stdcall SafeWriteBuf(UInt32 addr, const void* data, UInt32 len);

// 5 bytes
void __stdcall WriteRelJump(UInt32 jumpSrc, UInt32 jumpTgt);
void __stdcall WriteRelCall(UInt32 jumpSrc, UInt32 jumpTgt);

// 6 bytes
void WriteRelJnz(UInt32 jumpSrc, UInt32 jumpTgt);
void WriteRelJe(UInt32 jumpSrc, UInt32 jumpTgt);
void WriteRelJle(UInt32 jumpSrc, UInt32 jumpTgt);

// Size is the amount of bytes until the next instruction.
// Credits: Copied from JG, likely made thanks to lStewieAl.
static void PatchMemoryNop(ULONG_PTR Address, SIZE_T Size)
{
	DWORD d = 0;
	VirtualProtect((LPVOID)Address, Size, PAGE_EXECUTE_READWRITE, &d);

	for (SIZE_T i = 0; i < Size; i++)
		*(volatile BYTE*)(Address + i) = 0x90; //0x90 == opcode for NOP

	VirtualProtect((LPVOID)Address, Size, d, &d);

	FlushInstructionCache(GetCurrentProcess(), (LPVOID)Address, Size);
}
void NopFunctionCall(UInt32 addr, UInt32 numArgs);
void NopFunctionCall(UInt32 addr);
void NopIndirectCall(UInt32 addr, UInt32 numArgs);
void NopIndirectCall(UInt32 addr);

UInt32 GetRelJumpAddr(UInt32 jumpSrc);

UInt8* GetParentBasePtr(void* addressOfReturnAddress, bool lambda = false);

// Stores the function-to-call before overwriting it, to allow calling the overwritten function after our hook is over.
class CallDetour
{
	UInt32 overwritten_addr = 0;
public:
	void WriteRelCall(UInt32 jumpSrc, UInt32 jumpTgt)
	{
		if (*reinterpret_cast<UInt8*>(jumpSrc) != 0xE8) {
			_ERROR("Cannot write detour; jumpSrc is not a function call.");
			return;
		}
		overwritten_addr = GetRelJumpAddr(jumpSrc);
		::WriteRelCall(jumpSrc, jumpTgt);
	}
	[[nodiscard]] UInt32 GetOverwrittenAddr() const { return overwritten_addr; }
};