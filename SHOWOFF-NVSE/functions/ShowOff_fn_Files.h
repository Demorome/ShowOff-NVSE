#pragma once

#include <filesystem>
#include "internal/json.h"
#include <fstream>
#include <utility>

DEFINE_COMMAND_PLUGIN(ReadArrayFromJSON, , 0, 2, kParams_TwoStrings);




using json = nlohmann::json;

// Refresher on JSON syntax: https://attacomsian.com/blog/what-is-json
// JSON API docs: https://nlohmann.github.io/json/api/basic_json/

// Tries to get a "basic" (non-Array) element from a JSON ref.
bool Get_JSON_Val_As_Basic_NVSE_Elem(json::const_reference json_ref, ArrayElementR &elem)
{
	if (!json_ref.is_primitive()) return false;

	bool gotVal = false;  //return val.
	if (json_ref.is_number())
	{
		elem = json_ref.get<double>();
		gotVal = true;
	}
	else if (json_ref.is_string())
	{
		// Value can be either a Form or a String - check for Form first, otherwise default to String.

		//todo: Handle possible Form Strings!!
		// check if first char is "@"
		// Get the mod name.
		// Get the partial formId
		// Return the constructed form element.

		/* JIP syntax:
			*Forms must be prefixed with '@', followed by the source file name, then ':',
			and finally the six-digit reference ID (i.e. without the mod index). Example (for Craig Boone): @FalloutNV.esm:096BCE
		 */
		
		if (???)  // Try_Convert_Str_To_Form
		{
			
		}
		else  // Return string - default case
		{
			elem = json_ref.get<std::string>().c_str();
		}
		gotVal = true;
	}
	else
	{
		Log("Get_JSON_Val_As_Basic_NVSE_Elem - Warning - Unexpected primitive type received.");
	}
	return gotVal;
}



ArrayElementR Read_JSON_As_NVSE_Elem(json::const_reference json_ref, bool forceArrayType, Script *scriptObj)
{
	ArrayElementR json_as_elem;  //return val.
	std::vector<std::string> strMapKeys;  // filled when creating a StringMap-type array
	Vector<ArrayElementR> elems;

	

	for (auto& iter : json_ref.items())
	{
		//-----Handle Values
		ArrayElementR elem = Read_JSON_As_NVSE_Elem(iter.value(), forceArrayType, scriptObj);  // Recursion!
		elems.Append(elem);

		//-----Handle Keys
		if (forceArrayType) continue;  // Array-type array will have its keys automatically populated; do nothing.

		std::string strKey = iter.key();
		if (strKey.empty())  // guaranteed if "json_ref" is primitive (number, string, etc). Other cases?
		{
			Log("Read_JSON_As_NVSE_Elem() - ERROR - Key string was somehow null.");
			return json_as_elem;  // return invalid elem (would otherwise have to deal with Keys & Elems size mismatch).
		}
		
		strMapKeys.emplace_back(strKey);
	}

	//-----Assign Array value
	if (!elems.Empty())
	{
		if (forceArrayType)
		{
			json_as_elem = CreateArray(&elems[0], elems.Size(), scriptObj);
		}
		else  
		{
			if (strMapKeys.size() == elems.Size())  // slightly costly, but it'll be kept until I'm sure it's not needed.
			{
				const char* test = strMapKeys[0].c_str();  //todo: verify this hack!!
				json_as_elem = CreateStringMap(&test, &elems[0], elems.Size(), scriptObj);
			}
			else
			{
				_MESSAGE("Read_JSON_As_NVSE_Elem() - Mismatch of strMapKeys and elem size: %u vs %u", strMapKeys.size(), elems.Size());
			}
		}
	}
	
	return json_as_elem;
}

bool Cmd_ReadArrayFromJSON_Execute(COMMAND_ARGS)
{
	*result = 0;
	char json_path[MAX_PATH];  // relative to "Fallout New Vegas" folder.
	char* json_key_path = GetStrArgBuffer();  // the path in the JSON hierarchy
	UInt32 forceArrayType = false;  // Optional, forces the returned arrays to be "array" type (instead of sometimes appearing as "StringMap", such as in the case of a JSON Object).
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &json_path, json_key_path, &forceArrayType)) return true;

	ReplaceChr(json_path, '/', '\\');
	std::string JSON_Path = GetCurPath() + json_path;
	std::string keyPathStr = json_key_path;
	
	NVSEArrayVar* resArr = nullptr;
	
	try
	{
		std::ifstream i(JSON_Path);
		json j;
		i >> j;

		try
		{
			json::const_reference ref = j.at(json::json_pointer(keyPathStr));

			if (ref.is_primitive())
			{
				
			}
			else if (ref.is_array() || ref.is_object())
			{
				// Port the contents of the JSON ref to an NVSE array.
				resArr = Read_JSON_As_NVSE_Elem(ref, forceArrayType, scriptObj).Array();
			}
		}
		catch (json::out_of_range& e)
		{
			Log(e.what());
		}
		catch (json::parse_error& e)
		{
			Log(e.what());
		}
	}
	catch (json::exception& e)
	{
		Log("The JSON is incorrectly formatted! It will not be applied.");
		Log(FormatString("JSON error: %s\n", e.what()));
	}

	AssignArrayResult(resArr, result);  //todo: check what happens if empty array is passed!
	return true;
}