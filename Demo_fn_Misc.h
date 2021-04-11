#pragma once
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
