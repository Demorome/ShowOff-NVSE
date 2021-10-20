#include "SO_fn_Files.h"
#include "nvse/Utilities.h"

#include <filesystem>
#include <fstream>

//Made by anhatthezoo, requested by Trooper.
bool Cmd_CreateFolder_Execute(COMMAND_ARGS)
{
	*result = false;
	char folderPath[MAX_PATH]; // relative to "Fallout New Vegas" folder.
	folderPath[0] = 0;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &folderPath) && folderPath[0])
	{
		*result = std::filesystem::create_directories(GetFalloutDirectory() + folderPath);
	}
	return true;
}

bool Get_JSON_Val_As_Basic_NVSE_Elem(json::const_reference json_ref, ArrayElementR& elem)
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
		elem = ConvertStrToElem(json_ref.get<std::string>());
	}
	else
	{
		Log("Get_JSON_Val_As_Basic_NVSE_Elem - Warning - Unexpected primitive type received.");
	}
	return gotVal;
}

ArrayElementR Read_JSON_As_NVSE_Elem(json::const_reference json_ref, bool forceArrayType, Script* scriptObj)
{
	ArrayElementR json_as_elem;  //return val.

	if (Get_JSON_Val_As_Basic_NVSE_Elem(json_ref, json_as_elem)) return json_as_elem;
	// Code below is only for array-type elements.

	std::vector<std::string> strMapKeys;  // filled when creating a StringMap-type array
	Vector<ArrayElementR> elems;
	for (auto& iter : json_ref.items())
	{
		//-----Handle Values
		ArrayElementR elem = Read_JSON_As_NVSE_Elem(iter.value(), forceArrayType, scriptObj);  // Recursion! todo: fix infinite recursion
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
	std::string JSON_Path = GetCurPath() + "\\" + json_path;
	if (!std::filesystem::exists(JSON_Path)) return true;
	std::string keyPathStr = json_key_path;
	
	NVSEArrayVar* resArr = nullptr;  //assign this value as result.
	try
	{
		std::ifstream i(JSON_Path);
		json j;
		i >> j;
		try
		{
			json::const_reference ref = j.at(json::json_pointer(keyPathStr));  //todo: figure out crashes here
			ArrayElementR elem;
			if (Get_JSON_Val_As_Basic_NVSE_Elem(ref, elem))
			{
				// Force the creation of a single-element array; Read_JSON_As_NVSE_Elem() would make it a simple element instead.
				resArr = CreateArray(&elem, 1, scriptObj);
			}
			else if (ref.is_array() || ref.is_object())
			{
				// Port the contents of the JSON ref to an NVSE array.
				resArr = Read_JSON_As_NVSE_Elem(ref, forceArrayType, scriptObj).Array();  //todo: figure out stack overflow crashes here
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
		// Log statements copied from kNVSE.
		Log("The JSON is incorrectly formatted! It will not be applied.");
		Log(FormatString("JSON error: %s\n", e.what()));
	}
	AssignArrayResult(resArr, result);  //todo: check what happens if empty array is passed!
	return true;
}