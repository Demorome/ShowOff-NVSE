#include "jip_nvse.h"


bool (*WriteRecord)(uint32_t type, uint32_t version, const void* buffer, uint32_t length);
bool (*WriteRecordData)(const void* buffer, uint32_t length);
bool (*GetNextRecordInfo)(uint32_t* type, uint32_t* version, uint32_t* length);
uint32_t(*ReadRecordData)(void* buffer, uint32_t length);
bool (*ResolveRefID)(uint32_t refID, uint32_t* outRefID);
const char* (*GetSavePath)(void);
void (*WriteRecord8)(uint8_t inData);
void (*WriteRecord16)(uint16_t inData);
void (*WriteRecord32)(uint32_t inData);
void (*WriteRecord64)(const void* inData);
uint8_t(*ReadRecord8)();
uint16_t(*ReadRecord16)();
uint32_t(*ReadRecord32)();
void (*ReadRecord64)(void* outData);
void (*SkipNBytes)(uint32_t byteNum);
InventoryRef* (*InventoryRefGetForID)(uint32_t refID);

uint8_t TESForm::GetOverridingModIdx() const
{
	ModInfo* info = mods.GetLastItem();
	return info ? info->modIndex : 0xFF;
}


TESObjectREFR* __fastcall CreateRefForStack(TESObjectREFR* container, ContChangesEntry* menuEntry)
{
	return (container && menuEntry) ? InventoryRefCreateEntry(container, menuEntry->type, menuEntry->countDelta, menuEntry->extendData ? menuEntry->extendData->GetFirstItem() : NULL) : NULL;
}

__declspec(naked) float __fastcall GetAxisDistance(TESObjectREFR* ref1, TESObjectREFR* ref2, uint8_t axis)
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

bool Actor::IsInCombatWith(Actor* target) const
{
	return ThisCall<bool>(0x8BC700, this, target);
}

TESForm* LookupFormByRefID(uint32_t refID) {
	return TESForm::GetFormByNumericID(refID);
}

TESObjectWEAP* Actor::GetEquippedWeapon() const
{
	return ThisCall<TESObjectWEAP*>(0x8A1710, this);
}

//Ensure thread safety when modifying these globals!
AuxStringMapModsMap s_auxStringMapArraysPerm, s_auxStringMapArraysTemp;

uint32_t __fastcall GetSubjectID(TESForm* form, TESObjectREFR* thisObj)
{
	if (form) return IS_REFERENCE(form) ? ((TESObjectREFR*)form)->baseForm->GetFormID() : form->GetFormID();
	if (thisObj) return thisObj->GetFormID();
	return 0;
}

std::atomic<uint8_t> s_dataChangedFlags = kChangedFlag_None; // For AuxVar serialization.

bool TESForm::IsItem() const
{
	return TESContainer::ContainerCanHoldType(typeID);
}

__declspec(naked) bool __fastcall GetResolvedModIndex(uint8_t* pModIdx)
{
	__asm
	{
		movzx	edx, byte ptr[ecx]
		cmp		dl, 0xFF
		jz		retn1
		mov		eax, dword ptr ds:[0x11DDF38] // BGSSaveLoadGame::pSingleton
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

__declspec(naked) bool __stdcall HasChangeData(uint32_t refID)
{
	__asm
	{
		mov		eax, dword ptr ds : [0x11DDF38] // BGSSaveLoadGame::pSingleton
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

__declspec(naked) uint32_t __fastcall GetResolvedRefID(uint32_t refID)
{
	__asm
	{
		push	ecx
		movzx	edx, byte ptr[esp + 3]
		cmp		dl, 0xFF
		jz		retnArg
		mov		ecx, dword ptr ds : [0x11DDF38] // BGSSaveLoadGame::pSingleton
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