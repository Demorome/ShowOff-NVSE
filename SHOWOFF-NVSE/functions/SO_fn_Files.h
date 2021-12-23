#pragma once

//#define NOMINMAX 1

#include "json.h"
#include "config.hpp"
#include <tao/json.hpp>
#include "ShowOffNVSE.h"




DEFINE_COMMAND_ALT_PLUGIN(CreateFolder, CreateDirectory, "", false, kParams_OneString);


//==JSON functions==\\
// Refresher on JSON syntax: https://attacomsian.com/blog/what-is-json
// JSON API docs: https://nlohmann.github.io/json/api/basic_json/






#if _DEBUG



DEFINE_COMMAND_PLUGIN(ReadArrayFromJSONFile, "Creates a folder relative to the main folder(\Fallout New Vegas\)", false, kParams_TwoStrings_OneOptionalInt);

using json = nlohmann::json;

// Tries to get a "basic" (non-Array) element from a JSON ref.
bool Get_JSON_Val_As_Basic_NVSE_Elem(json::const_reference json_ref, ArrayElementR& elem);

ArrayElementR Read_JSON_As_NVSE_Elem(json::const_reference json_ref, bool forceArrayType, Script* scriptObj);


#endif