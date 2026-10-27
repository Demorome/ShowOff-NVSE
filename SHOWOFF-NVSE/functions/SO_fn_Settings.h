#pragma once

#include <CommandTable.h>
#include <ParamInfos.h>

DEFINE_COMMAND_PLUGIN(IsGameSetting, "Checks if a string refers to a valid Gamesetting", false, kParams_OneString);
DEFINE_COMMAND_PLUGIN(IsINISetting, "Checks if a string refers to a valid FalloutPrefs.ini / Fallout.ini setting.", false, kParams_OneString);
DEFINE_COMMAND_PLUGIN(ModNumericINISetting, "", false, kParams_OneString_OneFloat);
DEFINE_COMMAND_PLUGIN(ModNumericINISettingAlt, "", false, kParams_OneString_OneFloat);
DEFINE_COMMAND_PLUGIN(ModNumericGameSetting, "", false, kParams_OneString_OneFloat);