#include "GameObjects.h"

#include "decoding.h"  // newly added
#include "GameRTTI.h"
#include "GameExtraData.h"
#include "GameTasks.h"
#include "GameUI.h"
#include "SafeWrite.h"
#include "GameProcess.h"

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

InventoryChanges* TESObjectREFR::GetInventoryChanges()
{
	return CdeclCall<InventoryChanges*>(0x4BF220, this);
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

void PlayerCharacter::UpdateCamera(bool isCalledFromFunc21, bool _zero_skipUpdateLOD)
{
	ThisStdCall(0x94AE40, this, (UInt8)isCalledFromFunc21, (UInt8)_zero_skipUpdateLOD);
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

/*
// Code taken from JIP LN NVSE
__declspec(naked) NiAVObject* __fastcall TESObjectREFR::GetNiBlock2(const char* blockName) const
{
	__asm
	{
		mov		eax, [ecx + 0x64]
		test	eax, eax
		jz		done
		mov		eax, [eax + 0x14]
		test	eax, eax
		jz		done
		cmp[edx], 0
		jz		done
		mov		ecx, eax
		call	NiNode::GetBlock
	done :
		retn
	}
}

// Code taken from JIP LN NVSE
__declspec(naked) hkpRigidBody* __fastcall TESObjectREFR::GetRigidBody(const char* blockName) const
{
	__asm
	{
		call	TESObjectREFR::GetNiBlock2
		test	eax, eax
		jz		done
		mov		eax, [eax + 0x1C]
		test	eax, eax
		jz		done
		mov		eax, [eax + 0x10]
		test	eax, eax
		jz		done
		mov		eax, [eax + 8]
		xor edx, edx
		cmp		byte ptr[eax + 0x28], 1
		cmovnz	eax, edx
		done :
		retn
	}
}*/

// Copied from JohnnyGuitar NVSE
hkpRigidBody* TESObjectREFR::GetRigidBody(const char* nodeName) const
{
	if (NiNode* rootNode = GetNiNode())
	{
		NiNode* targetNode = (nodeName[0]) ? rootNode->GetNode(nodeName) : rootNode;
		if (targetNode && targetNode->m_collisionObject) 
		{
			if (bhkWorldObject* hWorldObj = targetNode->m_collisionObject->worldObj)
			{
				hkpRigidBody* rigidBody = (hkpRigidBody*)hWorldObj->refObject;
				UInt8 motionType = rigidBody->motion.type;
				if ((motionType == 2) || (motionType == 3) || (motionType == 6))
					return rigidBody;
			}
		}
	}
	return nullptr;
}

NiNode* TESObjectREFR::Get3D() const
{
	return ThisStdCall<NiNode*>(0x43FCD0, const_cast<TESObjectREFR*>(this));
}

TESObjectREFR* TESObjectREFR::PlaceAtMe(TESForm* toPlace, int count, int useNodePos, int direction)
{
	return CdeclCall<TESObjectREFR*>(0x5C4B30, this, toPlace, count, useNodePos, direction, 1.0f);
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

bool Actor::GetShouldAttack(Actor* target)
{
	// Check out 0x59ED30
	if (!target)
		return false;

	if (target->isInCombat && target->GetCombatController())
	{
		// CombatManager_992640
		if (ThisStdCall_B(0x992640, *(void**)0x11F1958, this, target)) // 0x11F1958 = g_combatManager
			return false;
	}
	// call Actor::GetShouldAttack
	int factionRelation_Out;
	return ThisStdCall_B(0x8B06D0, this, target, 0, &factionRelation_Out, 0);
}

void Actor::SetWantsWeaponOut(bool wantsWeaponOut)
{
	ThisStdCall(0x8A6840, this, (UInt8)wantsWeaponOut);
}

bool Actor::IsInReloadAnim()
{
	return ThisStdCall<bool>(0x8A8870, this);
}

bool Actor::IsDoingAttackAnimation() const
{
	if (baseProcess)
	{
		auto action = baseProcess->GetCurrentAnimAction();
		return action >= HighProcess::kAnimAction_Attack && action <= HighProcess::kAnimAction_Attack_Throw_Attach;
	}
	return false;
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

// Code copied from JIP's TESObjectREFR::DeleteReference
// This is just an ASM-optimised version of vanilla MarkForDelete.
__declspec(naked) void TESObjectREFR::MarkForDelete()
{
	__asm
	{
		push	ebp
		mov		ebp, esp
		push	ecx
		push	1
		mov		eax, [ecx]
		call	dword ptr[eax + 0xC4]
		push	1
		mov		ecx, [ebp - 4]
		mov		eax, [ecx]
		call	dword ptr[eax + 0xC8]
		push	0
		push	0
		mov		ecx, [ebp - 4]
		mov		eax, [ecx]
		call	dword ptr[eax + 0x1CC]
		mov		ecx, [ebp - 4]
		cmp		byte ptr[ecx + 0xF], 0xFF
		jnz		done
		lea		eax, [ebp - 4]
		push	eax
		mov		ecx, 0x11CACB8
		CALL_EAX(0x5AE3D0)
	done:
		leave
		retn
	}
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
