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



namespace JsonToNVSE
{
	using valType = tao::json::type;
	using DefaultJsonValue = tao::json::value;
	using ConfigJsonValue = tao::config::value;

	template< template< typename... > class Traits >
	ArrayElementL BasicJsonValueToArrayElem(tao::json::basic_value<Traits> const& val)
	{
		ArrayElementR elem;
		if (val.is_string())
		{
			elem = val.get_string().c_str();
		}
		else if (val.is_null())
		{
			//NULL isn't representable in Obscript, so it'll be an invalid array.
			elem = static_cast<NVSEArrayVar*>(nullptr);
		}
		else if (val.is_number())
		{
			elem = val.as<double>();
		}
		else if (val.is_boolean())
		{
			elem = val.get_boolean();
		}
		else {
			throw std::logic_error("SHOWOFF - GetArrayFromJSON: Non-Exhausive if checks.");
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
			for (auto const& value : arr)
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
		}
		else
		{
			throw std::logic_error("SHOWOFF - SubElementsToNVSEArray: Function called with invalid json object type.");
		}
		return resArr;
	}

	//Single-element JSON is valid
	//Single array/object is also valid
	//Otherwise, for multiple arrays/objects, you need to wrap in an array/object.
	template< template< typename... > class Traits >
	ArrayElementL GetNVSEFromJSON(tao::json::basic_value<Traits> const& val, Script* scriptObj)
	{
		if (auto const type = val.type();
			valType::ARRAY == type || valType::OBJECT == type)
		{
			return SubElementsToNVSEArray(val, scriptObj);
		}
		
		//return single element (NOT in an array).
		return BasicJsonValueToArrayElem(val);
	}

	enum Parser : UInt32
	{
		kParser_JSON = 0, kParser_JAXN, kParser_TaoConfig,
		kParser_Invalid
	};

	using JsonValueVariant = std::variant<DefaultJsonValue, ConfigJsonValue>;
	using JsonValueVariantRef = std::variant<std::reference_wrapper<DefaultJsonValue>, std::reference_wrapper<ConfigJsonValue>>;

	//MUST be called with valid Parser type.
	std::optional<JsonValueVariant> ReadJSONWithParser(Parser parser, const std::string &JSON_Path, const std::string_view& funcName)
	{
		try
		{
			JsonValueVariant retn;
			switch (parser)
			{
			case kParser_JSON:
				retn = tao::json::from_file(JSON_Path);
				break;
			case kParser_JAXN:
				retn = tao::json::jaxn::from_file(JSON_Path);
				break;
			case kParser_TaoConfig:
				retn = tao::config::from_file(JSON_Path);
				break;
			case kParser_Invalid:
			default:
				throw std::logic_error("SHOWOFF - ReadJSONWithParser >> somehow reached invalid case in Switch statement.");
			}
			return retn;
		}
		catch (tao::pegtl::parse_error& e)
		{
			if (IsConsoleMode())
				Console_Print("%s >> Could not parse JSON file, likely due to invalid formatting.", funcName.data());
			_MESSAGE("ReadArrayFromJSONFile >> PARSE ERROR (%s)", e.what());
		}
		return {};
	}

	std::optional<JsonValueVariantRef> GetJSONValueAtJSONPointer(const JsonValueVariant &value, const std::string& jsonPointer, const std::string_view& funcName)
	{
		try
		{
			return std::visit([&](auto&& val) -> JsonValueVariantRef {
				return val.at(tao::json::pointer(jsonPointer));
				//todo: ensure this always returns a valid value, or a caught exception!
				//TODO: also ensure it isn't making a copy!
				}, const_cast<JsonValueVariant&>(value));
		}
		catch (const std::runtime_error& e)
		{
			if (IsConsoleMode())
				Console_Print("%s >> Invalid JSON pointer arg.", funcName.data());
			_MESSAGE("%s >> JSON POINTER ERROR (%s)", funcName.data(), e.what());
		}
		catch (const std::out_of_range& e)
		{
			if (IsConsoleMode())
				Console_Print("%s >> Invalid JSON pointer arg.", funcName.data());
			_MESSAGE("%s >> JSON POINTER ERROR (OUT OF RANGE) (%s)", funcName.data(), e.what());
		}
		catch (const std::invalid_argument& e)
		{
			if (IsConsoleMode())
				Console_Print("%s >> Invalid JSON pointer arg.", funcName.data());
			_MESSAGE("%s >> JSON POINTER ERROR (Invalid Arg) (%s)", funcName.data(), e.what());
		}
		return {};
	}
	

	

}



bool Cmd_ReadFromJSONFile_Execute(COMMAND_ARGS)
{
	using namespace JsonToNVSE;
	*result = 0;
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		std::string json_path = eval.GetNthArg(0)->GetString();
		std::string jsonPointer = "";	// the path in the JSON hierarchy, pass "" to ignore this arg.
		Parser parser = kParser_JSON;
		if (auto const numArgs = eval.NumArgs();
			numArgs >= 2)
		{
			jsonPointer = eval.GetNthArg(1)->GetString();
			if (numArgs >= 3)
			{
				parser = static_cast<Parser>(eval.GetNthArg(2)->GetInt());
				if (parser >= kParser_Invalid)
					return true;
			}
		}
		std::ranges::replace(json_path, '/', '\\');
		std::string const JSON_Path = GetCurPath() + "\\" + std::move(json_path);
		if (!std::filesystem::exists(JSON_Path))
			return true;

		constexpr std::string_view funcName = { "ReadFromJSONFile" };
		if (auto jsonVal = ReadJSONWithParser(parser, JSON_Path, funcName))
		{
			if (auto const JsonRef = GetJSONValueAtJSONPointer(jsonVal.value(), jsonPointer, funcName))
			{
				std::visit([=, &eval](auto&& val) {
					auto const res = JsonToNVSE::GetNVSEFromJSON(val.get(), scriptObj);
					AssignScriptValueResult(&res, eval, PASS_COMMAND_ARGS);
					}, JsonRef.value());
			}
		}
	}
	return true;
}









#if _DEBUG


bool Cmd_WriteToJSONFile_Execute(COMMAND_ARGS)
{
	using namespace JsonToNVSE;
	*result = false;	//bSuccess
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		auto const elemToWrite = eval.GetNthArg(0)->GetScriptLocal();	//TODO
		
		std::string json_path = eval.GetNthArg(1)->GetString();
		std::string jsonPointer = "";	// the path in the JSON hierarchy, pass "" to ignore this arg.
		Parser parser = kParser_JSON;
		if (auto const numArgs = eval.NumArgs();
			numArgs >= 3)
		{
			jsonPointer = eval.GetNthArg(2)->GetString();
			if (numArgs >= 4)
			{
				parser = static_cast<Parser>(eval.GetNthArg(3)->GetInt());
				if (parser >= kParser_Invalid)
					return true;
			}
		}
		std::ranges::replace(json_path, '/', '\\');
		std::string const JSON_Path = GetCurPath() + "\\" + std::move(json_path);
		if (!std::filesystem::exists(JSON_Path))
			return true;

		constexpr std::string_view funcName = { "WriteToJSONFile" };
		if (auto jsonVal = ReadJSONWithParser(parser, JSON_Path, funcName))
		{
			if (auto const JsonRef = GetJSONValueAtJSONPointer(jsonVal.value(), jsonPointer, funcName))
			{
				std::visit([](auto&& val) {
					//val = JsonToNVSE::GetJSONFromNVSE(elemToWrite);
					}, JsonRef.value());
				//todo: somehow apply a merge patch with the change applied to the json value????
				*result = true;
			}
		}
	}
	return true;
}

//TODO: ApplyJSONPatchToFile



bool Cmd_DemoTestFile_Execute(COMMAND_ARGS)
{
	*result = 0;

	std::filesystem::path path = { "test" };
	
	auto i = tao::config::from_file(path);
	_MESSAGE("%i", static_cast<int>(i.type()));

	
	
	return true;
}
#endif