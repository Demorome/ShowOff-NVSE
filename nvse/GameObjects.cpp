#include "GameObjects.h"

#include "decoding.h"  // newly added
#include "GameRTTI.h"
#include "GameExtraData.h"
#include "GameTasks.h"
#include "GameUI.h"

ScriptEventList *TESObjectREFR::GetEventList() const
{
	ExtraScript *xScript = (ExtraScript*)extraDataList.GetByType(kExtraData_Script);
	return xScript ? xScript->eventList : NULL;
}

PlayerCharacter *PlayerCharacter::GetSingleton()
{
	return *(PlayerCharacter**)0x11DEA3C;
}

bool Actor::IsSneaking()
{
	Actor* actor = this;
	return (ThisStdCall<bool>(0x4997B0, actor));
}

__declspec(naked) TESContainer *TESObjectREFR::GetContainer()
{
	__asm
	{
		mov		eax, [ecx]
		mov		eax, [eax+0x100]
		call	eax
		test	al, al
		mov		eax, [ecx+0x20]
		jz		notActor
		add		eax, 0x64
		retn
	notActor:
		cmp		dword ptr [eax], kVtbl_TESObjectCONT
		jnz		notCONT
		add		eax, 0x30
		retn
	notCONT:
		xor		eax, eax
		retn
	}
}

bool TESObjectREFR::IsMapMarker()
{
	return baseForm->refID == 0x10;
}

bool PlayerCharacter::SetSkeletonPath(const char* newPath)
{
	if (!bThirdPerson) {
		// ###TODO: enable in first person
		return false;
	}

	//// store parent of current niNode
	//NiNode* niParent = (NiNode*)(renderState->niNode->m_parent);

	//// set niNode to NULL via BASE CLASS Set3D() method
	//ThisStdCall(s_TESObjectREFR_Set3D, this, NULL);

	//// modify model path
	//if (newPath) {
	//	TESNPC* base = DYNAMIC_CAST(baseForm, TESForm, TESNPC);
	//	base->model.SetPath(newPath);
	//}

	//// create new NiNode, add to parent
	//*(g_bUpdatePlayerModel) = 1;
	//NiNode* newNode = (NiNode*)ThisStdCall(s_PlayerCharacter_GenerateNiNode, this);

	//niParent->AddObject(newNode, 1);
	//*(g_bUpdatePlayerModel) = 0;
	//newNode->SetName("Player");

	//// get and store camera node
	//// ### TODO: pretty this up
	//UInt32 vtbl = *((UInt32*)newNode);
	//UInt32 vfunc = *((UInt32*)(vtbl + 0x58));
	//NiObject* cameraNode = (NiObject*)ThisStdCall(vfunc, newNode, "Camera01");
	//*g_3rdPersonCameraNode = cameraNode;

	//cameraNode = (NiObject*)ThisStdCall(vfunc, (NiNode*)this->firstPersonNiNode, "Camera01");
	//*g_1stPersonCameraNode = cameraNode;

	//Unk_52();

	return true;
}

void TESObjectREFR::Update3D()
{
	if (this == PlayerCharacter::GetSingleton())
		ThisStdCall(kUpdateAppearanceAddr, this);
	else
	{
		Set3D(NULL, true);
		ModelLoader::GetSingleton()->QueueReference(this, 1, 0);
	}
}

TESObjectREFR *TESObjectREFR::Create(bool bTemp)
{
	TESObjectREFR *refr = (TESObjectREFR*)GameHeapAlloc(sizeof(TESObjectREFR));
	ThisStdCall(s_TESObject_REFR_init, refr);
	if (bTemp) ThisStdCall(0x484490, refr);
	return refr;
}
/*
TESForm* GetPermanentBaseForm(TESObjectREFR* thisObj)	// For LevelledForm, find real baseForm, not temporary one.
{
	ExtraLeveledCreature * pXCreatureData = NULL;

	if (thisObj) {
		pXCreatureData = GetExtraType(thisObj->extraDataList, LeveledCreature);
		if (pXCreatureData && pXCreatureData->baseForm) {
			return pXCreatureData->baseForm;
		}
	}
	if (thisObj && thisObj->baseForm) {
		return thisObj->baseForm;
	}
	return NULL;
}

EquippedItemsList Actor::GetEquippedItems()
{
	EquippedItemsList itemList;
	ExtraContainerDataArray outEntryData;
	ExtraContainerExtendDataArray outExtendData;

	ExtraContainerChanges	* xChanges = static_cast <ExtraContainerChanges *>(extraDataList.GetByType(kExtraData_ContainerChanges));
	if(xChanges) {
		UInt32 count = xChanges->GetAllEquipped(outEntryData, outExtendData);
		for (UInt32 i = 0; i < count ; i++)
			itemList.push_back(outEntryData[i]->type);

	}

	return itemList;
}

ExtraContainerDataArray	Actor::GetEquippedEntryDataList()
{
	ExtraContainerDataArray itemArray;
	ExtraContainerExtendDataArray outExtendData;

	ExtraContainerChanges	* xChanges = static_cast <ExtraContainerChanges *>(extraDataList.GetByType(kExtraData_ContainerChanges));
	if(xChanges)
		xChanges->GetAllEquipped(itemArray, outExtendData);

	return itemArray;
}

ExtraContainerExtendDataArray	Actor::GetEquippedExtendDataList()
{
	ExtraContainerDataArray itemArray;
	ExtraContainerExtendDataArray outExtendData;

	ExtraContainerChanges	* xChanges = static_cast <ExtraContainerChanges *>(extraDataList.GetByType(kExtraData_ContainerChanges));
	if(xChanges)
		xChanges->GetAllEquipped(itemArray, outExtendData);

	return outExtendData;
}
*/


void Actor::SetAnimActionAndSequence(SInt32 animAction, BSAnimGroupSequence* animGroupSeq)
{
	ThisStdCall<void>(0x8A73E0, this, animAction, animGroupSeq);
}

// From JIP
__declspec(naked) UInt32 Actor::GetLevel()
{
	__asm
	{
		mov		ecx, [ecx + 0x20]
		add		ecx, 0x30
		CALL_EAX(0x47DED0)
		movzx	eax, ax
		retn
	}
}

bool Actor::IsInvisible()
{
	return (avOwner.GetActorValueInt(kAVCode_Invisibility) > 0) || (avOwner.GetActorValueInt(kAVCode_Chameleon) > 0);
}

void Actor::Kill(Actor* killer)
{
	ThisStdCall<void>(0x89D900, this, killer, 0.0f);
}

SInt32 Actor::GetDetectionLevelAlt(Actor* target, bool calculateSneakLevel)
{
	bool isTargetInCombat;
	if (target->IsPlayerRef())
		isTargetInCombat = ((PlayerCharacter*)target)->pcInCombat;
	else
		isTargetInCombat = target->isInCombat;

	bool out = false;  // throwaway variable to reference.
	
	// SInt32 __thiscall Actor_GetDetected(Actor *this, bool calculateSneakLevel, Actor *toDetect, int *out, char a5, bool isTargetInCombat, int a7, int a8)
	return ThisStdCall<SInt32>(0x8A0D10, this, calculateSneakLevel, target, &out, 0, isTargetInCombat, 0, 0);
}

// Same-ish code as what's used for Cmd_GetDetected_Eval
bool Actor::Detects(Actor* target)
{
	SInt32 const detectionLevel = this->GetDetectionLevelAlt(target, false);
	return detectionLevel > 0;
}

// From JIP
TESObjectREFR* TESObjectREFR::GetMerchantContainer()
{
	ExtraMerchantContainer* xMerchCont = GetExtraTypeJIP(&extraDataList, MerchantContainer);
	return xMerchCont ? xMerchCont->containerRef : NULL;
}

// from JIP
__declspec(naked) ContChangesEntry* TESObjectREFR::GetContainerChangesEntry(TESForm* itemForm) const
{
	__asm
	{
		push	kExtraData_ContainerChanges
		add		ecx, 0x44
		call	BaseExtraList::GetByType
		test	eax, eax
		jz		done
		mov		eax, [eax + 0xC]
		test	eax, eax
		jz		done
		mov		ecx, [eax]
		mov		edx, [esp + 4]
		ALIGN 16
		itemIter:
		test	ecx, ecx
			jz		retnNULL
			mov		eax, [ecx]
			mov		ecx, [ecx + 4]
			test	eax, eax
			jz		itemIter
			cmp[eax + 8], edx
			jnz		itemIter
			retn	4
			retnNULL:
		xor eax, eax
			done :
		retn	4
	}
}