#pragma once

#include "json.h"
#include "ShowOffNVSE.h"

// Refresher on JSON syntax: https://attacomsian.com/blog/what-is-json
// JSON API docs: https://nlohmann.github.io/json/api/basic_json/



DEFINE_COMMAND_PLUGIN(ReadArrayFromJSON, "", false, kParams_TwoStrings_OneOptionalInt);

DEFINE_COMMAND_PLUGIN(CreateFolder, , 0, 1, kParams_OneString);


using json = nlohmann::json;

// Tries to get a "basic" (non-Array) element from a JSON ref.
bool Get_JSON_Val_As_Basic_NVSE_Elem(json::const_reference json_ref, ArrayElementR& elem);

ArrayElementR Read_JSON_As_NVSE_Elem(json::const_reference json_ref, bool forceArrayType, Script* scriptObj);

bool Cmd_CreateFolder_Execute(COMMAND_ARGS);