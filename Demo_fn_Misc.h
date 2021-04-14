#pragma once
#include "GameRTTI.h"
#include "GameSettings.h"
#include "jip_nvse.h"
//#include <iso646.h>

DEFINE_COMMAND_PLUGIN(GetChallengeProgress, "Returns the progress made on a challenge.", 0, 1, kParams_Tomm_OneForm)
//If the challenge is beaten...
//...and is NOT set to "Recurring", will return the max Threshold value.
//......If the value was changed using SetChallengeProgress / ModChallengeProgress, its "progress" will change, but it still won't budge naturally.
//...and is set to "Recurring", will return 0. Afterwards, will increment as normal.
//......When going over the threshold using Mod/SetChallengeProgress, leftover progress is kept to increase the progress. This amount can even go over the threshold.
//......However, those functions DO NOT manually trigger challenge completion.
//......Afterwards, once triggered normally, the challenge will be completed once more, doing "Progress -= Threshold". After this, you could still have Progress > Threshold.


DEFINE_COMMAND_PLUGIN(SetChallengeProgress, "Changes the progress made on a challenge to a specific value.", 0, 2, kParams_OneForm_OneInt)
DEFINE_COMMAND_PLUGIN(ModChallengeProgress, "Modifies the progress made on a challenge.", 0, 2, kParams_OneForm_OneInt)

//Going into negative seems to work fine, it just delays the challenge progress.
//It also looks weird since it shows "0/x" progress when it returns to 0.

//FLAW TO FIX: Find a way to forcefully activate the Challenge completion.

bool Cmd_GetChallengeProgress_Execute(COMMAND_ARGS)
{
	TESChallenge *challenge;
	if (ExtractArgs(EXTRACT_ARGS, &challenge) && IS_TYPE(challenge, TESChallenge))
		*result = (int)challenge->amount;  //This can show up as negative.
	else *result = 0;
	return true;
}

bool Cmd_SetChallengeProgress_Execute(COMMAND_ARGS)
{
	TESChallenge* challenge;
	UInt32 value;
	if (ExtractArgs(EXTRACT_ARGS, &challenge, &value) && IS_TYPE(challenge, TESChallenge))
	{
		//if (value > challenge->threshold )
		challenge->amount = value;
		*result = 1;
	}
	else *result = 0;
	return true;
}

bool Cmd_ModChallengeProgress_Execute(COMMAND_ARGS)
{
	TESChallenge* challenge;
	UInt32 value;
	if (ExtractArgs(EXTRACT_ARGS, &challenge, &value) && IS_TYPE(challenge, TESChallenge))
	{
		//UInt32 const test_amount = challenge->amount + value;
		challenge->amount += value;
		*result = 1;
	}
	else *result = 0;
	return true;
}

DEFINE_COMMAND_PLUGIN(CompleteChallenge, "Completes a challenge.", 0, 1, kParams_Tomm_OneForm)
bool Cmd_CompleteChallenge_Execute(COMMAND_ARGS)
{
	TESChallenge* challenge;
	UInt32 value;
	if (ExtractArgs(EXTRACT_ARGS, &challenge, &value) && IS_TYPE(challenge, TESChallenge))
	{
		challenge->challengeflags |= 2;
		*result = 1; //success
	}
	else *result = 0;
	return true;
}



DEFINE_COMMAND_ALT_PLUGIN(SetBaseActorValue, SetBaseAV, , 0, 3, kParams_JIP_OneActorValue_OneFloat_OneOptionalActorBase); 
bool Cmd_SetBaseActorValue_Execute(COMMAND_ARGS) 
{
	UInt32 actorVal;
	float valueToSet;
	TESActorBase* actorBase = NULL;
	if (!ExtractArgs(EXTRACT_ARGS, &actorVal, &valueToSet, &actorBase)) return true;
	if (!actorBase)
	{
		if (!thisObj || !thisObj->IsActor()) return true;
		actorBase = (TESActorBase*)thisObj->baseForm;
	}
	UInt32 currentValue = *result = actorBase->avOwner.GetActorValue(actorVal);
	//Console_Print("Current Value %d", currentValue);
	actorBase->ModActorValue(actorVal, (valueToSet - currentValue));
	return true;
}

//DEFINE_COMMAND_ALT_PLUGIN(SetBaseActorValueAlt, SetBaseAVAlt, , 0, 3, ? ? ? ? ? );


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

DEFINE_COMMAND_PLUGIN(IsGamesetting, "Checks if a string refers to a valid Gamesetting", 0, 1, kParams_OneString);
bool Cmd_IsGamesetting_Execute(COMMAND_ARGS)
{
	char settingName[512];
	Setting* setting;
	GameSettingCollection* gmsts = GameSettingCollection::GetSingleton();

	if (ExtractArgs(EXTRACT_ARGS, &settingName)) {
		if (gmsts && gmsts->GetGameSetting(settingName, &setting)) {
			*result = 1;
			if (IsConsoleMode())
				Console_Print("IsGamesetting >> VALID GAMESETTING");
		}
		else {
			*result = 0;
			if (IsConsoleMode())
				Console_Print("IsGamesetting >> INVALID GAMESETTING");
		}
	}

	return true;
}

DEFINE_COMMAND_PLUGIN(IsINISetting, "Checks if a string refers to a valid FalloutPrefs.ini / Fallout.ini setting.", 0, 1, kParams_OneString);
bool Cmd_IsINISetting_Execute(COMMAND_ARGS)
{
	char settingName[512];
	Setting* setting;

	if (ExtractArgs(EXTRACT_ARGS, &settingName)) {
		if (GetIniSetting(settingName, &setting)) {
			*result = 1;
			if (IsConsoleMode())
				Console_Print("IsINISetting >> VALID INI SETTING");
		}
		else {
			*result = 0;
			if (IsConsoleMode())
				Console_Print("IsINISetting >> INVALID INI SETTING");
		}
	}

	return true;
}

#include "ArrayVar.h"

DEFINE_COMMAND_EXP(ar_DumpF, "dumps the contents of an array to a file for debugging purposes.", 0, kParams_JIP_OneString_OneInt);
bool Cmd_ar_DumpF_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 arrID;
	if (!ExtractArgs(EXTRACT_ARGS, &s_strArgBuffer, &arrID))
	{
		NVSEArrayVar* mainArray = LookupArrayByID(arrID), * column;
		if (!mainArray) return true;
		UInt32 numLines = 1, idx, cnt;
		TempArrayElements topLine(mainArray);
		if (!topLine.size) return true;
		Vector<TempArrayElements> columnBuffer(topLine.size);
		ArrayElementR* elem;
		TempArrayElements* colElements;
		for (idx = 0; idx < topLine.size; idx++)
		{
			elem = &topLine.elements[idx];
			if (column = elem->Array())
			{
				colElements = columnBuffer.Append(column);
				if (numLines < colElements->size)
					numLines = colElements->size;
			}
			else columnBuffer.Append(elem);
		}
		FileStream outputFile;
		if (outputFile.OpenWrite(s_strArgBuffer, true))
		{
			for (idx = 0; idx < numLines; idx++)
			{
				for (cnt = 0; cnt < topLine.size; cnt++)
				{
					if (columnBuffer[cnt].size > idx)
					{
						elem = &columnBuffer[cnt].elements[idx];
						switch (elem->GetType())
						{
						case 1:
							FltToStr(s_strValBuffer, elem->Number());
							outputFile.WriteStr(s_strValBuffer);
							break;
						case 2:
							if (elem->Form())
							{
								outputFile.WriteChar('@');
								outputFile.WriteStr(elem->Form()->RefToString());
							}
							else outputFile.WriteChar('0');
							break;
						case 3:
							outputFile.WriteChar('$');
							outputFile.WriteStr(elem->String());
							break;
						default:
							outputFile.WriteChar('0');
						}
					}
					else outputFile.WriteChar('0');
					if ((topLine.size - cnt) > 1)
						outputFile.WriteChar('\t');
				}
				if ((numLines - idx) > 1)
					outputFile.WriteChar('\n');
			}
			*result = 1;
		}
	}

	return true;
}