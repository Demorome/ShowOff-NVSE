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

DEFINE_COMMAND_PLUGIN_EXP_SAFE(SetINIValue, "", false, kNVSEParams_OneString_OneFloatOrString_TwoOptionalStrings_TwoOptionalBools);
DEFINE_COMMAND_PLUGIN_EXP_SAFE(SetINIFloatAlt, "", false, kNVSEParams_OneString_OneFloat_TwoOptionalStrings_TwoOptionalBools);
DEFINE_COMMAND_PLUGIN_EXP_SAFE(SetINIStringAlt, "", false, kNVSEParams_TwoStrings_TwoOptionalStrings_TwoOptionalBools);

DEFINE_COMMAND_PLUGIN_EXP_SAFE(GetINIFloatOrCreate, "", false, kNVSEParams_OneString_OneOptionalString_OneOptionalFloat_OneOptionalString_TwoOptionalBools);
DEFINE_COMMAND_PLUGIN_EXP_SAFE(GetINIStringOrCreate, "", false, kNVSEParams_OneString_ThreeOptionalStrings_TwoOptionalBools);
DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(GetINIFloatOrDefault, GetINIFloatAlt, "", false, kNVSEParams_OneString_OneOptionalString_OneOptionalFloat_OneOptionalBool);
DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(GetINIStringOrDefault, GetINIStringAlt, "", false, kNVSEParams_OneString_TwoOptionalStrings_OneOptionalBool);

DEFINE_COMMAND_PLUGIN_EXP_SAFE(HasINISetting, "", false, kNVSEParams_OneString_OneOptionalString_OneOptionalBool);

DEFINE_COMMAND_ALT_PLUGIN_EXP_SAFE(ClearFileCacheShowOff, CloseFileSO, "", false, kNVSEParams_OneString_OneNumber);


#if _DEBUG









#endif