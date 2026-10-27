#pragma once

#include <CommandTable.h>
#include <ParamInfos.h>
#include <NVSEParamInfoList.h>

DEFINE_COMMAND_PLUGIN(TopicInfoGetResponseStrings, "", false, kParams_OneForm);
DEFINE_COMMAND_PLUGIN(TopicInfoSetResponseStrings_OLD, "", false, kParams_OneForm_OneArray);
DEFINE_COMMAND_PLUGIN_EXP(TopicInfoSetResponseStrings, "", false, kNVSEParams_OneForm_OneArray);
DEFINE_COMMAND_PLUGIN(TopicInfoSetNthResponseString, "", false, kParams_OneForm_OneInt_OneString);