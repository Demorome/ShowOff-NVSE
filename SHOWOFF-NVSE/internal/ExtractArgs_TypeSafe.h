#pragma once

#include "PluginAPI.h"

#define EnableSafeExtractArgsTests true



namespace ArgTypes
{
	using NumberType = double;
	using FormType = TESForm*;
	using StringType = std::string_view;	//avoid needlessly copying string, leave that up as a later choice.
	using ArrayType = NVSEArrayVar*;
	using Pair = PluginTokenPair*;
	using Slice = PluginTokenSlice*;

	using BasicType = std::variant < FormType, NumberType, StringType, ArrayType >;
	using FormOrNumber = std::variant < FormType, NumberType >;
	using StringOrNumber = std::variant < StringType, NumberType >;
	using ArrayIndex = StringOrNumber;
}

namespace ParamTypeToReturnType
{
	template<kNVSEParamType param>
	struct Get
	{
		typedef void type;	//invalid void type by default
	};
	template <> struct Get<kNVSEParamType_Boolean>
	{
		typedef bool type;
	};
	template <> struct Get<kNVSEParamType_Form>
	{
		typedef ArgTypes::FormType type;
	};
	template <> struct Get<kNVSEParamType_BasicType>
	{
		typedef ArgTypes::BasicType type;
	};
	template <> struct Get<kNVSEParamType_Array>
	{
		typedef ArgTypes::ArrayType type;
	};
	template <> struct Get<kNVSEParamType_ArrayElement>
	{
		typedef ArgTypes::BasicType type;
	};
	template <> struct Get<kNVSEParamType_String>
	{
		typedef ArgTypes::StringType type;
	};
	/*
	template <> struct TestGetReturnType<kNVSEParamType_ArrayVarOrElement>
	{
		typedef BasicType type;	//todo: maybe make this std::variant < ArrayType, BasicType > ?
	};*/
	template <> struct Get<kNVSEParamType_StringOrNumber>
	{
		typedef ArgTypes::StringOrNumber type;
	};
	/*
	template <> struct TestGetReturnType<kNVSEParamType_Command>
	{
		typedef ?? type;
	};
	*/
	template <> struct Get<kNVSEParamType_FormOrNumber>
	{
		typedef ArgTypes::FormOrNumber type;
	};
	template <> struct Get<kNVSEParamType_NoTypeCheck>	//?????
	{
		typedef void* type;
	};
	template <> struct Get<kNVSEParamType_Number>
	{
		typedef ArgTypes::NumberType type;
	};
	template <> struct Get<kNVSEParamType_Pair>
	{
		typedef ArgTypes::Pair type;
	};
	template <> struct Get<kNVSEParamType_Slice>
	{
		typedef ArgTypes::Slice type;
	};
	//todo: missing Var types (not needed atm)

	template <kNVSEParamType param>
	using Get_t = typename Get<param>::type;
}



//Return type depends on param type
template <typename T>
[[nodiscard]] T GetNthArg(
	PluginExpressionEvaluator& eval, const size_t nthArg)
{
	if constexpr (auto const arg = eval.GetNthArg(nthArg); 
		std::is_same_v<T, ArgTypes::NumberType>)
	{
		return arg->GetFloat();
	}
	else if constexpr (std::is_same_v<T, ArgTypes::FormType>)
	{
		return arg->GetTESForm();
	}
	else if constexpr (std::is_same_v<T, ArgTypes::StringType>)
	{
		return arg->GetString();
	}
	else if constexpr (std::is_same_v<T, bool>)
	{
		return arg->GetBool();
	}
	else if constexpr (std::is_same_v<T, ArgTypes::ArrayType>)
	{
		return arg->GetArrayVar();
	}
	else if constexpr (std::is_same_v<T, ArgTypes::StringOrNumber>)
	{
		if (arg->CanConvertTo(kTokenType_String))
		{
			return arg->GetString();
		}
		return arg->GetFloat();	//assume bumber
	}
	else if constexpr (std::is_same_v<T, ArgTypes::FormOrNumber>)
	{
		ArrayElementR elem;
		arg->GetElement(elem);
		if (auto const form = elem.Form())
			return form;
		return elem.num;
	}
	else if constexpr (std::is_same_v<T, ArgTypes::BasicType>)
	{
		if (arg->CanConvertTo(kTokenType_Array))
		{
			return arg->GetArrayVar();
		}
		if (arg->CanConvertTo(kTokenType_String))
		{
			return arg->GetString();
		}
		if (arg->CanConvertTo(kTokenType_Form))
		{
			return arg->GetTESForm();
		}
		return arg->GetFloat();	//assume number
	}
	else if constexpr (std::is_same_v<T, ArgTypes::Slice>)
	{
		return arg->GetSlice();
	}
	else if constexpr (std::is_same_v<T, ArgTypes::Pair>)
	{
		return arg->GetPair();
	}
	else if constexpr (std::is_same_v<T, void*>)
	{
		//return ;
		static_assert(false, "No plugin code to extract void* arg");
	}
	else
	{
		static_assert(false, "Missing type case for GetNthArg");
	}
}

//additional type safety
struct NVSEParamInfo
{
	const char* typeStr;
	kNVSEParamType	typeID;		// ParamType
	UInt32	isOptional;	// do other bits do things?
};

template <size_t size>
[[nodiscard]] consteval size_t GetNumMandatoryArgs(const NVSEParamInfo(&params)[size])
{
	size_t i = 0;
	for (; i < size; i++)
	{
		if (params[i].isOptional)
			return i;
	}
	return i;
}

template <size_t size>
[[nodiscard]] consteval size_t GetNumOptionalArgs(const NVSEParamInfo(&params)[size])
{
	return size - GetNumMandatoryArgs(params);
}

//found at https://stackoverflow.com/questions/3368883/how-does-this-size-of-array-template-function-work#comment12975718_3368894
template <typename T, size_t n>
[[nodiscard]] consteval size_t array_size(const T(&)[n]) { return n; }


template <size_t size, const NVSEParamInfo(&params)[size], size_t ... Is>
[[nodiscard]] auto args_extract_tuple_seq(PluginExpressionEvaluator &eval, std::index_sequence<Is...>)
{
	return std::make_tuple(GetNthArg<ParamTypeToReturnType::Get_t<params[Is].typeID>>(eval, Is)...);
}

template <size_t numMandatoryArgs, size_t size, const NVSEParamInfo(&params)[size]>
[[nodiscard]] auto args_extract_tuple(PluginExpressionEvaluator& eval)
{
	return args_extract_tuple_seq<size, params>(eval, std::make_index_sequence<numMandatoryArgs>());
}

//Extracts the non-optional args of a function as a tuple, for compile-time type safety.
//Assumes .ExtractArgs returned true before being called, and that NumArgs is >= number of non-optional args in params.
template <size_t size, const NVSEParamInfo (&params)[size]>
[[nodiscard]] auto ExtractMandatoryArgsAsTuple(PluginExpressionEvaluator& eval)
{
	auto constexpr numArgs = GetNumMandatoryArgs(params);
	if constexpr (!numArgs)
	{
		static_assert(false, "ExtractMandatoryArgsAsTuple >> Attempting to extract mandatory args when there are none.");
	}
	return args_extract_tuple<numArgs, size, params>(eval);
}

template<size_t size, const NVSEParamInfo(&params)[size], class ArgsTuple, size_t ... Is>
[[nodiscard]] consteval bool ValidateOptionalArgs_Seq(std::index_sequence<Is...>)
{
	auto constexpr numMandatoryArgs = GetNumMandatoryArgs(params);	//used to offset to where optional args begin in params array.
	return (std::is_same_v< std::tuple_element_t<Is, ArgsTuple>, 
		ParamTypeToReturnType::Get_t<params[Is + numMandatoryArgs].typeID> > 
		&& ...);
}
//Checks if each arg in param pack == declared optional arg types
template<size_t size, const NVSEParamInfo(&params)[size], size_t numOptArgs, class ArgsTuple>
[[nodiscard]] consteval bool ValidateOptionalArgs()
{
	//index sequence for numOptArgs = 2: {0, 1}
	return ValidateOptionalArgs_Seq<size, params, ArgsTuple>(std::make_index_sequence<numOptArgs>());
}

//inspired by https://artificial-mind.net/blog/2020/10/31/constexpr-for
// -10000 points for griffyndor for this mess
template<size_t size, const NVSEParamInfo(&params)[size], size_t nthArg, size_t argEnd, size_t numMandatoryArgs, class ArgsTuple>
void ExtractOptionalArgs_Recursive(PluginExpressionEvaluator& eval, ArgsTuple &&args, UInt8 &numOptArgsToExtract)
{
	if constexpr (nthArg < argEnd)
	{
		if (numOptArgsToExtract--)
		{
			std::get<nthArg - numMandatoryArgs>(args) = GetNthArg<ParamTypeToReturnType::Get_t<params[nthArg].typeID>>(eval, nthArg);
			ExtractOptionalArgs_Recursive<size, params, nthArg + 1, argEnd, numMandatoryArgs>(eval, std::forward<ArgsTuple>(args), numOptArgsToExtract);
		}
	}
}

template<size_t size, const NVSEParamInfo(&params)[size], class ArgsTuple>
void ExtractOptionalArgs_Helper(PluginExpressionEvaluator& eval, ArgsTuple&&args)
{
	auto constexpr numMandatoryArgs = GetNumMandatoryArgs(params);
	UInt8 numOptArgsToExtract = eval.NumArgs() - numMandatoryArgs;
	ExtractOptionalArgs_Recursive<size, params, numMandatoryArgs, size, numMandatoryArgs>(eval, std::forward<ArgsTuple>(args), numOptArgsToExtract);
}

template <size_t size, const NVSEParamInfo(&params)[size], typename... ArgTypes>
void ExtractOptionalArgsFromPack(PluginExpressionEvaluator& eval, std::tuple<ArgTypes&...> &&args)
{
	using ArgsTuple = std::tuple<ArgTypes...>;

	auto constexpr numOptArgs = GetNumOptionalArgs(params);
	if constexpr (numOptArgs <= 0)
	{
		static_assert(false, "Cannot extract any optional args; all are mandatory.");
	}
	else if constexpr (numOptArgs != std::tuple_size_v<ArgsTuple>)
	{
		static_assert(false, "Provided number of optional args to extract does not match established count");
	}
	
	if constexpr (!ValidateOptionalArgs<size, params, numOptArgs, ArgsTuple>())
	{
		static_assert(false, "ExtractOptionalArgsFromPack >> Invalid types for optional args provided.");
	}

	ExtractOptionalArgs_Helper<size, params>(eval, std::forward< std::tuple<ArgTypes&...> >(args));
}



#if EnableSafeExtractArgsTests
//=====Testing zone


#include "CommandTable.h"

//Required to use safe non-MANUAL PluginExpressionEvaluator arg extraction functions (EXTRACT_MAND/OPT_ARGS_EXP).
//Params must be NVSEParamInfo array.
#define DEFINE_COMMAND_PLUGIN_EXP_SAFE_ALT(name, altName, description, refRequired, paramInfo) \
	static constexpr const NVSEParamInfo(&(kCommandParams_ ## name))[array_size(paramInfo)] = paramInfo; \
	DEFINE_CMD_FULL(name, altName, description, refRequired, array_size(paramInfo), reinterpret_cast<const ParamInfo*>(paramInfo), Cmd_Expression_Plugin_Parse)

//Required to use safe non-MANUAL PluginExpressionEvaluator arg extraction functions (EXTRACT_MAND/OPT_ARGS_EXP).
//Params must be NVSEParamInfo array.
#define DEFINE_COMMAND_PLUGIN_EXP_SAFE(name, description, refRequired, paramInfo) \
	DEFINE_COMMAND_PLUGIN_EXP_SAFE_ALT(name, , description, refRequired, paramInfo)

//Extracts mandatory args from params, for _EXP-defined functions (which use PluginExpressionEvaluator).
//Args are returned as a tuple; use structured binding to extract each arg.
#define EXTRACT_MAND_ARGS_EXP_MANUAL(params, eval) \
	ExtractMandatoryArgsAsTuple<array_size(params), params>(eval)

//Extracts mandatory args from a function's params, for _EXP-defined functions (which use PluginExpressionEvaluator).
//Args are returned as a tuple; use structured binding to extract each arg.
#define EXTRACT_MAND_ARGS_EXP(functionName, eval) \
	ExtractMandatoryArgsAsTuple<array_size(kCommandParams_ ## functionName), (kCommandParams_ ## functionName)>(eval)

//Extracts optional args from params (types are checked compile-time), for _EXP-defined functions (which use PluginExpressionEvaluator).
//Args must be packed inside a tuple, via std::tie().
#define EXTRACT_OPT_ARGS_EXP_MANUAL(params, eval, args) \
	ExtractOptionalArgsFromPack<array_size(params), params>(eval, args)

//Extracts optional args from a function's params (types are checked compile-time), for _EXP-defined functions (which use PluginExpressionEvaluator).
//Args must be packed inside a tuple, via std::tie().
#define EXTRACT_OPT_ARGS_EXP(functionName, eval, args) \
	ExtractOptionalArgsFromPack<array_size(kCommandParams_ ## functionName), (kCommandParams_ ## functionName)>(eval, args)


static constexpr NVSEParamInfo kNVSETestParams_OneArray[1] =
{
	{	"array",	kNVSEParamType_Array,	0	},
};

static constexpr NVSEParamInfo kNVSETestParams_OneArray_OneForm[2] =
{
	{	"array",	kNVSEParamType_Array,	0	},
	{	"form",	kNVSEParamType_Form,	0	},
};

static constexpr NVSEParamInfo kNVSETestParams_OneString[1] =
{
	{	"string",	kNVSEParamType_String,	0	},
};

static constexpr NVSEParamInfo kNVSETestParams_OneString_OneStringOrNumber[2] =
{
	{	"string",	kNVSEParamType_String,	0	},
	{	"string or num",	kNVSEParamType_StringOrNumber,	0	},
};

static constexpr NVSEParamInfo kNVSETestParams_OneBasicType_OneBoolean[2] =
{
	{	"string",	kNVSEParamType_BasicType,	0	},
	{	"bool",	kNVSEParamType_Boolean,	0	},
};
static constexpr NVSEParamInfo kNVSETestParams_OneBasicType_OneOptionalBoolean[2] =
{
	{	"string",	kNVSEParamType_BasicType,	0	},
	{	"bool",	kNVSEParamType_Boolean,	1	},
};

static constexpr NVSEParamInfo kNVSETestParams_OneNumber_OneOptionalString[2] =
{
	{	"num",	kNVSEParamType_Number,	0	},
	{	"string",	kNVSEParamType_String,	1	},
};

static constexpr NVSEParamInfo kNVSETestParams_OneOptionalString_TwoOptionalNumbers[3] =
{
	{	"string",	kNVSEParamType_String,	1	},
	{	"num",	kNVSEParamType_Number,	1	},
	{	"num",	kNVSEParamType_Number,	1	},
};

static constexpr NVSEParamInfo kNVSETestParams_OneOptionalStringOrNumber[1] =
{
	{	"string or number",	kNVSEParamType_StringOrNumber,	1	},
};

void TestSafeExtract_CompileTime(COMMAND_ARGS)
{
	PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
	
	auto [arg1, arg2] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneArray_OneForm, eval);
	static_assert(std::is_same_v<decltype(arg1), ArgTypes::ArrayType>);
	static_assert(std::is_same_v<decltype(arg2), ArgTypes::FormType>);

	
	//auto [arg1, arg2] = EXTRACT_ARGS_SAFE(kNVSETestParams_OneArray, eval);
	// -> throws compiler error, trying to extract more args than the params allow

	auto [arg1_1] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneString, eval);
	static_assert(std::is_same_v<decltype(arg1_1), ArgTypes::StringType>);

	auto [arg1_2, arg2_2] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneString_OneStringOrNumber, eval);
	static_assert(std::is_same_v<decltype(arg1_2), ArgTypes::StringType>);
	static_assert(std::is_same_v<decltype(arg2_2), ArgTypes::StringOrNumber>);

	auto [arg1_3, arg2_3] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneBoolean, eval);
	static_assert(std::is_same_v<decltype(arg1_3), ArgTypes::BasicType>);
	static_assert(std::is_same_v<decltype(arg2_3), bool>);

	//auto [arg1_4, arg2_4] = EXTRACT_ARGS_SAFE(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval);
	// -> throws compiler error, trying to extract more non-optional args than there are.
	auto [arg1_5] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval);
	static_assert(std::is_same_v<decltype(arg1_5), ArgTypes::BasicType>);
	bool optArg1_0;
	EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval, std::tie(optArg1_0));
	//int optArg1_1;	//wrong type, will fail to compile call below
	//EXTRACT_OPT_ARGS_SAFE<array_size(kNVSETestParams_OneBasicType_OneOptionalBoolean), kNVSETestParams_OneBasicType_OneOptionalBoolean>(eval, optArg1_1);

	// Below fails to compile; there are no mandatory args to extract.
	//auto fail = EXTRACT_MAND_ARGS_SAFE(kNVSETestParams_OneOptionalString_TwoOptionalNumbers, eval);
	ArgTypes::StringType optString1_0 = "test";
	// Below fails to compile; Provided number of optional args to extract does not match established count
	//EXTRACT_OPT_ARGS_SAFE(kNVSETestParams_OneOptionalString_TwoOptionalNumbers, eval, std::tie(optString1_0));
	float optNum1_0 = 0, optNum2_0 = 0;
	// Below fails to compile; number types don't match (always expects double for number param type).
	//EXTRACT_OPT_ARGS_SAFE(kNVSETestParams_OneOptionalString_TwoOptionalNumbers, eval, std::tie(optString1_0, optNum1_0, optNum2_0));
	double optNum1_1 = 0, optNum2_1 = 0;
	EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneOptionalString_TwoOptionalNumbers, eval, std::tie(optString1_0, optNum1_1, optNum2_1));


}

DEFINE_COMMAND_PLUGIN_EXP_SAFE(TestSafeExtract_OneArray, "", false, kNVSETestParams_OneArray);
bool Cmd_TestSafeExtract_OneArray_Execute(COMMAND_ARGS)
{
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		auto arg_tuple = EXTRACT_MAND_ARGS_EXP(TestSafeExtract_OneArray, eval);
		auto& [arg1] = arg_tuple;
		static_assert(std::is_same_v<decltype(arg1), ArgTypes::ArrayType>);

		Console_Print("TestSafeExtract_OneArray >> ArrayID: %u", reinterpret_cast<UInt32>(arg1));
	}
	return true;
}

DEFINE_COMMAND_PLUGIN_EXP_SAFE(TestSafeExtract_OneNumber_OneOptionalString, "", false, kNVSETestParams_OneNumber_OneOptionalString);
bool Cmd_TestSafeExtract_OneNumber_OneOptionalString_Execute(COMMAND_ARGS)
{
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		auto [num] = EXTRACT_MAND_ARGS_EXP(TestSafeExtract_OneNumber_OneOptionalString, eval);
		static_assert(std::is_same_v<decltype(num), ArgTypes::NumberType>);

		ArgTypes::StringType opt_string = "[My Optional String]";
		EXTRACT_OPT_ARGS_EXP(TestSafeExtract_OneNumber_OneOptionalString, eval, std::tie(opt_string));

		Console_Print("TestSafeExtract_OneNumber_OneOptionalString >> Number: %f, optional string: %s", num, opt_string.data());
	}
	return true;
}

DEFINE_COMMAND_PLUGIN_EXP_SAFE(TestSafeExtract_OneOptionalStringOrNumber, "", false, kNVSETestParams_OneOptionalStringOrNumber);
bool Cmd_TestSafeExtract_OneOptionalStringOrNumber_Execute(COMMAND_ARGS)
{
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		ArgTypes::StringOrNumber opt_stringOrNum = "[My Optional String]";
		EXTRACT_OPT_ARGS_EXP(TestSafeExtract_OneOptionalStringOrNumber, eval, std::tie(opt_stringOrNum));

		Console_Print("TestSafeExtract_OneOptionalStringOrNumber >> optional stringOrNum's type: %i", opt_stringOrNum.index());
		if (auto const stringPtr = std::get_if<ArgTypes::StringType>(&opt_stringOrNum))
		{
			Console_Print(" >> optional string %s", stringPtr->data());
		}
		else //assume num 
		{
			Console_Print(" >> optional number %f", std::get<ArgTypes::NumberType>(opt_stringOrNum));
		}
	}
	return true;
}


#endif