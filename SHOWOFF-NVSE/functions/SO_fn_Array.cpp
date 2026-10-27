#include "SO_fn_Array.h"

#include <GameForms.h>
#include <GameRTTI.h>
#include <decoding.h>

#include <memory>

extern NVSEArrayVarInterface* g_arrInterface;
extern bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);

//ripped code from FOSE's ListAddForm
void ListAddArray_Call(BGSListForm* pListForm, NVSEArrayVar* inArr, int32_t index, double* result/*, const bool checkForDupes = false*/) {
	uint32_t const size = g_arrInterface->GetArraySize(inArr);
	auto elements = std::make_unique<ArrayElementR[]>(size);
	g_arrInterface->GetElements(inArr, elements.get(), nullptr);

	if (auto const Add_Array_Element_To_FormList = [&](const int elemIndex) {
		auto const form = elements[elemIndex].Form();
		if (form == nullptr)
			return true;  //acts as a continue.
			if (uint32_t const addedAtIndex = pListForm->AddAt(form, index);
				addedAtIndex == eListInvalid) {
			*result = 0;	//error!
			return false;
		}
		return true;
	}; index == eListEnd) {
		for (uint32_t i = 0; i < size; i++) {
			if (!Add_Array_Element_To_FormList(i))
				break;
		}
	}
	else {
		for (int32_t i = size - 1; i >= 0; i--) {
			if (!Add_Array_Element_To_FormList(i))
				break;
		}
	}
}

bool Cmd_ListAddArray_OLD_Execute(COMMAND_ARGS) {
	*result = 1;	//bSuccess
	BGSListForm* pListForm = nullptr;
	uint32_t arrID = UINT32_MAX;
	int32_t index = eListEnd;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &pListForm, &arrID, &index))
		return true;

	if (!pListForm)
		return true;

	NVSEArrayVar* inArr = g_arrInterface->LookupArrayByID(arrID);
	if (!inArr)
		return true;
	ListAddArray_Call(pListForm, inArr, index, result);
	return true;
}

bool Cmd_ListAddArray_Execute(COMMAND_ARGS) {
	*result = 1;	//bSuccess
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs()) {
		auto const pListForm = DYNAMIC_CAST(eval.GetNthArg(0)->GetTESForm(), TESForm, BGSListForm);
		NVSEArrayVar* inArr = eval.GetNthArg(1)->GetArrayVar();
		int32_t index = eListEnd;
		if (eval.NumArgs() == 3)
			index = eval.GetNthArg(2)->GetInt();
		if (!pListForm || !inArr)
			return true;
		ListAddArray_Call(pListForm, inArr, index, result);
	}
	return true;
}

#ifdef _DEBUG
bool Cmd_ar_Test_Execute(COMMAND_ARGS) {
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs()) {
		auto const arrToken = eval.GetNthArg(0);
		auto const arr = arrToken->GetArrayVar();
		auto const size = g_arrInterface->GetArraySize(arr);
		Console_Print("ar_Test >> size of array: %u", size);
	}
	return true;
}

bool Cmd_Ar_GetInvalidRefs_Execute(COMMAND_ARGS) {
	//scrapped the code, had some bad practices.
	return true;
}
#endif