#pragma once
#include <PluginAPI.h>

#include "GameData.h"
#include "utility.h"

__declspec(naked) bool IsConsoleOpen()
{
	__asm
	{
		mov		al, byte ptr ds : [0x11DEA2E]
		test	al, al
		jz		done
		mov		eax, ds : [0x126FD98]
		mov		edx, fs : [0x2C]
		mov		eax, [edx + eax * 4]
		test	eax, eax
		jz		done
		mov		al, [eax + 0x268]
		done :
		retn
	}
}


#if 0 //not gonna bother with this for now
DebugLog s_log, s_debug, s_missingTextures;


bool (*WriteRecord)(UInt32 type, UInt32 version, const void* buffer, UInt32 length);
bool (*WriteRecordData)(const void* buffer, UInt32 length);
bool (*GetNextRecordInfo)(UInt32* type, UInt32* version, UInt32* length);
UInt32(*ReadRecordData)(void* buffer, UInt32 length);
bool (*ResolveRefID)(UInt32 refID, UInt32* outRefID);
const char* (*GetSavePath)(void);
CommandInfo* (*GetCmdByOpcode)(UInt32 opcode);
const char* (*GetStringVar)(UInt32 stringID);
bool (*AssignString)(COMMAND_ARGS, const char* newValue);
NVSEArrayVar* (*CreateArray)(const NVSEArrayElement* data, UInt32 size, Script* callingScript);
NVSEArrayVar* (*CreateStringMap)(const char** keys, const NVSEArrayElement* values, UInt32 size, Script* callingScript);
bool (*AssignCommandResult)(NVSEArrayVar* arr, double* dest);
void (*SetElement)(NVSEArrayVar* arr, const NVSEArrayElement& key, const NVSEArrayElement& value);
void (*AppendElement)(NVSEArrayVar* arr, const NVSEArrayElement& value);
UInt32(*GetArraySize)(NVSEArrayVar* arr);
NVSEArrayVar* (*LookupArrayByID)(UInt32 id);
bool (*GetElement)(NVSEArrayVar* arr, const NVSEArrayElement& key, NVSEArrayElement& outElement);
bool (*GetElements)(NVSEArrayVar* arr, NVSEArrayElement* elements, NVSEArrayElement* keys);
bool (*ExtractFormatStringArgs)(UInt32 fmtStringPos, char* buffer, COMMAND_ARGS_EX, UInt32 maxParams, ...);
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
			GetElements(srcArr, elements, NULL);
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
	GetElements(srcArr, data, NULL);
	return data;
}

/*
__declspec(naked) void __fastcall DoConsolePrint(double* result)
{
	__asm
	{
		call	IsConsoleOpen
		test	al, al
		jnz		proceed
		retn
		proceed :
		mov		edx, [ebp]
			mov		edx, [edx - 0x30]
			mov		edx, [edx]
			movsd	xmm0, qword ptr[ecx]
			push	ebp
			mov		ebp, esp
			sub		esp, 0x50
			lea		ecx, [ebp - 0x50]
			call	StrCopy
			mov		ecx, eax
			mov		dword ptr[ecx], ' >> '
			add		ecx, 4
			call	FltToStr
			lea		eax, [ebp - 0x50]
			push	eax
			push	eax
			mov		ecx, g_consoleManager
			mov		eax, 0x71D0A0
			call	eax
			mov		esp, ebp
			pop		ebp
			retn
	}
}

__declspec(naked) void __fastcall DoConsolePrintID(double* result)
{
	__asm
	{
		call	IsConsoleOpen
		test	al, al
		jnz		proceed
		retn
		proceed :
		mov		edx, [ebp]
			mov		edx, [edx - 0x30]
			mov		edx, [edx]
			push	ebp
			mov		ebp, esp
			sub		esp, 0x60
			push	esi
			mov		esi, [ecx]
			lea		ecx, [ebp - 0x60]
			call	StrCopy
			mov		ecx, eax
			mov		dword ptr[ecx], ' >> '
			add		ecx, 4
			test	esi, esi
			jnz		haveID
			mov		word ptr[ecx], '0'
			jmp		noEDID
			haveID :
		mov		edx, esi
			call	UIntToHex
			push	esi
			mov		esi, eax
			call	LookupFormByRefID
			test	eax, eax
			jz		noEDID
			mov		ecx, eax
			mov		eax, [ecx]
			call	dword ptr[eax + 0x130]
			test	eax, eax
			jz		noEDID
			cmp[eax], 0
			jz		noEDID
			mov		ecx, esi
			mov		word ptr[ecx], '\" '
			add		ecx, 2
			mov		edx, eax
			call	StrCopy
			mov		word ptr[eax], '\"'
			noEDID:
		pop		esi
			lea		eax, [ebp - 0x60]
			push	eax
			push	eax
			mov		ecx, g_consoleManager
			mov		eax, 0x71D0A0
			call	eax
			mov		esp, ebp
			pop		ebp
			retn
	}
}
*/
/*
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
*/

alignas(16) char
s_strArgBuffer[0x4000],
s_strValBuffer[0x10000],
s_dataPathFull[0x100] = "Data\\",
s_configPathFull[0x100] = "Data\\Config\\",
s_scriptsPathFull[0x100] = "Data\\NVSE\\plugins\\scripts\\",
s_modLogPathFull[0x100] = "Mod Logs\\";
char* s_dataPath, * s_configPath, * s_scriptsPath, * s_modLogPath;



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