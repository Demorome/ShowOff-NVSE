#pragma once
#include "MiscHooks.h"

DEFINE_COMMAND_PLUGIN(GetPosArray, "Returns an array of the 3 axis positions of a reference",
                      true, nullptr);
bool Cmd_GetPosArray_Execute(COMMAND_ARGS)
{
	ArrayElementL const posElems[] =
		{ thisObj->posX, thisObj->posY, thisObj->posZ };
	auto const arr = g_arrInterface->CreateArray(posElems, 3, scriptObj);
	g_arrInterface->AssignCommandResult(arr, result);
	return true;
}

DEFINE_COMMAND_PLUGIN(GetCompassTargets, , 0, kParams_OneOptionalInt);
bool Cmd_GetCompassTargets_Execute(COMMAND_ARGS)
{
	*result = 0;

	enum TargetFlag : UInt32
	{
		IncludeAll = 0,
		IncludeNonHostiles = 1,
		IncludeHostiles = 2,
	} includeWhat = IncludeAll;

	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &includeWhat))
		return true;

	NVSEArrayVar* resArr = g_arrInterface->CreateArray(nullptr, 0, scriptObj);
	g_arrInterface->AssignCommandResult(resArr, result);
	for (auto const iter : GetCompassTargets::g_TargetsInCompass)
	{
		if (includeWhat == IncludeAll
			|| (includeWhat == IncludeNonHostiles && !iter->isHostile)
			|| (includeWhat == IncludeHostiles && iter->isHostile))
		{
			g_arrInterface->AppendElement(resArr, ArrayElementL(iter->target));
		}
	}
	return true;
}