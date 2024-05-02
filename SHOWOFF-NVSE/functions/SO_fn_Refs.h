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

auto Cmd_PlaceAtReticle = (bool(__cdecl*)(COMMAND_ARGS)) 0x5DF040;
static ParamInfo Cmd_PlaceAtReticle_Params[] = {
	{ "formToPlace",	kParamType_AnyForm, 0 },
	{ "count",			kParamType_Integer,	1 },
	{ "minDistance",	kParamType_Float,	1 },
	{ "maxDistance",	kParamType_Float,	1 },
};

void CreateCommands_Refs(const NVSECommandBuilder& builder)
{
	builder.Create("PlaceAtReticleAlt", kRetnType_Form,
		{	{ "formToPlace",	kParamType_AnyForm, 0 },
			{ "count",			kParamType_Integer,	1 },
			{ "minDistance",	kParamType_Float,	1 },
			{ "maxDistance",	kParamType_Float,	1 },
			{ "healthPercent",	kParamType_Float,	1 },
		},	
		false,
		[](COMMAND_ARGS)
		{
			TESForm* formToPlace;
			UInt32 count = 1;
			float minDistance = 0.0f, maxDistance = 0.0f;
			PlaceAtReticleAlt::g_healthPercent = 1.0f;

			PlaceAtReticleAlt::g_lastPlacedRef = nullptr;
			*result = 0;

			if (ExtractArgsEx(EXTRACT_ARGS_EX, &formToPlace, &count, &minDistance, &maxDistance, &PlaceAtReticleAlt::g_healthPercent))
			{
				// Ugly hack to call the vanilla func by convincing it there's only 4 args to extract.
				{
					auto const oldNumArgs = NUM_ARGS;
					NUM_ARGS = std::size(Cmd_PlaceAtReticle_Params);
					Cmd_PlaceAtReticle(PASS_COMMAND_ARGS);
					NUM_ARGS = oldNumArgs;
				}

				if (auto* refRes = PlaceAtReticleAlt::g_lastPlacedRef)
				{
					REFR_RES = refRes->refID;
				}
			}

			// Reset the global, so it doesn't mess with regular PlaceAtReticle calls.
			PlaceAtReticleAlt::g_healthPercent = 1.0f;

			return true;
		}
	);
}