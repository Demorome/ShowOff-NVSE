#pragma once




DEFINE_COMMAND_ALT_PLUGIN(DumpFormList, FListDump, , 0, 2, kParams_OneFormList_OneOptionalString);
bool Cmd_DumpFormList_Execute(COMMAND_ARGS)
{
	BGSListForm* FList;
	char filepathStr[260] = "nope";
	*result = 0;
	
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &FList, &filepathStr)) return true;
	if (!FList) return true;

	char* buf = GetStrArgBuffer();;  //I have no idea what I'm doing with these char*, lol
	int bufLength = 0;

	//https://stackoverflow.com/questions/2674312/how-to-append-strings-using-sprintf
	bufLength += sprintf(buf+bufLength, "Dumping %s FormList [%08X], size %d:", FList->GetName(), FList->refID, FList->Count());
	
	int iIndex = 0;
	for (tList<TESForm>::Iterator iter = FList->list.Begin(); !iter.End(); ++iter, iIndex++)
	{
		TESForm* form = iter.Get();
		if (form)
		{
			TESFullName* formName = DYNAMIC_CAST(form, TESForm, TESFullName);
			if (formName)
			{
				bufLength += sprintf(buf+bufLength,"\n%d: %s (%s) [%08X]", iIndex, formName->name.m_data, form->GetName(), form->refID);
			}
			else
			{
				bufLength += sprintf(buf+bufLength, "\n%d: (name unavailable) (%s) [%08X]", iIndex, form->GetName(), form->refID);
			}
		}
	}
	if (strcmp(filepathStr, "nope") != 0)  //https://stackoverflow.com/a/1330559, strcmp returns non-zero if the string contents are not equal.
	{
		FileStreamJIP outputFile;
		if (outputFile.OpenWrite(filepathStr, true))
		{
			outputFile.WriteStr(buf);
			*result = 1;
		}
	}
	else
	{
		if (strlen(buf) < 512)
			Console_Print(buf);  //won't check for IsConsoleOpen, to mimic ar_Dump.
		else
			Console_Print_Long(buf);
		*result = 1;
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