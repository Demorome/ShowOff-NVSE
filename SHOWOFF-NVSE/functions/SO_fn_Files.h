#pragma once

//#define NOMINMAX 1

//#include "json.h"
#include "config.hpp"
#include <tao/json.hpp>
#include "ShowOffNVSE.h"




DEFINE_COMMAND_ALT_PLUGIN(CreateFolder, CreateDirectory, "", false, kParams_OneString);


//==JSON functions==\\
// Refresher on JSON syntax: https://attacomsian.com/blog/what-is-json
// JSON API docs: https://nlohmann.github.io/json/api/basic_json/

DEFINE_COMMAND_PLUGIN(ReadArrayFromJSONFile, "", false, kParams_TwoStrings_OneOptionalInt);







#if _DEBUG

DEFINE_COMMAND_PLUGIN(WriteArrayToJSONFile, "", false, kParams_TwoStrings_OneOptionalInt);




#endif