#include "GameFixes.h"

#include "SafeWrite.h"
#include "ShowOffNVSE.h"

namespace GameFixes
{
	namespace ShowRaceMenu
	{
		void Patch()
		{
			// Prevent ShowRaceMenu from resetting active temp effects.
			NopFunctionCall(0x7ADDC7, 1);

			// Prevent ShowRaceMenu from resetting abilities.
			PatchMemoryNop(0x7ADDD2, 5);
		}
	}

	namespace PatchResetCell
	{
		__declspec(naked) void GetShouldRespawnHook()
		{
			static const UInt32 retnAddr = 0x54E1CD,
				getShouldRespawnAddr = 0x881C90;

			enum {
				cellDetachTime = -0x4 //-0x8
			};
			_asm
			{
				//cmp [ebp + cellDetachTime], -1		//did not work!
				cmp dword ptr ss : [ebp + cellDetachTime] , -1
				jg doNormal
				jnz skipIniCheck //jump if -2, essentially
				cmp g_bResetInteriorResetsActors, 0
				jz doNormal
			skipIniCheck :
				mov al, 1
				jmp retnAddr

			doNormal :
				call getShouldRespawnAddr
				jmp retnAddr
			}
		}

		void WriteHook()
		{
			// replace jnz -> JG	(compares with -1, add support for -2)
			SafeWrite8(0x54E09A, 0x7F);

			//Replace Actor::GetShouldRespawn call in Cell::HandleResets
			//...Cuz it checks for time even though ResetInterior was called.
			WriteRelJump(0x54E1C8, (UInt32)GetShouldRespawnHook);
		}
	}

#if 0
	// TODO: FIX
	// Does not work for NPCs, introduces weird behavior where they no longer unequip their weapon.
	namespace EquipOrUnequipItem
	{
		// Vanilla "noEquip" flag for UnequipItem doesn't work; it goes unused.
		namespace FixNoEquip
		{
			CallDetour g_detour1stCall;
			CallDetour g_detour2ndCall;
			CallDetour g_detourDoFix;

			bool g_NoEquip = false;

			// Make it so the item cannot be equipped.
			// Replaces a call to ExtraDataList::GetCannotWear (0x418B10)
			bool __fastcall DoFix(ExtraDataList* nthExtraDataList, void* edx)
			{
				// Do regular code.
				// Cache result, because we'll be updating the CannotWear xData right after.
				auto cannotWear = ThisStdCall<bool>(g_detourDoFix.GetOverwrittenAddr(), nthExtraDataList);

				// Our addition: add "cannotWear" xData if g_NoEquip is true.
				if (!cannotWear && g_NoEquip)
				{
					// ExtraDataList::UpdateCannotWear
					ThisStdCall(0x41AB70, nthExtraDataList, true);
				}

				return cannotWear;
			}

			bool __fastcall FirstHook(TESForm* thisActor, void* edx, TESForm* item,
				SInt32 count, ExtraDataList* xData, bool isWorn, bool unused)
			{
				// NOTE: this trick assumes that we aren't being called by a detour!!
				auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
				auto noEquip = *reinterpret_cast<UInt32*>(ebp + 0x18);
				g_NoEquip = noEquip != 0;

				// do regular code, which will use the global
				bool result = ThisStdCall<bool>(g_detour1stCall.GetOverwrittenAddr(), thisActor, item, count,
					xData, isWorn, unused);

				// reset global so other stuff can work normally
				g_NoEquip = false;

				return result;
			}

			bool __fastcall SecondHook(TESForm* thisActor, void* edx, TESForm* item,
				SInt32 count, ExtraDataList* xData, bool isWorn, bool unused)
			{
				// NOTE: this trick assumes that we aren't being called by a detour!!
				auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
				auto noEquip = *reinterpret_cast<UInt32*>(ebp + 0x18);
				g_NoEquip = noEquip != 0;

				// do regular code, which will use the global
				bool result = ThisStdCall<bool>(g_detour1stCall.GetOverwrittenAddr(), thisActor, item, count,
					xData, isWorn, unused);

				// reset global so other stuff can work normally
				g_NoEquip = false;

				return result;
			}

			void WriteDelayedHooks()
			{
				// These two hooks will set the g_NoEquip global.
				g_detour1stCall.WriteRelCall(0x88D98A, (UInt32)FirstHook);
				g_detour2ndCall.WriteRelCall(0x88D964, (UInt32)SecondHook);

				// Uses the g_NoEquip global set up by the hooks above.
				g_detourDoFix.WriteRelCall(0x4C0DC8, (UInt32)DoFix);
			}
		}
	}
#endif

	void WriteFixes()
	{
		ShowRaceMenu::Patch();
		PatchResetCell::WriteHook();

#if 0
		if (g_bFixCaravanCurrencyRemoval)
		{
			TODO
		}
#endif
		/*
		//TODO: make optional?
		//TODO: need to learn more about why it kinda works..
		Experimental::FixOnAddBlockType::WriteHook();
		Experimental::FixOnDropAndOnUnequipBlockType::WriteHooks();
		*/
	}

	void WriteDelayedFixes()
	{
#if 0
		EquipOrUnequipItem::FixNoEquip::WriteDelayedHooks();
#endif
	}
}