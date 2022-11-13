#pragma once



//ripped code from FOSE's ListAddForm
void ListAddArray_Call(BGSListForm* pListForm, NVSEArrayVar* inArr, SInt32 index, double *result/*, const bool checkForDupes = false*/)
{
	UInt32 const size = g_arrInterface->GetArraySize(inArr);
	auto elements = std::make_unique<ArrayElementR[]>(size);
	g_arrInterface->GetElements(inArr, elements.get(), nullptr);

	if (auto const Add_Array_Element_To_FormList = [&](const int elemIndex)
	{
		auto const form = elements[elemIndex].Form();
		if (form == nullptr) 
			return true;  //acts as a continue.
		if (UInt32 const addedAtIndex = pListForm->AddAt(form, index);
			addedAtIndex == eListInvalid)
		{
			*result = 0;	//error!
			return false;
		}
		return true;
	}; index == eListEnd)
	{
		for (int i = 0; i < size; i++) {
			if (!Add_Array_Element_To_FormList(i)) 
				break;
		}
	}
	else
	{
		for (int i = size - 1; i >= 0; i--) {
			if (!Add_Array_Element_To_FormList(i)) 
				break;
		}
	}
}

DEFINE_COMMAND_ALT_PLUGIN(ListAddArray_OLD, AddArrayToFormList_OLD, "", 
	false, kParams_OneFormlist_OneArray_OneOptionalIndex);
bool Cmd_ListAddArray_OLD_Execute(COMMAND_ARGS)
{
	*result = 1;	//bSuccess
	BGSListForm* pListForm;
	UInt32 arrID;
	SInt32 index = eListEnd;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &pListForm, &arrID, &index))
		return true;
	NVSEArrayVar* inArr = g_arrInterface->LookupArrayByID(arrID);
	if (!pListForm || !inArr) 
		return true;
	ListAddArray_Call(pListForm, inArr, index, result);
	return true;
}

DEFINE_COMMAND_ALT_PLUGIN_EXP(ListAddArray, AddArrayToFormList, "", 
	false, kNVSEParams_OneForm_OneArray_OneOptionalIndex);
bool Cmd_ListAddArray_Execute(COMMAND_ARGS)
{
	*result = 1;	//bSuccess
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		auto const pListForm = DYNAMIC_CAST(eval.GetNthArg(0)->GetTESForm(), TESForm, BGSListForm);
		NVSEArrayVar* inArr = eval.GetNthArg(1)->GetArrayVar();
		SInt32 index = eListEnd;
		if (eval.NumArgs() == 3)
			index = eval.GetNthArg(2)->GetInt();
		if (!pListForm || !inArr) 
			return true;
		ListAddArray_Call(pListForm, inArr, index, result);
	}
	return true;
}













#ifdef _DEBUG




DEFINE_COMMAND_PLUGIN_EXP(ar_Test, "debug array func", false, kNVSEParams_OneArray /*Params_OneArray*/);

bool Cmd_ar_Test_Execute(COMMAND_ARGS)
{
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		auto const arrToken = eval.GetNthArg(0);
		auto const arr = arrToken->GetArrayVar();
		auto const size = g_arrInterface->GetArraySize(arr);
		Console_Print("ar_Test >> size of array: %u", size);
	}
	return true;
}


DEFINE_COMMAND_PLUGIN(Ar_GetInvalidRefs, "", false, kParams_OneArrayID);  //failed experiment
bool Cmd_Ar_GetInvalidRefs_Execute(COMMAND_ARGS)
{
	//scrapped the code, had some bad practices.
	return true;
}

#endif