#pragma once

// Everything here was ripped from JIP LN.

#include "ShowOffNVSE.h"
#include "GameProcess.h"

/*
#include <atomic>
#include "GameProcess.h"
#include "GameData.h"
#include "common/ICriticalSection.h"
*/

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
#define IS_ACTOR(form) ((*(UInt32**)form)[0x40] == kAddr_ReturnTrue)	//only works on references
#define NOT_ACTOR(form) ((*(UInt32**)form)[0x40] != kAddr_ReturnTrue)	//only works on references
#define IS_NODE(object) ((*(UInt32**)object)[3] == kAddr_ReturnThis)

extern bool (*WriteRecord)(UInt32 type, UInt32 version, const void* buffer, UInt32 length);
extern bool (*WriteRecordData)(const void* buffer, UInt32 length);
extern bool (*GetNextRecordInfo)(UInt32* type, UInt32* version, UInt32* length);
extern UInt32(*ReadRecordData)(void* buffer, UInt32 length);
extern bool (*ResolveRefID)(UInt32 refID, UInt32* outRefID);
extern const char* (*GetSavePath)(void);
extern void (*WriteRecord8)(UInt8 inData);
extern void (*WriteRecord16)(UInt16 inData);
extern void (*WriteRecord32)(UInt32 inData);
extern void (*WriteRecord64)(const void* inData);
extern UInt8(*ReadRecord8)();
extern UInt16(*ReadRecord16)();
extern UInt32(*ReadRecord32)();
extern void (*ReadRecord64)(void* outData);
extern void (*SkipNBytes)(UInt32 byteNum);

//todo: fix bug where second element in a 2D array is somehow nullptr, and not an inner array.
struct ArrayData_JIP
{
	UInt32			size;
	ArrayElementR* vals;
	ArrayElementR* keys;

	ArrayData_JIP(NVSEArrayVar* srcArr, bool isPacked)
	{
		size = GetArraySize(srcArr);
		if (size)
		{
			UInt32 alloc = size * sizeof(ArrayElementR);
			if (!isPacked) alloc *= 2;
			vals = (ArrayElementR*)AuxBuffer::Get(2, alloc);
			keys = isPacked ? nullptr : (vals + size);
			MemZero(vals, alloc);
			if (!GetArrayElements(srcArr, vals, keys))
				size = 0;
		}
	}

	~ArrayData_JIP()
	{
		if (size)
		{
			UInt32 count = keys ? (size * 2) : size;
			ArrayElementR* elems = vals;
			do
			{
				elems->~ElementR();
				elems++;
			} while (--count);
		}
	}
};


float __fastcall GetAxisDistance(TESObjectREFR* ref1, TESObjectREFR* ref2, UInt8 axis);

//If ref1 and ref2 are the same, distance = 0.
float GetDistance3D(TESObjectREFR* ref1, TESObjectREFR* ref2);

TESForm* __stdcall LookupFormByRefID(UInt32 refID);


extern std::atomic<UInt32> s_serializedVersion;

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
extern AuxStringMapModsMap s_auxStringMapArraysPerm, s_auxStringMapArraysTemp;  //Ensure thread safety when modifying these globals!!

UInt32 __fastcall GetSubjectID(TESForm* form, TESObjectREFR* thisObj);

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

extern std::atomic<UInt8> s_dataChangedFlags; // For AuxVar serialization.






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