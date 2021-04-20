#pragma once
#include "GameRTTI.h"
#include "SafeWrite.h"
#include "ShowOffNVSE.h"
#include "internal/StewieMagic.h"
#include "internal/jip_nvse.h"
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
		*result = (int)challenge->progress;  //This can show up as negative.
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
		challenge->progress = value;
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
		challenge->progress += value;
		*result = 1;
	}
	else *result = 0;
	return true;
}


#if IFYOULIKEBROKENSHIT
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
		/*if (!thisObj || !thisObj->IsActor()) return true;*/ //Idk why IsActor() can't be found, not gonna bother for now.
		actorBase = (TESActorBase*)thisObj->baseForm;
	}
	UInt32 currentValue = *result = actorBase->avOwner.GetActorValue(actorVal);
	//Console_Print("Current Value %d", currentValue);
	actorBase->ModActorValue(actorVal, (valueToSet - currentValue));
	return true;
}
#endif

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

#if IFYOULIKEBROKENSHIT

DEFINE_COMMAND_PLUGIN(GetItemRefValue, , 1, 0, NULL);
bool Cmd_GetItemRefValue_Execute(COMMAND_ARGS)
{
	*result = -1;
	if (thisObj)
	{
		ContChangesEntry tempEntry(NULL, 1, thisObj->baseForm);  //copying after GetCalculatedWeaponDamage from JIP, and c6.
		ExtraContainerChanges::ExtendDataList extendData;
		extendData.Init(&thisObj->extraDataList);
		tempEntry.extendData = &extendData;
		*result = ThisStdCall<double>(0x4BD400, &tempEntry);
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(GetItemRefHealth, , 1, 1, kParams_OneOptionalInt);
bool Cmd_GetItemRefHealth_Execute(COMMAND_ARGS)
{
	*result = -1;
	UINT32 bPercent = 0;
	if (thisObj && ExtractArgs(EXTRACT_ARGS, &bPercent))
	{
		ContChangesEntry tempEntry(NULL, 1, thisObj->baseForm);  //copying after GetCalculatedWeaponDamage from JIP, and c6.
		ExtraContainerChanges::ExtendDataList extendData;
		extendData.Init(&thisObj->extraDataList);
		tempEntry.extendData = &extendData;

		//double __thiscall ContChangesEntry::GetHealthPerc(ContChangesEntry * this, int a2)
		*result = ThisStdCall<double>(0x4BCDB0, &tempEntry, bPercent);
	}
	return true;
}

/*
DEFINE_COMMAND_PLUGIN(GetCalculatedItemWeight, , 1, 0, One);
bool Cmd_GetCalculatedItemWeight_Execute(COMMAND_ARGS)
{
	*result = -1;
	if (thisObj)
	{
		ContChangesEntry tempEntry(NULL, 1, thisObj->baseForm);  
		ExtraContainerChanges::ExtendDataList extendData;
		extendData.Init(&thisObj->extraDataList);
		tempEntry.extendData = &extendData;

		//double __thiscall ContChangesEntry::GetHealthPerc(ContChangesEntry * this, int a2)
		*result = ThisStdCall<double>(0x4BCDB0, &tempEntry, bPercent);
	}
	return true;
}
*/



#endif