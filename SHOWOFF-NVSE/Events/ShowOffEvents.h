#pragma once

// Credits to Karut (from JohnnyGuitar) for making the Event Framework.

DEFINE_COMMAND_PLUGIN(SetShowOffOnCornerMessageEventHandler, "", false, kParams_Event);


EventInformation* OnCornerMessage;

// iconPath and soundPath can be null.
bool __fastcall CornerMessageEventHook(HUDMainMenu* menu, void* edx, char* msgText, eEmotion IconType, char* iconPath, char* soundPath, float displayTime, bool instantEndCurrentMessage)
{
	std::string iconPathStr = "<void>", soundPathStr = "<void>";
	if (iconPath && iconPath[0]) iconPathStr = iconPath;
	if (soundPath && soundPath[0]) soundPathStr = soundPath;
	
	for (auto const& callback : OnCornerMessage->EventCallbacks) {
		FunctionCallScriptAlt(callback.ScriptForEvent, nullptr, OnCornerMessage->numMaxArgs, msgText, IconType, iconPathStr.c_str(), soundPathStr.c_str(), *(UInt32*)&displayTime);
	}
#if _DEBUG
	Console_Print("==CornerMessageEventHook==\n -msgText: %s\n -IconType: %d\n -iconPath: %s\n -soundPath: %s\n -displayTime: %f\n -instantEndCurrentMessage: %d", msgText, IconType, iconPath, soundPath, displayTime, instantEndCurrentMessage);
#endif
	return ThisStdCall_B(0x775380, menu, msgText, IconType, iconPath, soundPath, displayTime, instantEndCurrentMessage);
}


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


/*
DEFINE_COMMAND_PLUGIN(SetOnHitAltEventHandler, , 0, 3, kParams_JIP_OneForm_OneInt_OneOptionalForm);
bool Cmd_SetOnHitAltEventHandler_Execute(COMMAND_ARGS)
{
	return SetActorEventHandler_Execute(PASS_COMMAND_ARGS, s_onHitEventMap, kHookActorFlag3_OnHit, kHook_OnHitEvent);
}
*/


void HandleEventHooks()
{

	
#if _DEBUG
	// todo: add args
	OnCornerMessage = JGCreateEvent("OnCornerMessage", 5, 0, NULL); 
	WriteRelCall(0x705379, (UInt32)CornerMessageEventHook);
	WriteRelCall(0x7EE74D, (UInt32)CornerMessageEventHook);
	WriteRelCall(0x7EE87D, (UInt32)CornerMessageEventHook);
	WriteRelCall(0x7EEA6C, (UInt32)CornerMessageEventHook);
	WriteRelCall(0x833303, (UInt32)CornerMessageEventHook);
	WriteRelCall(0x8B959B, (UInt32)CornerMessageEventHook);
#endif
}