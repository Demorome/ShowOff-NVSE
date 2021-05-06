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