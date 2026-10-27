#pragma once

#include <CommandTable.h>
#include <ParamInfos.h>

DEFINE_COMMAND_ALT_PLUGIN(DumpFormList, FListDump, "", false, kParams_OneFormList_OneOptionalString_OneOptionalInt);
DEFINE_CMD_COND_PLUGIN(ConditionPrint, "Returns 1, and prints a message to console. Meant for testing if previous conditions passed.", false, NULL);
DEFINE_COMMAND_PLUGIN(GetShowOffDebugMode, "", false, NULL);
DEFINE_COMMAND_PLUGIN(SetShowOffDebugMode, "Set to 1 to enable debug prints for certain ShowOff functions.", false, kParams_OneInt);
DEFINE_COMMAND_ALT_PLUGIN(IsBaseForm, IsBasedForm, "", false, kParams_OneForm);
DEFINE_COMMAND_PLUGIN(IsOutsideMainThread, "", false, NULL);
DEFINE_COMMAND_PLUGIN(RefillPlayerAmmo, "", false, kParams_OneInt);
DEFINE_COMMAND_PLUGIN(KillAllHostiles, "", false, kParams_OneOptionalActorRef);
// TODO
DEFINE_COMMAND_ALT_PLUGIN(SetFlyCamera, SetFC, "", false, kParams_OneInt_FiveOptionalFloats);

#if _DEBUG
#include <NVSEParamInfoList.h>
#include <ExtractArgs_TypeSafe.h>
DEFINE_COMMAND_PLUGIN(Debug_UpdateWeather, "", false, NULL);
DEFINE_COMMAND_PLUGIN_EXP_SAFE(Debug_DispatchEvent, "", false, kNVSEParams_TwoNums_OneArray_OneStr_ThreeForms);
#endif