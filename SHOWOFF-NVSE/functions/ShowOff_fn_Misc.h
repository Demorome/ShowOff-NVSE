#pragma once

#include <unordered_set>

#include "GameRTTI.h"
#include "Utilities.h"


DEFINE_COMMAND_ALT_PLUGIN(ShowingOffDisable, ShowingOffDisableAltExCond2, "Does the same thing as vanilla Disable. For showing off!", 1, 1, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(ShowingOffEnable, ShowingOffEnableAltExCond2, "Does the same thing as vanilla Enable. For showing off!", 1, 1, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(ListAddList, AddFormListToFormList, "", 0, 3, kParams_TwoFormLists_OneOptionalIndex);
DEFINE_COMMAND_PLUGIN(MessageExAltShowoff, , 0, 22, kParams_JIP_OneFloat_OneInt_OneFormatString);
DEFINE_CMD_ALT_COND_PLUGIN(IsCornerMessageDisplayed, , "Returns 1/0 depending on if a corner message is displayed.", false, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(GetNumQueuedCornerMessages, , , false, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(IsAnimPlayingExCond, , "Same as IsAnimPlayingEx, but available as a condition. Had to cut the variationFlags filter.", true, kParams_JIP_OneInt_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(GetRadiationExtraData, , 1, 0, NULL);
DEFINE_COMMAND_PLUGIN(SetRadiationExtraData, , 1, 1, kParams_OneFloat);
DEFINE_CMD_ALT_COND_PLUGIN(PlayerHasNightVisionActive, , , false, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(PlayerIsUsingTurbo, , , false, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(PlayerHasCateyeEnabled, , , false, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(PlayerHasImprovedSpottingActive, , , false, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(PlayerIsDrinkingPlacedWater, , , false, NULL);
DEFINE_COMMAND_PLUGIN(SetIsPCAMurderer, , 0, 1, kParams_OneInt);
DEFINE_CMD_ALT_COND_PLUGIN(IsNight, , "Returns true if it's night according to the current (or specified) climate.", false, kParams_OneOptionalForm);
DEFINE_CMD_ALT_COND_PLUGIN(IsLimbCrippled, , "If no args are passed / arg is -1, returns true if actor has any crippled limbs. Otherwise, checks if the specified limb is crippled.", true, kParams_TwoOptionalInts);
DEFINE_CMD_ALT_COND_PLUGIN(GetNumCrippledLimbs, , , true, kParams_OneOptionalInt);
DEFINE_CMD_ALT_COND_PLUGIN(GetCrippledLimbsAsBitMask, , , true, kParams_OneOptionalInt);
DEFINE_CMD_ALT_COND_PLUGIN(GetNumBrokenEquippedItems, , , true, kParams_OneOptionalFloat_OneOptionalInt);
DEFINE_CMD_ALT_COND_PLUGIN(GetEquippedItemsAsBitMask, , , true, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(GetEquippedWeaponType, , , true, NULL);
DEFINE_COMMAND_PLUGIN(ClearShowoffSavedData, "", 0, 1, kParams_OneInt);
DEFINE_CMD_ALT_COND_PLUGIN(GetBaseEquippedWeight, , , true, kParams_OneOptionalFloat_OneOptionalInt);
DEFINE_CMD_ALT_COND_PLUGIN(GetCalculatedEquippedWeight, , "Accounts for perk effects + weapon mods.", true, kParams_OneOptionalFloat_OneOptionalInt);
DEFINE_CMD_ALT_COND_PLUGIN(GetCalculatedMaxCarryWeight, GetMaxCarryWeightPerkModified, "Accounts for GetMaxCarryWeight perk entry.", true, NULL);


bool(__cdecl* Cmd_Disable)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5C45E0;
bool(__cdecl* Cmd_Enable)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5C43D0;

bool Cmd_ShowingOffDisable_Execute(COMMAND_ARGS) {
	return Cmd_Disable(PASS_COMMAND_ARGS);
}

bool Cmd_ShowingOffEnable_Execute(COMMAND_ARGS) {
	return Cmd_Enable(PASS_COMMAND_ARGS);
}




//ripped code from FOSE's ListAddForm
bool Cmd_ListAddList_Execute(COMMAND_ARGS)
{
	*result = 1;
	BGSListForm* pListForm = nullptr;
	BGSListForm* pToAppendList = nullptr;
	UInt32 addAtIndex = eListEnd;

	ExtractArgsEx(EXTRACT_ARGS_EX, &pListForm, &pToAppendList, &addAtIndex);
	if (!pListForm || !pToAppendList) return true;

	auto Try_Adding_Form_From_FormList = [&](TESForm* form)
	{
		UInt32 const addedAtIndex = pListForm->AddAt(form, addAtIndex);
		if (addedAtIndex == eListInvalid)
		{
			*result = 0; //error
			return false;
		}
		return true;
	};

	// Need to append elements to our own array in order to iterate backwards, to append in descending key order.
	std::vector<TESForm*> formArr;
	
	for (tList<TESForm>::Iterator iter = pToAppendList->list.Begin(); !iter.End(); ++iter)
	{
		TESForm* form = iter.Get();
		if (form)
		{
			if (addAtIndex != eListEnd)
			{
				formArr.push_back(form);
			}
			else
			{
				if (!Try_Adding_Form_From_FormList(form)) break;
			}
		}
		else
		{
			_ERROR("ListAddList - found invalid form in formlist.");
			*result = 0;
			break;
		}
	}

	if (!formArr.empty())
	{
		for (int i = formArr.size() - 1; i >= 0; i--)
		{
			if (!Try_Adding_Form_From_FormList(formArr[i])) break;
		}
	}

	return true;
}

const UInt32 kMsgIconsPathAddr[] = { 0x10208A0, 0x10208E0, 0x1025CDC, 0x1030E78, 0x103A830, 0x1049638, 0x104BFE8 };

//99% ripped from JIP's MessageExAlt.
bool Cmd_MessageExAltShowoff_Execute(COMMAND_ARGS)
{
	float displayTime;
	UINT32 appendToQueue;
	char* buffer = GetStrArgBuffer();
	if (!ExtractFormatStringArgs(2, buffer, EXTRACT_ARGS_EX, kCommandInfo_MessageExAltShowoff.numParams, &displayTime, &appendToQueue))
		return true;

	char* msgPtr = GetNextTokenJIP(buffer, '|');
	msgPtr[0x203] = 0;
	if (*msgPtr)
	{
		if ((buffer[0] == '#') && (buffer[1] >= '0') && (buffer[1] <= '6') && !buffer[2])
			QueueUIMessage(msgPtr, 0, (const char*)kMsgIconsPathAddr[buffer[1] - '0'], NULL, displayTime, appendToQueue != 0);
		else QueueUIMessage(msgPtr, 0, buffer, NULL, displayTime, appendToQueue != 0);
	}
	else QueueUIMessage(buffer, 0, NULL, NULL, displayTime, appendToQueue != 0);

	return true;
}

// Inspired by JIP's "ClearMessageQueue"
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
bool Cmd_IsAnimPlayingExCond_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	auto const category = (UInt32)arg1;
	auto const subType = (UInt32)arg2;  //optional

	if (!thisObj) return true;
	if (category > 5 || category < 1) return true;
	if (subType > 23) return true;

	AnimData* animData = thisObj->GetAnimData();
	if (!animData) return true;
	const AnimGroupClassify* classify;
	for (UInt16 groupID : animData->animGroupIDs)
	{
		UInt32 const animID = groupID & 0xFF;
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

bool Cmd_GetRadiationExtraData_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (thisObj)
	{
		ExtraRadiation* xRadius = GetExtraTypeJIP(&thisObj->extraDataList, Radiation);
		if (xRadius) *result = xRadius->radiation;
	}
	return true;
}
bool Cmd_SetRadiationExtraData_Execute(COMMAND_ARGS)
{
	float fNewVal = 0;
	*result = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &fNewVal)) return true;
	if (thisObj)
	{
		ThisStdCall(0x422350, &thisObj->extraDataList, fNewVal);  //credits to c6 for pointing this address out.
		*result = 1;
	}
	return true;
}


bool Cmd_PlayerHasNightVisionActive_Execute(COMMAND_ARGS)
{
	*result = g_thePlayer->hasNightVisionApplied;
	return true;
}
bool Cmd_PlayerHasNightVisionActive_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_thePlayer->hasNightVisionApplied;
	return true;
}


bool Cmd_PlayerIsUsingTurbo_Execute(COMMAND_ARGS)
{
	*result = g_thePlayer->isUsingTurbo;  
	return true;
}
bool Cmd_PlayerIsUsingTurbo_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_thePlayer->isUsingTurbo;
	return true;
}

bool Cmd_PlayerHasCateyeEnabled_Execute(COMMAND_ARGS)
{
	*result = g_thePlayer->isCateyeEnabled;
	return true;
}
bool Cmd_PlayerHasCateyeEnabled_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_thePlayer->isCateyeEnabled;
	return true;
}

bool Cmd_PlayerHasImprovedSpottingActive_Execute(COMMAND_ARGS)
{
	*result = g_thePlayer->isSpottingImprovedActive;
	return true;
}
bool Cmd_PlayerHasImprovedSpottingActive_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_thePlayer->isSpottingImprovedActive;
	return true;
}

bool Cmd_PlayerIsDrinkingPlacedWater_Execute(COMMAND_ARGS)
{
	*result = g_thePlayer->isDrinkingPlacedWater;
	return true;
}
bool Cmd_PlayerIsDrinkingPlacedWater_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_thePlayer->isDrinkingPlacedWater;
	return true;
}

bool Cmd_SetIsPCAMurderer_Execute(COMMAND_ARGS)
{
	UInt32 bIsMurderer = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &bIsMurderer)) return true;
	g_thePlayer->bIsAMurderer = (bIsMurderer != 0);
	return true;
}


bool Cmd_IsNight_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	TESClimate* climate = (TESClimate*)arg1;
	Sky* sky = *g_currentSky;
	float const gameHour = ThisStdCall<double>(0x966A20, sky);
	float sunrise, sunset;
	if (climate && IS_TYPE(climate, TESClimate))
	{
		sunrise = ThisStdCall<UInt8>(0x595F10, climate, 1) / 6.0F;  //sunrise begin sprinkled with adjustments. 
		sunset = ThisStdCall<UInt8>(0x595F10, climate, 2) / 6.0F;  //Second arg determines which type of time to check.
	}
	else
	{
		sunrise = ThisStdCall<double>(0x595F50, sky);
		sunset = ThisStdCall<double>(0x595FC0, sky);
	}
	if (sunset <= gameHour || (sunrise >= gameHour))
		*result = 1;
	return true;
}
bool Cmd_IsNight_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESClimate* climate = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &climate)) return true;
	return Cmd_IsNight_Eval(0, climate, 0, result);
}

//credits to JIP LN's "CrippleLimb" for much of the structure.
bool Cmd_IsLimbCrippled_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!IS_ACTOR(thisObj)) return true;
	Actor* const actor = (Actor*)thisObj;
	UInt32 limbID = (UInt32)arg1;
	UInt32 const threshold = (UInt32)arg2;
	if (limbID == -1)
	{
		//loop through all limb health AVs and break if a single one is at 0 health (or below threshold).
		for (limbID = kAVCode_PerceptionCondition; limbID <= kAVCode_BrainCondition; limbID++)
		{
			if (actor->avOwner.GetActorValue(limbID) <= threshold)
			{
				*result = 1;
				break;
			}
		}
	}
	else if (limbID <= 6)
	{
		limbID += kAVCode_PerceptionCondition;
		if (actor->avOwner.GetActorValue(limbID) <= threshold) 
			*result = 1;
	}
	return true;
}
bool Cmd_IsLimbCrippled_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 limbID = -1;
	UInt32 threshold = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &limbID, &threshold)) return true;
	return Cmd_IsLimbCrippled_Eval(thisObj, (void*)limbID, (void*)threshold, result);
}

bool Cmd_GetNumCrippledLimbs_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!IS_ACTOR(thisObj)) return true;
	Actor* const actor = (Actor*)thisObj;
	UInt32 const threshold = (UInt32)arg1;
	UInt32 numCrippledLimbs = 0;
	for (UInt32 limbID = kAVCode_PerceptionCondition; limbID <= kAVCode_BrainCondition; limbID++)
	{
		if (actor->avOwner.GetActorValue(limbID) <= threshold)
		{
			numCrippledLimbs++;
		}
	}
	*result = numCrippledLimbs;
	return true;
}
bool Cmd_GetNumCrippledLimbs_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 threshold = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &threshold)) return true;
	return Cmd_GetNumCrippledLimbs_Eval(thisObj, (void*)threshold, 0, result);
}


bool Cmd_GetCrippledLimbsAsBitMask_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!IS_ACTOR(thisObj)) return true;
	Actor* const actor = (Actor*)thisObj;
	UInt32 const threshold = (UInt32)arg1;

	enum LimbGoneFlags
	{
		kFlag_Head = 1,
		kFlag_Torso = 2,
		kFlag_LeftArm = 4,
		kFlag_RightArm = 8,
		kFlag_LeftLeg = 0x10,
		kFlag_RightLeg = 0x20,
		kFlag_Brain = 0x40,
	};
	UInt32 flagsArr[7] = { kFlag_Head, kFlag_Torso, kFlag_LeftArm, kFlag_RightArm, kFlag_LeftLeg, kFlag_RightLeg, kFlag_Brain };

	UInt32 CrippledLimbsMask = 0;
	for (UInt32 limbID = kAVCode_PerceptionCondition; limbID <= kAVCode_BrainCondition; limbID++)
	{
		if (actor->avOwner.GetActorValue(limbID) <= threshold)
		{
			CrippledLimbsMask |= flagsArr[limbID - kAVCode_PerceptionCondition];
		}
	}
	*result = CrippledLimbsMask;
	return true;
}
bool Cmd_GetCrippledLimbsAsBitMask_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 threshold = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &threshold)) return true;
	return Cmd_GetCrippledLimbsAsBitMask_Eval(thisObj, (void*)threshold, 0, result);
}


// Get Equipped Object utility functions, from FOSE.
class MatchBySlot : public FormMatcher
{
	UInt32 m_slotMask;
public:
	MatchBySlot(UInt32 slot) : m_slotMask(TESBipedModelForm::MaskForSlot(slot)) {}
	bool Matches(TESForm* pForm) const {
		UInt32 formMask = 0;
		if (pForm) {
			if (IS_TYPE(pForm, TESObjectWEAP)) {
				formMask = TESBipedModelForm::eSlot_Weapon;
			}
			else {
				TESBipedModelForm* pBip = DYNAMIC_CAST(pForm, TESForm, TESBipedModelForm);
				if (pBip) {
					formMask = pBip->partMask;
				}
			}
		}
		return (formMask & m_slotMask) != 0;
	}
};

class MatchBySlotMask : public FormMatcher
{
	UInt32 m_targetMask;
	UInt32 m_targetData;
public:
	MatchBySlotMask(UInt32 targetMask, UInt32 targetData) : m_targetMask(targetMask) {}
	bool Matches(TESForm* pForm) const {
		UInt32 slotMask = 0;
		if (pForm) {
			if (IS_TYPE(pForm, TESObjectWEAP)) {
				slotMask = TESBipedModelForm::ePart_Weapon;
			}
			else {
				TESBipedModelForm* pBip = DYNAMIC_CAST(pForm, TESForm, TESBipedModelForm);
				if (pBip) {
					slotMask = pBip->partMask;
				}
			}
		}
		return ((slotMask & m_targetMask) == m_targetData);
	}
};

static EquipData FindEquipped(TESObjectREFR* thisObj, FormMatcher& matcher) {
	ExtraContainerChanges* pContainerChanges = static_cast<ExtraContainerChanges*>(thisObj->extraDataList.GetByType(kExtraData_ContainerChanges));
	return (pContainerChanges) ? pContainerChanges->FindEquipped(matcher) : EquipData();
}

typedef TESBipedModelForm::EPartBit EquippedItemIndex;
typedef TESBipedModelForm::ESlot EquippedItemSlot;

// https://stackoverflow.com/questions/50888127/how-can-i-use-an-unordered-set-with-a-custom-struct
class EquipDataHashFunction {
public:
	// id is returned as hash function
	size_t operator()(const EquipData& t) const
	{
		return (size_t)t.pForm;
	}
};

std::unordered_set<EquipData, EquipDataHashFunction> GetEquippedItemsData(TESObjectREFR* const thisObj, UInt32 const flags)
{
	enum FlagValues {
		// eSlot_ flags determine which slots to include.
		eSlot_Head = 0x1 << 0,
		eSlot_Hair = 0x1 << 1,
		eSlot_UpperBody = 0x1 << 2,
		eSlot_LeftHand = 0x1 << 3,
		eSlot_RightHand = 0x1 << 4,
		eSlot_Weapon = 0x1 << 5,
		eSlot_PipBoy = 0x1 << 6,
		eSlot_Backpack = 0x1 << 7,
		eSlot_Necklace = 0x1 << 8,
		eSlot_Headband = 0x1 << 9,
		eSlot_Hat = 0x1 << 10,
		eSlot_Eyeglasses = 0x1 << 11,
		eSlot_Nosering = 0x1 << 12,
		eSlot_Earrings = 0x1 << 13,
		eSlot_Mask = 0x1 << 14,
		eSlot_Choker = 0x1 << 15,
		eSlot_MouthObject = 0x1 << 16,
		eSlot_BodyAddon1 = 0x1 << 17,
		eSlot_BodyAddon2 = 0x1 << 18,
		eSlot_BodyAddon3 = 0x1 << 19,

		// iFilter_ flags determine what sort of item to include.
		iFilter_NoUnplayable = 0x1 << 20,
		iFilter_NoQuestItems = 0x1 << 21,
	};
	bool const skipUnplayable = (flags & iFilter_NoUnplayable) || (!flags);
	bool const noQuestItems = flags & iFilter_NoQuestItems;
	
	// Use a Set so as to avoid having duplicate EquipData
	// (since forms can occupy multiple equip slots).
	std::unordered_set<EquipData, EquipDataHashFunction> eqItems;
	for (UInt32 slotIdx = EquippedItemIndex::ePart_Head; slotIdx <= EquippedItemIndex::ePart_BodyAddon3; slotIdx++)
	{
		if (flags != 0)  //if flags == 0 (default), every equip slot is checked.
		{
			//Otherwise, skip if flag is not toggled on for item.
			if (flags && !(TESBipedModelForm::MaskForSlot(slotIdx) & flags)) continue;
		}
		MatchBySlot matcher(slotIdx);
		EquipData equipD = FindEquipped(thisObj, matcher);
		TESForm* form = equipD.pForm;
		if (!form) continue;
		if (skipUnplayable && !IsFormPlayable(form)) continue;
		if (noQuestItems && form->IsQuestItem()) continue;
		if (g_ShowFuncDebug)
			Console_Print("GetEquippedItemsData - SlotIdx: %u Form: [%08X] (%s)", slotIdx, equipD.pForm, equipD.pForm->GetName());
		eqItems.emplace(equipD);
	}
	return eqItems;
}

#if 0  // probably faster, but ValidBip01Names doesn't give access to ExtraData directly...
std::unordered_set<EquipData, EquipDataHashFunction> GetEquippedItemsDataAlt(TESObjectREFR* const thisObj, UInt32 const eqSlotFlags)
{
	// Use a set so as to avoid having duplicate EquipData
	// (since forms can occupy multiple equip slots).
	std::unordered_set<EquipData, EquipDataHashFunction> eqItems;
	if (!thisObj->IsCharacter()) return eqItems;
	if (ValidBip01Names* validBip = ((Character*)thisObj)->GetValidBip01Names()) 
	{
		for (ValidBip01Names::Data& slotData : validBip->slotData)
		{
			TESObjectARMO* armor = slotData.armor;
			if (armor)  // need to check for type too!
			{
#if _DEBUG
				Console_Print("GetEquippedItemsDataAlt - Partmask: %u Form: [%08X] (%s)", armor->bipedModel.partMask, armor, armor->GetName());
#endif
			}
		}
	}
	return eqItems;
}
#endif

UInt32 __fastcall GetNumBrokenEquippedItems_Call(TESObjectREFR* const thisObj, float threshold, UInt32 const flags)
{
	if (!IS_ACTOR(thisObj)) return 0;
	threshold /= 100.0F;  //expecting a number like 35, reduce to 0.35
	UInt32 numBrokenItems = 0;  //return value.
	auto eqItems = GetEquippedItemsData(thisObj, flags);
	for (auto const &iter : eqItems)
	{
		if (!iter.pForm) continue;
		if (g_ShowFuncDebug)
			Console_Print("GetNumBrokenEquippedItems - iter form: [%08X] (%s)", iter.pForm, iter.pForm->GetName());
		ExtraHealth* pXHealth = iter.pExtraData ? (ExtraHealth*)iter.pExtraData->GetByType(kExtraData_Health) : NULL; // modified health
		auto pHealth = DYNAMIC_CAST(iter.pForm, TESForm, TESHealthForm);  // base health 
		if (pXHealth && pHealth)  // If there's no pXHealth, it's at 100% health (no modified health extra data).
		{
			if ((pXHealth->health / (float)pHealth->health) <= threshold) numBrokenItems++;
			if (g_ShowFuncDebug)
				Console_Print("GetNumBrokenEquippedItems - health %% check being performed on %s. %%: %f vs %f threshold", iter.pForm->GetName(), (pXHealth->health / (float)pHealth->health), threshold);
		}
		else if (pHealth && threshold >= 1.0F) numBrokenItems++;
	}
	Console_Print("GetNumBrokenEquippedItems >> %u", numBrokenItems);
	return numBrokenItems;
}

bool Cmd_GetNumBrokenEquippedItems_Eval(COMMAND_ARGS_EVAL)
{
	float const threshold = *(float*)&arg1;
	auto const flags = (UInt32)arg2;
	*result = GetNumBrokenEquippedItems_Call(thisObj, threshold, flags);
	return true; 
}
bool Cmd_GetNumBrokenEquippedItems_Execute(COMMAND_ARGS)
{
	*result = 0;
	float threshold = 0;
	UInt32 flags = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &threshold, &flags)) return true;
	*result = GetNumBrokenEquippedItems_Call(thisObj, threshold, flags);
	return true;
}


bool Cmd_GetEquippedItemsAsBitMask_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!IS_ACTOR(thisObj)) return true;
	UInt32 flags = 0;  //return value.
	for (UInt32 slotIdx = EquippedItemIndex::ePart_Head; slotIdx <= EquippedItemIndex::ePart_BodyAddon3; slotIdx++)
	{
		MatchBySlot matcher(slotIdx);
		EquipData equipD = FindEquipped(thisObj, matcher);
		if (equipD.pForm)
		{
			if (g_ShowFuncDebug)
				Console_Print("GetEquippedItemsAsBitMask - Form: %s, mask to add: %u", equipD.pForm->GetName(), TESBipedModelForm::MaskForSlot(slotIdx));
			flags |= TESBipedModelForm::MaskForSlot(slotIdx);
		}
	}
	*result = flags;
	Console_Print("GetEquippedItemsAsBitMask >> %u", flags);
	return true;
}
bool Cmd_GetEquippedItemsAsBitMask_Execute(COMMAND_ARGS)
{
	return Cmd_GetEquippedItemsAsBitMask_Eval(thisObj, 0, 0, result);
}


#if 0
DEFINE_COMMAND_PLUGIN(UnequipItemsFromBitMask, , 1, 1, kParams_OneInt);
bool Cmd_UnequipItemsFromBitMask_Execute(COMMAND_ARGS)
{
	UInt32 flags = 0, noEquip = 0, noMessage = 1;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &flags, &noEquip, &noMessage)) return true;
	if (!IS_ACTOR(thisObj)) return true;
	Actor* actor = (Actor*)thisObj;
	for (UInt32 slotIdx = EquippedItemIndex::ePart_Head; slotIdx <= EquippedItemIndex::ePart_BodyAddon3; slotIdx++)
	{
		MatchBySlot matcher(slotIdx);
		EquipData equipD = FindEquipped(thisObj, matcher);
		if (equipD.pForm)
		{
			if (flags & TESBipedModelForm::MaskForSlot(slotIdx))
			{
				//Unequip item if its matching flag is set.
				actor->UnequipItem(equipD.pForm, 1, xData, 1, noEquip != 0, noMessage != 0);
			}
		}
	}
	return true;
}
#endif

bool Cmd_GetEquippedWeaponType_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!IS_ACTOR(thisObj)) return true;
	TESObjectWEAP* weapon = ((Actor*)thisObj)->GetEquippedWeapon();
	if (weapon)
		*result = weapon->eWeaponType;
	return true;
}
bool Cmd_GetEquippedWeaponType_Execute(COMMAND_ARGS)
{
	return Cmd_GetEquippedWeaponType_Eval(thisObj, 0, 0, result);
}

// Copied ClearJIPSavedData
bool Cmd_ClearShowoffSavedData_Execute(COMMAND_ARGS)
{
	UInt32 auxStringMaps;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &auxStringMaps)) return true;
	UInt8 modIdx = scriptObj->GetOverridingModIdx();
	if (auxStringMaps && s_auxStringMapArraysPerm.Erase((auxStringMaps == 2) ? 0xFF : modIdx))
		s_dataChangedFlags |= kChangedFlag_AuxStringMaps;
	return true;
}


float __fastcall GetBaseEquippedWeight_Call(TESObjectREFR* const thisObj, UInt32 const flags, float const minWeight)
{
	float totalWeight = 0;  //return val.
	auto eqItems = GetEquippedItemsData(thisObj, flags);
	for (auto const &iter : eqItems)
	{
		if (!iter.pForm) continue;
		auto pWeight = DYNAMIC_CAST(iter.pForm, TESForm, TESWeightForm);
		if (pWeight)
		{
			if (pWeight->weight >= minWeight)
			{
				totalWeight += pWeight->weight;
			}
		}
	}
	Console_Print("GetBaseEquippedWeight >> %f", totalWeight);
	return totalWeight;
}

bool Cmd_GetBaseEquippedWeight_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!IS_ACTOR(thisObj)) return true;
	float const minWeight = *(float*)&arg1;
	UInt32 const flags = (UInt32)arg2;
	*result = GetBaseEquippedWeight_Call(thisObj, flags, minWeight);
	return true;
}
bool Cmd_GetBaseEquippedWeight_Execute(COMMAND_ARGS)
{
	*result = 0;
	float minWeight = 0;
	UInt32 flags = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &minWeight, &flags)) return true;
	*result = GetBaseEquippedWeight_Call(thisObj, flags, minWeight);
	return true;
}

// Code structure lifted from 0x4D0900 (GetInventoryWeight)
float __fastcall GetCalculatedEquippedWeight_Call(TESObjectREFR* const thisObj, UInt32 const flags, float const minWeight)
{
	float totalWeight = 0;  //return val.
	bool isHardcore = g_thePlayer->isHardcore;
	if (!thisObj || !((Actor*)thisObj)->baseProcess) return 0.0F;
	auto eqItems = GetEquippedItemsData(thisObj, flags);
	for (auto const& iter : eqItems)
	{
		TESForm* item = iter.pForm;
		if (!item) continue;
		float itemWeight = 0;
		
		bool const isWeapon = IS_TYPE(item, TESObjectWEAP);
		ContChangesEntry* weapInfo = nullptr;
		if (isWeapon) {
			// Gather more information about the weapon reference (weapon mods).
			if (!(weapInfo = ((Actor*)thisObj)->baseProcess->GetWeaponInfo())) continue;
			if (item = weapInfo->type) {
				bool const hasDecreaseWeightMod = ThisStdCall<bool>(0x4BDA70, weapInfo, TESObjectWEAP::kWeaponModEffect_DecreaseWeight);
				itemWeight = ThisStdCall<double>(0x4BE380, (TESObjectWEAP*)item, hasDecreaseWeightMod);  //GetWeaponModdedWeight
				if (itemWeight >= 10.0) {
					float heavyWeaponWeightMult = 1.0;
					ApplyPerkModifiers(kPerkEntry_AdjustHeavyWeaponWeight, (Actor*)g_thePlayer, &heavyWeaponWeightMult);
					itemWeight = itemWeight * heavyWeaponWeightMult;
				}
			}
		}
		else {
			itemWeight = CdeclCall<double>(0x48EBC0, item, isHardcore);  // GetItemWeight. isHardcore check only affects ammo, but whatever.
		}

		if (itemWeight > 0.0F)
		{
			float hasPackRatFlt = 0.0;
			ApplyPerkModifiers(kPerkEntry_ModifyLightItems, (Actor*)thisObj, &hasPackRatFlt);
			if (hasPackRatFlt > 0.0)
			{
				float const fPackRatThreshold = *(float*)(0x11C6478 + 4);
				float const fPackRatModifier = *(float*)(0x11C64A8 + 4);
				if (fPackRatThreshold >= (double)itemWeight)
					itemWeight = itemWeight * fPackRatModifier;
			}
#if 0		// todo: figure out wtf 0x4D0D83 does.
			if (isWeapon && weapInfo)
			{
				//NOTE: Game does some weird jank to account for multiple DecreaseWeight effects.
				//I just really have no idea what's going on at 0x4D0D83 .
				bool const hasDecreaseWeightEffect = ThisStdCall<bool>(0x4BDA70, weapInfo, TESObjectWEAP::kWeaponModEffect_DecreaseWeight);
				if (hasDecreaseWeightEffect) {
					itemWeight *= ThisStdCall<float>(0x4BCF60, weapInfo->type, TESObjectWEAP::kWeaponModEffect_DecreaseWeight, 1);
				}
			}
#endif
			if (g_ShowFuncDebug)
				Console_Print("GetCalculatedEquippedWeight - Item: %s, Calculated Weight: %f", item->GetName(), itemWeight);

			if (itemWeight >= minWeight) 
				totalWeight += itemWeight;
		}
	}
	Console_Print("GetCalculatedEquippedWeight >> %f", totalWeight);
	return totalWeight;
}

bool Cmd_GetCalculatedEquippedWeight_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!IS_ACTOR(thisObj)) return true;
	float const minWeight = *(float*)&arg1;
	UInt32 const flags = (UInt32)arg2;
	*result = GetCalculatedEquippedWeight_Call(thisObj, flags, minWeight);
	return true;
}
bool Cmd_GetCalculatedEquippedWeight_Execute(COMMAND_ARGS)
{
	*result = 0;
	float minWeight = 0;
	UInt32 flags = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &minWeight, &flags)) return true;
	*result = GetCalculatedEquippedWeight_Call(thisObj, flags, minWeight);
	return true;
}


bool Cmd_GetCalculatedMaxCarryWeight_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!IS_ACTOR(thisObj)) return true;
	*result = ThisStdCall<double>(0x8A0C20, (Actor*)thisObj);
	return true;
}
bool Cmd_GetCalculatedMaxCarryWeight_Execute(COMMAND_ARGS)
{
	return Cmd_GetCalculatedMaxCarryWeight_Eval(thisObj, 0, 0, result);
}


#if _DEBUG


DEFINE_CMD_ALT_COND_PLUGIN(IsReferenceCloned, IsRefrCloned, "Checks if the reference's modIndex is 0xFF", 1, NULL);
bool Cmd_IsReferenceCloned_Execute(COMMAND_ARGS)
{
	//*result = thisObj->IsTemporary();  //todo: figure out what the IsTemporary flag does, cuz it doesn't determine 0xFF stuff.
	*result = thisObj->IsCloned();
	return true;
}
bool Cmd_IsReferenceCloned_Eval(COMMAND_ARGS_EVAL)
{
	*result = thisObj->IsCloned();
	return true;
}


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

// Takes from JIP's ClearMessageQueue.
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

// Takes from JIP's "ModBaseAV"
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
	actorBase->ModActorValue(actorVal, (valueToSet - currentValue));
	return true;
}

DEFINE_COMMAND_PLUGIN(GetItemRefValue, , 1, 0, NULL);
bool Cmd_GetItemRefValue_Execute(COMMAND_ARGS)
{
	*result = -1;
	if (thisObj)
	{
		ContChangesEntry tempEntry(NULL, 1, thisObj->baseForm);  //copying after GetCalculatedWeaponDamage from JIP, thx c6.
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

// Does whatever I want it to at the time.
DEFINE_COMMAND_PLUGIN(TestDemo, , 0, 0, NULL);
bool Cmd_TestDemo_Execute(COMMAND_ARGS)
{
	UInt32 bInt = 0;
	//if (!ExtractArgsEx(EXTRACT_ARGS_EX)) return true;
	*result = g_thePlayer->hasNightVisionApplied;
	return true;
}

#endif