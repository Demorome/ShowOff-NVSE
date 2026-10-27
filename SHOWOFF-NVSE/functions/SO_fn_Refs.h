#pragma once

#include <CommandTable.h>
#include <ParamInfos.h>

DEFINE_COMMAND_PLUGIN(GetPosArray, "Returns an array of the 3 axis positions of a reference", true, nullptr);
DEFINE_COMMAND_PLUGIN(GetCompassTargets, , false, kParams_OneOptionalInt);
DEFINE_CMD_COND_PLUGIN(GetIsActivationPromptShown, , false, nullptr);
DEFINE_CMD_COND_PLUGIN(GetProjectileRefIsStuck, , true, nullptr);
DEFINE_COMMAND_PLUGIN(PlaceAtReticleAlt, , true, Cmd_PlaceAtReticleAlt_Params);
DEFINE_COMMAND_PLUGIN(GetWorldOffsetPosArray, "Returns an array of the 3 axis positions of a reference based on the worldspace offsets", true, nullptr);