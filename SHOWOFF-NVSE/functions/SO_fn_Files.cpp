#include "SO_fn_Files.h"
#include "nvse/Utilities.h"

#include "config.hpp"
#include <tao/json.hpp>

#include <filesystem>
#include <fstream>

#define SI_SUPPORT_IOSTREAMS
#include "SimpleIni.h"

#define TEST_JSON_READ_PERFORMANCE false


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
		ArrayElementL elem;
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
	using JsonValueVariant_ContainsRef = std::variant<std::reference_wrapper<DefaultJsonValue>, std::reference_wrapper<ConfigJsonValue>>;
	using JsonValueVariantRef = std::reference_wrapper<JsonValueVariant>;

	using NewJsonValueVariant_OrRef = std::variant <JsonValueVariantRef, JsonValueVariant>;
	constexpr JsonValueVariant& GetRef(NewJsonValueVariant_OrRef &jsonArg)
	{
		return *std::visit(overloaded{
			[](JsonValueVariantRef& json) -> JsonValueVariant* {
				return &json.get();
			},
			[](JsonValueVariant& json) -> JsonValueVariant* {
				return &json;
			} 
		}, jsonArg);
	}

	Map<const char*, JsonValueVariant> g_CachedJSONFiles;
	ICriticalSection g_JsonMapLock;

	JsonValueVariant ReadJson_Unsafe(const Parser parser, const std::string& JSON_Path)
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

	//MUST be called with valid Parser type.
	std::optional<NewJsonValueVariant_OrRef> ReadJSONWithParser(
		const Parser parser, 
		const std::string &JSON_FullPath, 
		const std::string_view relativePath, 
		const std::string_view& funcName, 
		const bool cache)
	{
		if (auto const cachedRef = g_CachedJSONFiles.GetPtr(relativePath.data()))
		{
			return std::ref(*cachedRef);
		}
		
		try
		{
			auto parsedJson = ReadJson_Unsafe(parser, JSON_FullPath);
			if (!cache)
				return parsedJson;
			ScopedLock lock(g_JsonMapLock);
			return std::ref(g_CachedJSONFiles[relativePath.data()] = std::move(parsedJson) );
		}
		catch (tao::pegtl::parse_error& e)
		{
			if (IsConsoleMode() || g_ShowFuncDebug)
				Console_Print("%s >> Could not parse JSON file, likely due to invalid formatting.", funcName.data());
			_MESSAGE("ReadArrayFromJSONFile >> PARSE ERROR (%s)", e.what());
		}
		catch (std::filesystem::filesystem_error& e)
		{
			if (IsConsoleMode() || g_ShowFuncDebug)
				Console_Print("%s >> Could not find JSON file.", funcName.data());
			_MESSAGE("ReadArrayFromJSONFile >> FILE NOT FOUND (%s)", e.what());
		}
		return {};
	}

	std::optional<JsonValueVariant_ContainsRef> GetJSONValueAtJSONPointer(const JsonValueVariant &value, const std::string& jsonPointer, const std::string_view& funcName)
	{
		try
		{
			return std::visit([&](auto&& val) -> JsonValueVariant_ContainsRef {
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
					ScopedLock lock(g_JsonMapLock);	//in case the baseJson was retrieved from the map.
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
		std::string json_path_relative = eval.GetNthArg(0)->GetString();
		if (json_path_relative.empty())
			return false;
		std::string jsonPointer = "";	// the path in the JSON hierarchy, pass "" to get the root value.
		Parser parser = kParser_JSON;
		bool cache = false;
		if (auto const numArgs = eval.NumArgs();
			numArgs >= 2)
		{
			jsonPointer = eval.GetNthArg(1)->GetString();
			if (numArgs >= 3)
			{
				parser = static_cast<Parser>(eval.GetNthArg(2)->GetInt());
				if (parser >= kParser_Invalid)
					return false;
				if (numArgs >= 4)
				{
					cache = eval.GetNthArg(3)->GetBool();
				}
			}
		}

		auto [JSON_FullPath, relPath] = GetFullPath(std::move(json_path_relative));

		constexpr std::string_view funcName = { "ReadFromJSONFile" };
		bool success = false;
		if (auto jsonValOpt = ReadJSONWithParser(parser, JSON_FullPath, relPath, funcName, cache))
		{
			auto const jsonVal = GetRef(*jsonValOpt);
			if (auto const JsonRef = GetJSONValueAtJSONPointer(jsonVal, jsonPointer, funcName))
			{
				std::visit([scriptObj, &eval](auto&& val) {
					auto res = JsonToNVSE::GetNVSEFromJSON(val.get(), scriptObj);
					eval.AssignCommandResult(res);
					}, JsonRef.value());
				success = true;
			}
		}
		
		return success;
	}
}




bool Cmd_ReadFromJSONFile_Execute(COMMAND_ARGS)
{
#if TEST_JSON_READ_PERFORMANCE
	auto const start = std::chrono::high_resolution_clock::now();
#endif
	
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
#if TEST_JSON_READ_PERFORMANCE
	auto const end = std::chrono::high_resolution_clock::now();
	auto const duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::ofstream out("ReadFromJSONFile_Performance.txt", std::ios::app);
	out << duration.count() << " (ms)\n";
#endif

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
		std::string json_path_rel = eval.GetNthArg(1)->GetString();
		if (json_path_rel.empty())
			return true;
		std::string jsonPointer = "";	// the path in the JSON hierarchy, pass "" to get the root value.
		Parser parser = kParser_JSON;
		bool cache = false;
		bool saveFile = true;
		switch (auto const numArgs = eval.NumArgs();
			numArgs)
		{
		case 6:
			saveFile = eval.GetNthArg(5)->GetBool();
			[[fallthrough]];
		case 5:
			cache = eval.GetNthArg(4)->GetBool();
			[[fallthrough]];
		case 4:
			parser = static_cast<Parser>(eval.GetNthArg(3)->GetInt());
			if (parser >= kParser_Invalid)
				return true;
			[[fallthrough]];
		case 3:
			jsonPointer = eval.GetNthArg(2)->GetString();
			break;
		default:
			throw std::logic_error("Invalid case for switch!");
		}

		auto [JSON_FullPath, relPath] = GetFullPath(std::move(json_path_rel));
		auto const fileExisted = std::filesystem::exists(JSON_FullPath);
		NewJsonValueVariant_OrRef elemAsJSON = JsonToNVSE::GetJSONFromNVSE(elem, parser);

		//if jsonPointer is empty, then the entire file will be replaced; no point in reading it.
		bool const readFileAtJPointer = !jsonPointer.empty() && fileExisted;
		if (readFileAtJPointer)
		{
			constexpr std::string_view funcName = { "WriteToJSONFile" };
			if (auto jsonValOpt = ReadJSONWithParser(parser, JSON_FullPath, relPath, funcName, cache))
			{
				JsonValueVariant* jsonVal = std::get_if<JsonValueVariant>(&jsonValOpt.value());
				bool jsonCached = false; // assume new value was created (uncached)
				if (jsonVal)	//true only if no caching is involved.
				{
					if (!saveFile)	//it's all for nothing if changes aren't being cached and won't be saved to file.
						return true;
					//The reason this isn't checked earlier, with "cached" and "savefile", 
					// is because the user might've cached the file earlier (which will be retrieved even if "cached" = false).
				}
				else
				{
					jsonVal = &std::get_if<JsonValueVariantRef>(&jsonValOpt.value())->get();	//I hate myself
					jsonCached = true;
				}

				if (InsertValueAtJSONPointer(*jsonVal, GetRef(elemAsJSON), jsonPointer, funcName))
				{
					if (!jsonCached)
						elemAsJSON = std::move(*jsonVal);	//Move jsonVal if it's not owned by Map global
					else
					{
						elemAsJSON = std::ref(*jsonVal); //elemAsJSON points to global map value
						*result = true;	//success if cached data was modified
					}
				}
				else
					return true;
			}
			else //catch parsing errors
			{
				return true;
			}
		}

		if (saveFile)
		{
			if (std::ofstream output(JSON_FullPath);	//erase previous contents, potentially create new file.
				output.is_open())
			{
				std::visit([&output](auto&& val) {
					output << std::setw(4) << val;	//pretty-printed with tab indents
					}, GetRef(elemAsJSON));
				*result = true;	//success if data was written to file
			}
		}			

		if (!readFileAtJPointer && cache)
		{
			// File was either created, or was completely replaced; cache that new data.
			// Also possible that file didn't exist and wasn't created, but we can still cache that data.
			ScopedLock lock(g_JsonMapLock);
			g_CachedJSONFiles[relPath.data()] = std::move(GetRef(elemAsJSON));
			*result = true;	//success if cached data is changed
		}
	}
	return true;
}








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

	constexpr auto MaxStrArgLen = 0x80;
	using CharArr = std::array<char, MaxStrArgLen>;
	
	std::map<std::string, CSimpleIniA> g_CachedIniFiles;

	[[nodiscard]] std::optional<CharArr> GetINIConfigPath(const char* iniStr, Script* scriptObj)
	{
		CharArr configPath;
		char* iniPath = configPath.data() + 12;
		*iniPath = 0;
		if (iniStr)
			strcpy(iniPath, iniStr);
		if (!GetINIPath(iniPath, scriptObj))
			return {};
		return configPath;
	}
		
	namespace SetINIValue
	{
		using Args = std::tuple<CharArr, ArrayElementR, CharArr, const char*>;

		//Returns args in the order they are declared.
		std::optional<Args> GetArgs(COMMAND_ARGS)
		{
			if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
				eval.ExtractArgs())
			{
				CharArr sectionAndKey;
				strcpy(sectionAndKey.data(), eval.GetNthArg(0)->GetString());

				ArrayElementR newVal;
				eval.GetNthArg(1)->GetElement(newVal);
				const char* iniPath = nullptr;
				const char* comment = nullptr;
				if (auto const numArgs = eval.NumArgs();
					numArgs >= 3)
				{
					iniPath = eval.GetNthArg(2)->GetString();
					if (numArgs >= 4) {
						comment = eval.GetNthArg(3)->GetString();
					}
				}
				auto const configPath = GetINIConfigPath(iniPath, scriptObj);
				if (!configPath)
					return {};
				
				return std::make_tuple(sectionAndKey, newVal, configPath.value(), comment);
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
			if (newVal.GetType() == NVSEArrayVarInterface::kType_String)
			{
				if (auto const e = ini.SetValue(sectionAndKey.data(), keyName, newVal.str, comment);
					e >= SI_OK)	//if success
				{
					res = IniToNVSE::AsResult(e);
				}
				else return;
			}
			else //assume number
			{
				if (auto const e = ini.SetDoubleValue(sectionAndKey.data(), keyName, newVal.num, comment);
					e >= SI_OK)
				{
					res = IniToNVSE::AsResult(e);
				}
				else return;
			}
			
			if (ini.SaveFile(configPath.data(), false) < SI_OK)
				return;
			*result = res;
		}
	}
	
	namespace GetINIValue
	{
		//sectionAndKey, configPath(opt), defaultOrCreate(opt)
		using BaseArgs = std::tuple<CharArr, CharArr, ArrayElementR>;	//for regular Get
		using GetOrCreateArgs = std::tuple <CharArr, CharArr, ArrayElementR, std::string>;	//extra comment arg

		std::optional<BaseArgs> GetBaseArgs_Helper(PluginExpressionEvaluator& eval, Script* scriptObj)
		{
			CharArr sectionAndKey;
			strcpy(sectionAndKey.data(), eval.GetNthArg(0)->GetString());

			const char* iniPath = nullptr;
			ArrayElementR defaultVal_OrCreate = {};
			if (auto const numArgs = eval.NumArgs();
				numArgs >= 2)
			{
				iniPath = eval.GetNthArg(1)->GetString();

				if (numArgs >= 3)
				{
					eval.GetNthArg(2)->GetElement(defaultVal_OrCreate);
				}
			}
			auto const configPath = GetINIConfigPath(iniPath, scriptObj);
			if (!configPath)
				return {};

			return std::make_tuple(sectionAndKey, configPath.value(), defaultVal_OrCreate);
		}
		
		std::optional<BaseArgs> GetBaseArgs(COMMAND_ARGS)
		{
			if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
				eval.ExtractArgs())
			{
				return GetBaseArgs_Helper(eval, scriptObj);
			}
			return {};
		}
		
		std::optional<GetOrCreateArgs> Get_GetOrCreate_Args(COMMAND_ARGS)
		{
			if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
				eval.ExtractArgs())
			{
				if (auto const baseArgs = GetBaseArgs_Helper(eval, scriptObj))
				{
					std::string comment = {};
					if (eval.NumArgs() >= 4) {
						comment = eval.GetNthArg(3)->GetString();
					}
					return std::tuple_cat(baseArgs.value(), std::tie(comment));
				}
			}
			return {};
		}

		using StringOrFloat = std::variant <std::string, double>;
		
		// Section, key, ini.
		using UsefulBaseArgs = std::tuple <const CharArr&, char*, CSimpleIniA&>;
		
		// Section, key, ini, comment.
		using UsefulGetOrCreateArgs = std::tuple <const CharArr&, char*, CSimpleIniA, const std::string&>;

		namespace CallHelpers
		{
			void TryChangeDefaultResult(const ArrayElementR& newRes, StringOrFloat& result)
			{
				if (newRes.IsValid())
				{
					std::visit([&]<typename T0>(T0 & res) {
						using T = std::decay_t<T0>;
						if constexpr (std::is_same_v<T, double>)
						{
							result = newRes.num;
						}
						else if constexpr (std::is_same_v<T, std::string>)
						{
							result = newRes.str;
						}
						else
						{
							static_assert(false, "GetINIValue - TryChangeDefaultResult >> non-exhaustive visitor");
						}
					}, result);
				}
			}
		}

		void Call(const BaseArgs& args, StringOrFloat &result)
		{
			auto& [sectionAndKey, configPath, defaultValElem] = args;

			CallHelpers::TryChangeDefaultResult(defaultValElem, result);
			
			//sectionAndKey is made to only contain section name.
			auto const keyName = GetNextToken(const_cast<CharArr&>(sectionAndKey).data(), ":\\/");
			if (!keyName)
				return;

			//todo: add read from cache code (pass scriptObj, etc.)
			CSimpleIniA ini(true);
			if (ini.LoadFile(configPath.data()) < SI_OK)
				return;
					
			std::visit([&]<typename T0>(T0 &defaultVal) {
				using T = std::decay_t<T0>;
				if constexpr (std::is_same_v<T, double>)
				{
					result = ini.GetDoubleValue(sectionAndKey.data(), keyName, defaultVal);
				}
				else if constexpr (std::is_same_v<T, std::string>)
				{
					result = ini.GetValue(sectionAndKey.data(), keyName, defaultVal.c_str());
				}
				else
				{
					static_assert(false, "GetINIValue - Call_BaseArgs >> non-exhaustive visitor");
				}
			}, result);
		}

		void Call(const GetOrCreateArgs& args, StringOrFloat &result)
		{
			auto& [sectionAndKey, configPath, defaultValElem, comment] = args;

			CallHelpers::TryChangeDefaultResult(defaultValElem, result);

			//sectionAndKey is made to only contain section name.
			auto const keyName = GetNextToken(const_cast<CharArr&>(sectionAndKey).data(), ":\\/");
			if (!keyName)
				return;

			//todo: add read from cache code (pass scriptObj, etc.)
			CSimpleIniA ini(true);
			if (ini.LoadFile(configPath.data()) < SI_OK)
				return;

			bool hasCreatedValue;
			std::visit([&]<typename T0>(T0 & res) {
				using T = std::decay_t<T0>;
				if constexpr (std::is_same_v<T, double>)
				{
					result = ini.GetOrCreate(sectionAndKey.data(), keyName, res, comment.c_str(), true, &hasCreatedValue);
				}
				else if constexpr (std::is_same_v<T, std::string>)
				{
					result = ini.GetOrCreate(sectionAndKey.data(), keyName, res.c_str(), comment.c_str(), true, &hasCreatedValue);
				}
				else {
					static_assert(false, "GetINIValue - Call_GetOrCreateArgs >> non-exhaustive visitor");
				}
			}, result);
			
			if (hasCreatedValue)
				ini.SaveFile(configPath.data(), false);
		}

		void AssignResult(const StringOrFloat& res, COMMAND_ARGS)
		{
			std::visit([&]<typename T0>(T0 & resVisitor) {
				using T = std::decay_t<T0>;
				if constexpr (std::is_same_v<T, double>)
				{
					*result = resVisitor;
				}
				else if constexpr (std::is_same_v<T, std::string>)
				{
					g_strInterface->Assign(PASS_COMMAND_ARGS, resVisitor.c_str());
				}
				else {
					static_assert(false, "GetINIValue - AssignResult >> non-exhaustive visitor");
				}
			}, res);
		}
	};
	
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

bool Cmd_GetINIFloatOrCreate_Execute(COMMAND_ARGS)
{
	IniToNVSE::GetINIValue::StringOrFloat res = 0.0;
	if (auto const args = IniToNVSE::GetINIValue::Get_GetOrCreate_Args(PASS_COMMAND_ARGS))
	{
		IniToNVSE::GetINIValue::Call(args.value(), res);
	}
	IniToNVSE::GetINIValue::AssignResult(res, PASS_COMMAND_ARGS);
	return true;
}
bool Cmd_GetINIStringOrCreate_Execute(COMMAND_ARGS)
{
	IniToNVSE::GetINIValue::StringOrFloat res = "";
	if (auto const args = IniToNVSE::GetINIValue::Get_GetOrCreate_Args(PASS_COMMAND_ARGS))
	{
		IniToNVSE::GetINIValue::Call(args.value(), res);
	}
	IniToNVSE::GetINIValue::AssignResult(res, PASS_COMMAND_ARGS);
	return true;
}

bool Cmd_GetINIFloatOrDefault_Execute(COMMAND_ARGS)
{
	IniToNVSE::GetINIValue::StringOrFloat res = 0.0;
	if (auto const args = IniToNVSE::GetINIValue::GetBaseArgs(PASS_COMMAND_ARGS))
	{
		IniToNVSE::GetINIValue::Call(args.value(), res);
	}
	IniToNVSE::GetINIValue::AssignResult(res, PASS_COMMAND_ARGS);
	return true;
}
bool Cmd_GetINIStringOrDefault_Execute(COMMAND_ARGS)
{
	IniToNVSE::GetINIValue::StringOrFloat res = "";
	if (auto const args = IniToNVSE::GetINIValue::GetBaseArgs(PASS_COMMAND_ARGS))
	{
		IniToNVSE::GetINIValue::Call(args.value(), res);
	}
	IniToNVSE::GetINIValue::AssignResult(res, PASS_COMMAND_ARGS);
	return true;
}












#if _DEBUG


bool Cmd_DemoTestFile_Execute(COMMAND_ARGS)
{
	*result = 0;

	std::filesystem::path path = { "test" };
	
	auto i = tao::config::from_file(path);
	_MESSAGE("%i", static_cast<int>(i.type()));

	
	
	return true;
}
#endif