#pragma once

DEFINE_COMMAND_ALT_PLUGIN(ListAddList, AddFormListToFormList, "", 0, 3, kParams_TwoFormLists_OneOptionalIndex);

//ripped code from FOSE's ListAddForm
bool Cmd_ListAddList_Execute(COMMAND_ARGS)
{
	*result = 1;
	BGSListForm* pListForm = NULL;
	BGSListForm* pToAppendList = NULL;
	UInt32 index = eListEnd;

	ExtractArgsEx(EXTRACT_ARGS_EX, &pListForm, &pToAppendList, &index);
	if (!pListForm || !pToAppendList) return true;

	for (tList<TESForm>::Iterator iter = pToAppendList->list.Begin(); !iter.End(); ++iter)
	{
		TESForm* form = iter.Get();
		if (form)
		{
			UInt32 const addedAtIndex = pListForm->AddAt(form, index);
			if (addedAtIndex == eListInvalid)
			{
				*result = -1;
				break;
			}
		}
	}

	return true;
}



const UInt32 kMsgIconsPathAddr[] = { 0x10208A0, 0x10208E0, 0x1025CDC, 0x1030E78, 0x103A830, 0x1049638, 0x104BFE8 };

//99% ripped from JIP's MessageExAlt.
DEFINE_COMMAND_PLUGIN(MessageExAltShowoff, , 0, 22, kParams_JIP_OneFloat_OneInt_OneFormatString);
bool Cmd_MessageExAltShowoff_Execute(COMMAND_ARGS)
{
	float displayTime;
	UINT32 appendToQueue;
	if (!ExtractFormatStringArgs(2, s_strArgBuffer, EXTRACT_ARGS_EX, kCommandInfo_MessageExAltShowoff.numParams, &displayTime, &appendToQueue))
		return true;

	char* msgPtr = GetNextTokenJIP(s_strArgBuffer, '|');
	msgPtr[0x203] = 0;
	if (*msgPtr)
	{
		if ((s_strArgBuffer[0] == '#') && (s_strArgBuffer[1] >= '0') && (s_strArgBuffer[1] <= '6') && !s_strArgBuffer[2])
			QueueUIMessage(msgPtr, 0, (const char*)kMsgIconsPathAddr[s_strArgBuffer[1] - '0'], NULL, displayTime, appendToQueue != 0);
		else QueueUIMessage(msgPtr, 0, s_strArgBuffer, NULL, displayTime, appendToQueue != 0);
	}
	else QueueUIMessage(s_strArgBuffer, 0, NULL, NULL, displayTime, appendToQueue != 0);

	return true;
}


DEFINE_CMD_ALT_COND_PLUGIN(IsCornerMessageDisplayed, , "Returns 1/0 depending on if a corner message is displayed.", 0, NULL);
bool Cmd_IsCornerMessageDisplayed_Execute(COMMAND_ARGS)
{
	*result = !g_HUDMainMenu->queuedMessages.Empty();  
	//*result = (bool)g_HUDMainMenu->currMsgKey;
	//another way to check. Seems to be a bit slower to update when initially adding a message to the queue.
	return true;
}
bool Cmd_IsCornerMessageDisplayed_Eval(COMMAND_ARGS_EVAL)
{
	*result = !g_HUDMainMenu->queuedMessages.Empty();
	return true;
}

DEFINE_CMD_ALT_COND_PLUGIN(GetNumQueuedCornerMessages, , , 0, NULL);
bool Cmd_GetNumQueuedCornerMessages_Execute(COMMAND_ARGS)
{
	*result = g_HUDMainMenu->queuedMessages.Count();
	return true;
}
bool Cmd_GetNumQueuedCornerMessages_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_HUDMainMenu->queuedMessages.Count();
	return true;
}

#ifdef _DEBUG

DEFINE_COMMAND_PLUGIN(GetQueuedCornerMessages, "Returns the queued corner messages as a multidimensional array.", 0, 0, NULL);
bool Cmd_GetQueuedCornerMessages_Execute(COMMAND_ARGS)
{
	NVSEArrayVar* msgArr = g_arrInterface->CreateArray(NULL, 0, scriptObj);

	for (UINT32 iIndex = g_HUDMainMenu->queuedMessages.Count() + 1; ; --iIndex)
	{
		if (iIndex == 0) break;
		g_HUDMainMenu->queuedMessages.GetNthItem(iIndex);
		//no idea what to do with this :snig:
	}

	g_arrInterface->AssignCommandResult(msgArr, result);
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

struct ActivateRefEntry  //LinkedRefEntry, no way this will work..
{
	UInt32		linkID;
	UInt8		modIdx;

	void Set(UInt32 _linkID, UInt8 _modIdx)
	{
		linkID = _linkID;
		modIdx = _modIdx;
	}
};
UnorderedMap<UInt32, ActivateRefEntry> s_activateRefModified;
UnorderedMap<UInt32, UInt32> s_activateRefDefault, s_activateRefsTemp;


DEFINE_COMMAND_PLUGIN(SetEnableParent, , 1, 1, kParams_OneOptionalForm);

//Stole some code from JIP (TESObjectREFR::SetLinkedRef)
bool Cmd_SetEnableParent_Execute(COMMAND_ARGS)
{
	//TESObjectREFR* newParent = NULL;
	//if (!ExtractArgsEx(EXTRACT_ARGS_EX, &newParent)) return true;

#if 0
	ExtraDataList xData = thisObj->extraDataList;
	ExtraActivateRef* xActivateRef = GetExtraType(xData, ActivateRef);

	RemoveExtraData(&xData, xActivateRef, true);
	if (!newParent)
	{
		//xActivateRef->parentRefs.RemoveAll();  //DANGER! MUST TEST
		auto findDefID = s_activateRefDefault.Find(thisObj->refID);
		if (findDefID)
		{
			if (xActivateRef)
			{
				if (*findDefID)
				{
					TESForm* form = LookupFormByRefID(*findDefID);
					if (form && form->GetIsReference()) xLinkedRef->linkedRef = (TESObjectREFR*)form;
				}
				else RemoveExtraData(xData, xLinkedRef, true);
			}
			findDefID.Remove();
		}
		s_activateRefModified.Erase(thisObj->refID);
		return true;
	}
	else
	{

	}

	if (!linkObj)
	{
		auto findDefID = s_activateRefDefault.Find(refID);
		if (findDefID)
		{
			if (xLinkedRef)
			{
				if (*findDefID)
				{
					TESForm* form = LookupFormByRefID(*findDefID);
					if (form && form->GetIsReference()) xLinkedRef->linkedRef = (TESObjectREFR*)form;
				}
				else RemoveExtraData(&extraDataList, xLinkedRef, true);
			}
			findDefID.Remove();
		}
		s_activateRefModified.Erase(refID);
		return true;
	}
#endif
	return true;
}

double g_TestDemoVar = 0;

void __fastcall TestDemoFunc(double *stuff)
{
	*stuff -= 1;
}

DEFINE_COMMAND_PLUGIN(TestDemo, , 0, 0, NULL);
bool Cmd_TestDemo_Execute(COMMAND_ARGS)
{
	g_TestDemoVar += 2;
	TestDemoFunc(&g_TestDemoVar);
	*result = g_TestDemoVar;
	return true;
}

#endif