#pragma once

// Credits to Karut (from JohnnyGuitar) for making the Event Framework.

DEFINE_COMMAND_PLUGIN(SetShowOffOnCornerMessageEventHandler, , 0, 3, kParams_Event);


EventInformation* OnCornerMessage;


bool __fastcall handleCornerMessageEvent(HUDMainMenu* menu, void* edx, char* msgText, eEmotion IconType, char* iconPath, char* soundPath, float displayTime, bool instantEndCurrentMessage)
{
	for (auto const& callback : OnCornerMessage->EventCallbacks) {
		FunctionCallScript(callback.ScriptForEvent, NULL, 0, &EventResultPtr, OnCornerMessage->numMaxArgs);
	}
#if _DEBUG
	//Console_Print("==Testing hook==\n -msgText: %s\n -IconType: %d\n -iconPath: %s\n -soundPath: %s\n -displayTime: %f\n -instantEndCurrentMessage: %d", msgText, IconType, iconPath, soundPath, displayTime, instantEndCurrentMessage);
#endif
	return ThisStdCall_B(0x775380, menu, msgText, IconType, iconPath, soundPath, displayTime, instantEndCurrentMessage);
}


bool Cmd_SetShowOffOnCornerMessageEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = nullptr;
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
	OnCornerMessage = JGCreateEvent("OnCornerMessage", 0, 0, NULL); 
	FunctionCallScript = g_scriptInterface->CallFunction;
	WriteRelCall(0x705379, UINT32(handleCornerMessageEvent));
	WriteRelCall(0x7EE74D, UINT32(handleCornerMessageEvent));
	WriteRelCall(0x7EE87D, UINT32(handleCornerMessageEvent));
	WriteRelCall(0x7EEA6C, UINT32(handleCornerMessageEvent));
	WriteRelCall(0x833303, UINT32(handleCornerMessageEvent));
	WriteRelCall(0x8B959B, UINT32(handleCornerMessageEvent));
}