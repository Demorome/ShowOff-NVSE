#pragma once

#include <CommandTable.h>
#include <ParamInfos.h>

DEFINE_COMMAND_PLUGIN(SetFactionCombatReactionTemp, "Sets the combat reaction between two factions. Unlike SetAlly/SetEnemy, isn't savebaked.", false, kParams_SetFactionCombatReaction);
DEFINE_COMMAND_ALT_PLUGIN(SetAllyTemp, SetAllyShowOff, "Sets the combat reaction between two factions to something friendly. Unlike SetAlly, isn't savebaked.", false, kParams_SetAllyOrEnemyTemp);
DEFINE_COMMAND_ALT_PLUGIN(SetEnemyTemp, SetEnemyShowOff, "Sets the combat reaction between two factions to enemy/neutral status. Unlike SetEnemy, isn't savebaked.", false, kParams_SetAllyOrEnemyTemp);