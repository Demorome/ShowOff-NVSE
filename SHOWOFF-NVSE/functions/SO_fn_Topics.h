#pragma once


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

DEFINE_COMMAND_PLUGIN(TopicInfoSetResponseStrings, "", false, kParams_OneForm_OneArray);
bool Cmd_TopicInfoSetResponseStrings_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESTopicInfo* tInfo;
	UInt32 arrID;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &tInfo, &arrID) && IS_ID(tInfo, TESTopicInfo))
	{
		auto inArr = g_arrInterface->LookupArrayByID(arrID);
		if (g_arrInterface->GetContainerType(inArr) != NVSEArrayVarInterface::kArrType_Array)
		{
			return true;
		}
		
		auto const response = ThisStdCall<TESTopicInfoResponse**>(0x61E780, tInfo, nullptr);
		uint32_t loopCounter = 0;
		auto const arrSize = g_arrInterface->GetArraySize(inArr);
		for (auto responseIter = *response; responseIter && loopCounter < arrSize; responseIter = responseIter->next)
		{
			ArrayElementL keyElem = loopCounter, iterElem;
			g_arrInterface->GetElement(inArr, keyElem, iterElem);
			if (iterElem.IsValid())
			{
				responseIter->responseText.Set(iterElem.String());
			}
			loopCounter++;
		}
		*result = 1;
	}
	return true;
}






#if _DEBUG





#endif