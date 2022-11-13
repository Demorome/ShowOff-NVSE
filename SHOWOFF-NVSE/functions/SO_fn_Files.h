#pragma once

//#include "json.h"
#include "ShowOffNVSE.h"




DEFINE_COMMAND_ALT_PLUGIN(CreateFolder, CreateDirectory, "", false, kParams_OneString);


//==JSON functions==\\
// Refresher on JSON syntax: https://attacomsian.com/blog/what-is-json
// JSON API docs: https://nlohmann.github.io/json/api/basic_json/

DEFINE_COMMAND_ALT_PLUGIN_EXP(ReadFromJSONFile, ReadFromJSON, "", false, kNVSEParams_OneString_OneOptionalString_OneOptionalInt_OneOptionalBool);
DEFINE_COMMAND_ALT_PLUGIN_EXP(WriteToJSONFile, WriteToJSON, "", false, kNVSEParams_OneElem_OneString_OneOptionalString_OneOptionalInt_TwoOptionalBools);



//caution for desyncs with the other ini functions.

DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(SetINIValue_Cached, SetINIValC, "", false, kNVSEParams_OneString_OneFloatOrString_TwoOptionalStrings);
DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(SetINIInteger_Cached, SetINIIntC, "", false, kNVSEParams_OneString_OneInt_TwoOptionalStrings);

DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(GetINIFloatOrCreate_Cached, GetINIFltOrCreateC, "", false, kNVSEParams_OneString_OneOptionalString_OneOptionalFloat_OneOptionalString);
DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(GetINIStringOrCreate_Cached, GetINIStrOrCreateC, "", false, kNVSEParams_OneString_ThreeOptionalStrings);
DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(GetINIFloatOrDefault_Cached, GetINIFloat_Cached, "", false, kNVSEParams_OneString_OneOptionalString_OneOptionalFloat);
DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(GetINIStringOrDefault_Cached, GetINIString_Cached, "", false, kNVSEParams_OneString_TwoOptionalStrings);

DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(HasINISetting_Cached, HasINISettingC, "", false, kNVSEParams_OneString_OneOptionalString);
DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(SaveCachedIniFile, SaveCachedIni, "", false, kNVSEParams_OneOptionalString);
DEFINE_COMMAND_PLUGIN_EXP_SAFE(ReloadIniCache, "", false, kNVSEParams_OneOptionalString);

DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(ClearFileCacheShowOff, CloseFileSO, "", false, kNVSEParams_OneString_OneNumber);


#if _DEBUG



#endif