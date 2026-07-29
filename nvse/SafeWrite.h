#pragma once

#include <intrin.h>
#include <array>
#include <optional>

#define __HOOK __declspec(naked) void
// Credits to Kormakur for the idea.

void __stdcall SafeWrite8(uint32_t addr, uint32_t data);
void __stdcall SafeWrite16(uint32_t addr, uint32_t data);
void __stdcall SafeWrite32(uint32_t addr, uint32_t data);
void __stdcall SafeWriteBuf(uint32_t addr, const void* data, uint32_t len);

// 5 bytes, returns false if the original bytecode was changed.
bool __stdcall WriteRelJump(uint32_t jumpSrc, uint32_t jumpTgt, std::optional<std::array<uint8_t, 5>> originalBytes = {});
// 5 bytes
void __stdcall WriteRelCall(uint32_t jumpSrc, uint32_t jumpTgt);
// 5 bytes, returns false if there was a hook conflict.
bool __stdcall ReplaceCall(uint32_t jumpSrc, uint32_t jumpTgt, 
	std::optional<std::array<uint8_t, 4>> originalBytes = {}, 
	bool acceptOverwrite = false);

// 6 bytes
void WriteRelJnz(uint32_t jumpSrc, uint32_t jumpTgt);
void WriteRelJe(uint32_t jumpSrc, uint32_t jumpTgt);
void WriteRelJle(uint32_t jumpSrc, uint32_t jumpTgt);

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
void NopFunctionCall(uint32_t addr, uint32_t numArgs);
void NopFunctionCall(uint32_t addr);
void NopIndirectCall(uint32_t addr, uint32_t numArgs);
void NopIndirectCall(uint32_t addr);

// Taken from lStewieAl.
// Returns the address of the jump/called function, assuming there is one.
uint32_t GetRelJumpAddr(uint32_t jumpSrc);
bool AddrIsCall(uint32_t addr);
bool AddrIsRelJump(uint32_t addr);

uint8_t* GetParentBasePtr(void* addressOfReturnAddress, bool lambda = false);

extern bool g_showRuntimeHookConflictError;

// Stores the function-to-call before overwriting it, to allow calling the overwritten function after our hook is over.
class CallDetour
{
	uint32_t overwritten_addr = 0;
public:
	CallDetour() = default;
	CallDetour(uint32_t defaultCallAddr) : overwritten_addr(defaultCallAddr) { ; };

	void WriteDetourCall(uint32_t jumpSrc, uint32_t jumpTgt, std::optional<std::array<uint8_t, 4>> originalBytes = {})
	{
		overwritten_addr = GetRelJumpAddr(jumpSrc);
		if (!::ReplaceCall(jumpSrc, jumpTgt, originalBytes, false))
			overwritten_addr = 0;
	}
	[[nodiscard]] uint32_t GetOverwrittenAddr() const { return overwritten_addr; }
};

void ShowHookConflictErrorMsg();