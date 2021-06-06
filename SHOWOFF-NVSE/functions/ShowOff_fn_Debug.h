#pragma once

DEFINE_COMMAND_ALT_PLUGIN(DumpFormList, FListDump, , 0, 3, kParams_OneFormList_OneOptionalString_OneOptionalInt);
DEFINE_CMD_ALT_COND_PLUGIN(ConditionPrint, , "Returns 1, and prints a message to console. Meant for testing if previous conditions passed.", 0, NULL);


bool Cmd_DumpFormList_Execute(COMMAND_ARGS)
{
	BGSListForm* FList = nullptr;
	char fileName[MAX_PATH] = "default";
	UInt32 bAppend = 1;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &FList, &fileName, &bAppend) || !IS_TYPE(FList, BGSListForm
	)) return true;

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


#if _DEBUG

DEFINE_CMD_ALT_COND_PLUGIN(GetNVSEVersionFullAlt, , , 0, NULL);

bool Cmd_GetNVSEVersionFullAlt_Execute(COMMAND_ARGS)
{
	*result = PACKED_NVSE_VERSION;
	if (IsConsoleMode())
		Console_Print("GetNVSEVersionFullAlt >> %f.", result);
	return true;
}
bool Cmd_GetNVSEVersionFullAlt_Eval(COMMAND_ARGS_EVAL)
{
	//??
	return true;
}

#endif