#pragma once

#if _DEBUG
#include "ExtractArgs_TypeSafe.h"
#endif


DEFINE_COMMAND_PLUGIN(TopicInfoGetResponseStrings, "", false, kParams_OneForm);

/* ==Old code that lead to my findings==
String testStr;
ThisStdCall<void>(0x61E2B0, tInfo, &testStr, false);
*/
bool Cmd_TopicInfoGetResponseStrings_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESTopicInfo* tInfo;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &tInfo) && IS_ID(tInfo, TESTopicInfo))
	{
		auto responseArr = g_arrInterface->CreateArray(nullptr, 0, scriptObj);
		auto response = ThisStdCall<TESTopicInfoResponse**>(0x61E780, tInfo, nullptr);
		for (auto responseIter = *response; responseIter; responseIter = responseIter->next)
		{
			ArrayElementL strElem = responseIter->responseText.CStr();
			g_arrInterface->AppendElement(responseArr, strElem);
			//Console_Print("%s", responseIter->responseText.CStr());
		}
		g_arrInterface->AssignCommandResult(responseArr, result);
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(TopicInfoSetResponseStrings_OLD, "", false, kParams_OneForm_OneArray);
DEFINE_COMMAND_PLUGIN_EXP(TopicInfoSetResponseStrings, "", false, kNVSEParams_OneForm_OneArray);

void TopicInfoSetResponseStrings_Call(NVSEArrayVar *arr, TESTopicInfo* tInfo, double *result)
{
	if (g_arrInterface->GetContainerType(arr) != NVSEArrayVarInterface::kArrType_Array)
		return;
	auto const response = ThisStdCall<TESTopicInfoResponse**>(0x61E780, tInfo, nullptr);
	uint32_t loopCounter = 0;
	auto const arrSize = g_arrInterface->GetArraySize(arr);
	for (auto responseIter = *response; responseIter && loopCounter < arrSize; responseIter = responseIter->next)
	{
		ArrayElementR iterElem;
		g_arrInterface->GetElement(arr, ArrayElementL(iterElem), iterElem);
		if (iterElem.IsValid())
		{
			responseIter->responseText.Set(iterElem.String());
		}
		loopCounter++;
	}
	*result = 1;
}

bool Cmd_TopicInfoSetResponseStrings_OLD_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESTopicInfo* tInfo;
	UInt32 arrID;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &tInfo, &arrID) && IS_ID(tInfo, TESTopicInfo))
	{
		if (const auto inArr = g_arrInterface->LookupArrayByID(arrID))
			TopicInfoSetResponseStrings_Call(inArr, tInfo, result);
	}
	return true;
}

bool Cmd_TopicInfoSetResponseStrings_Execute(COMMAND_ARGS)
{
	*result = 0;	//bSuccess
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
#if Test_TypeSafeExtract
		auto args = (ExtractArgsTuple(eval, kNVSEParams_OneArray));
		//static_assert(std::is_same_v<decltype(args), NVSEArrayVar*>, "lolol");
#endif
		
		auto const tInfo = DYNAMIC_CAST(eval.GetNthArg(0)->GetTESForm(), TESForm, TESTopicInfo);
		auto const inArr = eval.GetNthArg(1)->GetArrayVar();
		if (!tInfo || !inArr)
			return true;
		TopicInfoSetResponseStrings_Call(inArr, tInfo, result);
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(TopicInfoSetNthResponseString, "", false, kParams_OneForm_OneInt_OneString);
bool Cmd_TopicInfoSetNthResponseString_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESTopicInfo* tInfo;
	UInt32 responseIdx;
	char newResponseStr[0x10000];
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &tInfo, &responseIdx, &newResponseStr) && IS_ID(tInfo, TESTopicInfo)
		&& newResponseStr[0])
	{
		auto const response = ThisStdCall<TESTopicInfoResponse**>(0x61E780, tInfo, nullptr);
		uint32_t loopCounter = 0;
		for (auto responseIter = *response; responseIter; responseIter = responseIter->next)
		{
			// Does nothing if responseIdx is too high, but still loops around.
			if (loopCounter == responseIdx)
			{
				responseIter->responseText.Set(newResponseStr);
				*result = 1;
				return true;
			}
			loopCounter++;
		}
	}
	return true;
}







#if _DEBUG





#endif