#pragma once

// Credits to Karut (from JohnnyGuitar) for making the Event Framework.

DEFINE_COMMAND_ALT_PLUGIN(SetShowOffOnCornerMessageEventHandler, SetOnCornerMessageEventHandler, "", false, kParams_Event);
DEFINE_COMMAND_ALT_PLUGIN(SetShowOffOnActorValueChangeEventHandler, SetOnAVChangeEventHandler, "", false, kParams_Event);	//TODO: change args


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
		// Avoid throwing exceptions with bad char*
		std::string msgTextStr, iconPathStr, soundPathStr;
		msgTextStr = iconPathStr = soundPathStr = "<void>";
		if (msgText && msgText[0]) msgTextStr = msgText;
		if (iconPath && iconPath[0]) iconPathStr = iconPath;
		if (soundPath && soundPath[0]) soundPathStr = soundPath;

		for (auto const& callback : OnCornerMessage->EventCallbacks) {
			FunctionCallScriptAlt(callback.ScriptForEvent, nullptr, OnCornerMessage->numMaxArgs, msgTextStr.c_str(), IconType, iconPathStr.c_str(), soundPathStr.c_str(), *(UInt32*)&displayTime);
		}
#if _DEBUG
		Console_Print("==CornerMessageEventHook==\n -msgText: %s\n -IconType: %d\n -iconPath: %s\n -soundPath: %s\n -displayTime: %f\n -instantEndCurrentMessage: %d", msgText, IconType, iconPath, soundPath, displayTime, instantEndCurrentMessage);
#endif
		return ThisStdCall_B(0x775380, menu, msgText, IconType, iconPath, soundPath, displayTime, instantEndCurrentMessage);
	}
	
	void WriteHook()
	{
		// SPECIAL CASE: 0x8B959B is Nop'd by Tweaks if bNoCrippleCriticalMessages = 1 && g_bPatchCripple = 1.
		auto const tweakConflictAdr = 0x8B959B;
		
		std::vector cornerMessageHookJmpSrc{ 0x705379, 0x7EE74D, 0x7EE87D, 0x7EEA6C, 0x833303, tweakConflictAdr };

		if (GetINIValFromTweaks("bNoCrippleCriticalMessages")
			&& GetINIValFromTweaks("bPatchCripple", "Cripple-Critical Messages"))
		{
			cornerMessageHookJmpSrc.erase(
				std::find(cornerMessageHookJmpSrc.begin(), cornerMessageHookJmpSrc.end(), tweakConflictAdr)
			);
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

void RegisterEvents()
{
	OnCornerMessage = JGCreateEvent("OnCornerMessage", 5, 0, NULL);
	OnActorValueChange = JGCreateEvent("OnActorValueChange", 4, 0, NULL);
}

namespace HandleHooks
{
	void HandleEventHooks()
	{
		CornerMessageHooks::WriteHook();
		//ActorValueChangeHooks::WriteHook();
	}
}
