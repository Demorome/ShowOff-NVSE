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










#if _DEBUG

#if 0
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
#endif

namespace JsonToNVSE
{
	using valType = tao::json::type;
	using DefaultJsonValue = tao::json::value;
	using ConfigJsonValue = tao::config::value;

	template< template< typename... > class Traits >
	ArrayElementL BasicJsonValueToArrayElem(tao::json::basic_value<Traits> const& val)
	{
		ArrayElementR elem;
		if (auto const type = val.type();
			valType::STRING == type)
		{
			elem = val.get_string().c_str();
		}
		else if (valType::NULL_ == type)
		{
			//NULL isn't representable in Obscript, so it'll just be 0 instead.
			elem = 0.0;
		}
		else if (valType::SIGNED == type || valType::UNSIGNED == type || valType::DOUBLE == type)
		{
			elem = val.as<double>();
		}
		else if (valType::BOOLEAN == type)
		{
			elem = val.get_boolean();
		}
		else {
			throw std::runtime_error("SHOWOFF - GetArrayFromJSON: Non-Exhausive if checks.");
		}
		return elem;
	}

	//Case 1: json val is 1-dimensional -> create 1D array
	//Case 2: json val is multidimensional -> create array with sub-arrays (use recursion)
	template< template< typename... > class Traits >
	NVSEArrayVar* SubElementsToNVSEArray(tao::json::basic_value<Traits> const& val, Script* scriptObj)
	{
		NVSEArrayVar* resArr;
		if (auto const type = val.type();
			valType::ARRAY == type)
		{
			auto const& arr = val.get_array();
			Vector<ArrayElementL> arrData(arr.size());
			for (auto const &value : arr)
			{
				if (auto const type = value.type();
					valType::ARRAY == type || valType::OBJECT == type)
				{
					arrData.Append(ArrayElementL{ SubElementsToNVSEArray(value, scriptObj) });
				}
				else
				{
					arrData.Append(BasicJsonValueToArrayElem(value));
				}
			}
			resArr = g_arrInterface->CreateArray(arrData.Data(), arr.size(), scriptObj);
		}
		else if (valType::OBJECT == type)
		{
			auto const& obj = val.get_object();
			resArr = g_arrInterface->CreateStringMap(nullptr, nullptr, 0, scriptObj); 
			for (auto const& [key, value] : obj)
			{
				if (auto const type = value.type();
					valType::ARRAY == type || valType::OBJECT == type)
				{
					g_arrInterface->SetElement(resArr, ArrayElementL(key.c_str()), ArrayElementL(SubElementsToNVSEArray(value, scriptObj)));
				}
				else
				{
					g_arrInterface->SetElement(resArr, ArrayElementL(key.c_str()), BasicJsonValueToArrayElem(value));
				}
			}
#if 0
			Map<const char*, ArrayElementR> strMapData(obj.size());
			for (auto const &[key, value] : obj)
			{
				if (auto const type = value.type();
					valType::ARRAY == type || valType::OBJECT == type)
				{
					strMapData[key.c_str()] = SubElementsToNVSEArray(value, scriptObj);
				}
				else
				{
					strMapData[key.c_str()] = BasicJsonValueToArrayElem(value);
				}
			}
			//resArr = g_arrInterface->CreateStringMap(strMapData.Keys().Data(), strMapData.Values().Data(), obj.size(), scriptObj);	//todo: find why .Keys() returns garbage, but not .stdKeys()
			resArr = g_arrInterface->CreateStringMap(strMapData.stdKeys().data(), strMapData.Values().Data(), obj.size(), scriptObj);
#endif
		}
		else
		{
			throw std::runtime_error("SHOWOFF - SubElementsToNVSEArray: Function called with invalid json object type.");
		}
		return resArr;
	}

	//Single-element JSON is valid
	//Single array/object is also valid
	//Otherwise, for multiple arrays/objects, you need to wrap in an array/object.
	template< template< typename... > class Traits >
	NVSEArrayVar* GetArrayFromJSON(tao::json::basic_value<Traits> const& val, Script* scriptObj)
	{
		NVSEArrayVar* resArr = nullptr;

		if (auto const type = val.type();
			valType::ARRAY == type || valType::OBJECT == type)
		{
			resArr = SubElementsToNVSEArray(val, scriptObj);
		}
		else //create array with a single element.
		{
			auto const elem = BasicJsonValueToArrayElem(val);
			if (elem.IsValid())
				resArr = CreateArray(&elem, 1, scriptObj);
		}
		return resArr;
	}
}



bool Cmd_ReadArrayFromJSONFile_Execute(COMMAND_ARGS)
{
	using namespace JsonToNVSE;
	*result = 0;
	char json_path[MAX_PATH];  // relative to "Fallout New Vegas" folder.
	char* json_key_path = GetStrArgBuffer();  // the path in the JSON hierarchy	//todo: likely abandonned, too tricky... would have to learn more about JSON pointers.
	//pass "ignore" or "" to ignore this arg.
	
	enum Parser : UInt32
	{
		kParser_JSON = 0, kParser_JAXN, kParser_TaoConfig,
		kParser_Invalid
	} parser = kParser_JSON;
	
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &json_path, json_key_path, &parser) || parser >= kParser_Invalid)
		return true;

	ReplaceChr(json_path, '/', '\\');
	std::string const JSON_Path = GetCurPath() + "\\" + json_path;
	
	if (!std::filesystem::exists(JSON_Path)) return true;
#if 0
	std::string keyPathStr = json_key_path;
#endif

	try
	{
		std::variant<DefaultJsonValue, ConfigJsonValue> jsonVal;
		switch (parser)
		{
		case kParser_JSON:
			jsonVal = tao::json::from_file(JSON_Path);
			break;
		case kParser_JAXN:
			jsonVal = tao::json::jaxn::from_file(JSON_Path);
			break;
		case kParser_TaoConfig:
			jsonVal = tao::config::from_file(JSON_Path);
			break;
		case kParser_Invalid:
		default:
			throw std::runtime_error("SHOWOFF - ReadArrayFromJSONFile >> somehow reached invalid case in Switch statement.");
		}
		std::visit([=](auto&& val) {
			if (auto const resArr = JsonToNVSE::GetArrayFromJSON(val, scriptObj))
				AssignArrayResult(resArr, result);
			}, jsonVal);
	}
	catch (tao::pegtl::parse_error &e)
	{
		if (IsConsoleMode())
			Console_Print("ReadArrayFromJSON >> Could not parse JSON file, likely due to invalid formatting.");
		_MESSAGE("ReadArrayFromJSON >> PARSE ERROR (%s)", e.what());
	}
	
#if 0
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
#endif
	

	return true;
}



bool Cmd_DemoTestFile_Execute(COMMAND_ARGS)
{
	*result = 0;

	std::filesystem::path path = { "test" };
	
	auto i = tao::config::from_file(path);
	_MESSAGE("%i", static_cast<int>(i.type()));

	
	
	return true;
}
#endif