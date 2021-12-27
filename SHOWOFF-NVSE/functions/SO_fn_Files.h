#pragma once

//#define NOMINMAX 1

//#include "json.h"
#include "ShowOffNVSE.h"




DEFINE_COMMAND_ALT_PLUGIN(CreateFolder, CreateDirectory, "", false, kParams_OneString);


//==JSON functions==\\
// Refresher on JSON syntax: https://attacomsian.com/blog/what-is-json
// JSON API docs: https://nlohmann.github.io/json/api/basic_json/

DEFINE_COMMAND_PLUGIN_EXP(ReadFromJSONFile, "", false, kNVSEParams_OneString_OneOptionalString_OneOptionalInt);







#if _DEBUG

DEFINE_COMMAND_PLUGIN_EXP(WriteToJSONFile, "", false, kNVSEParams_OneElem_OneString_OneOptionalString_OneOptionalInt);




#endif