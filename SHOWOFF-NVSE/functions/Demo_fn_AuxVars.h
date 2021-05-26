#pragma once

//For Auxiliary StringMap Arrays (AuxStringMap, ASM)

#define DoLater 0

DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetSize, AuxStringMapSize, , 0, 1, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetType, AuxStringMapGetType, , 0, 2, kParams_JIP_OneString_OneOptionalForm);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetFloat, AuxStringMapGetFlt, , 0, 2, kParams_JIP_OneString_OneOptionalForm);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetRef, AuxStringMapGetRef, , 0, 2, kParams_JIP_OneString_OneOptionalForm);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetString, AuxStringMapGetStr, , 0, 2, kParams_JIP_OneString_OneOptionalForm);

#if DoLat
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetValue, AuxStringMapGetVal, , 0, 2, kParams_JIP_OneString_OneOptionalForm);
#endif

DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetFirst, AuxStringMapFirst, , 0, 1, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetNext, AuxStringMapNext, , 0, 1, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetKeys, AuxStringMapKeys, , 0, 1, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetAll, AuxStringMapGetAll, , 0, 1, kParams_OneInt);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetFloat, AuxStringMapSetFlt, , 0, 3, kParams_JIP_OneString_OneDouble_OneOptionalForm);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetRef, AuxStringMapSetRef, , 0, 3, kParams_JIP_OneString_OneForm_OneOptionalForm);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetString, AuxStringMapSetStr, , 0, 3, kParams_JIP_TwoStrings_OneOptionalForm);

#if DoLater
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetValue, AuxStringMapSetVal, , 0, 3, kParams_JIP_OneString_OneInt_OneOptionalForm);
#endif

DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayErase, AuxStringMapErase, , 0, 2, kParams_JIP_OneString_OneOptionalForm);

#if DoLater
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayValidate, AuxStringMapValidate, , 0, 1, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayDestroy, AuxStringMapDestroy, , 0, 1, kParams_OneString);
#endif

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

bool Cmd_AuxStringMapArrayGetType_Execute(COMMAND_ARGS)
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
				*result = value->GetType();
		}
	}
	return true;
}

bool Cmd_AuxStringMapArrayGetFloat_Execute(COMMAND_ARGS)
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
				*result = value->GetFlt();
		}
	}
	return true;
}

bool Cmd_AuxStringMapArrayGetRef_Execute(COMMAND_ARGS)
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
				REFR_RES = value->GetRef();
		}
	}
	return true;
}

bool Cmd_AuxStringMapArrayGetString_Execute(COMMAND_ARGS)
{
	const char* resStr = NULL;
	char varName[0x50];
	TESForm* form = NULL;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &form))
	{
		AuxStringMapIDsMap* idsMap = ASMFind(scriptObj, varName);
		if (idsMap)
		{
			AuxVariableValue* value = idsMap->GetPtr(GetSubjectID(form, thisObj));
			if (value)
				resStr = value->GetStr();
		}
	}
	AssignString(PASS_COMMAND_ARGS, resStr);
	return true;
}

#if DoLater
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

AuxStringMapIDsMap::Iterator s_auxStringMapIterator;

NVSEArrayVar* __fastcall AuxStringMapArrayIterator(Script* scriptObj, char* varName, bool getFirst)
{
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
	ArrayElementL elements[2] = { LookupFormByRefID(s_auxStringMapIterator.Key()), s_auxStringMapIterator().GetAsElement() };
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
	s_tempElements.Clear();
	for (auto idIter = idsMap->Begin(); idIter; ++idIter)
		s_tempElements.Append(LookupFormByRefID(idIter.Key()));
	if (!s_tempElements.Empty())
		AssignArrayResult(CreateArray(s_tempElements.Data(), s_tempElements.Size(), scriptObj), result);
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
		s_tempElements.Clear();
		for (auto idIter = varIter().Begin(); idIter; ++idIter)
			s_tempElements.Append(LookupFormByRefID(idIter.Key()));
		SetElement(varsMap, ArrayElementL(varIter.Key()), ArrayElementL(CreateArray(s_tempElements.Data(), s_tempElements.Size(), scriptObj)));
	}
	AssignArrayResult(varsMap, result);
	return true;
}

AuxVariableValue* __fastcall AuxStringMapAddValue(TESForm* form, TESObjectREFR* thisObj, Script* scriptObj, char* varName)
{
	if (varName[0])
	{
		UInt32 keyID = GetSubjectID(form, thisObj);
		if (keyID)
		{
			AuxStringMapInfo varInfo(scriptObj, varName);
#if DoLater
			if (varInfo.isPerm)
				s_dataChangedFlags |= kChangedFlag_RefMaps;
#endif
			return &varInfo.ModsMap()[varInfo.modIndex][varName][keyID];
		}
	}
	return NULL;
}

bool Cmd_AuxStringMapArraySetFloat_Execute(COMMAND_ARGS)
{
	char varName[0x50];
	TESForm* form = NULL;
	double fltVal;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &fltVal, &form))
	{
		AuxVariableValue* value = AuxStringMapAddValue(form, thisObj, scriptObj, varName);
		if (value)
			value->SetFlt(fltVal);
	}
	return true;
}

bool Cmd_AuxStringMapArraySetRef_Execute(COMMAND_ARGS)
{
	char varName[0x50];
	TESForm* form = NULL, * refVal;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &refVal, &form))
	{
		AuxVariableValue* value = AuxStringMapAddValue(form, thisObj, scriptObj, varName);
		if (value)
			value->SetRef(refVal);
	}
	return true;
}

bool Cmd_AuxStringMapArraySetString_Execute(COMMAND_ARGS)
{
	char varName[0x50];
	TESForm* form = NULL;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &s_strValBuffer, &form))
	{
		AuxVariableValue* value = AuxStringMapAddValue(form, thisObj, scriptObj, varName);
		if (value)
			value->SetStr(s_strValBuffer);
	}
	return true;
}

#if DoLater
bool Cmd_AuxStringMapArraySetValue_Execute(COMMAND_ARGS)  //undocumented, prolly since it's a bit unwieldy having to use the array middleman
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
				GetElements(srcArr, &element, NULL);
				value->SetElem(element);
			}
		}
	}
	return true;
}
#endif

bool Cmd_AuxStringMapArrayErase_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	TESForm* form = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &form) || !varName[0]) return true;
	AuxStringMapInfo varInfo(scriptObj, varName);
	auto findMod = varInfo.ModsMap().Find(varInfo.modIndex);
	if (!findMod) return true;
	auto findVar = findMod().Find(varName);
	if (!findVar) return true;
	auto findID = findVar().Find(GetSubjectID(form, thisObj));
	if (!findID) return true;
	findID.Remove();
	if (findVar().Empty())
	{
		findVar.Remove();
		if (findMod().Empty()) findMod.Remove();
	}
	else *result = (int)findVar().Size();
#if DoLater
	if (varInfo.isPerm)
		s_dataChangedFlags |= kChangedFlag_RefMaps;
#endif
	return true;
}

#if DoLater
//  Make this clear out entries with VALUES that are invalid, instead of checking the keys.
bool Cmd_AuxStringMapArrayValidate_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &varName) || !varName[0]) return true;
	AuxStringMapInfo varInfo(scriptObj, varName);
	auto findMod = varInfo.ModsMap().Find(varInfo.modIndex);
	if (!findMod) return true;
	auto findVar = findMod().Find(varName);
	if (!findVar) return true;
	bool cleaned = false;
	for (auto idIter = findVar().Begin(); idIter; ++idIter)
	{
		if (LookupFormByRefID(idIter.Key())) continue;
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
		s_dataChangedFlags |= kChangedFlag_RefMaps;
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
	findVar.Remove();
	if (findMod().Empty()) findMod.Remove();
	if (varInfo.isPerm)
		s_dataChangedFlags |= kChangedFlag_RefMaps;
	return true;
}

#endif