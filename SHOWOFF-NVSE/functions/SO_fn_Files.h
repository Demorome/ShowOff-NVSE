#pragma once

//#include "json.h"
#include "ShowOffNVSE.h"




DEFINE_COMMAND_ALT_PLUGIN(CreateFolder, CreateDirectory, "", false, kParams_OneString);


//==JSON functions==\\
// Refresher on JSON syntax: https://attacomsian.com/blog/what-is-json
// JSON API docs: https://nlohmann.github.io/json/api/basic_json/

DEFINE_COMMAND_PLUGIN_EXP(ReadFromJSONFile, "", false, kNVSEParams_OneString_OneOptionalString_OneOptionalInt_OneOptionalBool);
DEFINE_COMMAND_PLUGIN_EXP(WriteToJSONFile, "", false, kNVSEParams_OneElem_OneString_OneOptionalString_OneOptionalInt_TwoOptionalBools);



//TODO: cache all the INI files in config for fast lookup. SetINIValue will save any changes.
//caution for desyncs with other ini functions (todo: should update JIP instead when done to avoid this issue).

DEFINE_COMMAND_PLUGIN_EXP_SAFE(SetINIValue_Cached, "", false, kNVSEParams_OneString_OneFloatOrString_TwoOptionalStrings);

DEFINE_COMMAND_PLUGIN_EXP_SAFE(GetINIFloatOrCreate_Cached, "", false, kNVSEParams_OneString_OneOptionalString_OneOptionalFloat_OneOptionalString);
DEFINE_COMMAND_PLUGIN_EXP_SAFE(GetINIStringOrCreate_Cached, "", false, kNVSEParams_OneString_ThreeOptionalStrings);
DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(GetINIFloatOrDefault_Cached, GetINIFloat_Cached, "", false, kNVSEParams_OneString_OneOptionalString_OneOptionalFloat);
DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(GetINIStringOrDefault_Cached, GetINIString_Cached, "", false, kNVSEParams_OneString_TwoOptionalStrings);

DEFINE_COMMAND_PLUGIN_EXP_SAFE(HasINISetting_Cached, "", false, kNVSEParams_OneString_OneOptionalString);
DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(SaveCachedIniFile, SaveCachedIni, "", false, kNVSEParams_OneOptionalString);
DEFINE_COMMAND_PLUGIN_EXP_SAFE(ReloadIniCache, "", false, kNVSEParams_OneOptionalString);

DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(ClearFileCacheShowOff, CloseFileSO, "", false, kNVSEParams_OneString_OneNumber);


#if _DEBUG



#endif