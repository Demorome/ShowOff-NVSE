#include "SafeWrite.h"

#include "GameAPI.h" // for MessageBox

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
bool __stdcall WriteRelJump(UInt32 jumpSrc, UInt32 jumpTgt, std::optional<std::array<UInt8, 5>> originalBytes)
{
	if (originalBytes.has_value())
	{
		auto& oldBytes = originalBytes.value();
		for (int i = 0; i < oldBytes.size(); ++i)
		{
			if (*reinterpret_cast<UInt8*>(jumpSrc + i) != oldBytes[i])
			{
				_ERROR("Cannot write jump hook at address 0x%X; another plugin's hook already overwrote that code.", jumpSrc);
				ShowHookConflictErrorMsg();
				return false;
			}
		}
	}

	// ask to be able to modify the desired region of code (normally programs prevent code being modified by other code to prevent exploits)
	UInt32 oldProtect;
	VirtualProtect((void*)jumpSrc, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	
	*(UInt8*)jumpSrc = 0xE9;  // write the 'long jump' instruction
	*(UInt32*)(jumpSrc + 1) = jumpTgt - jumpSrc - 5;  // write the relative offset 

	// restore old protection of code
	VirtualProtect((void*)jumpSrc, 5, oldProtect, &oldProtect);
	return true;
}

void __stdcall WriteRelCall(UInt32 jumpSrc, UInt32 jumpTgt)
{
	// ask to be able to modify the desired region of code (normally programs prevent code being modified by other code to prevent exploits)
	UInt32 oldProtect;
	VirtualProtect((void*)jumpSrc, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

	*(UInt8*)(jumpSrc) = 0xE8; // write call instruction
	*(UInt32*)(jumpSrc + 1) = jumpTgt - jumpSrc - 5;  // replace the relative offset for the existing call

	// restore old protection of code
	VirtualProtect((void*)jumpSrc, 5, oldProtect, &oldProtect);
}

bool __stdcall ReplaceCall(UInt32 jumpSrc, UInt32 jumpTgt, std::optional<std::array<UInt8, 4>> originalBytes, bool acceptOverwrite)
{
	if (*reinterpret_cast<UInt8*>(jumpSrc) != 0xE8) {
		if (!acceptOverwrite)
		{
			_ERROR("Cannot replace call at address 0x%X; another plugin's hook made it no longer a function call.", jumpSrc);
			ShowHookConflictErrorMsg();
		}
		return false;
	}

	if (originalBytes.has_value())
	{
		auto& oldBytes = originalBytes.value();
		for (int i = 0; i < oldBytes.size(); ++i)
		{
			// +1 due to already having checked the first byte for call instruction (0xE8)
			if (*reinterpret_cast<UInt8*>(jumpSrc + i + 1) != oldBytes[i])
			{
				if (!acceptOverwrite)
				{
					_ERROR("Cannot replace call at address 0x%X; another plugin's hook already overwrote that code.", jumpSrc);
					ShowHookConflictErrorMsg();
				}
				return false;
			}
		}
	}

	// ask to be able to modify the desired region of code (normally programs prevent code being modified by other code to prevent exploits)
	UInt32 oldProtect;
	VirtualProtect((void*)jumpSrc, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	
	*(UInt32*)(jumpSrc + 1) = jumpTgt - jumpSrc - 5;  // replace the relative offset for the existing call

	// restore old protection of code
	VirtualProtect((void*)jumpSrc, 5, oldProtect, &oldProtect);
	return true;
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

// Taken from xNVSE
UInt8* GetParentBasePtr(void* addressOfReturnAddress, bool lambda)
{
	auto* basePtr = static_cast<UInt8*>(addressOfReturnAddress) - 4;
#if _DEBUG
	if (lambda) // in debug mode, lambdas are wrapped inside a closure wrapper function, so one more step needed
		basePtr = *reinterpret_cast<UInt8**>(basePtr);
#endif
	return *reinterpret_cast<UInt8**>(basePtr);
}

bool g_showedRuntimeHookConflictError = false;

void ShowHookConflictErrorMsg()
{
	if (!g_showedRuntimeHookConflictError)
	{
		Console_Print("Showoff xNVSE: Detected conflict with another plugin detected while trying to hook the game; please report what you see in the log file, or check if there is an update available.");
		g_showedRuntimeHookConflictError = true;
	}
}
