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

DEFINE_COMMAND_PLUGIN(GetCompassTargets, , false, kParams_OneOptionalInt);
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
			|| (includeWhat == IncludeNonHostiles && !iter.isHostile)
			|| (includeWhat == IncludeHostiles && iter.isHostile))
		{
			g_arrInterface->AppendElement(resArr, ArrayElementL(iter.target));
		}
	}
	return true;
}

DEFINE_CMD_COND_PLUGIN(GetIsActivationPromptShown, , false, nullptr);
bool Cmd_GetIsActivationPromptShown_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_HUDMainMenu->tile0A8->GetValueFloat(kTileValue_visible);
	return true;
}
bool Cmd_GetIsActivationPromptShown_Execute(COMMAND_ARGS)
{
	return Cmd_GetIsActivationPromptShown_Eval(nullptr, nullptr, nullptr, result);
}

DEFINE_COMMAND_PLUGIN(GetProjectileRefIsStuck, , true, nullptr);
bool Cmd_GetProjectileRefIsStuck_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (thisObj && thisObj->IsProjectile())
		*result = (static_cast<Projectile*>(thisObj)->projFlags & Projectile::kProjFlag_IsStuck) != 0;
	return true;
}