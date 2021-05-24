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
				*result = 0; //error
				break;
			}
		}
	}

	return true;
}

DEFINE_COMMAND_PLUGIN(MessageExAltShowoff, , 0, 22, kParams_JIP_OneFloat_OneInt_OneFormatString);
const UInt32 kMsgIconsPathAddr[] = { 0x10208A0, 0x10208E0, 0x1025CDC, 0x1030E78, 0x103A830, 0x1049638, 0x104BFE8 };

//99% ripped from JIP's MessageExAlt.
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


//Code ripped from JIP's IsAnimPlayingEx
DEFINE_CMD_ALT_COND_PLUGIN(IsAnimPlayingExCond, , "Same as IsAnimPlayingEx, but available as a condition. Had to cut the variationFlags filter.", 1, kParams_JIP_OneInt_OneOptionalInt);
bool Cmd_IsAnimPlayingExCond_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	UInt32 category = (UInt32)arg1;
	UInt32 subType = (UInt32)arg2;  //optional

	if (!thisObj) return true;
	if (category > 5 || category < 1) return true;
	if (subType > 23) return true;

	AnimData* animData = thisObj->GetAnimData();
	if (!animData) return true;
	UInt32 animID;
	const AnimGroupClassify* classify;
	for (UInt16 groupID : animData->animGroupIDs)
	{
		animID = groupID & 0xFF;
		if (animID >= 245) continue;
		classify = &s_animGroupClassify[animID];
		if (classify->category == category && (category >= 4 || (!subType || classify->subType == subType)))
		{
			*result = 1;
			break;
		}
	}

	return true;
}
bool Cmd_IsAnimPlayingExCond_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 category, subType = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &category, &subType)) return true;

	return Cmd_IsAnimPlayingExCond_Eval(thisObj, (void*)category, (void*)subType, result);
}



#ifdef _DEBUG



DEFINE_COMMAND_PLUGIN(SetCellFullNameAlt, "Like SetCellFullName but accepts a string.", 0, 2, kParams_JIP_OneCell_OneString);
bool Cmd_SetCellFullNameAlt_Execute(COMMAND_ARGS)
{
	TESObjectCELL* cell;
	char newName[256]; 
	
	ExtractArgsEx(EXTRACT_ARGS_EX, &cell, &newName);
	if (!cell) return true;

	TESFullName* fullName = &cell->fullName;  //wtf. Seems to work, idk why
	ThisStdCall(0x489100, fullName, newName);  //rip, needs something more to save-bake
	//fullName->name.Set(newName); //crashes
	
	return true;
}

DEFINE_COMMAND_PLUGIN(GetCellFullName, , 0, 1, kParams_JIP_OneCell);
bool Cmd_GetCellFullName_Execute(COMMAND_ARGS)
{
	TESObjectCELL* cell;

	ExtractArgsEx(EXTRACT_ARGS_EX, &cell);
	if (!cell) return true;

	TESFullName* fullName = &cell->fullName;  //wtf. Seems to work, idk why
	const char* oldName = fullName->name.CStr();
	g_strInterface->Assign(PASS_COMMAND_ARGS, oldName);
	
	return true;
}


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