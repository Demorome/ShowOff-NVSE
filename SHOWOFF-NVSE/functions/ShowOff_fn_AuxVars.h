#pragma once

//For Auxiliary StringMap Arrays (AuxStringMap, ASM, not to confuse with ASM = assembly)

#define DoLaterMaybe 0

DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetSize, AuxStringMapSize, , 0, 1, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetValueType, AuxStringMapGetValueType, , 0, 2, kParams_TwoStrings);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetFloat, AuxStringMapGetFlt, , 0, 2, kParams_TwoStrings);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetRef, AuxStringMapGetRef, , 0, 2, kParams_TwoStrings);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetString, AuxStringMapGetStr, , 0, 2, kParams_TwoStrings);

#if DoLaterMaybe
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetValue, AuxStringMapGetVal, , 0, 2, kParams_TwoStrings);
#endif

DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetFirst, AuxStringMapFirst, , 0, 1, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetNext, AuxStringMapNext, , 0, 1, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetKeys, AuxStringMapKeys, , 0, 1, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetAll, AuxStringMapGetAll, , 0, 1, kParams_OneInt);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetAsArray, AuxStringMapGetAsArr, , 0, 1, kParams_OneString);

DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetFromArray, AuxStringMapSetFromArr, , 0, 3, kParams_OneString_OneArray_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetFloat, AuxStringMapSetFlt, , 0, 3, kParams_TwoStrings_OneDouble);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetRef, AuxStringMapSetRef, , 0, 3, kParams_TwoStrings_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetString, AuxStringMapSetStr, , 0, 3, kParams_ThreeStrings);

#if DoLaterMaybe
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetValue, AuxStringMapSetVal, , 0, 3, kParams_JIP_OneString_OneInt_OneOptionalForm);
#endif

DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayEraseKey, AuxStringMapEraseKey, , 0, 2, kParams_TwoStrings);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayValidateValues, AuxStringMapValidateVals, , 0, 1, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayDestroy, AuxStringMapDestroy, , 0, 1, kParams_OneString);


AuxStringMapIDsMap* ASMFind(Script* scriptObj, char* varName)
{
	if (!varName[0]) return NULL;
	AuxStringMapInfo varInfo(scriptObj, varName);
	AuxStringMapVarsMap* findMod = varInfo.ModsMap().GetPtr(varInfo.modIndex);
	if (!findMod) return NULL;
	return findMod->GetPtr(varName);
}

bool Cmd_AuxStringMapArrayGetSize_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName))
	{
		AuxStringMapIDsMap* idsMap = ASMFind(scriptObj, varName);
		if (idsMap) *result = (int)idsMap->Size();
	}
	return true;
}

bool Cmd_AuxStringMapArrayGetValueType_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	char keyName[0x50];
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &keyName))
	{
		AuxStringMapIDsMap* idsMap = ASMFind(scriptObj, varName);
		if (idsMap)
		{
			AuxVariableValue* value = idsMap->GetPtr(keyName);
			if (value)
				*result = value->GetType();
		}
	}
	return true;
}

bool Cmd_AuxStringMapArrayGetFloat_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	char keyName[0x50];
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &keyName))
	{
		AuxStringMapIDsMap* idsMap = ASMFind(scriptObj, varName);
		if (idsMap)
		{
			AuxVariableValue* value = idsMap->GetPtr(keyName);
			if (value)
				*result = value->GetFlt();
		}
	}
	return true;
}

bool Cmd_AuxStringMapArrayGetRef_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	char keyName[0x50];
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &keyName))
	{
		AuxStringMapIDsMap* idsMap = ASMFind(scriptObj, varName);
		if (idsMap)
		{
			AuxVariableValue* value = idsMap->GetPtr(keyName);
			if (value)
				REFR_RES = value->GetRef();
		}
	}
	return true;
}

bool Cmd_AuxStringMapArrayGetString_Execute(COMMAND_ARGS)
{
	const char* resStr = NULL;
	char varName[0x50];
	char keyName[0x50];
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &keyName))
	{
		AuxStringMapIDsMap* idsMap = ASMFind(scriptObj, varName);
		if (idsMap)
		{
			AuxVariableValue* value = idsMap->GetPtr(keyName);
			if (value)
				resStr = value->GetStr();
		}
	}
	AssignString(PASS_COMMAND_ARGS, resStr);
	return true;
}

#if DoLaterMaybe
bool Cmd_AuxStringMapArrayGetValue_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	TESForm* form = NULL;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &form))
	{
		AuxStringMapIDsMap* idsMap = ASMFind(scriptObj, varName);
		if (idsMap)
		{
			AuxVariableValue* value = idsMap->GetPtr(GetSubjectID(form, thisObj));
			if (value)
			{
				ArrayElementL element(value->GetAsElement());
				AssignArrayResult(CreateArray(&element, 1, scriptObj), result);
			}
		}
	}
	return true;
}
#endif

AuxStringMapIDsMap::Iterator s_auxStringMapIterator; //need to ensure thread safety!

NVSEArrayVar* __fastcall AuxStringMapArrayIterator(Script* scriptObj, char* varName, bool getFirst)
{
	ScopedLock lock(&g_Lock);
	AuxStringMapIDsMap* idsMap = ASMFind(scriptObj, varName);
	if (!idsMap) return NULL;
	if (getFirst || (s_auxStringMapIterator.Table() != idsMap))
	{
		s_auxStringMapIterator.Init(*idsMap);
		if (!s_auxStringMapIterator)
			return NULL;
	}
	else if (!s_auxStringMapIterator.IsValid())
		return NULL;
	ArrayElementL elements[2] = { s_auxStringMapIterator.Key(), s_auxStringMapIterator().GetAsElement() };
	++s_auxStringMapIterator;
	return CreateArray(elements, 2, scriptObj);
}

bool Cmd_AuxStringMapArrayGetFirst_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName))
	{
		NVSEArrayVar* pairArr = AuxStringMapArrayIterator(scriptObj, varName, true);
		if (pairArr) AssignArrayResult(pairArr, result);
	}
	return true;
}

bool Cmd_AuxStringMapArrayGetNext_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName))
	{
		NVSEArrayVar* pairArr = AuxStringMapArrayIterator(scriptObj, varName, false);
		if (pairArr) AssignArrayResult(pairArr, result);
	}
	return true;
}

bool Cmd_AuxStringMapArrayGetKeys_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &varName)) return true;
	AuxStringMapIDsMap* idsMap = ASMFind(scriptObj, varName);
	if (!idsMap) return true;
	Vector<ArrayElementL> elems;
	for (auto idIter = idsMap->Begin(); idIter; ++idIter)
		elems.Append(idIter.Key());
	if (!elems.Empty())
		AssignArrayResult(CreateArray(elems.Data(), elems.Size(), scriptObj), result);
	return true;
}

bool Cmd_AuxStringMapArrayGetAll_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 type;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &type)) return true;
	AuxStringMapInfo varInfo(scriptObj, type);
	AuxStringMapVarsMap* findMod = varInfo.ModsMap().GetPtr(varInfo.modIndex);
	if (!findMod || findMod->Empty()) return true;
	NVSEArrayVar* varsMap = CreateStringMap(NULL, NULL, 0, scriptObj);
	for (auto varIter = findMod->Begin(); varIter; ++varIter)
	{
		Vector<ArrayElementL> elems;
		for (auto idIter = varIter().Begin(); idIter; ++idIter)
			elems.Append(idIter.Key());
		SetElement(varsMap, ArrayElementL(varIter.Key()), ArrayElementL(CreateArray(elems.Data(), elems.Size(), scriptObj)));
	}
	AssignArrayResult(varsMap, result);
	return true;
}

bool Cmd_AuxStringMapArrayGetAsArray_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &varName)) return true;
	AuxStringMapIDsMap* idsMap = ASMFind(scriptObj, varName);
	if (!idsMap) return true;
	NVSEArrayVar* varsMap = CreateStringMap(NULL, NULL, 0, scriptObj);
	for (auto idIter = idsMap->Begin(); idIter; ++idIter)
	{
		SetElement(varsMap, ArrayElementL(idIter.Key()), ArrayElementL(idIter().GetAsElement()));
	}
	AssignArrayResult(varsMap, result);
	return true;
}


AuxVariableValue* __fastcall AuxStringMapAddValue(Script* scriptObj, char* varName, char* keyName)
{
	if (varName[0] && keyName[0])
	{
		AuxStringMapInfo varInfo(scriptObj, varName);
		if (varInfo.isPerm)
			s_dataChangedFlags |= kChangedFlag_AuxStringMaps;
		ScopedLock lock(&g_Lock);  //since ModsMap() returns the global.
		return &varInfo.ModsMap()[varInfo.modIndex][varName][keyName];
	}
	return NULL;
}

bool Cmd_AuxStringMapArraySetFloat_Execute(COMMAND_ARGS)
{
	char varName[0x50];
	char keyName[0x50];
	double fltVal;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &keyName, &fltVal))
	{
		AuxVariableValue* value = AuxStringMapAddValue(scriptObj, varName, keyName);
		if (value)
			value->SetFlt(fltVal);
	}
	return true;
}

bool Cmd_AuxStringMapArraySetRef_Execute(COMMAND_ARGS)
{
	char varName[0x50];
	char keyName[0x50];
	TESForm* refVal;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &keyName, &refVal))
	{
		AuxVariableValue* value = AuxStringMapAddValue(scriptObj, varName, keyName);
		if (value)
			value->SetRef(refVal);
	}
	return true;
}

bool Cmd_AuxStringMapArraySetString_Execute(COMMAND_ARGS)
{
	char varName[0x50];
	char keyName[0x50];
	char* buffer = GetStrArgBuffer();
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &keyName, buffer))
	{
		AuxVariableValue* value = AuxStringMapAddValue(scriptObj, varName, keyName);
		if (value)
			value->SetStr(buffer);
	}
	return true;
}

bool Cmd_AuxStringMapArraySetFromArray_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	UInt32 arrID = NULL;
	UINT32 bAppend = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &arrID, &bAppend)) return true;
	g_Lock.Enter();  //since ModsMap() returns a global.
	AuxStringMapInfo varInfo(scriptObj, varName);
	auto findMod = varInfo.ModsMap().Find(varInfo.modIndex);
	if (findMod)
	{
		auto findVar = findMod().Find(varName);
		if (findVar)
		{
			if (findVar().Empty() || !bAppend)
			{
				findVar.Remove();
				if (findMod().Empty()) findMod.Remove();
			}
		}
	}
	g_Lock.Leave();
	NVSEArrayVar* inArr = LookupArrayByID(arrID);
	if (!inArr) return true;
	UInt32 size = GetArraySize(inArr);
	NVSEArrayElement* elements = new NVSEArrayElement[size];
	NVSEArrayElement* keys = new NVSEArrayElement[size];
	GetArrayElements(inArr, elements, keys);
	if (keys[0].GetType() == NVSEArrayVarInterface::kType_String)  //only works by passing StringMap arrays
	{
		for (int i = 0; i < size; i++)
		{
			char* keyName = CopyCString(keys[i].String());
			AuxVariableValue* value = AuxStringMapAddValue(scriptObj, varName, keyName);
			if (!value) continue;
			value->SetElem(elements[i]);
		}
	}
	if (varInfo.isPerm)
		s_dataChangedFlags |= kChangedFlag_AuxStringMaps;
	delete[] elements;
	delete[] keys;
	return true;
}

#if DoLaterMaybe
bool Cmd_AuxStringMapArraySetValue_Execute(COMMAND_ARGS)  //a bit unwieldy having to use the array middleman
{
	char varName[0x50];
	UInt32 arrID;
	TESForm* form = NULL;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &arrID, &form))
	{
		NVSEArrayVar* srcArr = LookupArrayByID(arrID);
		if (srcArr && (GetArraySize(srcArr) == 1))
		{
			AuxVariableValue* value = AuxStringMapAddValue(form, thisObj, scriptObj, varName);
			if (value)
			{
				ArrayElementR element;
				GetArrayElements(srcArr, &element, NULL);
				value->SetElem(element);
			}
		}
	}
	return true;
}
#endif

bool Cmd_AuxStringMapArrayEraseKey_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	char keyName[0x50];
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &keyName) || !varName[0]) return true;
	AuxStringMapInfo varInfo(scriptObj, varName);
	auto findMod = varInfo.ModsMap().Find(varInfo.modIndex);
	if (!findMod) return true;
	auto findVar = findMod().Find(varName);
	if (!findVar) return true;
	auto findID = findVar().Find(keyName);
	if (!findID) return true;
	ScopedLock lock(&g_Lock);
	findID.Remove();
	if (findVar().Empty())
	{
		findVar.Remove();
		if (findMod().Empty()) findMod.Remove();
	}
	else *result = (int)findVar().Size();
	if (varInfo.isPerm)
		s_dataChangedFlags |= kChangedFlag_AuxStringMaps;
	return true;
}


//  Clears out entries with VALUES that are invalid, instead of checking the keys (which is what RefMapValidate does).
bool Cmd_AuxStringMapArrayValidateValues_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &varName) || !varName[0]) return true;
	AuxStringMapInfo varInfo(scriptObj, varName);
	auto findMod = varInfo.ModsMap().Find(varInfo.modIndex);
	if (!findMod) return true;
	auto findVar = findMod().Find(varName);
	if (!findVar) return true;
	ScopedLock lock(&g_Lock);
	bool cleaned = false;
	for (auto idIter = findVar().Begin(); idIter; ++idIter)
	{
		auto refVal = idIter().GetRef();
		if (!refVal) continue;
		if (LookupFormByID(refVal)) continue;
		idIter.Remove();
		cleaned = true;
	}
	if (findVar().Empty())
	{
		findVar.Remove();
		if (findMod().Empty()) findMod.Remove();
	}
	else *result = (int)findVar().Size();
	if (cleaned && varInfo.isPerm)
		s_dataChangedFlags |= kChangedFlag_AuxStringMaps;
	return true;
}


bool Cmd_AuxStringMapArrayDestroy_Execute(COMMAND_ARGS)
{
	char varName[0x50];
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &varName) || !varName[0]) return true;
	AuxStringMapInfo varInfo(scriptObj, varName);
	auto findMod = varInfo.ModsMap().Find(varInfo.modIndex);
	if (!findMod) return true;
	auto findVar = findMod().Find(varName);
	if (!findVar) return true;
	ScopedLock lock(&g_Lock);
	findVar.Remove();
	if (findMod().Empty()) findMod.Remove();
	if (varInfo.isPerm)
		s_dataChangedFlags |= kChangedFlag_AuxStringMaps;
	return true;
}

#undef DoLaterMaybe