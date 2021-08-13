#pragma once

DEFINE_COMMAND_ALT_PLUGIN(DumpFormList, FListDump, "", false, kParams_OneFormList_OneOptionalString_OneOptionalInt);
DEFINE_CMD_COND_PLUGIN(ConditionPrint, "Returns 1, and prints a message to console. Meant for testing if previous conditions passed.", false, NULL);
DEFINE_COMMAND_PLUGIN(GetShowOffDebugMode, "", false, NULL);
DEFINE_COMMAND_PLUGIN(SetShowOffDebugMode, "Set to 1 to enable debug prints for certain ShowOff functions.", false, kParams_OneInt);
DEFINE_COMMAND_PLUGIN(IsBaseForm, "", false, kParams_OneForm);


bool Cmd_DumpFormList_Execute(COMMAND_ARGS)
{
	BGSListForm* FList = nullptr;
	char fileName[MAX_PATH] = "default";
	UInt32 bAppend = 1;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &FList, &fileName, &bAppend) 
		|| !IS_TYPE(FList, BGSListForm)) return true;

	if (strcmp(fileName, "default") == 0)  //if they are the same, i.e. filepathStr was not passed.
	{
		FList->Dump();
	}
	else
	{
		FList->DumpToFile(fileName, bAppend);
	}
	return true;
}

//Would be cool if this could return whatever is calling this condition, or at what address etc..
bool Cmd_ConditionPrint_Execute(COMMAND_ARGS)
{
	*result = 1;
	if (IsConsoleMode())
		Console_Print("TestCondition >> 1. Not meant for use as a script function.");
	return true;
}
bool Cmd_ConditionPrint_Eval(COMMAND_ARGS_EVAL)
{
	*result = 1;
	UINT32 refID = 0;
	const char* edID = NULL;
	if (thisObj)
	{
		refID = thisObj->refID;
		edID = thisObj->GetName();  //Only works with JG's bLoadEditorIDs = 1, otherwise returns an empty string.
	}
	Console_Print("TestCondition >> 1. thisObj: [%08X] (%s)", refID, edID);
	return true;
}

bool Cmd_GetShowOffDebugMode_Execute(COMMAND_ARGS)
{
	*result = g_ShowFuncDebug;
	return true;
}
bool Cmd_SetShowOffDebugMode_Execute(COMMAND_ARGS)
{
	UInt32 bOn = false;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &bOn)) return true;
	g_ShowFuncDebug = bOn;
	return true;
}

bool Cmd_IsBaseForm_Execute(COMMAND_ARGS)
{
	*result = false;
	TESForm* form;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &form)) return true;
	if (form && !IS_REFERENCE(form))
	{
		*result = true;
	}
	return true;
}


#if _DEBUG

DEFINE_CMD_ALT_COND_PLUGIN(GetNVSEVersionFullAlt, , , 0, NULL);
bool Cmd_GetNVSEVersionFullAlt_Execute(COMMAND_ARGS)
{
	*result = PACKED_NVSE_VERSION;
	if (IsConsoleMode())
		Console_Print("GetNVSEVersionFullAlt >> %f.", result);  //does not work how I'd hoped, lol.
	return true;
}
bool Cmd_GetNVSEVersionFullAlt_Eval(COMMAND_ARGS_EVAL)
{
	//??
	return true;
}

#endif