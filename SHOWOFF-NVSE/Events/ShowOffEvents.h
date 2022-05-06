#pragma once
#include "EventParams.h"

// Credits to Karut (from JohnnyGuitar) for making the Event Framework.

DEFINE_COMMAND_ALT_PLUGIN(SetShowOffOnCornerMessageEventHandler, SetOnCornerMessageEventHandler, "", false, kParams_Event);
#if _DEBUG
DEFINE_COMMAND_ALT_PLUGIN(SetShowOffOnActorValueChangeEventHandler, SetOnAVChangeEventHandler, "", false, kParams_Event);	//TODO: change args
#endif

EventInformation* OnCornerMessage;
EventInformation* OnActorValueChange;


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

bool Cmd_SetShowOffOnActorValueChangeEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove;
	Script* script;
	UInt32 flags = 0;  //reserved for future use
	//todo: extract AV code filter, positive or negative change bool filter
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags) || NOT_TYPE(script, Script))) return true;
	if (OnActorValueChange)
	{
		//TODO: Add filters
		if (setOrRemove)
			OnActorValueChange->RegisterEvent(script, NULL);
		else OnActorValueChange->RemoveEvent(script, NULL);
	}
	return true;
}


/*
DEFINE_COMMAND_PLUGIN(SetOnHitAltEventHandler, , 0, 3, kParams_JIP_OneForm_OneInt_OneOptionalForm);
bool Cmd_SetOnHitAltEventHandler_Execute(COMMAND_ARGS)
{
	return SetActorEventHandler_Execute(PASS_COMMAND_ARGS, s_onHitEventMap, kHookActorFlag3_OnHit, kHook_OnHitEvent);
}
*/


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
			_MESSAGE("Conflict detected with (presumably) Tweaks, preventing hook from being installed at %x", tweakConflictAddr);
		}
		
		for (auto const jmpSrc : cornerMessageHookJmpSrc)
		{
			WriteRelCall(jmpSrc, (UInt32)CornerMessageEventHook);
		}
	}
}

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
		WriteRelCall(0x66EE58, (UInt32)HandleAVChangeHook);
	}
}

namespace OnPreActivate
{
	constexpr char eventName[] = "ShowOff:OnPreActivate";

	bool __fastcall HandleEvent(TESObjectREFR* activated, Actor* activator)
	{
		auto constexpr resultCallback = [](NVSEArrayVarInterface::Element& result, void* shouldActivateAdrr) -> bool
		{
			if (UInt32 &shouldActivate = *static_cast<UInt32*>(shouldActivateAdrr))
			{
				shouldActivate = result.Bool();
			}
			return true;
		};
		UInt32 shouldActivate = true;
		g_eventInterface->DispatchEventAlt(eventName, resultCallback, &shouldActivate, activated, activator, shouldActivate);

#if _DEBUG
		Console_Print("OnActivate HOOK - Activated: [%08X], activator: [%08X]", activated ? activated->refID : 0, 
			activator ? activator->refID : 0);
#endif
		return shouldActivate != 0;
	}	//result in AL

	__declspec(naked) void Hook()
	{
		static UInt32 const retnAddr = 0x57334C, getBaseForm = 0x7AF430,
			retnFalse = 0x573396;
		enum
		{
			activator = 8
		};
		_asm
		{
			//Check if this instance of TESObjectREFR::Activate was called by Activate func.
			//(We don't want the event to run for that)
			mov eax, dword ptr [ebp + 4]	//rtn addr
			cmp eax, 0x5B5B1D	//one of the return addresses to Activate_Execute
			je doNormal
			cmp eax, 0x5B5B4D
			je doNormal

			pushad	//unknown what the __fastcall function will preserve, so store everything.
			mov edx, dword ptr [ebp + activator]
			call HandleEvent
			test al, al
			popad
			jnz doNormal
			jmp retnFalse

			doNormal:
			call getBaseForm	
			jmp retnAddr
		}
	}

	void WriteHook()
	{
		WriteRelJump(0x573347, (UInt32)Hook);
	}
}

namespace PreActivateInventoryItem
{
	constexpr char eventName[] = "ShowOff:OnPreActivateInventoryItem";

	static const UInt32 g_inventoryMenuSelectionAddr = 0x11D9EA8;

	bool __fastcall CanUseItem(ContChangesEntry* itemEntry, void* edx)
	{
		if (!itemEntry || !itemEntry->type)
			return false;

		auto constexpr resultCallback = [](NVSEArrayVarInterface::Element& result, void* shouldActivateAdrr) -> bool
		{
			if (UInt32& shouldActivate = *static_cast<UInt32*>(shouldActivateAdrr))
			{
				shouldActivate = result.Bool();
			}
			return true;
		};
		UInt32 shouldActivate = true;
		auto const itemForm = itemEntry->type;
		TESObjectREFR* invRef = itemEntry->extendData
			? InventoryRefCreateEntry(g_thePlayer, itemEntry->type, itemEntry->countDelta, itemEntry->extendData->GetFirstItem())
			: nullptr;

		g_eventInterface->DispatchEventAlt(eventName, resultCallback, &shouldActivate, 
			g_thePlayer, itemForm, invRef, shouldActivate);

		if (g_ShowFuncDebug)
			Console_Print("CanActivateItemHook: CanActivate: %i, Item: [%08X], %s, type: %u", shouldActivate, itemForm->refID, itemForm->GetName(), itemForm->typeID);

		return shouldActivate;
	}

	__declspec(naked) void HookOnClickAmmo()
	{
		_asm
		{
			//ecx = g_inventoryMenuSelection
			mov		eax, [ecx + 8]
			test	eax, eax
			jz		Done

			call	CanUseItem // register stomping should be fine.
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

	__declspec(naked) void HookHandleOnClickEquipOrUnEquip()
	{
		static const UInt32 endHandleClick = 0x780B8E,
			HandleEquipOrUnEquip = 0x780D60;

		_asm
		{
			//ecx = InventoryMenu
			mov		ecx, g_inventoryMenuSelectionAddr
			mov		ecx, dword ptr ds : [ecx]
			call	CanUseItem // register stomping should be fine.
			test	al, al
			jz		PreventActivation

			mov		ecx, [ebp-0x64] 
			call	HandleEquipOrUnEquip 
			ret

			PreventActivation :
			add		esp, 4	// remove return addr that was pushed.
			jmp		endHandleClick
		}
	}

	__declspec(naked) void HookHandleHotkeyEquipOrUnEquip()
	{
		static const UInt32 normalRetnAdrr = 0x701FB3,
			endFunctionAddr = 0x702130;
		_asm
		{
			//ecx = ContChangesEntry
			call	CanUseItem  //register stomping should be fine
			test	al, al
			jz		PreventActivation

			mov		ecx, [ebp - 0xC]  //in case ecx got stomped, restore it
			mov		eax, [ecx + 8]
			jmp		normalRetnAdrr

			PreventActivation :
			jmp		endFunctionAddr
		}
	}

	void WriteHooks()
	{
		// Replace "call InventoryMenu::HandleEquipOrUnEquip"
		WriteRelCall(0x7805CC, (UInt32)HookHandleOnClickEquipOrUnEquip);

		// Replace "call TESForm::GetFlags(g_inventoryMenuSelection)"
		WriteRelCall(0x780648, (UInt32)HookOnClickAmmo);

		// Replace "call TESForm::GetFlags(entry)"
		WriteRelJump(0x701FAE, (UInt32)HookHandleHotkeyEquipOrUnEquip);
	}
}


void RegisterEvents()
{
	OnCornerMessage = JGCreateEvent("OnCornerMessage", 5, 0, NULL);
#if _DEBUG
	OnActorValueChange = JGCreateEvent("OnActorValueChange", 4, 0, NULL);
#endif

	using EventFlags = NVSEEventManagerInterface::EventFlags;

	auto constexpr RegisterEvent =
		[]<UInt8 N>(const char* eventName, EventParamType(&paramTypes)[N], EventFlags flags = EventFlags::kFlags_None)
	{
		g_eventInterface->RegisterEvent(eventName, std::size(paramTypes), paramTypes, flags);
	};

	RegisterEvent(OnPreActivate::eventName, kEventParams_OneReference_OneInt);
	RegisterEvent(PreActivateInventoryItem::eventName, kEventParams_OneBaseForm_OneReference_OneInt);

#if _DEBUG
	constexpr char DebugEventName[] = "ShowOff:DebugEvent";
	RegisterEvent(DebugEventName,kEventParams_OneInt_OneFloat_OneArray_OneString_OneForm_OneReference_OneBaseform);
#endif
}

namespace HandleHooks
{
	void HandleEventHooks()
	{
		OnPreActivate::WriteHook();
		PreActivateInventoryItem::WriteHooks();
#if _DEBUG
		//ActorValueChangeHooks::WriteHook();
#endif
	}

	void HandleDelayedEventHooks()
	{
		CornerMessageHooks::WriteDelayedHook();
	}

}
