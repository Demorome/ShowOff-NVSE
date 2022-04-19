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

namespace OnActivate
{
	constexpr char eventName[] = "ShowOff:OnRegularActivate";

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
			popad
			test al, al
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


using EventParamType = NVSEEventManagerInterface::ParamType;

static EventParamType kEventParams_OneForm_OneInt[2] =
{
	EventParamType::eParamType_AnyForm,
	EventParamType::eParamType_Int
};

static EventParamType kEventParams_OneInt_OneFloat_OneArray_OneString_OneForm_OneReference_OneBaseform[] =
{
	EventParamType::eParamType_Int,
	EventParamType::eParamType_Float,
	EventParamType::eParamType_Array,
	EventParamType::eParamType_String,
	EventParamType::eParamType_AnyForm,
	EventParamType::eParamType_Reference,
	EventParamType::eParamType_BaseForm,
};

/*
static EventParamType kEventParams_GameEvent[2] =
{
	EventParamType::eParamType_AnyForm, EventParamType::eParamType_AnyForm
};

static EventParamType kEventParams_OneForm[1] =
{
	EventParamType::eParamType_AnyForm,
};


static EventParamType kEventParams_OneString[1] =
{
	EventParamType::eParamType_String
};

static EventParamType kEventParams_OneInteger[1] =
{
	EventParamType::eParamType_Integer
};

static EventParamType kEventParams_TwoIntegers[2] =
{
	EventParamType::eParamType_Integer, EventParamType::eParamType_Integer
};

static EventParamType kEventParams_OneFloat_OneRef[2] =
{
	 EventParamType::eParamType_Float, EventParamType::eParamType_AnyForm
};

static EventParamType kEventParams_OneRef_OneInt[2] =
{
	EventParamType::eParamType_AnyForm, EventParamType::eParamType_Integer
};

static EventParamType kEventParams_OneArray[1] =
{
	EventParamType::eParamType_Array
};
*/
void RegisterEvents()
{
	using EventFlags = NVSEEventManagerInterface::EventFlags;

	OnCornerMessage = JGCreateEvent("OnCornerMessage", 5, 0, NULL);
	OnActorValueChange = JGCreateEvent("OnActorValueChange", 4, 0, NULL);

	g_eventInterface->RegisterEvent(OnActivate::eventName, std::size(kEventParams_OneForm_OneInt), 
		kEventParams_OneForm_OneInt, EventFlags::kFlags_None);


#if _DEBUG
	constexpr char DebugEventName[] = "ShowOff:DebugEvent";
	g_eventInterface->RegisterEvent(DebugEventName, std::size(kEventParams_OneInt_OneFloat_OneArray_OneString_OneForm_OneReference_OneBaseform),
		kEventParams_OneInt_OneFloat_OneArray_OneString_OneForm_OneReference_OneBaseform, EventFlags::kFlags_None);
#endif
}

namespace HandleHooks
{
	void HandleEventHooks()
	{
		//ActorValueChangeHooks::WriteHook();
		OnActivate::WriteHook();
#if _DEBUG
		
#endif
	}

	void HandleDelayedEventHooks()
	{
		CornerMessageHooks::WriteDelayedHook();
	}

}
