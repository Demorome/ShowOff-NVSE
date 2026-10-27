#pragma once

#include <CommandTable.h>
#include <ParamInfos.h>
#include <NVSEParamInfoList.h>

DEFINE_COMMAND_ALT_PLUGIN(ListAddArray_OLD, AddArrayToFormList_OLD, "", false, kParams_OneFormlist_OneArray_OneOptionalIndex);
DEFINE_COMMAND_ALT_PLUGIN_EXP(ListAddArray, AddArrayToFormList, "", false, kNVSEParams_OneForm_OneArray_OneOptionalIndex);

#ifdef _DEBUG
DEFINE_COMMAND_PLUGIN_EXP(ar_Test, "debug array func", false, kNVSEParams_OneArray /*Params_OneArray*/);
DEFINE_COMMAND_PLUGIN(Ar_GetInvalidRefs, "", false, kParams_OneArrayID);  //failed experiment
#endif