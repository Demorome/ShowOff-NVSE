#pragma once

#include <CommandTable.h>
#include <ParamInfos.h>

DEFINE_COMMAND_PLUGIN(AuxTimerStart, "Starts an auxvar timer on a form, or can restart it if exists already.", false, kParams_OneString_OneOptionalDouble_OneOptionalInt_OneOptionalForm);
DEFINE_COMMAND_ALT_PLUGIN(AuxTimerStop, AuxTimerErase, "Stops/erases an auxvar timer on a form.", false, kParams_OneString_OneOptionalInt_OneOptionalForm);
DEFINE_COMMAND_PLUGIN(AuxTimerPaused, "Gets/Sets if an auxvar timer is paused.", false, kParams_OneString_OneOptionalInt_OneOptionalForm);
DEFINE_COMMAND_PLUGIN(AuxTimerTimeElapsed, "Gets how much time passed since timer started (limited by the original countdown time, and not accounting for paused time).", false, kParams_OneString_OneOptionalInt_OneOptionalForm);
DEFINE_COMMAND_PLUGIN(AuxTimerTimeToCountdown, "Gets the original countdown time.", false, kParams_OneString_OneOptionalForm);
DEFINE_COMMAND_PLUGIN(AuxTimerTimeLeft, "Gets the time left for the countdown, -1 if it does not exist.", false, kParams_OneString_OneOptionalForm);