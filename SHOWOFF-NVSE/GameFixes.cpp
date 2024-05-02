#include "GameFixes.h"

#include "SafeWrite.h"
#include "ShowOffNVSE.h"

namespace GameFixes
{
	namespace ShowRaceMenu
	{
		void WriteHooks()
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

	namespace ShowSleepWaitMenu
	{
		UInt32 g_isSleep = false;

		namespace ShowSleepWaitMenu_ParseParameters
		{
			// Will retrieve the isSleep arg value.
			__declspec(naked) void Hook()
			{
				static const UInt32 retnAddr = 0x5E00E0;

				enum {
					isSleepOffset = -0x8
				};
				_asm
				{
					mov		ecx, dword ptr [ebp + isSleepOffset]
					mov		g_isSleep, ecx
					
					// do regular code
					mov     ecx, g_thePlayer
					jmp		retnAddr
				}
			}

			void WriteHook()
			{
				// Warning: xNVSE has a hook at 0x5E00E0 in Cmd_ShowSleepWaitMenu
				WriteRelJump(0x5E00DA, (UInt32)Hook);
			}
		}

		namespace CreateSleepWaitMenu
		{
			CallDetour g_detour;
			void __cdecl Hook(bool isSleep_always1)
			{
				// Use isSleep global value that stores the value from the script function call.
				CdeclCall(g_detour.GetOverwrittenAddr(), g_isSleep);
			}

			void WriteDelayedHook()
			{
				g_detour.WriteDetourCall(0x96A269, (UInt32)Hook);
			}
		}

		void WriteDelayedHooks()
		{
			// Fix for ShowSleepWaitMenu (script function)
			// If checkPreconditions == 1, then it always opens Sleep menu, regardless of isSleep arg
			ShowSleepWaitMenu_ParseParameters::WriteHook(); // doesn't need to be delayed, but w/e
			CreateSleepWaitMenu::WriteDelayedHook();
		}
	}

	namespace PlaceAtReticle
	{
		// Vanilla PlaceAtReticle spawns items in with 100x their maximum health.
		void WriteHook()
		{
			// (Presumably) replace "* 100.0" with "* 1.0"
			// Credits to lStewieAl
			SafeWrite16(0x5DF4DC, 0x2070);
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
				g_detour1stCall.WriteDetourCall(0x88D98A, (UInt32)FirstHook);
				g_detour2ndCall.WriteDetourCall(0x88D964, (UInt32)SecondHook);

				// Uses the g_NoEquip global set up by the hooks above.
				g_detourDoFix.WriteDetourCall(0x4C0DC8, (UInt32)DoFix);
			}
		}
	}
#endif
	/* Doesn't fix the bug, RIP
	namespace PatchGetVATSValueNotWorkingForDamagePreview
	{
		CallDetour g_detour;

		void* __cdecl HookGetCurrentQueuedAction()
		{
			auto* result = CdeclCall<void*>(g_detour.GetOverwrittenAddr());
			auto* vats = VATSMenu::GetSingleton();
			if (!result && vats && VATSCameraData::GetSingleton()->mode == 2)
				result = &vats->targetInfo;
			return result;
		}

		// Issue is likely caused by GetCurrentVATSQueuedAction at 0x7F5280, which is only called by GetVATSValue.
		// For whatever reason, VATSMenu's byte118 is set to 0 when GetCurrentVATSQueuedAction is invoked to calculate the preview damage.
		// It only returns the queued action if it's set to 1, so we'll temporarily set it to 1 when calculating preview damage to mitigate potential issues.
		double __cdecl HookGetHealthDamage(void* vatsQueuedAction)
		{
			auto* vats = VATSMenu::GetSingleton();
			vats->byte118 = 1;
			auto result = CdeclCall<double>(g_detour.GetOverwrittenAddr(), vatsQueuedAction);
			vats->byte118 = 0;
			return result;
		}

		void WriteHook()
		{
			//g_detour.WriteDetourCall(0x7F4A72, (UInt32)HookGetHealthDamage);
			g_detour.WriteDetourCall(0x594E9A, (UInt32)HookGetCurrentQueuedAction);
		}
	}
	*/

	void WriteFixes()
	{
		ShowRaceMenu::WriteHooks();
		PatchResetCell::WriteHook();
		PlaceAtReticle::WriteHook();
#if 0
		PatchGetVATSValueNotWorkingForDamagePreview::WriteHook();

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
		ShowSleepWaitMenu::WriteDelayedHooks();
#if 0
		EquipOrUnequipItem::FixNoEquip::WriteDelayedHooks();
#endif
	}
}