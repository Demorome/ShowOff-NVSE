#pragma once

DEFINE_COMMAND_ALT_PLUGIN(DumpFormList, FListDump, "", false, kParams_OneFormList_OneOptionalString_OneOptionalInt);
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

DEFINE_CMD_COND_PLUGIN(ConditionPrint,
	"Returns 1, and prints a message to console. Meant for testing if previous conditions passed.",
	false, NULL);
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

DEFINE_COMMAND_PLUGIN(GetShowOffDebugMode, "", false, NULL);
bool Cmd_GetShowOffDebugMode_Execute(COMMAND_ARGS)
{
	*result = g_ShowFuncDebug;
	return true;
}
DEFINE_COMMAND_PLUGIN(SetShowOffDebugMode, "Set to 1 to enable debug prints for certain ShowOff functions.",
	false, kParams_OneInt);
bool Cmd_SetShowOffDebugMode_Execute(COMMAND_ARGS)
{
	UInt32 bOn = false;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &bOn)) return true;
	g_ShowFuncDebug = bOn;
	return true;
}

DEFINE_COMMAND_ALT_PLUGIN(IsBaseForm, IsBasedForm, "", false, kParams_OneForm);
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

DEFINE_COMMAND_PLUGIN(IsOutsideMainThread, "", false, NULL);
bool Cmd_IsOutsideMainThread_Execute(COMMAND_ARGS)
{
	*result = GetCurrentThreadId() != g_mainThreadID;
	return true;
}

DEFINE_COMMAND_PLUGIN(RefillPlayerAmmo, "", false, kParams_OneInt);
bool Cmd_RefillPlayerAmmo_Execute(COMMAND_ARGS)
{
	*result = 0; // bSuccess
	SInt32 count;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &count))
		return true;

	const auto weapInfo = g_thePlayer->baseProcess->GetWeaponInfo();
	if (!weapInfo)
		return true;
	const auto weap = static_cast<TESObjectWEAP*>(weapInfo->type);

	// Player reload code happens at 0x9497AA, copying that.
	const auto regenRate = ThisStdCall<double>(0x709430, weap, weapInfo->HasWeaponMod(TESObjectWEAP::kWeaponModEffect_RegenerateAmmo_Seconds));
	if (regenRate > 0)
	{
		if (IsConsoleMode())
			Console_Print("Cannot refill a weapon with an ammo regen per second!");
		return true;
	}

	const auto hasExtendedClip = weapInfo->HasWeaponMod(TESObjectWEAP::kWeaponModEffect_IncreaseClipCapacity);
	if (const auto ammoInfo = g_thePlayer->baseProcess->GetAmmoInfo())
	{
		g_thePlayer->AddItem(ammoInfo->type, nullptr, count);
		auto const clipMax = ThisStdCall<SInt32>(0x4FE160, weap, hasExtendedClip);
		auto const numLeftToAdd = clipMax - ammoInfo->countDelta;
		ammoInfo->countDelta += std::min(numLeftToAdd, count);
	}
	else if (const auto defaultAmmo = ThisStdCall<TESAmmo*>(0x474920, &weap->ammo))
	{
		g_thePlayer->AddItem(defaultAmmo, nullptr, count);
		// player was fully out of ammo; reload the weapon.
		g_thePlayer->Reload2(weap, 2, hasExtendedClip, false); // 0x95D3F0
	}
	else
	{
		if (IsConsoleMode())
			Console_Print("Unable to find default ammo for the weapon!");
		return true;
	}
	*result = 1;
	return true;
}

// Copied JIP's GetActorsByProcessingLevel code
DEFINE_COMMAND_PLUGIN(KillAllHostiles, "", false, kParams_OneOptionalActorRef);
bool Cmd_KillAllHostiles_Execute(COMMAND_ARGS)
{
	Actor* killer = nullptr;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &killer))
		return true;
	ProcessManager* procMngr = ProcessManager::GetSingleton();
	MobileObject** objArray = procMngr->objects.data, ** arrEnd = objArray;
	objArray += procMngr->beginOffsets[0];
	arrEnd += procMngr->endOffsets[0];
	for (; objArray != arrEnd; objArray++)
	{
		auto actor = static_cast<Actor*>(*objArray);
		if (actor && IS_ACTOR(actor))
		{
			if (actor->IsInCombatWith(g_thePlayer)
				|| actor->GetShouldAttack(g_thePlayer))
			{
				actor->Kill(killer);
			}
		}
	}
	return true;
}


// TODO
DEFINE_COMMAND_ALT_PLUGIN(SetFlyCamera, SetFC, "", false, kParams_OneInt_FiveOptionalFloats);
bool Cmd_SetFlyCamera_Execute(COMMAND_ARGS)
{
	enum Modes : SInt32
	{
		kMode_Off = 0,
		// These are the modes from TFC, offset by +1.
		kMode_SetFlyCamOn_Default,
		kMode_SetFlyCamOn_FreezeTime_SetUnkByteToTrue,
		kMode_SetFlyCamOn_FreezeTime_SetUnkByteToTrue_2, /*duplicate functionality*/
		kMode_SetFlyCamOn_SetUnkByteToTrue,

		// Does not enable flycam
		kMode_SetUnkByteToFalse,

		kMode_SetFlyCamOn_FreezeTime_SetUnkByteToTrue_3 /*duplicate functionality*/
	};



	Modes mode;
	/*
	double pos[3] = { g_thePlayer->posX, g_thePlayer->posY,
		g_thePlayer->posZ + (g_thePlayer->GetScaledHeight() * g_thePlayer->eyeHeight) };
	double rot[2] = { g_thePlayer->rotZ, g_thePlayer->rotX };
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &mode, &pos, &rot))
		return true;
	*/

	//g_thePlayer->cam

	return true;
}





#if _DEBUG


DEFINE_COMMAND_PLUGIN(Debug_UpdateWeather, "", false, NULL);
bool Cmd_Debug_UpdateWeather_Execute(COMMAND_ARGS)
{
	*result = false;
	if (auto const sky = Sky::GetSingleton())
	{
		ThisStdCall(0x63D1D0, sky);
		*result = true;
	}
	return true;
}

DEFINE_COMMAND_PLUGIN_EXP_SAFE(Debug_DispatchEvent, "", false, kNVSEParams_TwoNums_OneArray_OneStr_ThreeForms);
bool Cmd_Debug_DispatchEvent_Execute(COMMAND_ARGS)
{
	*result = false;
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		int num1;
		float num2;
		NVSEArrayVar* arr;
		const char* str;
		TESForm* anyForm;
		TESForm* reference;
		TESForm* baseForm;
		EXTRACT_ALL_ARGS_EXP(Debug_DispatchEvent, eval, std::tie(num1, num2, arr, str, anyForm, reference, baseForm),
			g_NoArgs);
		void* num2Formatted = *(void**)&num2;
		g_eventInterface->DispatchEvent("ShowOff:DebugEvent", g_thePlayer, num1, num2Formatted, arr, str, anyForm, reference, baseForm);
		*result = true;
	}
	return true;
}


#endif
