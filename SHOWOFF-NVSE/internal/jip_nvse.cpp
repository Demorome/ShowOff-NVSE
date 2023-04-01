#include "jip_nvse.h"


bool (*WriteRecord)(UInt32 type, UInt32 version, const void* buffer, UInt32 length);
bool (*WriteRecordData)(const void* buffer, UInt32 length);
bool (*GetNextRecordInfo)(UInt32* type, UInt32* version, UInt32* length);
UInt32(*ReadRecordData)(void* buffer, UInt32 length);
bool (*ResolveRefID)(UInt32 refID, UInt32* outRefID);
const char* (*GetSavePath)(void);
void (*WriteRecord8)(UInt8 inData);
void (*WriteRecord16)(UInt16 inData);
void (*WriteRecord32)(UInt32 inData);
void (*WriteRecord64)(const void* inData);
UInt8(*ReadRecord8)();
UInt16(*ReadRecord16)();
UInt32(*ReadRecord32)();
void (*ReadRecord64)(void* outData);
void (*SkipNBytes)(UInt32 byteNum);
InventoryRef* (*InventoryRefGetForID)(UInt32 refID);

UInt8 TESForm::GetOverridingModIdx() const
{
	ModInfo* info = mods.GetLastItem();
	return info ? info->modIndex : 0xFF;
}


TESObjectREFR* __fastcall CreateRefForStack(TESObjectREFR* container, ContChangesEntry* menuEntry)
{
	return (container && menuEntry) ? InventoryRefCreateEntry(container, menuEntry->type, menuEntry->countDelta, menuEntry->extendData ? menuEntry->extendData->GetFirstItem() : NULL) : NULL;
}

__declspec(naked) float __fastcall GetAxisDistance(TESObjectREFR* ref1, TESObjectREFR* ref2, UInt8 axis)
{
	__asm
	{
		xorps	xmm0, xmm0
		mov		al, [esp + 4]
		test	al, 1
		jz		doneX
		movss	xmm0, [ecx + 0x30]
		subss	xmm0, [edx + 0x30]
		mulss	xmm0, xmm0
		doneX :
		test	al, 2
			jz		doneY
			movss	xmm1, [ecx + 0x34]
			subss	xmm1, [edx + 0x34]
			mulss	xmm1, xmm1
			addss	xmm0, xmm1
			doneY :
		test	al, 4
			jz		doneZ
			movss	xmm1, [ecx + 0x38]
			subss	xmm1, [edx + 0x38]
			mulss	xmm1, xmm1
			addss	xmm0, xmm1
			doneZ :
		sqrtss	xmm0, xmm0
			movss[esp + 4], xmm0
			fld		dword ptr[esp + 4]
			retn	4
	}
}

//If ref1 and ref2 are the same, distance = 0.
float GetDistance3D(TESObjectREFR* ref1, TESObjectREFR* ref2)
{
	return GetAxisDistance(ref1, ref2, 7);
}

__declspec(naked) bool __fastcall Actor::IsInCombatWith(Actor* target) const
{
	__asm
	{
		mov		eax, [ecx + 0x12C]
		test	eax, eax
		jz		done
		mov		ecx, [eax + 4]
		mov		eax, [eax + 8]
		test	eax, eax
		jz		done
		ALIGN 16
		iterHead:
		cmp[ecx], edx
			jz		rtnTrue
			add		ecx, 4
			dec		eax
			jnz		iterHead
			retn
			rtnTrue :
		mov		al, 1
			done :
			retn
	}
}

__declspec(naked) TESForm* __stdcall LookupFormByRefID(UInt32 refID)
{
	__asm
	{
		mov		ecx, ds: [0x11C54C0]
		mov		eax, [esp + 4]
		xor edx, edx
		div		dword ptr[ecx + 4]
		mov		eax, [ecx + 8]
		mov		eax, [eax + edx * 4]
		test	eax, eax
		jz		done
		mov		edx, [esp + 4]
		ALIGN 16
		iterHead:
		cmp[eax + 4], edx
			jz		found
			mov		eax, [eax]
			test	eax, eax
			jnz		iterHead
			retn	4
			found:
		mov		eax, [eax + 8]
			done :
			retn	4
	}
}

TESObjectWEAP* Actor::GetEquippedWeapon() const
{
	if (baseProcess)
	{
		ContChangesEntry* weaponInfo = baseProcess->GetWeaponInfo();
		if (weaponInfo) return (TESObjectWEAP*)weaponInfo->type;
	}
	return NULL;
}

//Ensure thread safety when modifying these globals!
AuxStringMapModsMap s_auxStringMapArraysPerm, s_auxStringMapArraysTemp;

UInt32 __fastcall GetSubjectID(TESForm* form, TESObjectREFR* thisObj)
{
	if (form) return IS_REFERENCE(form) ? ((TESObjectREFR*)form)->baseForm->refID : form->refID;
	if (thisObj) return thisObj->refID;
	return 0;
}

std::atomic<UInt8> s_dataChangedFlags = kChangedFlag_None; // For AuxVar serialization.


__declspec(naked) ExtraContainerChanges::EntryDataList* TESObjectREFR::GetContainerChangesList()
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
		mov		eax, [eax]
		done:
		retn
	}
}

const bool kInventoryType[121] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0
};

bool TESForm::IsItem() const
{
	return kInventoryType[this->typeID];
}

__declspec(naked) bool __fastcall GetResolvedModIndex(UInt8* pModIdx)
{
	__asm
	{
		movzx	edx, byte ptr[ecx]
		cmp		dl, 0xFF
		jz		retn1
		mov		eax, g_BGSSaveLoadGame
		mov		al, [eax + edx + 0x44]
		cmp		al, 0xFF
		jz		retn0
		mov[ecx], al
		retn1 :
		mov		al, 1
			retn
			retn0 :
		xor al, al
			retn
	}
}

__declspec(naked) bool __stdcall HasChangeData(UInt32 refID)
{
	__asm
	{
		mov		eax, g_BGSSaveLoadGame
		mov		ecx, [eax]
		mov		eax, [esp + 4]
		xor edx, edx
		div		dword ptr[ecx + 4]
		mov		eax, [ecx + 8]
		mov		eax, [eax + edx * 4]
		test	eax, eax
		jz		done
		mov		edx, [esp + 4]
		ALIGN 16
		iterHead:
		cmp[eax + 4], edx
			jz		found
			mov		eax, [eax]
			test	eax, eax
			jnz		iterHead
			retn	4
			found:
		mov		al, 1
			done :
			retn	4
	}
}

__declspec(naked) UInt32 __fastcall GetResolvedRefID(UInt32 refID)
{
	__asm
	{
		push	ecx
		movzx	edx, byte ptr[esp + 3]
		cmp		dl, 0xFF
		jz		retnArg
		mov		ecx, g_BGSSaveLoadGame
		mov		al, [ecx + edx + 0x44]
		cmp		al, 0xFF
		jz		retn0
		mov[esp + 3], al
		retnArg :
		pop		eax
			retn
			retn0 :
		xor eax, eax
			pop		ecx
			retn
	}
}