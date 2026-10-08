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
	return ThisCall<ScriptEventList*>(0x5673E0, this);
}

PlayerCharacter *PlayerCharacter::GetSingleton()
{
	return *(PlayerCharacter**)0x11DEA3C;
}

bool Actor::IsSneaking() const {
	return ThisCall<bool>(0x4997B0, this);
}

TESContainer *TESObjectREFR::GetContainer() const {
	return ThisCall<TESContainer*>(0x55D310, this);
}

bool TESObjectREFR::IsMapMarker()
{
	return baseForm->GetFormID() == 0x10;
}

InventoryChanges* TESObjectREFR::GetInventoryChanges()
{
	return CdeclCall<InventoryChanges*>(0x4BF220, this);
}

void PlayerCharacter::UpdateCamera(bool abForce, bool abSkipUpdateLOD)
{
	ThisCall(0x94AE40, this, abForce, abSkipUpdateLOD);
}

void TESObjectREFR::Update3D()
{
	if (this == PlayerCharacter::GetSingleton())
		ThisCall(kUpdateAppearanceAddr, this);
	else
	{
		Set3D(NULL, true);
		ModelLoader::GetSingleton()->QueueReference(this, 1, 0);
	}
}

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
				uint8_t motionType = rigidBody->motion.type;
				if ((motionType == 2) || (motionType == 3) || (motionType == 6))
					return rigidBody;
			}
		}
	}
	return nullptr;
}

NiNode* TESObjectREFR::Get3DSimple() const
{
	return ThisCall<NiNode*>(0x43FCD0, this);
}

TESObjectREFR* TESObjectREFR::PlaceAtMe(TESForm* toPlace, int count, int useNodePos, int direction)
{
	return CdeclCall<TESObjectREFR*>(0x5C4B30, this, toPlace, count, useNodePos, direction, 1.0f);
}

TESObjectREFR *TESObjectREFR::Create(bool bTemp)
{
	TESObjectREFR *refr = (TESObjectREFR*)GameHeapAlloc(sizeof(TESObjectREFR));
	ThisCall(s_TESObject_REFR_init, refr);
	if (bTemp) ThisCall(0x484490, refr);
	return refr;
}

void Actor::SetAnimAction(int32_t animAction, BSAnimGroupSequence* animGroupSeq)
{
	ThisCall<void>(0x8A73E0, this, animAction, animGroupSeq);
}

// From JIP
uint16_t Actor::GetLevel() {
	return ThisCall<uint16_t>(0x87F9F0, this);
}

bool Actor::IsInvisible()
{
	return (avOwner.GetActorValueInt(kAVCode_Invisibility) > 0) || (avOwner.GetActorValueInt(kAVCode_Chameleon) > 0);
}

void Actor::Kill(Actor* killer)
{
	ThisCall<void>(0x89D900, this, killer, 0.0f);
}

bool Actor::GetShouldAttack(Actor* target)
{
	// Check out 0x59ED30
	if (!target)
		return false;

	if (target->isInCombat && target->GetCombatController())
	{
		// CombatManager_992640
		if (ThisCall<bool>(0x992640, *(void**)0x11F1958, this, target)) // 0x11F1958 = g_combatManager
			return false;
	}
	// call Actor::GetShouldAttack
	int factionRelation_Out;
	return ThisCall<bool>(0x8B06D0, this, target, 0, &factionRelation_Out, 0);
}

void Actor::SetWantWeaponDrawn(bool wantsWeaponOut)
{
	ThisCall(0x8A6840, this, wantsWeaponOut);
}

bool Actor::IsReloading() const
{
	return ThisCall<bool>(0x8A8870, this);
}

bool Actor::IsAttacking() const
{
	return ThisCall<bool>(0x894900, this);
}

int32_t Actor::GetDetectionLevelAlt(Actor* target, bool calculateSneakLevel)
{
	bool isTargetInCombat;
	if (target->IsPlayerRef())
		isTargetInCombat = ((PlayerCharacter*)target)->pcInCombat;
	else
		isTargetInCombat = target->isInCombat;

	bool bLineofSight = false;
	bool b360LineOfSight = false;
	return ThisCall<int32_t>(0x8A0D10, this, calculateSneakLevel, target, &bLineofSight, isInCombat, isTargetInCombat, 0, &b360LineOfSight);
}

// Same-ish code as what's used for Cmd_GetDetected_Eval
bool Actor::Detects(Actor* target)
{
	int32_t const detectionLevel = this->GetDetectionLevelAlt(target, false);
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
