#pragma once


DEFINE_COMMAND_ALT_PLUGIN(DumpFormList, LListDump, , 0, 1, kParams_FormList);
bool Cmd_DumpFormList_Execute(COMMAND_ARGS)
{
	BGSListForm* FList;
	if (ExtractArgs(EXTRACT_ARGS, &FList) && IsConsoleOpen() && FList)
	{
		Console_Print("Dumping %s FormList [%08X], size %d:", FList->GetName(), FList->refID, FList->Count());
		for (tList<TESForm>::Iterator iter = FList->list.Begin(); !iter.End(); ++iter) {
			if (iter.Get()) {
				TESFullName* formName = DYNAMIC_CAST(iter.Get(), TESForm, TESFullName);
				Console_Print("%s [%08X]", formName->name.m_data, iter.Get()->refID);
			}
		}
	}
	return true;
}

//Would be cool if this could return whatever is calling this condition, or at what address etc..
DEFINE_CMD_ALT_COND_PLUGIN(TestCondition, , "Returns 1, and prints a message to console. Meant for testing if previous conditions passed.", 0, NULL);
bool Cmd_TestCondition_Execute(COMMAND_ARGS)
{
	*result = 1;
	if (IsConsoleOpen())
		Console_Print("TestCondition >> 1. Not meant for use as a script function.");
	return true;
}
bool Cmd_TestCondition_Eval(COMMAND_ARGS_EVAL)
{
	*result = 1;
	UINT32 refID = 0;
	const char* edID = NULL;
	if (thisObj)
	{
		refID = thisObj->refID;
		TESForm* form = thisObj;
		edID = form->GetName();  //Only works with JG's bLoadEditorIDs = 1, otherwise returns an empty string.
	}
	Console_Print("TestCondition >> 1. thisObj: [%08X] (%s)", refID, edID);
	return true;
}