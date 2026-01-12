#include "decoding.h"


#include "GameBSExtraData.h"
#include "GameForms.h"
#include "GameObjects.h"
#include "GameTasks.h"
#include "GameTiles.h"
#include "netimmerse.h"
#include "NiPoint.h"
#include "GameUI.h"
#include "havok.h"
#include "GameAPI.h"

// Credits: JIPLN, lStewieAl, JG, and surely others.




FontHeightData s_fontHeightDatas[90];

// From JIP
TESObjectREFR* Projectile::GetImpactRef() const
{
	if (hasImpacted)
	{
		const ListNode<ImpactData>* traverse = impactDataList.Head();
		do
		{
			ImpactData* impactData = traverse->data;
			if (impactData && impactData->refr)
			{
				return impactData->refr;
			}
		} while (traverse = traverse->next);
	}
	return nullptr;
}

// From Tweaks
Projectile* __cdecl Projectile::Spawn(BGSProjectile* projectile, Actor* source, CombatController* combatCtrl, TESObjectWEAP* sourceWeap,
	NiPoint3 pos, float rotZ, float rotX, float angularMomentumZ, float angularMomentumX, TESObjectCELL* cell, bool ignoreGravity)
{
	return CdeclCall<Projectile*>(0x9BCA60, projectile, source, combatCtrl, sourceWeap, pos, rotZ, rotX, 0, 0, 0, ignoreGravity, angularMomentumZ, angularMomentumX, cell);
}

__declspec(naked) NiVector3* FontManager::GetStringDimensions(NiVector3* outDims, const char* srcString, UInt32 fontID, UInt32 maxFlt, UInt32 startIdx)
{
	static const UInt32 procAddr = 0xA1B020;
	__asm	jmp		procAddr
}

DebugText* DebugText::GetSingleton()
{
	return ((DebugText * (*)(bool))0xA0D9E0)(true);
}

DebugText::DebugLine* DebugText::GetDebugInput()
{
	DebugLine* linesPtr = lines, * result = lines;
	UInt32 counter = 200;
	do
	{
		linesPtr++;
		if (!linesPtr->isVisible) break;
		if (result->offsetY < linesPtr->offsetY)
			result = linesPtr;
	} 	while (--counter);
	return result;
}

ExtraEnableStateChildren* ExtraEnableStateChildren::Create()
{
	UInt32* dataPtr = (UInt32*)GameHeapAlloc(sizeof(ExtraEnableStateChildren));
	dataPtr[0] = kVtbl_ExtraEnableStateChildren;
	dataPtr[1] = kExtraData_EnableStateChildren;
	dataPtr[2] = 0;
	//tList is inline, not a pointer - init m_listHead directly
	dataPtr[3] = 0; //children.m_listHead.data
	dataPtr[4] = 0; //children.m_listHead.next
	return reinterpret_cast<ExtraEnableStateChildren*>(dataPtr);
}

ExtraEnableStateParent* ExtraEnableStateParent::Create(TESObjectREFR* parent)
{
	UInt32* dataPtr = (UInt32*)GameHeapAlloc(sizeof(ExtraEnableStateParent));
	dataPtr[0] = kVtbl_ExtraEnableStateParent;
	dataPtr[1] = kExtraData_EnableStateParent;
	dataPtr[2] = 0;
	dataPtr[3] = (UInt32)parent;
	dataPtr[4] = 0; //flags + pad
	return reinterpret_cast<ExtraEnableStateParent*>(dataPtr);
}

__declspec(naked) void SystemColorManager::SystemColor::SetColorRGB(UInt32 r, UInt32 g, UInt32 b)
{
	__asm
	{
		xor eax, eax
		mov		ah, [esp + 4]
		mov		al, [esp + 8]
		shl		eax, 0x10
		mov		ah, [esp + 0xC]
		mov		al, 0xFF
		push	eax
		mov		eax, [ecx]
		call	dword ptr[eax + 8]
		retn	0xC
	}
}

AnimGroupClassify s_animGroupClassify[] =
{
	{1, 1, 0, 0}, {1, 1, 0, 0}, {1, 1, 0, 0}, {2, 1, 0, 0}, {2, 2, 0, 0}, {2, 3, 0, 0}, {2, 4, 0, 0}, {2, 1, 1, 0}, {2, 2, 1, 0},
	{2, 3, 1, 0}, {2, 4, 1, 0}, {2, 1, 2, 0}, {2, 2, 2, 0}, {2, 3, 2, 0}, {2, 4, 2, 0}, {2, 3, 4, 0}, {2, 4, 4, 0}, {3, 0, 0, 0},
	{3, 0, 1, 0}, {3, 0, 2, 0}, {3, 0, 4, 0}, {3, 0, 5, 0}, {3, 0, 6, 0}, {1, 2, 0, 0}, {1, 2, 0, 0}, {1, 2, 0, 0}, {3, 1, 0, 0},
	{3, 1, 1, 0}, {3, 1, 2, 0}, {3, 1, 4, 0}, {3, 1, 5, 0}, {3, 1, 6, 0}, {3, 2, 0, 0}, {3, 2, 1, 0}, {3, 2, 2, 0}, {3, 2, 4, 0},
	{3, 2, 5, 0}, {3, 2, 6, 0}, {3, 3, 0, 0}, {3, 3, 1, 0}, {3, 3, 2, 0}, {3, 3, 4, 0}, {3, 3, 5, 0}, {3, 3, 6, 0}, {3, 4, 0, 0},
	{3, 4, 1, 0}, {3, 4, 2, 0}, {3, 4, 4, 0}, {3, 4, 5, 0}, {3, 4, 6, 0}, {3, 5, 0, 0}, {3, 5, 1, 0}, {3, 5, 2, 0}, {3, 5, 4, 0},
	{3, 5, 5, 0}, {3, 5, 6, 0}, {3, 6, 0, 0}, {3, 6, 1, 0}, {3, 6, 2, 0}, {3, 6, 4, 0}, {3, 6, 5, 0}, {3, 6, 6, 0}, {3, 7, 0, 0},
	{3, 7, 1, 0}, {3, 7, 2, 0}, {3, 7, 4, 0}, {3, 7, 5, 0}, {3, 7, 6, 0}, {3, 8, 0, 0}, {3, 8, 1, 0}, {3, 8, 2, 0}, {3, 8, 4, 0},
	{3, 8, 5, 0}, {3, 8, 6, 0}, {3, 10, 0, 0}, {3, 10, 1, 0}, {3, 10, 2, 0}, {3, 10, 4, 0}, {3, 10, 5, 0}, {3, 10, 6, 0}, {3, 11, 0, 0},
	{3, 11, 1, 0}, {3, 11, 2, 0}, {3, 11, 4, 0}, {3, 11, 5, 0}, {3, 11, 6, 0}, {3, 12, 0, 0}, {3, 12, 1, 0}, {3, 12, 2, 0}, {3, 12, 4, 0},
	{3, 12, 5, 0}, {3, 12, 6, 0}, {3, 23, 0, 0}, {3, 23, 0, 0}, {3, 23, 0, 0}, {3, 23, 0, 0}, {3, 23, 0, 0}, {3, 23, 0, 0}, {3, 23, 0, 0},
	{3, 23, 0, 0}, {3, 23, 0, 0}, {3, 23, 0, 0}, {3, 21, 0, 0}, {3, 21, 1, 0}, {3, 21, 2, 0}, {3, 21, 4, 0}, {3, 21, 5, 0}, {3, 21, 6, 0},
	{3, 22, 0, 0}, {3, 22, 1, 0}, {3, 22, 2, 0}, {3, 22, 4, 0}, {3, 22, 5, 0}, {3, 22, 6, 0}, {3, 13, 0, 0}, {3, 13, 1, 0}, {3, 13, 2, 0},
	{3, 13, 4, 0}, {3, 13, 5, 0}, {3, 13, 6, 0}, {3, 14, 0, 0}, {3, 14, 1, 0}, {3, 14, 2, 0}, {3, 14, 4, 0}, {3, 14, 5, 0}, {3, 14, 6, 0},
	{3, 15, 0, 0}, {3, 15, 1, 0}, {3, 15, 2, 0}, {3, 15, 4, 0}, {3, 15, 5, 0}, {3, 15, 6, 0}, {3, 16, 0, 0}, {3, 16, 1, 0}, {3, 16, 2, 0},
	{3, 16, 4, 0}, {3, 16, 5, 0}, {3, 16, 6, 0}, {3, 17, 0, 0}, {3, 17, 1, 0}, {3, 17, 2, 0}, {3, 17, 4, 0}, {3, 17, 5, 0}, {3, 17, 6, 0},
	{3, 9, 0, 0}, {3, 9, 1, 0}, {3, 9, 2, 0}, {3, 9, 4, 0}, {3, 9, 5, 0}, {3, 9, 6, 0}, {3, 18, 0, 0}, {3, 18, 1, 0}, {3, 18, 2, 0},
	{3, 18, 4, 0}, {3, 18, 5, 0}, {3, 18, 6, 0}, {3, 19, 0, 0}, {3, 19, 1, 0}, {3, 19, 2, 0}, {3, 19, 4, 0}, {3, 19, 5, 0}, {3, 19, 6, 0},
	{3, 20, 0, 0}, {3, 20, 1, 0}, {3, 20, 2, 0}, {3, 20, 4, 0}, {3, 20, 5, 0}, {3, 20, 6, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 3, 0, 0},
	{1, 3, 0, 0}, {1, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0},
	{4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0},
	{4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {4, 0, 0, 0}, {5, 0, 0, 0},
	{5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0},
	{5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0}, {5, 0, 0, 0},
	{5, 0, 0, 0}, {5, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {2, 5, 0, 0}, {2, 5, 0, 0}, {2, 5, 0, 0}, {1, 4, 0, 0},
	{1, 4, 0, 0}, {1, 4, 0, 0}, {1, 4, 0, 0}, {1, 4, 0, 0}, {1, 4, 0, 0}, {2, 5, 0, 0}, {2, 5, 0, 0}, {2, 5, 0, 0}, {2, 5, 0, 0}, {1, 0, 0, 0},
	{2, 5, 0, 0}, {2, 5, 0, 0}, {2, 5, 0, 0}, {2, 5, 0, 0}
};

bool IsInVATS()
{
	return VATSCameraData::GetSingleton() && VATSCameraData::GetSingleton()->mode != VATSCameraData::kVATSMode_None;
}

void __cdecl PlayMenuSound(MenuSoundCode soundCode)
{
	return CdeclCall(0x717280, soundCode);
}

/*
#include "netimmerse.h"
#include <utility.h>*/

// Taken from JohnnyGuitar NVSE
NiNode* NiNode::GetNode(const char* nodeName)
{
	NiAVObject* found = GetBlock(nodeName);
	return found ? found->GetNiNode() : NULL;
}

// Taken from JohnnyGuitar NVSE
NiAVObject* NiNode::GetBlock(const char* blockName) {
	if (StrEqualCI(m_blockName, blockName))
		return this;
	NiAVObject* found = NULL;
	for (NiTArray<NiAVObject*>::Iterator iter(m_children); !iter.End(); ++iter) {
		if (!*iter) continue;
		if (iter->GetNiNode())
			found = ((NiNode*)*iter)->GetBlock(blockName);
		else if (StrEqualCI(iter->m_blockName, blockName))
			found = *iter;
		else continue;
		if (found) break;
	}
	return found;
}
