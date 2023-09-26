#pragma once
#include "EventParams.h"
#include "GameEffects.h"

// Credits to Karut (from JohnnyGuitar) for making the Event Framework.
EventInformation* OnCornerMessage; 

DEFINE_COMMAND_ALT_PLUGIN(SetShowOffOnCornerMessageEventHandler, SetOnCornerMessageEventHandler, "", false, kParams_Event);
bool Cmd_SetShowOffOnCornerMessageEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove;
	Script* script;
	UInt32 flags = 0;  //reserved for future use
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags) || NOT_TYPE(script, Script))) return true;
	if (OnCornerMessage)
	{
		if (setOrRemove)
			OnCornerMessage->RegisterEvent(script, NULL);
		else OnCornerMessage->RemoveEvent(script, NULL);
	}
	return true;
}

EventInformation* OnAuxTimerStart;
DEFINE_COMMAND_ALT_PLUGIN(SetOnAuxTimerStartHandler, SetOnTimerStartHandler, "", false,
	kParams_EventNoFlag_OneString_OneOptionalForm);
bool Cmd_SetOnAuxTimerStartHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove;
	Script* script;
	GenericFilters filters[2];
	char strBuf[0x80];
	filters[0].form = nullptr;
	filters[1].str = strBuf;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, filters[1].str, &filters[0].form)
		|| NOT_TYPE(script, Script)))
	{
		return true;
	}

	if (!filters[0].form)
	{
		if (!thisObj)
			thisObj = g_thePlayer;
		filters[0].form = thisObj;
	}

	filters[0].refID = filters[0].form->refID;
	if (OnAuxTimerStart)
	{
		if (setOrRemove)
			OnAuxTimerStart->RegisterEvent(script, (void**)&filters);
		else OnAuxTimerStart->RemoveEvent(script, (void**)&filters);
	}
	return true;
}

EventInformation* OnAuxTimerStop;
DEFINE_COMMAND_ALT_PLUGIN(SetOnAuxTimerStopHandler, SetOnTimerStopHandler, "", false,
	kParams_EventNoFlag_OneString_OneOptionalForm);
bool Cmd_SetOnAuxTimerStopHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove;
	Script* script;
	GenericFilters filters[2];
	char strBuf[0x80];
	filters[0].form = nullptr;
	filters[1].str = strBuf;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, filters[1].str, &filters[0].form)
		|| NOT_TYPE(script, Script)))
	{
		return true;
	}

	if (!filters[0].form)
	{
		if (!thisObj)
			thisObj = g_thePlayer;
		filters[0].form = thisObj;
		if (!filters[0].form)
			return true;
	}

	filters[0].refID = filters[0].form->refID;
	if (OnAuxTimerStop)
	{
		if (setOrRemove)
			OnAuxTimerStop->RegisterEvent(script, (void**)filters);
		else OnAuxTimerStop->RemoveEvent(script, (void**)filters);
	}
	return true;
}

EventInformation* OnAuxTimerUpdate;
DEFINE_COMMAND_ALT_PLUGIN(SetOnAuxTimerUpdateHandler, SetOnTimerUpdateHandler, "", false,
	kParams_EventNoFlag_OneString_OneOptionalForm);
bool Cmd_SetOnAuxTimerUpdateHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove;
	Script* script;
	GenericFilters filters[2];
	char strBuf[0x80];
	filters[0].form = nullptr;
	filters[1].str = strBuf;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, filters[1].str, &filters[0].form)
		|| NOT_TYPE(script, Script)))
	{
		return true;
	}

	if (!filters[0].form)
	{
		if (!thisObj)
			thisObj = g_thePlayer;
		filters[0].form = thisObj;
		if (!filters[0].form)
			return true;
	}

	filters[0].refID = filters[0].form->refID;
	if (OnAuxTimerUpdate)
	{
		if (setOrRemove)
			OnAuxTimerUpdate->RegisterEvent(script, (void**)filters);
		else OnAuxTimerUpdate->RemoveEvent(script, (void**)filters);
	}
	return true;
}

void FlushJGInterfaceEvents()
{
	OnAuxTimerStop->FlushEventCallbacks();
	OnAuxTimerStart->FlushEventCallbacks();
	OnAuxTimerUpdate->FlushEventCallbacks();
}

namespace CornerMessageHooks
{
	// iconPath and soundPath can be null.
	bool __fastcall CornerMessageEventHook(HUDMainMenu* menu, void* edx, char* msgText, eEmotion IconType, char* iconPath, char* soundPath, float displayTime, bool instantEndCurrentMessage)
	{
		bool bFireEvent = true;
		
		// Avoid throwing exceptions with bad char*
		std::string msgTextStr, iconPathStr, soundPathStr;
		msgTextStr = iconPathStr = soundPathStr = "<void>";
		if (msgText && msgText[0]) msgTextStr = msgText;
		else bFireEvent = false;
		if (iconPath && iconPath[0]) iconPathStr = iconPath;
		if (soundPath && soundPath[0]) soundPathStr = soundPath;

		if (bFireEvent) {
			for (auto const& callback : OnCornerMessage->EventCallbacks) {
				FunctionCallScriptAlt(callback.ScriptForEvent, nullptr, OnCornerMessage->numMaxArgs, msgTextStr.c_str(), IconType, iconPathStr.c_str(), soundPathStr.c_str(), *(UInt32*)&displayTime);
			}
		}
#if false
		Console_Print("==CornerMessageEventHook==\n -msgText: %s\n -IconType: %d\n -iconPath: %s\n -soundPath: %s\n -displayTime: %f\n -instantEndCurrentMessage: %d", msgText, IconType, iconPath, soundPath, displayTime, instantEndCurrentMessage);
#endif
		return ThisStdCall_B(0x775380, menu, msgText, IconType, iconPath, soundPath, displayTime, instantEndCurrentMessage);
	}
	
	void WriteDelayedHook()
	{
		// SPECIAL CASE: 0x8B959B is Nop'd by Tweaks if bNoCrippleCriticalMessages = 1 && g_bPatchCripple = 1.
		auto const tweakConflictAddr = 0x8B959B;
		auto const vanillaDerefAddr = 0xEBBDE0E8;

		// TODO: write a RelJump inside the function itself for compatibility.
		
		std::vector cornerMessageHookJmpSrc{ 0x705379, 0x7EE74D, 0x7EE87D, 0x7EEA6C, 0x833303, tweakConflictAddr };

		// Credits for hook collision detection method goes to lStewieAl.
		if (*(UInt32*)tweakConflictAddr != vanillaDerefAddr)
		{
			cornerMessageHookJmpSrc.erase(
				std::find(cornerMessageHookJmpSrc.begin(), cornerMessageHookJmpSrc.end(), tweakConflictAddr)
			);
#if _DEBUG
			_DMESSAGE("Conflict detected with (presumably) Tweaks, preventing hook from being installed at %x", tweakConflictAddr);
#endif
		}
		
		for (auto const jmpSrc : cornerMessageHookJmpSrc)
		{
			ReplaceCall(jmpSrc, (UInt32)CornerMessageEventHook);
		}
	}
}

#if 0
namespace ActorValueChangeHooks
{
	void __fastcall ActorValueChange_EventHandler(ActorValueOwner *avOwner, int avCode, float previousVal, float newVal, ActorValueOwner *attackerAvOwner)
	{
		// todo: get forms from avOwners, pass to UDF
		
		for (auto const& callback : OnActorValueChange->EventCallbacks) 
		{
			FunctionCallScriptAlt(callback.ScriptForEvent, nullptr, OnActorValueChange->numMaxArgs, *(UInt32*)&previousVal, *(UInt32*)&newVal);
		}
		
#if _DEBUG
		Console_Print("==ActorValueChangeHook==\n");
#endif
	}

	__declspec(naked) void HandleAVChangeHook()
	{
		static const UInt32 overwrittenFuncAddr = 0x406D70;
		
		enum Offsets
		{
			avCode = 0xC,
			affectedAvOwner = 0x8,
			attackerAvOwner = 0x18,
			newVal = 0x14,
			previousVal = 0x10,
		};

		_asm
		{
			push    ebp
			mov     ebp, esp

			
			// Call GetActorValueInfo (what was overwritten)
			// AV code was pushed on stack right before this.
			call overwrittenFuncAddr
			// result is now in eax, which will not be affected before being checked at 0x66EE60.

			
			// Set up to call ActorValueChange_EventHandler
			/*
			mov ecx, [ebp + affectedAvOwner]
			mov edx, [ebp + avCode]

			// Is pushing directly a bad thing?
			push [ebp + affectedAvOwner]
			push [ebp + newVal]
			push [ebp + previousVal]
			
			call ActorValueChange_EventHandler
			*/
			// Function is __fastcall, so no need to clean up the args.

			
			mov     esp, ebp
			pop     ebp
			// Pop return address from the stack (given by Call).
			ret
		}
	}
	
	void WriteHook()
	{
		// Replace "GetActorValueInfo(avCode)" call
		ReplaceCall(0x66EE58, (UInt32)HandleAVChangeHook);
	}
}
#endif

namespace OnPreActivate
{
	constexpr char eventName[] = "ShowOff:OnPreActivate";

	UInt32 __fastcall HandleEvent(TESObjectREFR* activated, Actor* activator)
	{
		// Will be set if opening with a key.
		auto* doorAboutToBeDoubleActivated = *reinterpret_cast<TESObjectREFR**>(0x11C9350);
		if (doorAboutToBeDoubleActivated && doorAboutToBeDoubleActivated == activated)
			return true;

		// Will be set if opening with a key.
		auto* containerAboutToBeDoubleActivated = *reinterpret_cast<TESObjectREFR**>(0x11C92D0);
		if (containerAboutToBeDoubleActivated && containerAboutToBeDoubleActivated == activated)
			return true;

		// Need to check a bunch of conditions to verify event would run as if hooking from 0x573347

		// Check if activating ash/goo pile.
		// Old OnPreActivate didn't run for them, only their sub-activation to get the actor's inventory.
		if (activated->baseForm == *reinterpret_cast<TESForm**>(0x11CA27C)
			|| activated->baseForm == *reinterpret_cast<TESForm**>(0x11CA280))
		{
			return true;
		}

		if (activator == g_thePlayer)
		{
			if (InterfaceManager::GetSingleton()->pipBoyMode == 2)
				return 2;  // sneakily patches NVSE's OnActivate to not run in this instance
			// Magic number 2 = will be handled in my ASM as 1, but will still early-exit the func.

			if (g_thePlayer->is3rdPersonVisible != g_thePlayer->bThirdPerson)
				return false; // sneakily patches NVSE's OnActivate to not run in this instance

			if (g_thePlayer->GetIsChildSize(0) && StdCall<bool>(0x8859E0, activated)) // TESObjectREFR::CanChildUse
				return true; // whatever, let NVSE's OnActivate run here, as a treat

			if (activated->IsDestroyed() && !activated->IsActor())
				return true; // NVSE's OnActivate will run here, yes, and to change it now would probably break stuff.
		}

		auto constexpr resultCallback = [](NVSEArrayVarInterface::Element& result, void* shouldActivateAdrr) -> bool
		{
			if (UInt32 &shouldActivate = *static_cast<UInt32*>(shouldActivateAdrr))
			{
				if (result.IsValid())
					shouldActivate = result.Bool();
			}
			return true;
		};
		UInt32 shouldActivate = true;
		g_eventInterface->DispatchEventAlt(eventName, resultCallback, &shouldActivate, activated, activator, &shouldActivate);
		return shouldActivate;
	}	//result in EAX

	__HOOK Hook()
	{
		static UInt32 const NormalRetnAddr = 0x57318E,
			EarlyEndAddr = 0x5737A3;
		enum
		{
			activator = 8
		};
		_asm
		{
			//== Do regular code
			mov [ebp - 0x12C], ecx
			mov [ebp - 1], 0

			//== Our code
			// Check if this instance of TESObjectREFR::Activate was called by Activate func.
			// (We don't want the event to run for that)
			mov		eax, dword ptr [ebp + 4]	//rtn addr
			cmp		eax, 0x5B5B1D	//one of the return addresses to Activate_Execute
			je		DoNormal
			cmp		eax, 0x5B5B4D
			je		DoNormal
			// Check if called by fade-in func (avoid running event twice for load door activation)
			cmp		eax, 0x8FEEB9
			je		DoNormal
			// Avoid running event if opening locked ref - already had the opportunity to prevent activation before lockpick menu.
			cmp		eax, 0x78F95B
			je		DoNormal
			// Avoid running event from StartConversation script func call 
			cmp		eax, 0x5C8961
			je		DoNormal
			cmp		eax, 0x5C8A01
			je		DoNormal
			// Avoid running event from SetOpenState script func call
			cmp		eax, 0x5CED9B
			je		DoNormal
			cmp		eax, 0x5CEDCB
			je		DoNormal

			mov		edx, dword ptr [ebp + activator]
			call	HandleEvent
			cmp		eax, 2
			je		EarlyExitButRetnTrue
			test	eax, eax
			jnz		DoNormal
			// else, return false
			XOR		al, al
			jmp		EarlyEndAddr

		EarlyExitButRetnTrue:
			mov		al, 1
			jmp		EarlyEndAddr

		DoNormal:
			jmp		NormalRetnAddr
		}
	}

	void WriteHook()
	{
		// OnPreActivate hook used to be at 0x573347, but was moved higher to allow preventing NVSE's OnActivate from running.
		// NVSE OnActivate hook = 0x57318E
		WriteRelJump(0x573184, (UInt32)Hook);
	}
}

namespace OnPreScriptedActivate
{
	constexpr char eventName[] = "ShowOff:OnPreScriptedActivate";

	bool __fastcall HandleEvent(TESObjectREFR* activated, TESObjectREFR* actionRef, UInt32 runOnActivateBlock)
	{
		auto constexpr resultCallback = [](NVSEArrayVarInterface::Element& result, void* shouldActivateAddr) -> bool
			{
				if (UInt32& shouldActivate = *static_cast<UInt32*>(shouldActivateAddr))
					if (result.IsValid())
						shouldActivate = result.Bool();
				return true;
			};
		UInt32 shouldActivate = true;

		g_eventInterface->DispatchEventAlt(eventName, resultCallback, &shouldActivate,
			activated, actionRef, runOnActivateBlock, &shouldActivate);

		return shouldActivate != 0;
	}

	CallDetour g_detourActivateFunc;
	bool __fastcall MaybePreventScriptedActivation_Activate(TESObjectREFR* actionRef, void* edx)
	{
		auto isDisabled = ThisStdCall<bool>(g_detourActivateFunc.GetOverwrittenAddr(), actionRef);
		if (isDisabled)
			return true;

		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
		auto* toActivate = *reinterpret_cast<TESObjectREFR**>(ebp + 0x10);
		auto runOnActivateBlock = *reinterpret_cast<UInt32*>(ebp - 0x8);
		return !HandleEvent(toActivate, actionRef, runOnActivateBlock);
	}

	void WriteDelayedHooks()
	{
		// replaces TESForm::IsDisabled call in Activate_Execute
		g_detourActivateFunc.WriteDetourCall(0x5B5AD0, (UInt32)MaybePreventScriptedActivation_Activate);
	}
}

static const UInt32 g_inventoryMenuSelectionAddr = 0x11D9EA8;

// Will only run for the player, since I'm too lazy to make actors not get stuck in a trying-to-activate loop.
namespace PreActivateInventoryItem
{
	constexpr char eventName[] = "ShowOff:OnPreActivateInventoryItem";

	// Will also run on special activation, with an arg to indicate if it is.
	constexpr char eventNameAlt[] = "ShowOff:OnPreActivateInventoryItemAlt";

	bool CanUseItemRef(TESObjectREFR* invRef, bool isHotkeyUse)
	{
		if (!invRef || !invRef->baseForm)
			return false;

		auto constexpr resultCallback = [](NVSEArrayVarInterface::Element& result, void* shouldActivateAddr) -> bool
		{
			if (UInt32& shouldActivate = *static_cast<UInt32*>(shouldActivateAddr))
			{
				if (result.IsValid())
					shouldActivate = result.Bool();
			}
			return true;
		};
		UInt32 shouldActivate = true;

		UInt32 selectedHotkey = 0;
		if (isHotkeyUse)
		{
			// This trick works because HookHandleHotkeyEquipOrUnEquip was a jump, not a call.
			auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
			auto const hotkeyData = *reinterpret_cast<HotKeyWheel**>(ebp - 0x1C);
			selectedHotkey = hotkeyData->selectedHotkey + 1;
		}

		g_eventInterface->DispatchEventAlt(eventName, resultCallback, &shouldActivate, 
			g_thePlayer, invRef->baseForm, invRef, &shouldActivate, selectedHotkey);

		// Since we're inside this function, we can assume no special activation is going on.
		UInt32 isSpecialActivation = false;
		g_eventInterface->DispatchEventAlt(eventNameAlt, resultCallback, &shouldActivate,
			g_thePlayer, invRef->baseForm, invRef, &shouldActivate, selectedHotkey, isSpecialActivation);

		if (g_ShowFuncDebug)
			Console_Print("CanActivateItemHook: CanActivate: %i, Item: [%08X], %s, type: %u", 
				shouldActivate, invRef->baseForm->refID, invRef->baseForm->GetName(), invRef->baseForm->typeID);

		return shouldActivate;
	}

	bool __fastcall CanUseItemEntry(ContChangesEntry* itemEntry, void* edx, bool isHotkeyUse)
	{
		if (!itemEntry)
			return false;
		auto* invRef = CreateRefForStack(g_thePlayer, itemEntry);
		return CanUseItemRef(invRef, isHotkeyUse);
	}

	__HOOK HookHandleOnClickEquipOrUnEquip()
	{
		static const UInt32 endHandleClick = 0x780B8E,
			HandleEquipOrUnEquip = 0x780D60;

		_asm
		{
			//ecx = InventoryMenu
			mov		ecx, g_inventoryMenuSelectionAddr
			mov		ecx, dword ptr ds : [ecx]
			push	0
			call	CanUseItemEntry // register stomping should be fine.
			test	al, al
			jz		PreventActivation

			mov		ecx, [ebp - 0x64]
			call	HandleEquipOrUnEquip
			ret

			PreventActivation :
			add		esp, 4	// remove return addr that was pushed.
			jmp		endHandleClick
		}
	}

	__HOOK HookOnClickAmmo()
	{
		_asm
		{
			//ecx = g_inventoryMenuSelection
			mov		eax, [ecx + 8]
			test	eax, eax
			jz		Done

			push	0
			call	CanUseItemEntry // register stomping should be fine.
			test	al, al
			jz		PreventActivation

			mov		ecx, g_inventoryMenuSelectionAddr
			mov		ecx, dword ptr ds : [ecx]
			mov		eax, [ecx + 8]
			ret

			PreventActivation :
			mov		eax, 0
			Done :
			ret
		}
	}

	__HOOK HookHandleHotkeyEquipOrUnEquip()
	{
		static const UInt32 normalRetnAdrr = 0x701FB3,
			endFunctionAddr = 0x702130;
		_asm
		{
			//ecx = ContChangesEntry
			push	1
			call	CanUseItemEntry //register stomping should be fine
			test	al, al
			jz		PreventActivation

			mov		ecx, [ebp - 0xC]  //in case ecx got stomped, restore it
			mov		eax, [ecx + 8]
			jmp		normalRetnAdrr

			PreventActivation :
			jmp		endFunctionAddr
		}
	}

	namespace HandleSpecialActivation
	{
		// Should only be called if EquipItem, Tweak's bQuickUse, or another special item activation method is used.
		bool CanSpecialUseItemRef(TESObjectREFR* invRef)
		{
			if (!invRef || !invRef->baseForm)
				return false;

			auto constexpr resultCallback = [](NVSEArrayVarInterface::Element& result, void* shouldActivateAddr) -> bool
			{
				if (UInt32& shouldActivate = *static_cast<UInt32*>(shouldActivateAddr))
				{
					if (result.IsValid())
						shouldActivate = result.Bool();
				}
				return true;
			};
			UInt32 shouldActivate = true;

			// Will always be 0, since this is a special activation.
			UInt32 selectedHotkey = 0;

			// Dispatch alt event w/ isSpecialActivation = true
			// (Don't dispatch regular event, to maintain backwards compatibility)
			UInt32 isSpecialActivation = true;
			g_eventInterface->DispatchEventAlt(eventNameAlt, resultCallback, &shouldActivate,
				g_thePlayer, invRef->baseForm, invRef, &shouldActivate, selectedHotkey, isSpecialActivation);

			if (g_ShowFuncDebug)
				Console_Print("Can(Special)UseItemRef hook: CanActivate: %i, Item: [%08X], %s, type: %u",
					shouldActivate, invRef->baseForm->refID, invRef->baseForm->GetName(), invRef->baseForm->typeID);

			return shouldActivate;
		}

		namespace UnequipItem
		{
			bool __fastcall CanUnequipItemWithExtraData(Actor* this_ECX, TESForm* itemForm_EDX, 
				int count, ExtraDataList* xData)
			{
				// We only care about preventing activation for the player
				if (!itemForm_EDX || this_ECX != g_thePlayer)
					return true;
				auto* invRef = InventoryRefCreateEntry(this_ECX, itemForm_EDX, count, xData);
				return CanSpecialUseItemRef(invRef);
			}

			__HOOK Hook()
			{
				static const UInt32 normalReturnAddr = 0x88C79A,
					endFunctionAddr = 0x88C829;
				_asm
				{
					// Copy regular code we just overwrote
					mov		[ebp - 0x4], ecx

					// Avoid running our checks twice by checking if the func we're hooking was called by a func we already hooked.
					mov		eax, [ebp + 0x4] // get retn addr
					cmp		eax, 0x780F58 // in InventoryMenu::HandleEquipOrUnEquip
					je		DoRegularCode
					cmp		eax, 0x7020C4 // in HotKeyWheel::HandleEquipUnequip
					je		DoRegularCode

					// Check if we should prevent unequipping
					mov		edx, [ebp + 0x8] // item
					mov		eax, [ebp + 0x10] // xData
					push	eax
					mov		eax, [ebp + 0xC] // count
					push	eax
					call	CanUnequipItemWithExtraData
					// (stack is cleaned up by callee)
					test	al, al
					jz		PreventActivation

				DoRegularCode :
					mov     eax, [ebp - 0x4]
					jmp		normalReturnAddr

				PreventActivation :
					jmp		endFunctionAddr
				}
			}
		}

		namespace EquipItem
		{
			bool __fastcall HandleHook(Actor* actor, TESForm* itemForm)
			{
				// Avoid running our checks twice by checking if this func was called by a func we already hooked.
				auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
				auto returnAddr = *reinterpret_cast<UInt32*>(ebp + 0x4);
				if (returnAddr == 0x780F31	/* in InventoryMenu::HandleEquipOrUnEquip */
					|| returnAddr == 0x702084 /* in HotKeyWheel::HandleEquipUnequip */)
				{
					return true;
				}

				switch (itemForm->typeID)
				{
				case kFormType_TESObjectARMO:
				case kFormType_TESObjectWEAP:
				case kFormType_TESAmmo:
				case kFormType_BGSProjectile:
				case kFormType_TESObjectBOOK:
				case kFormType_IngredientItem:
				case kFormType_AlchemyItem:
				{

					// We only care about preventing activation for the player
					if (actor != g_thePlayer)
						return true;

					auto* xData = *reinterpret_cast<ExtraDataList**>(ebp + 0x10);
					auto count = *reinterpret_cast<int*>(ebp + 0xC);

					auto* invRef = InventoryRefCreateEntry(actor, itemForm, count, xData);
					return CanSpecialUseItemRef(invRef);
				}
				default:
					return true; // in case someone wants to hook this func later to support more formtypes.
				}
			}

			__HOOK Hook()
			{
				static const UInt32 NormalReturnAddr = 0x88C659,
					EndFunctionAddr = 0x88C753;
				_asm
				{
					// do regular code
					sub     esp, 8
					mov		[ebp - 4], ecx

					// our code
					mov		edx, [ebp + 0x8] // item
					call	HandleHook
					test	al, al
					jz		PreventEquip
					// else, do regular code
					jmp		NormalReturnAddr

				PreventEquip :
					jmp		EndFunctionAddr
				}
			}

			void WriteDelayedHooks()
			{
				// Avoids previous hook conflict with Tweaks, which WriteRelJumps at 0x88C659
				WriteRelJump(0x88C653, (UInt32)Hook, std::to_array<UInt8>({ 0x83, 0xEC, 0x08, 0x89, 0x4D }));
			}
		}
	}

	void WriteHooks()
	{
		// Replace "call InventoryMenu::HandleEquipOrUnEquip"
		ReplaceCall(0x7805CC, (UInt32)HookHandleOnClickEquipOrUnEquip);

		// New hook to handle special plugin/scripted inv item activation.
		WriteRelJump(0x88C794, (UInt32)HandleSpecialActivation::UnequipItem::Hook);

		// TODO: WRITE HOOK FOR changing ammo via hotkey 2
		// eh, bit too niche for the headache
		// maybe at 0x4BF88B
	}

	void WriteDelayedHooks()
	{
		// Replace "call TESForm::GetFlags(g_inventoryMenuSelection)"
		// Delayed to (hopefully) crush NVAC conflict
		WriteRelCall(0x780648, (UInt32)HookOnClickAmmo);

		// Replace "call TESForm::GetFlags(entry)"
		// Delayed to (hopefully) crush NVAC conflict
		WriteRelJump(0x701FAE, (UInt32)HookHandleHotkeyEquipOrUnEquip);

		// New hook to handle special plugin/scripted inv item activation.
		HandleSpecialActivation::EquipItem::WriteDelayedHooks();
	}
}

namespace PreDropInventoryItem
{
	constexpr char eventName[] = "ShowOff:OnPreDropInventoryItem";

	bool HandleEvent()
	{
		auto constexpr resultCallback = [](NVSEArrayVarInterface::Element& result, void* shouldDropAddr) -> bool
		{
			if (UInt32& shouldDrop = *static_cast<UInt32*>(shouldDropAddr))
				if (result.IsValid())
					shouldDrop = result.Bool();
			return true;
		};
		UInt32 shouldDrop = true;

		auto const itemEntry = *reinterpret_cast<ContChangesEntry**>(g_inventoryMenuSelectionAddr);
		auto const itemForm = itemEntry->type;
		auto* invRef = CreateRefForStack(g_thePlayer, itemEntry);

		g_eventInterface->DispatchEventAlt(eventName, resultCallback, &shouldDrop,
			g_thePlayer, itemForm, invRef, &shouldDrop);

		return shouldDrop;
	}

	__declspec(naked) void Hook()
	{
		static UInt32 const IsEnoughRoomNearPlayerToDropItem = 0x9614B0,
			NotEnoughRoomAddr = 0x780AD2, CancelledByEventAddr = 0x780B8E,
			DropItemAddr = 0x780AFC;
		_asm
		{
			call	IsEnoughRoomNearPlayerToDropItem
			movzx	ecx, al
			test	ecx, ecx
			jz		NotEnoughRoom

			call	HandleEvent
			movzx	ecx, al
			test	ecx, ecx
			jz		CancelledByEvent

			jmp		DropItemAddr

			NotEnoughRoom :
			jmp		NotEnoughRoomAddr

			CancelledByEvent :  // skip showing the "not enough room" UI message.
			jmp		CancelledByEventAddr
		}
	}

	void WriteHook()
	{
		WriteRelJump(0x780AC6, (UInt32)Hook);
	}
}

namespace OnQuestAdded
{
	constexpr char eventName[] = "ShowOff:OnQuestAdded";

	void __cdecl handleQuestUpdateMessage(TESQuest* quest)
	{
		g_eventInterface->DispatchEvent(eventName, nullptr, quest);
		CdeclCall(0x77A480, quest);
	}

	void WriteHook()
	{
		ReplaceCall(0x5EC66E, (UInt32)handleQuestUpdateMessage);
	}
}

namespace OnCalculateSellPrice
{
	constexpr char eventNameAdd[] = "ShowOff:OnCalculateSellPrice:+";
	constexpr char eventNameSub[] = "ShowOff:OnCalculateSellPrice:-";
	constexpr char eventNameMult[] = "ShowOff:OnCalculateSellPrice:*";

	void HandleEvent(float& newPrice, ContChangesEntry* itemEntry)
	{
		auto* baseItem = itemEntry->type;
		auto* invRef = CreateRefForStack(g_thePlayer, itemEntry);

		auto constexpr multCallback = [](NVSEArrayVarInterface::Element& result, void* newPriceAddr) -> bool
		{
			if (result.type != NVSEArrayVarInterface::kType_Numeric) [[unlikely]]
				return true;
			float& newPrice = *static_cast<float*>(newPriceAddr);
			newPrice *= result.Number();
			return true;
		};
		g_eventInterface->DispatchEventAlt(eventNameMult, multCallback, &newPrice, g_thePlayer, baseItem, invRef);

		auto constexpr addCallback = [](NVSEArrayVarInterface::Element& result, void* newPriceAddr) -> bool
		{
			if (result.type != NVSEArrayVarInterface::kType_Numeric) [[unlikely]]
				return true;
			float& newPrice = *static_cast<float*>(newPriceAddr);
			newPrice += result.Number();
			return true;
		};
		g_eventInterface->DispatchEventAlt(eventNameAdd, addCallback, &newPrice, g_thePlayer, baseItem, invRef);

		auto constexpr subCallback = [](NVSEArrayVarInterface::Element& result, void* newPriceAddr) -> bool
		{
			if (result.type != NVSEArrayVarInterface::kType_Numeric) [[unlikely]]
				return true;
			float& newPrice = *static_cast<float*>(newPriceAddr);
			newPrice -= result.Number();
			return true;
		};
		g_eventInterface->DispatchEventAlt(eventNameSub, subCallback, &newPrice, g_thePlayer, baseItem, invRef);

#if _DEBUG && 0
		Console_Print("== ShowOff:OnCalculateSellPrice - newPrice: %f, itemEditorID: %s ==", 
			newPrice, itemEntry->type->GetName());
#endif
	}

	CallDetour g_override;

	// Recalculate sell price of an item.
	static double __cdecl HookFAbs(float price)
	{
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());  //credits to Kormakur for this trick.

		auto& newPrice = *reinterpret_cast<float*>(ebp - 0xC);
		auto* itemEntry = *reinterpret_cast<ContChangesEntry**>(ebp + 0xC);
		HandleEvent(newPrice, itemEntry);

		return CdeclCall<double>(g_override.GetOverwrittenAddr(), newPrice);
	}

	void WriteDelayedHook() //avoid conflict with NVAC hook
	{
		g_override.WriteDetourCall(0x72EFFE, (UInt32)HookFAbs);
	}
}

namespace OnProjectileStuff
{
	// Dark magic stolen from JIP's ProjectileImpactHook.
	// Without it, projectile is reported as being in the wrong location (in script functions, at the time of the event).
	void UpdateProjectilePosition(Projectile* proj)
	{
		auto* baseProj = static_cast<BGSProjectile*>(proj->baseForm);
		if (baseProj->type != 2)
		{
			if (auto* data = proj->impactDataList.Head()->Data();
				data != nullptr)
			{
				*proj->PosVector() = data->pos;
			}
		}
	}
}

namespace OnPreProjectileExplode
{
	extern std::unordered_set<Projectile*> g_forceSpawnCollisionEffects;
}

namespace OnProjectileDestroy
{
	using namespace OnProjectileStuff;
	constexpr char eventName[] = "ShowOff:OnProjectileDestroy";


	void __fastcall HandleEvent(Projectile* proj)
	{
		UpdateProjectilePosition(proj);
		g_eventInterface->DispatchEvent(eventName, proj, proj->sourceRef, proj->sourceWeap);
		OnPreProjectileExplode::g_forceSpawnCollisionEffects.erase(proj);
	}

	void __declspec(naked) Projectile_Free_Hook()
	{
		static UInt32 const retnAddr = 0x9BC489;
		_asm
		{
			push	ecx
			call	HandleEvent  // ecx should still be the Projectile (this)
			pop		ecx
			mov		eax, dword ptr ds : [0x11C16BC]
			jmp		retnAddr
		}
	}

	void WriteHook()
	{
		WriteRelJump(0x9BC484, (UInt32)Projectile_Free_Hook);
	}
}
namespace OnProjectileCreate
{
	using namespace OnProjectileStuff;
	constexpr char eventName[] = "ShowOff:OnProjectileCreate";

	void __fastcall HandleEvent(Projectile* proj, Actor* projOwner, TESObjectWEAP* weapon)
	{
		g_eventInterface->DispatchEvent(eventName, proj, projOwner, weapon);
	}

	void __declspec(naked) CreateProjectile_Hook()
	{
		static UInt32 const retnAddr = 0x9BD523;
		_asm
		{
			mov		ecx, [ebp - 0x14] // liveProjectile (arg1)
			mov		edx, [ebp + 0xC] // actor (arg2)
			mov		esi, [ebp + 0x14] // weapon (arg3)
			push	esi
			call	HandleEvent

			// do the regular code
			mov		eax, [ebp - 0x14]
			mov		ecx, [ebp - 0xC]
			jmp		retnAddr
		}
	}

	void WriteHook()
	{
		WriteRelJump(0x9BD51D, (UInt32)CreateProjectile_Hook);
	}
}
namespace OnProjectileImpact
{
	using namespace OnProjectileStuff;
	constexpr char eventName[] = "ShowOff:OnProjectileImpact";

	void __fastcall HandleEvent(Projectile* proj, void* edx)
	{
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());

		// Only run if a new impact has been detected.
		if (bool const detectedNewImpact = *reinterpret_cast<char*>(ebp - 0xD))
		{
			UpdateProjectilePosition(proj);
			g_eventInterface->DispatchEvent(eventName, proj, proj->sourceRef, proj->sourceWeap, proj->GetImpactRef());
		}
	}

	void __declspec(naked) EndFunctionHook()
	{
		UInt32 static const ContinueEndAddr = 0x9C20CF;
		__asm
		{
			mov		ecx, [ebp - 0x58] // proj
			call	HandleEvent

			mov		al, [ebp - 0xD]
			mov     ecx, [ebp - 0xC]
			jmp		ContinueEndAddr
		}
	}

	void WriteHooks()
	{
		//Don't hook 0x9C20BF, because JIP does so already (ProjectileImpactHook).
		WriteRelJump(0x9C20C9, (UInt32)EndFunctionHook);
	}
}

namespace OnLockpickMenuClose
{
	constexpr char eventName[] = "ShowOff:OnLockpickMenuClose";

	enum class CloseReason : UInt32
	{
		kLockOpened = 0,
		kLockForceBroken,
		kStewieNoMoreLockpicksAfterFailedForceAttempt,
		kManualExit
	};

	void HandleEvent(const LockPickMenu* menu, CloseReason reason)
	{
		g_eventInterface->DispatchEvent(eventName, menu->targetRef, reason);
	}

	inline CallDetour g_ForceOpenAttemptOverride;
	int __cdecl ForceOpenAttemptHook(int playerLockpickSkill, int lockLevel)
	{
		auto const chanceResult = CdeclCall<int>(g_ForceOpenAttemptOverride.GetOverwrittenAddr(), playerLockpickSkill, lockLevel);

		auto const menu = LockPickMenu::GetSingleton();
		// do nothing if the lock is already successfully forced and rotating
		if (menu->isForceRotate)
			return chanceResult;

		int randomNum;
		_asm {
			mov randomNum, esi
		}

		if (randomNum >= chanceResult) // if player failed to force open the lock
		{
			// Compatibility with Tweak's ForceLockUsesBobbyPins
			if (*reinterpret_cast<UInt8*>(0x79040D) == 0xE8)
			{
				// Assume Tweak's hook was installed.
				// While this is still a failed attempt, it is no longer guaranteed to close the menu,
				// ..since it now only closes when player runs out of lockpicks.
				if (float numBobbyPins = menu->tile30->GetValueFloat(*(UInt32*)0x11DA1F8) - 1.0F;
					numBobbyPins <= 0)
				{
					HandleEvent(menu, CloseReason::kStewieNoMoreLockpicksAfterFailedForceAttempt);
				}
				return chanceResult;
			} //else, Tweaks hook is not installed.
			HandleEvent(menu, CloseReason::kLockForceBroken);
		}
		return chanceResult;
	}

	void __cdecl ManualExitHook()
	{
		auto const menu = LockPickMenu::GetSingleton();
		menu->stage = 7;
		HandleEvent(menu, CloseReason::kManualExit);
	}

	void __cdecl OpenLockHook()
	{
		auto const menu = LockPickMenu::GetSingleton();
		menu->stage = 7;
		HandleEvent(menu, CloseReason::kLockOpened);
	}

	void WriteHooks()
	{
		ReplaceCall(0x790383, (UInt32)ManualExitHook);
		ReplaceCall(0x78F97B, (UInt32)OpenLockHook);
		// replace CalculateForceLockChance(..)
		g_ForceOpenAttemptOverride.WriteDetourCall(0x7903D9, (UInt32)ForceOpenAttemptHook);
	}
}

namespace OnShowCornerMessage
{
	constexpr char eventName[] = "ShowOff:OnShowCornerMessage";

	static std::string g_msgText, g_iconPath, g_soundPath;

	// Warning: msgText is not fully formatted, i.e. can have "&sUActnVats" and other formatting tricks.
	void DispatchEvent(HUDMainMenu::QueuedMessage* msg)
	{
		auto constexpr postCallback = [](void* nothing, NVSEEventManagerInterface::DispatchReturn retn)
		{
			g_msgText.clear();
			g_iconPath.clear();
			g_soundPath.clear();
		};

		// Keep strings alive in case of dispatch deferral.
		g_msgText = msg->msgText;
		g_iconPath = msg->iconPath;
		g_soundPath = msg->soundPath;

		void* displayTime = *(void**)&msg->displayTime;
		// Event can run outside main thread; defer dispatch in that case.
		g_eventInterface->DispatchEventThreadSafe(eventName, postCallback, nullptr, g_msgText.c_str(),
			g_iconPath.c_str(), g_soundPath.c_str(), displayTime);
	}

	bool __fastcall tListIsEmptyHook(tList<HUDMainMenu::QueuedMessage>* msgList)
	{
		const bool isEmpty = msgList->Empty();
		if (!isEmpty)
			DispatchEvent(msgList->Head()->data);

		return isEmpty;
	}

	void WriteHooks()
	{
		// For delayed (queued) messages.
		ReplaceCall(0x77578D, (UInt32)tListIsEmptyHook);
	}
}

namespace OnQueueCornerMessage
{
	constexpr char eventName[] = "ShowOff:OnQueueCornerMessage";

	// Warning: msgText is not fully formatted, i.e. can have "&sUActnVats" and other formatting tricks.
	void DispatchEvent(HUDMainMenu::QueuedMessage* msg)
	{
		void* displayTime = *(void**)&msg->displayTime;
		g_eventInterface->DispatchEvent(eventName, nullptr, msg->msgText, 
			msg->iconPath, msg->soundPath, displayTime);
	}

	template <bool ShowMsgNow>
	void __fastcall tListAppendHook(tList<HUDMainMenu::QueuedMessage>* msgList, void* edx, 
		HUDMainMenu::QueuedMessage** msg)
	{
		// Run our code
		DispatchEvent(*msg);
		if constexpr (ShowMsgNow)
			OnShowCornerMessage::DispatchEvent(*msg);

		// Regular code
		ThisStdCall(0x905820, msgList, msg);
	}

	void __fastcall tListInsertHook(tList<HUDMainMenu::QueuedMessage>* msgList, void* edx,
		HUDMainMenu::QueuedMessage** msg)
	{
		// Run our code
		DispatchEvent(*msg);

		// Regular code
		ThisStdCall(0x5AE3D0, msgList, msg);
	}

	void WriteHooks()
	{
		ReplaceCall(0x7754FA, (UInt32)tListAppendHook<true>);
		ReplaceCall(0x775624, (UInt32)tListAppendHook<false>);
		ReplaceCall(0x775610, (UInt32)tListInsertHook);
	}
}

namespace OnFireWeapon
{
	constexpr char eventName[] = "ShowOff:OnFireWeapon";

	// Runs before ModifyChanceForAmmoItem perk entry point can add items.
	// Runs before the weapon is damaged from firing.
	// Runs before JIP's SetOnFireWeaponEventHandler
	// Unlike the above, works in Godmode.
	void __fastcall DispatchEvent(Actor* actor, TESObjectWEAP* weap)
	{
		g_eventInterface->DispatchEvent(eventName, actor, weap);
	}

	void __declspec(naked) Hook()
	{
		UInt32 static const retnAddr = 0x523199;
		__asm
		{
			// regular code
			mov		eax, [edx + 0x100]
			call	eax
			push	eax  // store result

			// Our code
			mov		ecx, [ebp + 0x8] //actor
			mov		edx, [ebp - 0x238] //weapon
			call	DispatchEvent

			// back to regular
			pop		eax  // restore result
			jmp		retnAddr
		}
	}

	void WriteHook()
	{
		WriteRelJump(0x523191, (UInt32)Hook);
	}
}

namespace OnCalculateEffectEntryMagnitude
{
	constexpr char eventName[] = "ShowOff:OnCalculateEffectMagnitude";

	inline NumberModifications<float> g_MagnitudeModModifiers;
	inline EffectItem* g_liveEffectItem = nullptr;

	double HandleEvent(float modifier, UInt32 isHostile, UInt8* ebp)
	{
		auto* target = *reinterpret_cast<MagicTarget**>(ebp - 0x28);
		auto* caster = *reinterpret_cast<MagicCaster**>(ebp + 8);
		auto* magicItem = *reinterpret_cast<MagicItem**>(ebp + 0xC);
		auto* itemForm = DYNAMIC_CAST(magicItem, MagicItem, TESForm);

		const auto* activeEff = *reinterpret_cast<ActiveEffect**>(ebp + 0x10);
		auto* baseEffect = activeEff->effectItem->setting;

		void* multArg = *(void**)&modifier;
		void* magnitude = *(void**)&activeEff->magnitude;
		void* duration = *(void**)&activeEff->duration; // not modified by perk effects, like Modify Positive Chem Duration

		// Store effect Item for one frame, so it can be queried in a function inside the event.
		g_liveEffectItem = activeEff->effectItem;

		// WARNING: can run outside main thread (i.e when loading).
		// Thus, no calls to Console_Print should occur during then!
		// Can't delay this dispatch, otherwise effects wouldn't apply in time.
		g_eventInterface->DispatchEvent(eventName, target->GetActor(), caster->GetActor(), 
			itemForm, baseEffect, activeEff->spellType, activeEff->enchantObject, isHostile, multArg,
			magnitude, duration);

		g_liveEffectItem = nullptr;
			
		g_MagnitudeModModifiers.ModValue(modifier);
		g_MagnitudeModModifiers.Clear();

		return modifier;
	}

	double __cdecl HandleHostileEffect(float normalModifier)
	{
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
		auto magnitudeMult = HandleEvent(normalModifier, true, ebp);

		// TODO: Cap magnitudeMult by max player resistance (if target is the player).

		return magnitudeMult;
	}
	double __cdecl HandleNormalEffect(float normalModifier)
	{
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());

		// No cap needs to be applied, since the player isn't resisting anything.
		return HandleEvent(normalModifier, false, ebp);
	}

	void __declspec(naked) CalcRegularEffectHook()
	{
		__asm
		{
			//result in st(0)
			push	ecx
			fstp	[esp]
			call	HandleNormalEffect  //result in st(0)
			//add	esp, 4

			// do regular code
			mov     esp, ebp
			pop     ebp
			retn    0xC
		}
	}

	void __declspec(naked) CalcHostileEffectResistHook()
	{
		__asm
		{
			// just called CalculateSpellMagnitudeMultFromResistance, result in st(0)
			push	ecx
			fstp	[esp]
			call	HandleHostileEffect  //result in st(0)
			//add	esp, 4  //not needed

			// do regular code
			mov     esp, ebp
			pop     ebp
			retn    0xC
		}
	}

	//TODO: hook near 0x7E0C1F (watch for Tweaks)
	void WriteHooks()
	{
		// Overwrite function epilogue
		WriteRelJump(0x8C4322, (UInt32)CalcRegularEffectHook);

		// Overwrite code following call to CalculateSpellMagnitudeMultFromResistance
		WriteRelJump(0x8C431C, (UInt32)CalcHostileEffectResistHook);
	}
}

DEFINE_COMMAND_PLUGIN(SetLiveEffectMagnitudeModifier, "", false, kParams_OneFloat_OneString);
bool Cmd_SetLiveEffectMagnitudeModifier_Execute(COMMAND_ARGS)
{
	*result = false;
	float fMod;
	char modType[2];
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &fMod, &modType) || !modType[0])
		return true;

	ScopedLock lock(g_Lock);
	*result = OnCalculateEffectEntryMagnitude::g_MagnitudeModModifiers.TryAddMod(fMod, modType[0]);

	return true;
}

DEFINE_COMMAND_PLUGIN(GetLiveEffectBaseTrait, "", false, kParams_OneInt);
// Copied code from JIP's GetNthEffectTraitNumeric
bool Cmd_GetLiveEffectBaseTrait_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 traitID;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &traitID))
		return true;
	EffectItem* effItem = OnCalculateEffectEntryMagnitude::g_liveEffectItem;
	if (!effItem) return true;
	switch (traitID)
	{
	case 0:
		*result = effItem->magnitude;
		break;
	case 1:
		*result = effItem->area;
		break;
	case 2:
		*result = effItem->duration;
		break;
	case 3:
		*result = effItem->range;
		break;
	case 4:
		*result = effItem->actorValueOrOther;
		break;
	case 5:
		*result = effItem->cost;  // unused in NV?
		break;
	}
	return true;
}

namespace OnPCMiscStatChange
{
	constexpr char eventName[] = "ShowOff:OnPCMiscStatChange";

	int __cdecl HookGetMenuID()
	{
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
		auto const statCode = *reinterpret_cast<MiscStatCode*>(ebp + 0x8);
		auto const modVal = *reinterpret_cast<int*>(ebp + 0xC);
		auto const newVal = CdeclCall<int>(0x5A3370, statCode); // GetPCMiscStat
		g_eventInterface->DispatchEventThreadSafe(eventName, nullptr, g_thePlayer, statCode, modVal, newVal);

		// Do regular code
		return 1003; // StatsMenu::GetMenuID
	}

	void WriteHook()
	{
		// replace "call StatsMenu::GetMenuID"
		ReplaceCall(0x4D5E6A, (UInt32)HookGetMenuID);
	}
}

namespace OnDisplayOrCompleteObjective
{
	constexpr char onDisplayName[] = "ShowOff:OnDisplayObjective";
	constexpr char onCompleteName[] = "ShowOff:OnCompleteObjective";

	static CallDetour GetQuestDetour;

	TESQuest* __fastcall HookGetQuest(BGSQuestObjective* objective)
	{
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
		const auto newStatus = *reinterpret_cast<UInt32*>(ebp + 8);

		if (newStatus == 1 && !objective->status)
		{
			g_eventInterface->DispatchEventThreadSafe(onDisplayName, nullptr, nullptr, objective->quest, objective->objectiveId);
		}
		else if (newStatus == 3 && objective->status == 1 && !objective->quest->IsCompleted())
		{
			g_eventInterface->DispatchEventThreadSafe(onCompleteName, nullptr, nullptr, objective->quest, objective->objectiveId);
		}

		return ThisStdCall<TESQuest*>(GetQuestDetour.GetOverwrittenAddr(), objective);
	}

	void WriteDelayedHook()
	{
		// Add compatibility with Tweaks by indirectly calling the function at the address.
		GetQuestDetour.WriteDetourCall(0x5EC5DC, (UInt32)HookGetQuest);
		// We could have hooked the HUDMainMenu::SetQuestUpdateText calls,
		// ..but that would cause incompatibility with Tweaks' "No Quest Messages".
	}
}

// Like NVSE OnAdd, but runs for all vanilla OnAdd instances.
// (Needs unique hooks, NVSE hooks didn't expect non-references to be passed to MergeScriptEvent).
// Relevant addresses: 0x57506B, 0x574AFA, 0x574C28, 0x574D00, 0x574F03
// Event runs slightly AFTER vanilla OnAdd.
namespace OnAddAlt
{
	constexpr char eventName[] = "ShowOff:OnAdd";

	// non-null if picking up a reference in the game world
	static TESObjectREFR* g_AddedItemRef = nullptr;

	struct InvRefCreatingInfo
	{
		void Fill(TESObjectREFR* owner, TESForm* item, ExtraDataList* xData, SInt32 count)
		{
			m_owner = owner; m_item = item; m_xData = xData; m_count = count;
		}
		TESObjectREFR* GetInvRef()
		{
			if (m_cachedInvRef)
				return m_cachedInvRef;
			if (!m_item || !m_owner)
				return nullptr;
			m_cachedInvRef = InventoryRefCreateEntry(m_owner, m_item, m_count, m_xData);
			return m_cachedInvRef;
		}
		void Reset()
		{
			m_owner = {}; m_item = {}; m_xData = {}; m_count = {}; m_cachedInvRef = {};
		}
	private:
		TESObjectREFR* m_owner{};
		TESForm* m_item{};
		ExtraDataList* m_xData{};
		SInt32 m_count{}; //xCount?

		TESObjectREFR* m_cachedInvRef{};
	};
	static InvRefCreatingInfo g_InvRefCreatingInfo;

	// using a function so creating an invRef is on-demand, instead of a constant drain.
	TESObjectREFR* GetItemRef()
	{
		if (g_AddedItemRef)
			return g_AddedItemRef;
		//else, must get invRef.
		return g_InvRefCreatingInfo.GetInvRef();
	}

	void HandleEvent(TESObjectREFR* newOwner, TESForm* item, ExtraDataList* xData, SInt32 count)
	{
		g_InvRefCreatingInfo.Fill(newOwner, item, xData, count);
		g_eventInterface->DispatchEvent(eventName, nullptr, item, newOwner);
		g_InvRefCreatingInfo.Reset();
	}

	void HandleEvent(TESObjectREFR* newOwner, TESObjectREFR* itemRef)
	{
		g_AddedItemRef = itemRef;
		g_eventInterface->DispatchEvent(eventName, itemRef, itemRef->baseForm, newOwner);
		g_AddedItemRef = nullptr;
	}

	namespace HandleAddItem
	{
		void __fastcall HookExtraContainerChanges_Unk(void* xChanges, void* edx, TESForm* item, ExtraDataList* xData, SInt32 count)
		{
			auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
			auto* newOwner = *reinterpret_cast<TESObjectREFR**>(ebp - 0xC);

			HandleEvent(newOwner, item, xData, count);

			// Call ExtraContainerChanges_4C29A0 (which we overwrote)
			ThisStdCall(0x4C29A0, xChanges, item, xData, count);
		}

		void WriteHook()
		{
			ReplaceCall(0x575091, (UInt32)HookExtraContainerChanges_Unk);
		}
	}

	namespace EquipForRef
	{
		void __fastcall HookEquipForRef(ExtraContainerChanges::Data* xChanges, void* edx, TESForm* item, SInt32 count,
			TESObjectREFR* newOwner, ExtraDataList* xData, int a6, int a7)
		{
			HandleEvent(newOwner, item, xData, count);

			// Call ExtraContainerChanges__Data__EquipForRef (which we overwrote)
			ThisStdCall(0x4BFFE0, xChanges, item, count, newOwner, xData, a6, a7);
		}

		void WriteHook()
		{
			ReplaceCall(0x574B19, (UInt32)HookEquipForRef);
		}
	}

	namespace PickUpItem
	{
		namespace InitialOnAdd
		{
			void __cdecl HookMergeScriptEvent(TESObjectREFR* newOwner, ExtraDataList* xData, int eventId)
			{
				CdeclCall(MergeScriptEventAddr, newOwner, xData, eventId);

				auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
				auto* itemRef = *reinterpret_cast<TESObjectREFR**>(ebp + 8);

				HandleEvent(newOwner, itemRef);
			}

			void WriteHook()
			{
				ReplaceCall(0x574C28, (UInt32)HookMergeScriptEvent);
			}
		}

		namespace RockItProjectile
		{
			void __cdecl HookMergeScriptEvent(TESObjectREFR* newOwner, ExtraDataList* xData, int eventId)
			{
				CdeclCall(MergeScriptEventAddr, newOwner, xData, eventId);

				auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
				const auto* rockItContChanges = *reinterpret_cast<ContChangesEntry**>(ebp - 0x60);

				//todo: is it safe to use countDelta here? Should Count extradata be checked? idk
				HandleEvent(newOwner, rockItContChanges->type, xData, rockItContChanges->countDelta);
			}

			void WriteHook()
			{
				ReplaceCall(0x574D00, (UInt32)HookMergeScriptEvent);
			}
		}
	
		namespace OtherProjectile
		{
			void __cdecl HookMergeScriptEvent(TESObjectREFR* newOwner, ExtraDataList* xData, int eventId)
			{
				CdeclCall(MergeScriptEventAddr, newOwner, xData, eventId);

				auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
				auto* form = *reinterpret_cast<TESForm**>(ebp - 0x24);

				// todo: verify if assuming count = 1 is safe!
				HandleEvent(newOwner, form, xData, 1);
			}

			void WriteHook()
			{
				ReplaceCall(0x574F03, (UInt32)HookMergeScriptEvent);
			}
		}

		void WriteHooks()
		{
			InitialOnAdd::WriteHook();
			RockItProjectile::WriteHook();
			OtherProjectile::WriteHook();
		}
	}

	void WriteHooks()
	{
		HandleAddItem::WriteHook();
		EquipForRef::WriteHook();
		PickUpItem::WriteHooks();
	}
}

DEFINE_COMMAND_ALT_PLUGIN(GetAddedItemRefShowOff, GetAddedItemRefSO, "", false, nullptr);
bool Cmd_GetAddedItemRefShowOff_Execute(COMMAND_ARGS)
{
	if (auto const itemRef = OnAddAlt::GetItemRef())
	{
		REFR_RES = itemRef->refID;
	}
	else *result = 0;
	return true;
}

namespace OnReadBook
{
	// Runs right after the book is read, before the UI message is queued.
	constexpr char eventName[] = "ShowOff:OnReadBook";

	CallDetour g_detour;

	void __cdecl Hook(MiscStatCode code)
	{
		// do regular code
		CdeclCall(g_detour.GetOverwrittenAddr(), code);

		// our code
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
		auto* book = *reinterpret_cast<TESObjectBOOK**>(ebp - 0x28);
		auto* reader = *reinterpret_cast<Actor**>(ebp + 8);

		g_eventInterface->DispatchEvent(eventName, reader, book);
	}

	void WriteDelayedHook()
	{
		// replace "call IncPCMiscStat"
		g_detour.WriteDetourCall(0x515195, (UInt32)Hook);
	}
}

namespace OnDispositionChange
{
	// Runs right before the disposition is changed.
	constexpr char eventName[] = "ShowOff:OnDispositionChange";

	CallDetour g_detour;

	int __fastcall HandleEvent(int dispMod, void* edx)
	{
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
		auto* actor = *reinterpret_cast<Actor**>(ebp - 0x20);

		if (dispMod) // if any changes are made
			g_eventInterface->DispatchEvent(eventName, actor, dispMod);

		// ensure EAX is still what it should be.
		return dispMod;
	}

	__HOOK TruncateFloat_Hook()
	{
		__asm
		{
			// call what would be TruncateFloat
			call	g_detour.overwritten_addr

			// result in EAX (int)
			mov		ecx, eax
			call	HandleEvent

			retn
		}
	}

	void WriteDelayedHooks()
	{
		// Replace TruncateFloat calls
		g_detour.WriteDetourCall(0x87FC3A, (UInt32)TruncateFloat_Hook);

		// Don't bother saving the likely identical call function addr.
		ReplaceCall(0x87FC73, (UInt32)TruncateFloat_Hook);
	}
}

#if 0
// DO NOT USE
// Runs when actors are loading in, which will lead to buggy behavior if handlers depend on this event.
namespace OnPreLifeStateChange
{
	constexpr char eventName[] = "ShowOff:OnPreLifeStateChange";

	CallDetour g_detour;

	void** __fastcall GetINIValueAddrHook(Setting* thisSetting)
	{
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
		auto* actor = *reinterpret_cast<Actor**>(ebp - 0x2C); // see 0x8A1825
		auto newLifeState = *reinterpret_cast<UInt32*>(ebp + 0x8);
		UInt32 oldLifeState = actor->lifeState;

		g_eventInterface->DispatchEvent(eventName, actor, oldLifeState, newLifeState);

#if _DEBUG
		Console_Print("OnPreLifeStateChange HOOK >> RAN. oldLifeState: %u, newLifeState: %u", oldLifeState, newLifeState);
#endif

		// do regular code
		return ThisStdCall<void**>(g_detour.GetOverwrittenAddr(), thisSetting);
	}

	void WriteDelayedHook()
	{
		g_detour.WriteDetourCall(0x8A182D, (UInt32)GetINIValueAddrHook);
	}
}
#endif

namespace OnExplosionHit
{
	constexpr char eventName[] = "ShowOff:OnExplosionHit";

	/*
	void* __fastcall HandleEvent(Explosion* explosion, void* edx, Actor* actor)
	{
		auto* hitParts = ThisStdCall<void*>(0x9B1720, explosion, actor);
		if (hitParts)
		{
			g_eventInterface->DispatchEvent(eventName, explosion, actor);
		}
		return hitParts;
	}
	*/

	float* g_explosionHitDmgPtr = nullptr;

	void __cdecl HandleEvent(void* hitData, Actor* source, Actor* target, Explosion* explosion, float fDamage)
	{
		g_explosionHitDmgPtr = &fDamage;
		g_eventInterface->DispatchEvent(eventName, explosion, target, source);

		CdeclCall(0x9B5770, hitData, source, target, explosion, fDamage);
		g_explosionHitDmgPtr = nullptr;
	}

	void WriteHooks()
	{
		//ReplaceCall(0x89BA03, (UInt32)HandleEvent);
		ReplaceCall(0x9B04EF, (UInt32)HandleEvent);
	}
}

DEFINE_COMMAND_PLUGIN(GetExplosionHitDamage, "", false, nullptr);
bool Cmd_GetExplosionHitDamage_Execute(COMMAND_ARGS)
{
	auto* dmgPtr = OnExplosionHit::g_explosionHitDmgPtr;
	*result = dmgPtr ? *dmgPtr : -1.0f;
	return true;
}
DEFINE_COMMAND_PLUGIN(SetExplosionHitDamage, "", false, kParams_OneFloat);
bool Cmd_SetExplosionHitDamage_Execute(COMMAND_ARGS)
{
	float fNewDmg;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &fNewDmg))
		return true;
	auto* dmgPtr = OnExplosionHit::g_explosionHitDmgPtr;
	if (dmgPtr)
		*dmgPtr = fNewDmg;
	return true;
}

namespace OnPreProjectileExplode
{
	constexpr char eventName[] = "ShowOff:OnPreProjectileExplode";

	std::unordered_set<Projectile*> g_forceSpawnCollisionEffects;

	bool __fastcall HandleEvent(Projectile* proj, void* edx)
	{
		auto constexpr resultCallback = [](NVSEArrayVarInterface::Element& result, void* shouldExplodeAddr) -> bool
			{
				if (UInt32& shouldExplode = *static_cast<UInt32*>(shouldExplodeAddr))
					if (result.IsValid())
						shouldExplode = result.Bool();
				return true;
			};
		UInt32 shouldExplode = true;

		g_eventInterface->DispatchEventAlt(eventName, resultCallback, &shouldExplode,
			proj, proj->sourceRef, &shouldExplode);

		if (!shouldExplode)
		{
			auto* baseProj = static_cast<BGSProjectile*>(proj->baseForm);
			if (!(baseProj->projFlags & BGSProjectile::kFlags_AltTrigger) &&
				!(baseProj->projFlags & BGSProjectile::kFlags_Detonates))
			{
				g_forceSpawnCollisionEffects.insert(proj);
			}
		}

		return shouldExplode;
	}

	__HOOK MaybePreventExplosionHook()
	{
		static UInt32 const NormalReturnAddr = 0x9C35FC,
			EarlyEndAddr = 0x9C391A;
		enum Offsets
		{
			Proj = -0xA0
		};
		_asm
		{
			mov		ecx, [ebp + Proj]
			call	HandleEvent
			test	al, al
			jz		PreventExplosion
			// else, do normal code	
			mov		ecx, [ebp + Proj]
			mov		eax, [ecx + 0xFC] // projectile->sourceRef
			jmp		NormalReturnAddr
		PreventExplosion:
			jmp		EarlyEndAddr
		}
	}

	bool __fastcall CheckShouldForceSpawnCollisionEffects(Projectile* proj, void* edx)
	{
		return g_forceSpawnCollisionEffects.contains(proj);
	}

	__HOOK MaybeForceSpawnCollisionEffectsHook()
	{
		static UInt32 const NormalReturnAddr = 0x9C1F31,
			ForceSpawnCollisionAddr = 0x9C1F6F;
		_asm
		{
			call	CheckShouldForceSpawnCollisionEffects
			test	al, al
			jnz		ForceSpawn
			// else, do normal code	
			mov		ecx, [ebp - 0x58] // projectile
			mov		eax, [ecx + 0x20] // projectile->baseForm
			jmp		NormalReturnAddr
		ForceSpawn:
			jmp		ForceSpawnCollisionAddr
		}
	}

	void WriteHook() 
	{
		WriteRelJump(0x9C35F1, (UInt32)MaybePreventExplosionHook);
		WriteRelJump(0x9C1F2C, (UInt32)MaybeForceSpawnCollisionEffectsHook);
	}
}

namespace OnPreRemoveItemFromMenu
{
	constexpr char eventName[] = "ShowOff:OnPreRemoveItemFromMenu";

	enum RemovalContext : UInt32
	{
		kContext_BarterMenu = 0,
		kContext_NormalContainerMenu,
		kContext_Pickpocket,
		kContext_Teammate,
		kContext_RockItLauncher,
		kContext_InventoryMenu,
		kContext_RepairMenu
	};

	bool __fastcall HandleEvent(ContChangesEntry* toRemove, TESObjectREFR* oldContainer, TESObjectREFR* newContainer, 
		RemovalContext ctx)
	{
		auto constexpr resultCallback = [](NVSEArrayVarInterface::Element& result, void* shouldRemoveAddr) -> bool
			{
				if (UInt32& shouldRemove = *static_cast<UInt32*>(shouldRemoveAddr))
					if (result.IsValid())
						shouldRemove = result.Bool();
				return true;
			};
		UInt32 shouldRemove = true;

		g_eventInterface->DispatchEventAlt(eventName, resultCallback, &shouldRemove,
			oldContainer, toRemove->type, newContainer, ctx, &shouldRemove);

		return shouldRemove;
	}

	namespace HandleForBarterMenu
	{
		bool __fastcall HandleBarterMenuEvent(ContChangesEntry* toRemove, void* edx)
		{
			auto* menu = BarterMenu::Get();
			bool isSelling = menu->currentItems == &menu->leftItems;
			if (isSelling)
				return HandleEvent(toRemove, g_thePlayer, menu->merchantRef, kContext_BarterMenu);
			else
				return HandleEvent(toRemove, menu->merchantRef, g_thePlayer, kContext_BarterMenu);
		}

		__HOOK MaybePreventBarterMenuTransfer()
		{
			static UInt32 const NormalReturnAddr = 0x72D7EC,
				EarlyEndAddr = 0x72DAFF;
			_asm
			{
				// ecx is currently g_barterMenuSelection
				push	ecx  // storing for later
				call	HandleBarterMenuEvent
				test	al, al
				jz		PreventRemoval
				// else, do normal code
				pop		ecx
				mov		eax, dword ptr[ecx + 4]
				jmp		NormalReturnAddr
			PreventRemoval :
				pop		ecx
				jmp		EarlyEndAddr
			}
		}

		// Delayed to overwrite potential inline.
		void WriteDelayedHook()
		{
			WriteRelJump(0x72D7E7, (UInt32)HandleForBarterMenu::MaybePreventBarterMenuTransfer);
		}
	}

	namespace HandleForContainerMenu
	{
		bool __fastcall HandleContainerMenuEvent(ContChangesEntry* toRemove, void* edx)
		{
			auto* menu = ContainerMenu::GetSingleton();
			bool isPlayerDropping = menu->currentItems == &menu->leftItems;
			if (isPlayerDropping)
				return HandleEvent(toRemove, g_thePlayer, menu->containerRef, static_cast<RemovalContext>(menu->mode));
			else
				return HandleEvent(toRemove, menu->containerRef, g_thePlayer, static_cast<RemovalContext>(menu->mode));
		}

		__HOOK MaybePreventContainerMenuTransfer()
		{
			static UInt32 const NormalReturnAddr = 0x75BF08,
				EarlyEndAddr = 0x75C20D;
			_asm
			{
				// ecx = g_containerMenuSelection
				push	ecx // store ecx
				call	HandleContainerMenuEvent
				test	al, al
				jz		PreventRemoval
				// else, do normal code
				pop		ecx
				mov		eax, dword ptr[ecx + 8]
				jmp		NormalReturnAddr
			PreventRemoval :
				pop		ecx
				jmp		EarlyEndAddr
			}
		}

		// Delayed to overwrite potential inline.
		void WriteDelayedHook()
		{
			WriteRelJump(0x75BF03, (UInt32)MaybePreventContainerMenuTransfer);
		}
	}

	namespace HandleForInventoryMenu
	{
		bool __fastcall HandleInventoryMenuEvent(ContChangesEntry* toRemove, void* edx)
		{
			return HandleEvent(toRemove, g_thePlayer, nullptr, kContext_InventoryMenu);
		}

		__HOOK MaybePreventContainerMenuTransfer()
		{
			static UInt32 const NormalReturnAddr = 0x780B07,
				EarlyEndAddr = 0x780B8E;
			_asm
			{
				// ecx = g_invMenuSelection
				push	ecx // store ecx
				call	HandleInventoryMenuEvent
				test	al, al
				jz		PreventRemoval
				// else, do normal code
				pop		ecx
				mov		eax, dword ptr[ecx + 4]
				jmp		NormalReturnAddr
			PreventRemoval :
				pop		ecx
				jmp		EarlyEndAddr
			}
		}

		// Delayed to overwrite potential inline.
		void WriteDelayedHook()
		{
			WriteRelJump(0x780B02, (UInt32)MaybePreventContainerMenuTransfer);
		}
	}

	namespace HandleForRepairMenu
	{
		CallDetour g_detour;

		bool __fastcall HandleRepairMenuEvent(Tile* tile, void* edx, UInt32 tileValue)
		{
			auto result = ThisStdCall<bool>(g_detour.GetOverwrittenAddr(), tile, tileValue);
			if (!result)
				return result;
			auto* menu = RepairMenu::Get();
			auto* toRemove = ThisStdCall<ContChangesEntry*>(0x7A1910, &menu->repairItems); // GetSelectedListItem
			return HandleEvent(toRemove, g_thePlayer, nullptr, kContext_RepairMenu);
		}

		void WriteDelayedHook()
		{
			// Replace Tile::IsFloatValueNotNull call
			g_detour.WriteDetourCall(0x7B5BF1, (UInt32)HandleRepairMenuEvent);
		}
	}

	void WriteDelayedHooks()
	{
		HandleForBarterMenu::WriteDelayedHook();
		HandleForContainerMenu::WriteDelayedHook();
		HandleForInventoryMenu::WriteDelayedHook();
		HandleForRepairMenu::WriteDelayedHook();
	}
}

using EventFlags = NVSEEventManagerInterface::EventFlags;

template<UInt8 N>
bool RegisterEvent(const char* eventName, EventParamType(&paramTypes)[N], 
	EventFlags flags = EventFlags::kFlags_None)
{
	return g_eventInterface->RegisterEvent(eventName, std::size(paramTypes), paramTypes, flags);
};
bool RegisterEvent(const char* eventName, nullptr_t null,
	EventFlags flags = EventFlags::kFlags_None)
{
	return g_eventInterface->RegisterEvent(eventName, 0, nullptr, flags);
};

void RegisterEvents()
{
	OnCornerMessage = JGCreateEvent("OnCornerMessage", 5, 0, NULL);
	OnAuxTimerStart = JGCreateEvent("OnTimerStart", 2, 2, CreateOneFormOneStringFilter);
	OnAuxTimerStop = JGCreateEvent("OnTimerStop", 2, 2, CreateOneFormOneStringFilter);
	OnAuxTimerUpdate = JGCreateEvent("OnTimerUpdate", 3, 2, CreateOneFormOneStringFilter);

	RegisterEvent(OnPreActivate::eventName, kEventParams_OneReference_OneIntPtr);
	RegisterEvent(PreActivateInventoryItem::eventName, kEventParams_OneBaseForm_OneReference_OneIntPtr_OneInt);
	RegisterEvent(PreActivateInventoryItem::eventNameAlt, kEventParams_OneBaseForm_OneReference_OneIntPtr_TwoInts);
	RegisterEvent(OnQuestAdded::eventName, kEventParams_OneBaseForm);

	//TODO: document / modify!
	RegisterEvent(OnCalculateSellPrice::eventNameAdd, kEventParams_OneBaseForm_OneReference);
	RegisterEvent(OnCalculateSellPrice::eventNameSub, kEventParams_OneBaseForm_OneReference);
	RegisterEvent(OnCalculateSellPrice::eventNameMult, kEventParams_OneBaseForm_OneReference);


	RegisterEvent(OnProjectileDestroy::eventName, kEventParams_OneReference_OneBaseForm, EventFlags::kFlag_FlushOnLoad);
	RegisterEvent(OnProjectileCreate::eventName, kEventParams_OneReference_OneBaseForm, EventFlags::kFlag_FlushOnLoad);
	RegisterEvent(OnProjectileImpact::eventName, kEventParams_OneReference_OneBaseForm_OneReference, EventFlags::kFlag_FlushOnLoad);

	RegisterEvent(OnLockpickMenuClose::eventName, kEventParams_OneInt);

	RegisterEvent(OnQueueCornerMessage::eventName, kEventParams_ThreeStrings_OneFloat);
	RegisterEvent(OnShowCornerMessage::eventName, kEventParams_ThreeStrings_OneFloat);
	RegisterEvent(OnFireWeapon::eventName, kEventParams_OneBaseForm, EventFlags::kFlag_FlushOnLoad);

#if _DEBUG
	RegisterEvent(OnCalculateEffectEntryMagnitude::eventName,
		kEventParams_OneReference_TwoBaseForms_OneInt_OneBaseForm_OneInt_ThreeFloats);
#endif

	RegisterEvent(OnPCMiscStatChange::eventName, kEventParams_ThreeInts);

	RegisterEvent(OnDisplayOrCompleteObjective::onDisplayName, kEventParams_OneBaseForm_OneInt);
	RegisterEvent(OnDisplayOrCompleteObjective::onCompleteName, kEventParams_OneBaseForm_OneInt);

	// v1.60
	RegisterEvent(OnAddAlt::eventName, kEventParams_OneBaseForm_OneReference);
	RegisterEvent(OnReadBook::eventName, kEventParams_OneBaseForm);
	RegisterEvent(OnDispositionChange::eventName, kEventParams_OneInt);
	RegisterEvent(PreDropInventoryItem::eventName, kEventParams_OneBaseForm_OneReference_OneIntPtr);
#if 0
	RegisterEvent(OnPreLifeStateChange::eventName, kEventParams_TwoInts);
#endif

	// v1.65
	RegisterEvent(OnPreScriptedActivate::eventName, kEventParams_OneReference_OneInt_OneIntPtr);
	RegisterEvent(OnExplosionHit::eventName, kEventParams_TwoReferences);

	// v1.70
	RegisterEvent(OnPreProjectileExplode::eventName, kEventParams_OneReference_OneIntPtr, EventFlags::kFlag_FlushOnLoad);
	RegisterEvent(OnPreRemoveItemFromMenu::eventName, kEventParams_OneBaseForm_OneReference_OneInt_OneIntPtr);
	

	/*
	// For debugging the Event API
	constexpr char DebugEventName[] = "ShowOff:DebugEvent";
	RegisterEvent(DebugEventName,kEventParams_OneInt_OneFloat_OneArray_OneString_OneForm_OneReference_OneBaseform);
	*/
}

namespace EventHandling
{
	void HandleGameLoopEvents()
	{

	}
}

namespace HandleHooks
{
	void HandleEventHooks()
	{
		OnPreActivate::WriteHook();
		PreActivateInventoryItem::WriteHooks();
		PreDropInventoryItem::WriteHook();
		OnQuestAdded::WriteHook();
	
		OnProjectileDestroy::WriteHook();
		OnProjectileCreate::WriteHook();
		OnProjectileImpact::WriteHooks();
	
		OnLockpickMenuClose::WriteHooks();
		OnQueueCornerMessage::WriteHooks();
		OnShowCornerMessage::WriteHooks();
		OnFireWeapon::WriteHook();
#if _DEBUG
		OnCalculateEffectEntryMagnitude::WriteHooks();
#endif
		OnPCMiscStatChange::WriteHook();
		OnAddAlt::WriteHooks();

		// v1.65
		OnExplosionHit::WriteHooks();

		// v1.70
		OnPreProjectileExplode::WriteHook();
#if _DEBUG
#endif
	}

	void HandleDelayedEventHooks()
	{
		CornerMessageHooks::WriteDelayedHook();
		OnDisplayOrCompleteObjective::WriteDelayedHook();
		OnCalculateSellPrice::WriteDelayedHook();
		OnReadBook::WriteDelayedHook();
		OnDispositionChange::WriteDelayedHooks();
		PreActivateInventoryItem::WriteDelayedHooks();
#if 0
		OnPreLifeStateChange::WriteDelayedHook();
#endif
		// v1.65
		OnPreScriptedActivate::WriteDelayedHooks();

		// v1.70
		OnPreRemoveItemFromMenu::WriteDelayedHooks();
#if _DEBUG
#endif
	}

}
