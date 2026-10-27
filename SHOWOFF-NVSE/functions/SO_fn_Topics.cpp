#include "SO_fn_Topics.h"

#include <GameForms.h>
#include <GameRTTI.h>

extern NVSEArrayVarInterface* g_arrInterface;
extern bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);

/* ==Old code that lead to my findings==
String testStr;
ThisCall<void>(0x61E2B0, tInfo, &testStr, false);
*/
bool Cmd_TopicInfoGetResponseStrings_Execute(COMMAND_ARGS) {
	*result = 0;
	TESTopicInfo* tInfo = nullptr;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &tInfo) && tInfo && IS_ID(tInfo, TESTopicInfo)) {
		auto responseArr = g_arrInterface->CreateArray(nullptr, 0, scriptObj);
		auto response = ThisCall<TESTopicInfoResponse**>(0x61E780, tInfo, nullptr);
		for (auto responseIter = *response; responseIter; responseIter = responseIter->next) {
			ArrayElementL strElem = responseIter->responseText.CStr();
			g_arrInterface->AppendElement(responseArr, strElem);
			//Console_Print("%s", responseIter->responseText.CStr());
		}
		g_arrInterface->AssignCommandResult(responseArr, result);
	}
	return true;
}

void TopicInfoSetResponseStrings_Call(NVSEArrayVar* arr, TESTopicInfo* tInfo, double* result) {
	if (g_arrInterface->GetContainerType(arr) != NVSEArrayVarInterface::kArrType_Array)
		return;
	auto const response = ThisCall<TESTopicInfoResponse**>(0x61E780, tInfo, nullptr);
	uint32_t loopCounter = 0;
	auto const arrSize = g_arrInterface->GetArraySize(arr);
	for (auto responseIter = *response; responseIter && loopCounter < arrSize; responseIter = responseIter->next) {
		ArrayElementR iterElem;
		g_arrInterface->GetElement(arr, ArrayElementL(iterElem), iterElem);
		if (iterElem.IsValid()) {
			responseIter->responseText.Set(iterElem.String());
		}
		loopCounter++;
	}
	*result = 1;
}

bool Cmd_TopicInfoSetResponseStrings_OLD_Execute(COMMAND_ARGS) {
	*result = 0;
	TESTopicInfo* tInfo = nullptr;
	uint32_t arrID = UINT32_MAX;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &tInfo, &arrID) && tInfo && IS_ID(tInfo, TESTopicInfo)) {
		if (const auto inArr = g_arrInterface->LookupArrayByID(arrID))
			TopicInfoSetResponseStrings_Call(inArr, tInfo, result);
	}
	return true;
}

bool Cmd_TopicInfoSetResponseStrings_Execute(COMMAND_ARGS) {
	*result = 0;	//bSuccess
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs()) {
		auto const tInfo = DYNAMIC_CAST(eval.GetNthArg(0)->GetTESForm(), TESForm, TESTopicInfo);
		auto const inArr = eval.GetNthArg(1)->GetArrayVar();
		if (!tInfo || !inArr)
			return true;
		TopicInfoSetResponseStrings_Call(inArr, tInfo, result);
	}
	return true;
}

bool Cmd_TopicInfoSetNthResponseString_Execute(COMMAND_ARGS) {
	*result = 0;
	TESTopicInfo* tInfo = nullptr;
	uint32_t responseIdx = UINT32_MAX;
	char newResponseStr[0x10000] = {};
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &tInfo, &responseIdx, &newResponseStr) && tInfo && IS_ID(tInfo, TESTopicInfo)
		&& newResponseStr[0]) {
		auto const response = ThisCall<TESTopicInfoResponse**>(0x61E780, tInfo, nullptr);
		uint32_t loopCounter = 0;
		for (auto responseIter = *response; responseIter; responseIter = responseIter->next) {
			// Does nothing if responseIdx is too high, but still loops around.
			if (loopCounter == responseIdx) {
				responseIter->responseText.Set(newResponseStr);
				*result = 1;
				return true;
			}
			loopCounter++;
		}
	}
	return true;
}