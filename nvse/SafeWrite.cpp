#include "SafeWrite.h"

#include "GameAPI.h" // for MessageBox

#include "ShowOffNVSE.h" // to delay error prints until console manager is fully loaded

void __stdcall SafeWrite8(uint32_t addr, uint32_t data)
{
	DWORD oldProtect;
	VirtualProtect((void*)addr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
	*(uint8_t*)addr = data;
	VirtualProtect((void*)addr, 4, oldProtect, &oldProtect);
}

void __stdcall SafeWrite16(uint32_t addr, uint32_t data)
{
	DWORD oldProtect;
	VirtualProtect((void*)addr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
	*(uint16_t*)addr = data;
	VirtualProtect((void*)addr, 4, oldProtect, &oldProtect);
}

void __stdcall SafeWrite32(uint32_t addr, uint32_t data)
{
	DWORD oldProtect;
	VirtualProtect((void*)addr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
	*(uint32_t*)addr = data;
	VirtualProtect((void*)addr, 4, oldProtect, &oldProtect);
}

void __stdcall SafeWriteBuf(uint32_t addr, const void* data, uint32_t len)
{
	DWORD oldProtect;
	VirtualProtect((void*)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy((void*)addr, data, len);
	VirtualProtect((void*)addr, len, oldProtect, &oldProtect);
}

// Use if it's not a 2 byte jz
bool __stdcall WriteRelJump(uint32_t jumpSrc, uint32_t jumpTgt, std::optional<std::array<uint8_t, 5>> originalBytes)
{
	if (originalBytes.has_value())
	{
		auto& oldBytes = originalBytes.value();
		for (int i = 0; i < oldBytes.size(); ++i)
		{
			if (*reinterpret_cast<uint8_t*>(jumpSrc + i) != oldBytes[i])
			{
				_ERROR("Cannot write jump hook at address 0x%X; another plugin's hook already overwrote that code with %X %X %X %X %X.", 
					jumpSrc, *reinterpret_cast<uint8_t*>(jumpSrc), *reinterpret_cast<uint8_t*>(jumpSrc + 1), *reinterpret_cast<uint8_t*>(jumpSrc + 2),
					*reinterpret_cast<uint8_t*>(jumpSrc + 3), *reinterpret_cast<uint8_t*>(jumpSrc + 4));
				ShowHookConflictErrorMsg();
				return false;
			}
		}
	}

	// ask to be able to modify the desired region of code (normally programs prevent code being modified by other code to prevent exploits)
	DWORD oldProtect;
	VirtualProtect((void*)jumpSrc, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	
	*(uint8_t*)jumpSrc = 0xE9;  // write the 'long jump' instruction
	*(uint32_t*)(jumpSrc + 1) = jumpTgt - jumpSrc - 5;  // write the relative offset 

	// restore old protection of code
	VirtualProtect((void*)jumpSrc, 5, oldProtect, &oldProtect);
	return true;
}

void __stdcall WriteRelCall(uint32_t jumpSrc, uint32_t jumpTgt)
{
	// ask to be able to modify the desired region of code (normally programs prevent code being modified by other code to prevent exploits)
	DWORD oldProtect;
	VirtualProtect((void*)jumpSrc, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

	*(uint8_t*)(jumpSrc) = 0xE8; // write call instruction
	*(uint32_t*)(jumpSrc + 1) = jumpTgt - jumpSrc - 5;  // replace the relative offset for the existing call

	// restore old protection of code
	VirtualProtect((void*)jumpSrc, 5, oldProtect, &oldProtect);
}

bool __stdcall ReplaceCall(uint32_t jumpSrc, uint32_t jumpTgt, std::optional<std::array<uint8_t, 4>> originalBytes, bool acceptOverwrite)
{
	if (!AddrIsCall(jumpSrc)) {
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
			if (*reinterpret_cast<uint8_t*>(jumpSrc + i + 1) != oldBytes[i])
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
	DWORD oldProtect;
	VirtualProtect((void*)jumpSrc, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	
	*(uint32_t*)(jumpSrc + 1) = jumpTgt - jumpSrc - 5;  // replace the relative offset for the existing call

	// restore old protection of code
	VirtualProtect((void*)jumpSrc, 5, oldProtect, &oldProtect);
	return true;
}

void WriteRelJnz(uint32_t jumpSrc, uint32_t jumpTgt)
{
	// jnz rel32, same as jne
	SafeWrite16(jumpSrc, 0x850F);
	SafeWrite32(jumpSrc + 2, jumpTgt - jumpSrc - 2 - 4);
}

__declspec(noinline) void WriteRelJe(uint32_t jumpSrc, uint32_t jumpTgt)
{
	// je rel32, same as jz
	SafeWrite16(jumpSrc, 0x840F);
	SafeWrite32(jumpSrc + 2, jumpTgt - jumpSrc - 2 - 4);
}

void WriteRelJle(uint32_t jumpSrc, uint32_t jumpTgt)
{
	// jle rel32
	SafeWrite16(jumpSrc, 0x8E0F);
	SafeWrite32(jumpSrc + 2, jumpTgt - jumpSrc - 2 - 4);
}

// numArgs does not factor in *this objects.
// Taken from lStewieAl.
void NopFunctionCall(uint32_t addr, uint32_t numArgs)
{
	if (numArgs == 0)
	{
		// write 5 byte nop instead of add esp, 0
		SafeWriteBuf(addr, "\x0F\x1F\x44\x00\x00", 5);
	}
	else
	{
		DWORD oldProtect;
		VirtualProtect((void*)addr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
		*(uint16_t*)addr = 0xC483; // add esp, X
		*(uint8_t*)(addr + 2) = numArgs * 4;
		*(uint16_t*)(addr + 3) = 0xFF89; // mov edi, edi (nop)
		VirtualProtect((void*)addr, 4, oldProtect, &oldProtect);
	}
}

// Taken from lStewieAl.
void NopFunctionCall(uint32_t addr)
{
	NopFunctionCall(addr, 0);
}

// Taken from lStewieAl.
void NopIndirectCall(uint32_t addr, uint32_t numArgs)
{
	NopFunctionCall(addr, numArgs);
	SafeWrite8(addr + 5, 0x90);
}

// Taken from lStewieAl.
void NopIndirectCall(uint32_t addr)
{
	NopIndirectCall(addr, 0);
}

uint32_t GetRelJumpAddr(uint32_t jumpSrc)
{
	// Gets the relative part *(jmpSrc + 1), adds the address of jumpSrc to make it an absolute address instead of relative, 
	// ...and adds 5 (since the relative address ignores the 5 bytes of the jump/call)
	return *(uint32_t*)(jumpSrc + 1) + jumpSrc + 5;
}

bool AddrIsCall(uint32_t addr)
{
	return *reinterpret_cast<uint8_t*>(addr) == 0xE8;
}

bool AddrIsRelJump(uint32_t addr)
{
	return *reinterpret_cast<uint8_t*>(addr) == 0xE9; // long jump
}

// Taken from xNVSE
uint8_t* GetParentBasePtr(void* addressOfReturnAddress, bool lambda)
{
	auto* basePtr = static_cast<uint8_t*>(addressOfReturnAddress) - 4;
#if _DEBUG
	if (lambda) // in debug mode, lambdas are wrapped inside a closure wrapper function, so one more step needed
		basePtr = *reinterpret_cast<uint8_t**>(basePtr);
#endif
	return *reinterpret_cast<uint8_t**>(basePtr);
}

bool g_showedRuntimeHookConflictError = false;

void ShowHookConflictErrorMsg()
{
	if (!g_showedRuntimeHookConflictError)
	{
		std::string msg = "Showoff xNVSE: Detected conflict with another plugin detected while trying to hook the game; please report what you see in ShowOffNVSE.log, or check if there is an update available.";
		if (PlayerCharacter::GetSingleton() != nullptr) // if plugin init is complete
			Console_Print(msg.c_str());
		else
			g_deferredPrints.emplace_back(std::move(msg));
		g_showedRuntimeHookConflictError = true;
	}
}
