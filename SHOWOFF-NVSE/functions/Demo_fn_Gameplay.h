#pragma once

DEFINE_COMMAND_ALT_PLUGIN(SetPlayerCanPickpocketEquippedItems, SetPCCanStealEqItems, "Toggles the ability to pickpocket equipped items.", 0, 1, kParams_OneInt);
bool Cmd_SetPlayerCanPickpocketEquippedItems_Execute(COMMAND_ARGS)
{
	UInt32 bOn;
	if (NUM_ARGS && ExtractArgs(EXTRACT_ARGS, &bOn))
	{
		bool bCheck = canPlayerPickpocketEqItems();
		if (bOn && !bCheck)
		{
			// replace check in ContainerMenu::ShouldHideItem while pickpocketting for item being worn, with a check the target is a child
			//Courtesy of lStewieAl!
			WriteRelCall(0x75E87A, UInt32(ContainerMenuCheckIsTargetChild));
		}
		else if (!bOn && bCheck)
		{
			// revert the change to the check in ContainerMenu::ShouldHideItem.
			WriteRelCall(0x75E87A, 0x4BDDD0);
		}
	}
	return true;
}

DEFINE_CMD_ALT_COND_PLUGIN(GetPlayerCanPickpocketEquippedItems, GetPCCanStealEqItems, "Checks if the player can pickpocket equipped items.", 0, NULL);
bool Cmd_GetPlayerCanPickpocketEquippedItems_Eval(COMMAND_ARGS_EVAL)
{
	*result = canPlayerPickpocketEqItems();
	return true;
}
bool Cmd_GetPlayerCanPickpocketEquippedItems_Execute(COMMAND_ARGS)
{
	*result = canPlayerPickpocketEqItems();
	return true;
}

DEFINE_CMD_ALT_COND_PLUGIN(GetPCCanFastTravel, GetCanFastTravel, "Returns whether or not the player can Fast Travel", 0, NULL);
bool Cmd_GetPCCanFastTravel_Eval(COMMAND_ARGS_EVAL)
{
	*result = (g_thePlayer->byte66D & 1) != 0;
	return true;
}
bool Cmd_GetPCCanFastTravel_Execute(COMMAND_ARGS)
{
	*result = (g_thePlayer->byte66D & 1) != 0;
	return true;
}

DEFINE_CMD_ALT_COND_PLUGIN(GetPCCanSleepWait, GetCanSleepWait, "Returns whether or not the player can Sleep/Wait", 0, NULL);
bool Cmd_GetPCCanSleepWait_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_thePlayer->canSleepWait;
	return true;
}
bool Cmd_GetPCCanSleepWait_Execute(COMMAND_ARGS)
{
	Cmd_GetPCCanSleepWait_Eval(thisObj, NULL, NULL, result);
	return true;
}

DEFINE_COMMAND_ALT_PLUGIN(SetPCCanSleepWait, SetCanSleepWait, "Sets whether or not the player can Sleep/Wait", 0, 1, kParams_OneInt);
bool Cmd_SetPCCanSleepWait_Execute(COMMAND_ARGS)
{
	UInt32 bOn;
	if (ExtractArgs(EXTRACT_ARGS, &bOn))
		g_thePlayer->canSleepWait = (bOn != 0);
	return true;
}


DEFINE_CMD_ALT_COND_PLUGIN(IsWeaponMelee, , "Returns 1 if the weapon's base form is of one of the three weapon types belonging to melee-range weapons.", 1, kParams_OneOptionalObjectID);
bool Cmd_IsWeaponMelee_Eval(COMMAND_ARGS_EVAL)
{
	//Console_Print("thisObj: [%0.8X]", thisObj->baseForm->GetId());
	*result = 0;
	TESForm* form;
	if (arg1)
	{
		form = (TESForm*)arg1;
	}
	else if (thisObj)
	{
		form = thisObj->baseForm;
	}
	else return true;

	auto const weapon = DYNAMIC_CAST(form, TESForm, TESObjectWEAP);
	if (!weapon) return true;

	UINT8 weapType = weapon->eWeaponType;
	*result = weapType <= 2;
	return true;
}
bool Cmd_IsWeaponMelee_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESObjectWEAP* weapon = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &weapon)) return true;

	return Cmd_IsWeaponMelee_Eval(thisObj, weapon, 0, result);
}

DEFINE_CMD_ALT_COND_PLUGIN(IsEquippedWeaponMelee, , "Returns 1 if the calling actor's equipped weapon's base form is of one of the three weapon types belonging to melee-range weapons.", 1, NULL);
bool Cmd_IsEquippedWeaponMelee_Eval(COMMAND_ARGS_EVAL)
{
	// Not recommended to use this function for certain perk effects, like Calculate Weap. Damage;
	// since it will affect the DAM that appears in the UI for other weapons, as long as the currently equipped weapon is a melee weap.

	*result = 0;
	if (thisObj)
	{
		if (!thisObj->IsActor()) return true;
		TESObjectWEAP* weapon = ((Actor*)thisObj)->GetEquippedWeapon();
		if (weapon)
		{
			*result = weapon->eWeaponType <= 2;
		}
		else
		{
			*result = 1;
#if _DEBUG
			Console_Print("IsEquippedWeaponMelee >> 1 >> No equipped weapon found, assuming that the default unarmed is equipped.");
#endif
		}
	}
	return true;
}
bool Cmd_IsEquippedWeaponMelee_Execute(COMMAND_ARGS)
{
	return Cmd_IsEquippedWeaponMelee_Eval(thisObj, nullptr, nullptr, result);
}

DEFINE_CMD_ALT_COND_PLUGIN(IsWeaponRanged, , "Returns 1 if the weapon's base form is one of the weapon types belonging to NON melee-range weapons.", 1, kParams_OneOptionalObjectID);
bool Cmd_IsWeaponRanged_Eval(COMMAND_ARGS_EVAL)
{
	//Console_Print("thisObj: [%0.8X]", thisObj->baseForm->GetId());
	*result = 0;
	TESForm* form;
	if (arg1)
	{
		form = (TESForm*)arg1;
	}
	else if (thisObj)
	{
		form = thisObj->baseForm;
	}
	else return true;

	auto const weapon = DYNAMIC_CAST(form, TESForm, TESObjectWEAP);
	if (!weapon) return true;

	UINT8 weapType = weapon->eWeaponType;
	*result = weapType >= 3 && weapType <= 13;

	return true;
}
bool Cmd_IsWeaponRanged_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESObjectWEAP* weapon = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &weapon)) return true;

	return Cmd_IsWeaponRanged_Eval(thisObj, weapon, 0, result);
}

DEFINE_CMD_ALT_COND_PLUGIN(IsEquippedWeaponRanged, , "Returns 1 if the calling actor's equipped weapon's base form is one of the weapon types belonging to NON melee-range weapons.", 1, NULL);
bool Cmd_IsEquippedWeaponRanged_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (thisObj)
	{
		if (!thisObj->IsActor()) return true;
		TESObjectWEAP* weapon = ((Actor*)thisObj)->GetEquippedWeapon();
		if (weapon)
		{
			UINT8 weapType = weapon->eWeaponType;
			*result = weapType >= 3 && weapType <= 13;
		}
	}
	return true;
}
bool Cmd_IsEquippedWeaponRanged_Execute(COMMAND_ARGS)
{
	return Cmd_IsEquippedWeaponRanged_Eval(thisObj, nullptr, nullptr, result);
}

DEFINE_CMD_ALT_COND_PLUGIN(GetChallengeProgress, , "Returns the progress made on a challenge.", 0, kParams_OneChallenge)
bool Cmd_GetChallengeProgress_Execute(COMMAND_ARGS)
{
	TESChallenge* challenge;
	if (ExtractArgs(EXTRACT_ARGS, &challenge) && IS_TYPE(challenge, TESChallenge))
		*result = (int)challenge->progress;  //This can show up as negative.
	else *result = 0;
	return true;
}
bool Cmd_GetChallengeProgress_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (arg1)
	{
		TESChallenge* challenge = (TESChallenge*)arg1;
		if (IS_TYPE(challenge, TESChallenge))
			*result = (int)challenge->progress;
	}
#if _DEBUG
	Console_Print("GetChallengeProgress >> %f", *result);
#endif
	return true;
}



#ifdef _DEBUG



/*Notes on GetChallengeProgress w/ other funcs:*/
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



DEFINE_COMMAND_PLUGIN(GetProjectileRefFlag, , 1, 0, NULL);
bool Cmd_GetProjectileRefFlag_Execute(COMMAND_ARGS)
{
	GrenadeProjectile* projectile = (GrenadeProjectile*)thisObj;
	//if (!(projectile->projFlags & 0x200) && (projectile->sourceRef != g_thePlayer) && ((((BGSProjectile*)thisObj->baseForm)->projFlags & 0x426) == 0x26))
	//if (!(projectile->projFlags & 0x200) && ((((BGSProjectile*)thisObj->baseForm)->projFlags & 0x426) == 0x26))
	//if (!(projectile->projFlags & 0x200))
	*result = ((Projectile*)thisObj)->projFlags;
	
	return true;
}

DEFINE_COMMAND_PLUGIN(SetProjectileRefFlag, , 1, 1, kParams_OneInt);
bool Cmd_SetProjectileRefFlag_Execute(COMMAND_ARGS)
{
	UInt32 flag;
	if (ExtractArgs(EXTRACT_ARGS, &flag))
		((Projectile*)thisObj)->projFlags = flag;

	//if ((((BGSProjectile*)thisObj->baseForm)->projFlags & 0x426) == 0x26);
	//	Console_Print("Flag was set");
	return true;
}



DEFINE_COMMAND_PLUGIN(SetPCCanPickpocketInCombat, , 0, 1, kParams_OneInt);
bool Cmd_SetPCCanPickpocketInCombat_Execute(COMMAND_ARGS)
{
	UInt32 bOn;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &bOn)) return true;
	g_canPlayerPickpocketInCombat = bOn;
	return true;
}


DEFINE_COMMAND_PLUGIN(SetNoEquip, "Returns 1 if the inventory ref was sucessfully set to NoEquip (or the flag was properly cleared). Returns 2 if it was already set.", 1, 1, kParams_OneInt);
bool Cmd_SetNoEquip_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 noEquip;
	if (ExtractArgs(EXTRACT_ARGS, &noEquip))
	{
		InventoryRef* invRef = InventoryRefGetForID(thisObj->refID);
		if (!invRef) return true;
		ExtraDataList* xData = invRef ? invRef->xData : NULL;
		if (xData)
		{
			if (!noEquip)
			{
				RemoveExtraType(xData, kExtraData_CannotWear);
				*result = 1;
			}
			else if (!xData->HasType(kExtraData_CannotWear) && !xData->HasType(kExtraData_Worn))  //I just replaced the "IsEquipped" extra data check from JIP's SetNoUnequip.
			{
				AddExtraData(xData, ExtraCannotWear::Create());
				*result = 1;
			}
			else if (xData->HasType(kExtraData_CannotWear))
				*result = 2;
		}
	} 
	return true;
}

DEFINE_COMMAND_PLUGIN(GetFastTravelFlags, , 0, 0, NULL);
bool Cmd_GetFastTravelFlags_Execute(COMMAND_ARGS)
{
	*result = (g_thePlayer->byte66D & 1) != 0;
	return true;
}

/*
DEFINE_COMMAND_PLUGIN(SetParentRef, , 1, 1, kParams_OneOptionalForm);
bool Cmd_SetParentRef_Execute(COMMAND_ARGS)
{
	ExtraDataList* xData = thisObj ? thisObj->xData : NULL;
	if (xData)
	{
		if (!noEquip)
		{
			RemoveExtraType(xData, kExtraData_CannotWear);
			*result = 1;
		}
		else if (!xData->HasType(kExtraData_CannotWear) && !xData->HasType(kExtraData_Worn))  //I just replaced the "IsEquipped" extra data check from JIP's SetNoUnequip.
		{
			AddExtraData(xData, ExtraCannotWear::Create());
			*result = 1;
		}
		else if (xData->HasType(kExtraData_CannotWear))
			*result = 2;
	}
	
	return true;
}
*/

//DEFINE_CMD_ALT_COND_PLUGIN(GetEquippedWeaponType,, "Returns the type of weapon equipped by the calling actor, 0 if the calling ref isn't an actor.", 0, 1, kParams_OneOptionalObjectID);
bool GetEquippedWeaponInfo(TESObjectREFR* thisObj, float range, UInt32 flags)
{
	return true;
}
bool Cmd_GetEquippedWeaponType_Eval(COMMAND_ARGS_EVAL)
{
	TESObjectWEAP* pWeapon;
	pWeapon->eWeaponType;
	//*result = 
	return true;
}
bool Cmd_GetEquippedWeaponType_Execute(COMMAND_ARGS)
{
	//*result = 
	return true;
}




#endif