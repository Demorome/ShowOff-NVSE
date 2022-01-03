#include "SO_fn_Files.h"
#include "nvse/Utilities.h"

#include "config.hpp"
#include <tao/json.hpp>

#include <filesystem>
#include <fstream>

#include "SimpleIni.h"

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
			if (IsConsoleMode() || g_ShowFuncDebug)
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
			if (IsConsoleMode() || g_ShowFuncDebug)
				Console_Print("%s >> Invalid JSON pointer arg.", funcName.data());
			_MESSAGE("%s >> JSON POINTER ERROR (%s)", funcName.data(), e.what());
		}
		catch (const std::out_of_range& e)
		{
			if (IsConsoleMode() || g_ShowFuncDebug)
				Console_Print("%s >> Invalid JSON pointer arg.", funcName.data());
			_MESSAGE("%s >> JSON POINTER ERROR (OUT OF RANGE) (%s)", funcName.data(), e.what());
		}
		catch (const std::invalid_argument& e)
		{
			if (IsConsoleMode() || g_ShowFuncDebug)
				Console_Print("%s >> Invalid JSON pointer arg.", funcName.data());
			_MESSAGE("%s >> JSON POINTER ERROR (Invalid Arg) (%s)", funcName.data(), e.what());
		}
		return {};
	}

	//assume numeric or string element (from a Map/StringMap's keys).
	std::string ConvertKeyElemToStr(NVSEArrayElement& elem)
	{
		if (auto const type = elem.GetType();
			type == NVSEArrayVarInterface::kType_Numeric)
		{
			return std::to_string(elem.num);
		}
		else if (type == NVSEArrayVarInterface::kType_String)
		{
			return elem.str;
		}
		throw std::logic_error("SHOWOFF - ConvertKeyElemToStr >> gave wrongly typed elem");
	}

	template< template< typename... > class Traits >
	tao::json::basic_value<Traits> BasicArrayElemToJsonValue(NVSEArrayElement& elem)
	{
		if (auto const type = elem.GetType();
			type == NVSEArrayVarInterface::kType_Numeric)
		{
			return tao::json::basic_value<Traits>(elem.num);
		}
		else if (type == NVSEArrayVarInterface::kType_Form)
		{
			if (elem.form)
				return tao::json::basic_value<Traits>(elem.form->refID);
			return tao::json::basic_value<Traits>(0u);
		}
		else if (type == NVSEArrayVarInterface::kType_String)
		{
			return tao::json::basic_value<Traits>(std::string(elem.str));
		}
		throw std::logic_error("SHOWOFF - BasicArrayElemToJsonValue >> Received wrongly typed elem.");
	}
	
	template< template< typename... > class Traits >
	tao::json::basic_value<Traits> GetJSONFromNVSE_Helper(NVSEArrayElement& elem)
	{
		tao::json::basic_value<Traits> value;
		if (elem.GetType() == NVSEArrayVarInterface::kType_Array)
		{
			if (!elem.arr)
			{
				value = tao::json::null;	//invalid array = json null
			}
			else
			{
				if (auto const arrType = g_arrInterface->GetContainerType(elem.arr);
					arrType == NVSEArrayVarInterface::ContainerTypes::kArrType_Array)
				{
					value = tao::json::empty_array;
					ArrayData const data = { elem.arr, true };
					for (auto i = 0; i < data.size; i++)
					{
						if (auto const type = data.vals[i].GetType();
							type == NVSEArrayVarInterface::kType_Array)
						{
							value.emplace_back(GetJSONFromNVSE_Helper<Traits>(data.vals[i]));	//recursion
						}
						else
						{
							value.emplace_back(BasicArrayElemToJsonValue<Traits>(data.vals[i]));
						}
					}

				}
				else if (arrType == NVSEArrayVarInterface::ContainerTypes::kArrType_StringMap
					|| arrType == NVSEArrayVarInterface::ContainerTypes::kArrType_Map)		//if Map-type, convert numeric keys to string.
				{
					value = tao::json::empty_object;
					ArrayData const data = { elem.arr, false };
					for (auto i = 0; i < data.size; i++)
					{
						if (auto const type = data.vals[i].GetType();
							type == NVSEArrayVarInterface::kType_Array)
						{
							value[ConvertKeyElemToStr(data.keys[i])] = GetJSONFromNVSE_Helper<Traits>(data.vals[i]);	//recursion
						}
						else
						{
							value[ConvertKeyElemToStr(data.keys[i])] = BasicArrayElemToJsonValue<Traits>(data.vals[i]);
						}
					}
				}
				else //invalid
				{
					value = tao::json::null;
				}
			}
		}
		else
		{
			value = BasicArrayElemToJsonValue<Traits>(elem);
		}
		return value;
	}
	
	//Parser arg determines the type of variant to return.
	//Assume elem and parser are valid.
	JsonValueVariant GetJSONFromNVSE(NVSEArrayElement &elem, Parser parser)
	{
		switch (parser)
		{
		case kParser_JSON:
		case kParser_JAXN:
			return GetJSONFromNVSE_Helper<tao::json::traits>(elem);
		case kParser_TaoConfig:
			return GetJSONFromNVSE_Helper<tao::config::traits>(elem);
		default:
			throw std::logic_error("SHOWOFF - GetJSONFromNVSE >> Reached invalid default case");
		}
	}


	bool InsertValueAtJSONPointer(JsonValueVariant &baseVariant, const JsonValueVariant &insertVariant, const std::string &jsonPointer, 
		const std::string_view &funcName)
	{
		return std::visit([&]<typename T0, typename T1>(T0 &&base, T1 &&insert) -> bool {
			if constexpr (decay_equiv<T0, T1>::value)
			{
				try
				{
					base.insert(tao::json::pointer(jsonPointer), insert);
					return true;
				}
				catch(std::exception &e)
				{
					if (IsConsoleMode() || g_ShowFuncDebug)
						Console_Print("%s >> Invalid JSON pointer arg.", funcName.data());
					_MESSAGE("%s >> JSON POINTER ERROR (%s)", funcName.data(), e.what());
					return false;
				}
			}
			else
				throw std::logic_error("SHOWOFF - InsertValueAtJSONPointer >> both visitors should be of the same type.");
		}, baseVariant, insertVariant);
	}
	

	//return false in case of failure, for error reporting.
	bool TryReadFromJSONFile(PluginExpressionEvaluator& eval, Script* scriptObj)
	{
		std::string json_path = eval.GetNthArg(0)->GetString();
		std::string jsonPointer = "";	// the path in the JSON hierarchy, pass "" to get the root value.
		Parser parser = kParser_JSON;
		if (auto const numArgs = eval.NumArgs();
			numArgs >= 2)
		{
			jsonPointer = eval.GetNthArg(1)->GetString();
			if (numArgs >= 3)
			{
				parser = static_cast<Parser>(eval.GetNthArg(2)->GetInt());
				if (parser >= kParser_Invalid)
					return false;
			}
		}
		std::ranges::replace(json_path, '/', '\\');
		std::string const JSON_Path = GetCurPath() + "\\" + std::move(json_path);
		if (!std::filesystem::exists(JSON_Path))
			return false;

		constexpr std::string_view funcName = { "ReadFromJSONFile" };
		if (auto jsonVal = ReadJSONWithParser(parser, JSON_Path, funcName))
		{
			if (auto const JsonRef = GetJSONValueAtJSONPointer(jsonVal.value(), jsonPointer, funcName))
			{
				std::visit([scriptObj, &eval](auto&& val) {
					auto res = JsonToNVSE::GetNVSEFromJSON(val.get(), scriptObj);
					eval.AssignCommandResult(res);
					}, JsonRef.value());
				return true;
			}
		}
		return false;
	}
}





bool Cmd_ReadFromJSONFile_Execute(COMMAND_ARGS)
{
	using namespace JsonToNVSE;
	*result = 0;
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		if (!TryReadFromJSONFile(eval, scriptObj))
		{
			ArrayElementL invalidRetn;
			eval.AssignCommandResult(invalidRetn);	//report error, since return value is too ambiguous.
			//will inform script. If in console mode OR in ShowOff debug mode, will inform the nature of the error.
			//todo: report better error instead!
		}
	}
	//eval is unlikely to fail extracting args, so don't bother error reporting.
	return true;
}

bool Cmd_WriteToJSONFile_Execute(COMMAND_ARGS)
{
	using namespace JsonToNVSE;
	*result = false;	//bSuccess
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		ArrayElementR elem;
		eval.GetNthArg(0)->GetElement(elem);
		if (!elem.IsValid())
			return true;
		std::string json_path = eval.GetNthArg(1)->GetString();
		std::string jsonPointer = "";	// the path in the JSON hierarchy, pass "" to get the root value.
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
		auto elemAsJSON = JsonToNVSE::GetJSONFromNVSE(elem, parser);

		constexpr std::string_view funcName = { "WriteToJSONFile" };
		if (jsonPointer != "" && std::filesystem::exists(JSON_Path))
		{
			if (auto jsonVal = ReadJSONWithParser(parser, JSON_Path, funcName))
			{
				if (InsertValueAtJSONPointer(jsonVal.value(), elemAsJSON, jsonPointer, funcName))
				{
					elemAsJSON = std::move(*jsonVal);
				}
				else
					return true;
			}
		}

		if (std::ofstream output(JSON_Path);
			output.is_open())
		{
			std::visit([&output](auto&& val) {
				output << std::setw(4) << val;	//pretty-printed with tab indents
				}, elemAsJSON);
			*result = true;
		}
	}
	return true;
}






#if _DEBUG


namespace IniToNVSE
{
	//dark magic copied from JIP
	bool __fastcall GetINIPath(char* iniPath, Script* scriptObj)
	{
		if (!*iniPath)
		{
			UInt8 modIdx = scriptObj->GetOverridingModIdx();
			if (modIdx == 0xFF) return false;
			StrCopy(iniPath, g_dataHandler->GetNthModName(modIdx));
		}
		else ReplaceChr(iniPath, '/', '\\');
		UInt32 length = StrLen(iniPath);
		char* dotPos = FindChrR(iniPath, length, '.');
		if (dotPos)
		{
			*(UInt32*)(dotPos + 1) = 'ini';
		}
		else
		{
			*(UInt32*)(iniPath + length) = 'ini.';
			iniPath[length + 4] = 0;
		}
		//append data/config to start of string
		*(UInt32*)(iniPath - 12) = 'atad';
		*(UInt32*)(iniPath - 8) = 'noc\\';
		*(UInt32*)(iniPath - 4) = '\\gif';
		return true;
	}

	enum CommandResult
	{
		kResult_Error = -1,
		kResult_Ok = 0,   //!< No error
		kResult_Updated = 1,   //!< An existing value was updated
		kResult_Inserted = 2,   //!< A new value was inserted
	};

	CommandResult AsResult(SI_Error e)
	{
		//don't care about types of errors, just that an error has occured.
		return std::max(static_cast<CommandResult>(e), kResult_Error);
	}

	namespace SetINIValue
	{
		constexpr auto MaxStrArgLen = 0x80;
		using CharArr = std::array<char, MaxStrArgLen>;
		using StringOrFloat = std::variant<std::string, double>;
		using Args = std::tuple<CharArr, StringOrFloat, CharArr, const char*>;

		//Returns args in the order they are declared.
		std::optional<Args> GetArgs(COMMAND_ARGS)
		{
			if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
				eval.ExtractArgs())
			{
				CharArr sectionAndKey;
				strcpy(sectionAndKey.data(), eval.GetNthArg(0)->GetString());

				ArrayElementR newValElem;
				StringOrFloat newVal;
				eval.GetNthArg(1)->GetElement(newValElem);
				if (newValElem.GetType() == NVSEArrayVarInterface::kType_String)
				{
					newVal = newValElem.str;	//todo: ensure this performs an actual copy
				}
				else //assume number
				{
					newVal = newValElem.num;
				}

				CharArr configPath;
				char *iniPath = configPath.data() + 12;
				*iniPath = 0;
				const char* comment = nullptr;
				if (auto const numArgs = eval.NumArgs();
					numArgs >= 3)
				{
					strcpy(iniPath, eval.GetNthArg(2)->GetString());

					if (numArgs >= 4) {
						comment = eval.GetNthArg(3)->GetString();
					}
				}
				if (!GetINIPath(iniPath, scriptObj))
					return {};
				
				return std::make_tuple(sectionAndKey, newVal, configPath, comment);
			}
			return {};
		}

		void Call(const Args &args, double *result)
		{
			auto& [sectionAndKey, newVal, configPath, comment] = args;

			auto const keyName = GetNextToken(const_cast<std::array<char, MaxStrArgLen>&>(sectionAndKey).data(), ":\\/");
			//sectionAndKey now only contains Section
			if (!keyName) return;

			CSimpleIniA ini(true);	//todo: read from cache
			ini.LoadFile(configPath.data());	//ignore errors, file will be created if it did not exist.

			CommandResult res;
			if (!std::visit(overloaded{
					[&](const std::string& str) -> bool
					{
						if (auto const e = ini.SetValue(sectionAndKey.data(), keyName, str.c_str(), comment);
							e >= SI_OK)	//if success
						{
							res = IniToNVSE::AsResult(e);
							return true;
						}
						return false;
					},
					[&](const double num) -> bool
					{
						if (auto const e = ini.SetDoubleValue(sectionAndKey.data(), keyName, num, comment);
							e >= SI_OK)
						{
							res = IniToNVSE::AsResult(e);
							return true;
						}
						return true;
					},
				}, newVal))
			{
				return;
			}
			
			if (ini.SaveFile(configPath.data(), false) < SI_OK)
				return;
			*result = res;
		}
	}
}


//Most code copied from JIP LN NVSE's GetINIString
bool Cmd_HasINISetting_Execute(COMMAND_ARGS)
{
	*result = false;
	char configPath[0x80], sectionName[0x80], *iniPath = configPath + 12;
	*iniPath = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &sectionName, iniPath) || !IniToNVSE::GetINIPath(iniPath, scriptObj))
		return true;

	char* keyName = GetNextToken(sectionName, ":\\/");
	CSimpleIniA ini(true);	
	if (ini.LoadFile(configPath) < SI_OK)
		return true;

	if (auto const val = ini.GetValue(sectionName, keyName, nullptr);
		val && *val)
	{
		*result = true;
	}
	return true;
}



bool Cmd_SetINIValue_Execute(COMMAND_ARGS)
{
	*result = IniToNVSE::kResult_Error;
	if (auto const args = IniToNVSE::SetINIValue::GetArgs(PASS_COMMAND_ARGS))
	{
		IniToNVSE::SetINIValue::Call(args.value(), result);
	}
	return true;
}

bool Cmd_SetINIFloatAlt_Execute(COMMAND_ARGS)
{
	return Cmd_SetINIValue_Execute(PASS_COMMAND_ARGS);
}

bool Cmd_SetINIStringAlt_Execute(COMMAND_ARGS)
{
	return Cmd_SetINIValue_Execute(PASS_COMMAND_ARGS);
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