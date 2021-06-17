#pragma once

// Everything here was ripped from JIP LN.

#include "GameProcess.h"

extern ICriticalSection g_Lock;  //todo: replace with unique Lock

enum  //Memory Addresses
{
	kAddr_AddExtraData = 0x40FF60, 
	kAddr_RemoveExtraType = 0x410140,
	kAddr_LoadModel = 0x447080,
	kAddr_ApplyAmmoEffects = 0x59A030,
	kAddr_MoveToMarker = 0x5CCB20,
	kAddr_ApplyPerkModifiers = 0x5E58F0,
	kAddr_ReturnThis = 0x6815C0,
	kAddr_PurgeTerminalModel = 0x7FFE00,
	kAddr_EquipItem = 0x88C650,
	kAddr_UnequipItem = 0x88C790,
	kAddr_ReturnTrue = 0x8D0360,
	kAddr_TileGetFloat = 0xA011B0,
	kAddr_TileSetFloat = 0xA012D0,
	kAddr_TileSetString = 0xA01350,
	kAddr_InitFontInfo = 0xA12020,
};

#define IS_REFERENCE(form) ((*(UInt32**)form)[0x3C] == kAddr_ReturnTrue)
#define NOT_REFERENCE(form) ((*(UInt32**)form)[0x3C] != kAddr_ReturnTrue)
#define IS_ACTOR(form) ((*(UInt32**)form)[0x40] == kAddr_ReturnTrue)
#define NOT_ACTOR(form) ((*(UInt32**)form)[0x40] != kAddr_ReturnTrue)
#define IS_NODE(object) ((*(UInt32**)object)[3] == kAddr_ReturnThis)


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

UInt8 TESForm::GetOverridingModIdx()
{
	ModInfo* info = mods.GetLastItem();
	return info ? info->modIndex : 0xFF;
}

struct InventoryRef
{
	TESForm*			type;			// 00
	ContChangesEntry*	entry;			// 04
	ExtraDataList*		xData;			// 08
	TESObjectREFR*		containerRef;	// 0C
	TESObjectREFR*		tempRef;		// 10
	UInt8				pad14[24];		// 14
	bool				doValidation;	// 2C
	bool				removed;		// 2D
	UInt8				pad2E[2];		// 2E

	SInt32 GetCount();
	ExtraDataList* CreateExtraData();
};

InventoryRef* (*InventoryRefGetForID)(UInt32 refID);


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

__declspec(naked) bool Actor::IsInCombatWith(Actor* target)
{
	__asm
	{
		mov		eax, [ecx + 0x12C]
		test	eax, eax
		jz		done
		mov		ecx, [eax + 4]
		mov		eax, [eax + 8]
		mov		edx, [esp + 4]
		test	eax, eax
		jnz		iterHead
	done:
		retn	4
		lea		esp, [esp]
		fnop
	iterHead:
		cmp		[ecx], edx
		jz		rtnTrue
		add		ecx, 4
		dec		eax
		jnz		iterHead
		retn	4
	rtnTrue:
		mov		al, 1
		retn	4
	}
}

__declspec(naked) TESForm* __stdcall LookupFormByRefID(UInt32 refID)
{
	__asm
	{
		mov		ecx, ds: [0x11C54C0]
		mov		eax, [esp+4]
		xor		edx, edx
		div		dword ptr[ecx+4]
		mov		eax, [ecx+8]
		mov		eax, [eax+edx*4]
		test	eax, eax
		jz		done
		mov		edx, [esp+4]
		ALIGN 16
	iterHead:
		cmp		[eax+4], edx
		jz		found
		mov		eax, [eax]
		test	eax, eax
		jnz		iterHead
		retn	4
	found:
		mov		eax, [eax+8]
	done :
		retn	4
	}
}

TESObjectWEAP* Actor::GetEquippedWeapon()
{
	if (baseProcess)
	{
		ContChangesEntry* weaponInfo = baseProcess->GetWeaponInfo(); 
		if (weaponInfo) return (TESObjectWEAP*)weaponInfo->type;
	}
	return NULL;
}



std::atomic<UInt32> s_serializedVersion = 9;

class AuxVariableValue
{
	UInt8		type;
	UInt8		pad01[3];
	UInt16		alloc;
	UInt16		length;
	union
	{
		double	num;
		UInt32	refID;
		char* str;
	};

	void Clear()
	{
		if (alloc)
		{
			Pool_Free(str, alloc);
			alloc = 0;
		}
	}

	void ReadValData()
	{
		if (type == 1)
		{
			if (s_serializedVersion < 10)
			{
				refID = ReadRecord32();
				num = *(float*)&refID;
			}
			else ReadRecord64(&num);
		}
		else if (type == 2)
		{
			refID = ReadRecord32();
			ResolveRefID(refID, &refID);
		}
		else
		{
			length = ReadRecord16();
			if (length)
			{
				alloc = AlignNumAlloc<char>(length + 1);
				str = (char*)Pool_Alloc(alloc);
				ReadRecordData(str, length);
				str[length] = 0;
			}
		}
	}

public:
	AuxVariableValue() : alloc(0) {}
	AuxVariableValue(UInt8 _type) : type(_type), alloc(0) { ReadValData(); }
	AuxVariableValue(NVSEArrayElement& elem) : alloc(0) { SetElem(elem); }

	~AuxVariableValue() { Clear(); }

	UInt8 GetType() const { return type; }
	double GetFlt() const { return (type == 1) ? num : 0; }
	UInt32 GetRef() const { return (type == 2) ? refID : 0; }
	const char* GetStr() const { return alloc ? str : NULL; }

	void SetFlt(double value)
	{
		ScopedLock lock(g_Lock);   
		Clear();
		type = 1;
		num = value;
	}

	void SetRef(TESForm* value)
	{
		ScopedLock lock(g_Lock);
		Clear();
		type = 2;
		refID = value ? value->refID : 0;
	}

	void SetStr(const char* value)
	{
		ScopedLock lock(g_Lock);
		type = 4;
		length = StrLen(value);
		if (length)
		{
			UInt16 size = length + 1;
			if (alloc < size)
			{
				if (alloc) Pool_Free(str, alloc);
				alloc = AlignNumAlloc<char>(size);
				str = (char*)Pool_Alloc(alloc);
			}
			memcpy(str, value, size);
		}
		else if (alloc)
			*str = 0;
	}

	void SetElem(NVSEArrayElement& elem)
	{
		ScopedLock lock(g_Lock);
		if (elem.GetType() == 2) SetRef(elem.form);
		else if (elem.GetType() == 3) SetStr(elem.str);
		else SetFlt(elem.num);
	}

	ArrayElementL GetAsElement() const
	{
		if (type == 2) return ArrayElementL(LookupFormByRefID(refID));
		if (type == 4) return ArrayElementL(GetStr());
		return ArrayElementL(num);
	}

	void WriteValData() const
	{
		WriteRecord8(type);
		if (type == 1)
			WriteRecord64(&num);
		else if (type == 2)
			WriteRecord32(refID);
		else
		{
			WriteRecord16(length);
			if (length) WriteRecordData(str, length);
		}
	}
};
STATIC_ASSERT(sizeof(AuxVariableValue) == 0x10);

typedef UnorderedMap<char*, AuxVariableValue> AuxStringMapIDsMap;
typedef UnorderedMap<char*, AuxStringMapIDsMap> AuxStringMapVarsMap;
typedef UnorderedMap<UInt32, AuxStringMapVarsMap> AuxStringMapModsMap;
AuxStringMapModsMap s_auxStringMapArraysPerm, s_auxStringMapArraysTemp;  //Ensure thread safety when modifying these globals!!

UInt32 __fastcall GetSubjectID(TESForm* form, TESObjectREFR* thisObj)
{
	if (form) return IS_REFERENCE(form) ? ((TESObjectREFR*)form)->baseForm->refID : form->refID;
	if (thisObj) return thisObj->refID;
	return 0;
}

struct AuxStringMapInfo
{
	UInt32		modIndex;
	bool		isPerm;

	AuxStringMapInfo(Script* scriptObj, char* varName)
	{
		isPerm = (varName[0] != '*');
		modIndex = (varName[!isPerm] == '_') ? 0xFF : scriptObj->GetOverridingModIdx();
	}

	AuxStringMapInfo(Script* scriptObj, UInt8 type)
	{
		isPerm = !(type & 1);
		modIndex = (type > 1) ? 0xFF : scriptObj->GetOverridingModIdx();
	}

	AuxStringMapModsMap& ModsMap() { return isPerm ? s_auxStringMapArraysPerm : s_auxStringMapArraysTemp; }
};

std::atomic<UInt8> s_dataChangedFlags = 0; //For AuxVar serialization.


#if 0 //not gonna bother with this for now
DebugLog s_log, s_debug, s_missingTextures;


bool (*CallFunction)(Script* funcScript, TESObjectREFR* callingObj, TESObjectREFR* container, NVSEArrayElement* result, UInt8 numArgs, ...);

DataHandler* g_dataHandler = NULL;


struct TempArrayElements
{
	UInt32			size;
	ArrayElementR* elements;
	bool			doFree;

	TempArrayElements(NVSEArrayVar* srcArr)
	{
		doFree = true;
		size = GetArraySize(srcArr);
		if (size)
		{
			elements = (ArrayElementR*)calloc(size, sizeof(ArrayElementR));
			GetArrayElements(srcArr, elements, NULL);
		}
		else elements = NULL;
	}
	TempArrayElements(ArrayElementR* element) : size(1), elements(element), doFree(false) {}
	~TempArrayElements()
	{
		if (!doFree || !elements) return;
		ArrayElementR* dataPtr = elements;
		do
		{
			dataPtr->~ElementR();
			dataPtr++;
		} 		while (--size);
		free(elements);
	}
};

ArrayElementR* __fastcall GetArrayData(NVSEArrayVar* srcArr, UInt32* size)
{
	*size = GetArraySize(srcArr);
	if (!*size) return NULL;
	ArrayElementR* data = (ArrayElementR*)GetAuxBuffer(s_auxBuffers[2], *size * sizeof(ArrayElementR));
	MemZero(data, *size * sizeof(ArrayElementR));
	GetArrayElements(srcArr, data, NULL);
	return data;
}

const char kDumpLvlListIndentStr[] = "                                                  ";
UInt8 s_dumpLvlListIndent = 50;

void BGSLevL::Dump() 
{
	ListData* data;
	TESForm* form;
	TESLeveledList* lvlList;
	ListNode<ListData>* iter = list.Head();
	do
	{
		if (!(data = iter->data)) continue;
		form = data->form;
		lvlList = form->GetLvlList();
		Console_Print("%s%s [%08X] Level: %d Count: %d Health: %.2f", kDumpLvlListIndentStr + s_dumpLvlListIndent,
			lvlList ? "(LeveledList)" : form->GetTheName(), form->refID, data->level, data->count, data->extra ? data->extra->health : 0);
		if (lvlList)
		{
			s_dumpLvlListIndent -= 5;
			lvlList->Dump();
			s_dumpLvlListIndent += 5;
		}
	} 	while (iter = iter->next);
}


UnorderedMap<UInt32, const char*> s_refStrings;

const char* TESForm::RefToString()
{
	const char** findID;
	if (!s_refStrings.Insert(refID, &findID)) return *findID;
	const char* modName = g_dataHandler->GetNthModName(modIndex);
	UInt32 length = StrLen(modName);
	char* refStr = (char*)malloc(length + 8);
	if (length) memcpy(refStr, modName, length);
	refStr[length++] = ':';
	UIntToHex(refStr + length, refID & 0xFFFFFF);
	*findID = refStr;
	return refStr;
}

UnorderedSet<ContChangesEntry*> s_tempContChangesEntries(0x40);
void DoDeferredFreeEntries()
{
	for (auto iter = s_tempContChangesEntries.Begin(); iter; ++iter)
	{
		if (iter->extendData)
			GameHeapFree(iter->extendData);
		GameHeapFree(*iter);
	}
	s_tempContChangesEntries.Clear();
}
#endif