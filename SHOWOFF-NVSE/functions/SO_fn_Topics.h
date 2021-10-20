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





#if _DEBUG





#endif