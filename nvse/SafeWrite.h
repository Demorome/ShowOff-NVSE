#pragma once

#include <intrin.h>
#include <array>
#include <optional>

#define __HOOK __declspec(naked) void
// Credits to Kormakur for the idea.

void __stdcall SafeWrite8(UInt32 addr, UInt32 data);
void __stdcall SafeWrite16(UInt32 addr, UInt32 data);
void __stdcall SafeWrite32(UInt32 addr, UInt32 data);
void __stdcall SafeWriteBuf(UInt32 addr, const void* data, UInt32 len);

// 5 bytes, returns false if the original bytecode was changed.
bool __stdcall WriteRelJump(UInt32 jumpSrc, UInt32 jumpTgt, std::optional<std::array<UInt8, 5>> originalBytes = {});
// 5 bytes
void __stdcall WriteRelCall(UInt32 jumpSrc, UInt32 jumpTgt);
// 5 bytes, returns false if there was a hook conflict.
bool __stdcall ReplaceCall(UInt32 jumpSrc, UInt32 jumpTgt, 
	std::optional<std::array<UInt8, 4>> originalBytes = {}, 
	bool acceptOverwrite = false);

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

extern bool g_showRuntimeHookConflictError;

// Stores the function-to-call before overwriting it, to allow calling the overwritten function after our hook is over.
class CallDetour
{
	UInt32 overwritten_addr = 0;
public:
	CallDetour() = default;
	CallDetour(UInt32 defaultCallAddr) : overwritten_addr(defaultCallAddr) { ; };

	void WriteDetourCall(UInt32 jumpSrc, UInt32 jumpTgt, std::optional<std::array<UInt8, 4>> originalBytes = {})
	{
		overwritten_addr = GetRelJumpAddr(jumpSrc);
		if (!::ReplaceCall(jumpSrc, jumpTgt, originalBytes, false))
			overwritten_addr = 0;
	}
	[[nodiscard]] UInt32 GetOverwrittenAddr() const { return overwritten_addr; }
};

void ShowHookConflictErrorMsg();