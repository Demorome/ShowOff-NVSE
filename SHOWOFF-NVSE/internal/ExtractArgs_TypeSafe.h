#pragma once

#include "PluginAPI.h"

#define EnableSafeExtractArgsTests true


namespace Utilities 
{
	template<typename T> struct is_variant : std::false_type {};

	template<typename ...Args>
	struct is_variant<std::variant<Args...>> : std::true_type {};

	//Credits: https://stackoverflow.com/questions/57134521/how-to-check-if-template-argument-is-stdvariant
	template<typename T>
	inline constexpr bool is_variant_v = is_variant<T>::value;

	template<typename T, typename ... U>
	bool constexpr is_any_of_v = (std::is_same_v<T, U> || ...);

}

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
	using namespace Utilities;
	
	template<kNVSEParamType param>
	struct Get
	{
		// indicates the default return type for a given param. 
		using type = void;	//invalid void type by default

		//When extracting optional args, allows alternate types to be used as references to be modified.
		template <typename T>
		static consteval bool CanExtractAs()
		{
			return false;
		}
	};

	//Retrieves the default return type for a given arg parameter.
	template <kNVSEParamType param>
	using Get_t = typename Get<param>::type;

	template <kNVSEParamType param, typename T>
	consteval bool CanExtractParamAs()
	{
		return Get<param>::template CanExtractAs<T>();
	}

	template <class Variant, kNVSEParamType param, size_t ...Is>
	consteval bool VariantContains1OfType_Seq(std::index_sequence<Is...> seq)
	{
		constexpr int numOfType = (CanExtractParamAs<param, std::variant_alternative_t<Is, Variant>>() + ...);
		return numOfType == 1;
	}
	
	//Checks if the variant has exactly 1 alternative which can be extracted from the param type.
	template <class Variant, kNVSEParamType param, size_t N, typename Is = std::make_index_sequence<N>>
	constexpr bool VariantContains1OfType = VariantContains1OfType_Seq<Variant, param>(Is{});

	//Create explicit template class instantions to define the default arg to extract them as,
	//plus define possible conversions.
	template <> struct Get<kNVSEParamType_Boolean>		//Note that boolean param type covers more cases than just Number param type casted to bool!
	{
		using type = bool;

		template <typename T>
		static consteval bool CanExtractAs()
		{
			return std::is_same_v<type, T>;
		}
	};
	template <> struct Get<kNVSEParamType_Number>
	{
		using type = ArgTypes::NumberType;

		template <typename T>
		static consteval bool CanExtractAs()
		{
			return std::is_arithmetic_v<T>;
		}
	};
	template <> struct Get<kNVSEParamType_Form>
	{
		using type = ArgTypes::FormType;

		template <typename T>
		static consteval bool CanExtractAs()
		{
			return std::is_same_v<type, T>;
		}
	};
	template <> struct Get<kNVSEParamType_String>
	{
		using type = ArgTypes::StringType;

		template <typename T>
		static consteval bool CanExtractAs()
		{
			return is_any_of_v<T, type, std::string, const char*>;
		}
	};
	template <> struct Get<kNVSEParamType_Array>
	{
		using type = ArgTypes::ArrayType;

		template <typename T>
		static consteval bool CanExtractAs()
		{
			return std::is_same_v<type, T>;
		}
	};
	template <> struct Get<kNVSEParamType_BasicType>
	{
		using type = ArgTypes::BasicType;

		template <typename T>
		static consteval bool CanExtractAs()
		{
			if constexpr (is_variant_v<T>)
			{
				auto constexpr variant_size = 4;
				if constexpr (std::variant_size_v<T> != variant_size)
					return false;

				//Check if variant contains all 4 basic types (could be at either index for the variant).
				if constexpr (!VariantContains1OfType<T, kNVSEParamType_Number, variant_size>) {
					return false;
				}
				if constexpr (!VariantContains1OfType<T, kNVSEParamType_String, variant_size>){
					return false;
				}
				if constexpr (!VariantContains1OfType<T, kNVSEParamType_Array, variant_size>) {
					return false;
				}
				if constexpr (!VariantContains1OfType<T, kNVSEParamType_Form, variant_size>) {
					return false;
				}
				return true;
			}
			return false;
		}
	};
	template <> struct Get<kNVSEParamType_ArrayElement> : Get<kNVSEParamType_BasicType>
	{};

	/*
	template <> struct TestGetReturnType<kNVSEParamType_ArrayVarOrElement>
	{
		typedef BasicType type;	//todo: maybe make this std::variant < ArrayType, BasicType > ?
	};*/

	template <> struct Get<kNVSEParamType_StringOrNumber>
	{
		using type = ArgTypes::StringOrNumber;
		
		//TODO: allow std::variant<std::string, anyNum> with CanConvertTo()
		// will require static_cast for the number
		template <typename T>
		static consteval bool CanExtractAs()
		{
			if constexpr (is_variant_v<T>)
			{
				auto constexpr variant_size = 2;
				if constexpr (std::variant_size_v<T> != variant_size)
					return false;

				//Check if variant contains 1 valid string type, 1 numeric type (could be at either index for the variant).
				if constexpr (!VariantContains1OfType<T, kNVSEParamType_Number, variant_size>)
				{
					return false;
				}
				if constexpr (!VariantContains1OfType<T, kNVSEParamType_String, variant_size>)
				{
					return false;
				}
				return true;
			}
			return false;
		}
	};
	/*
	template <> struct TestGetReturnType<kNVSEParamType_Command>
	{
		typedef ?? type;
	};
	*/
	template <> struct Get<kNVSEParamType_FormOrNumber>
	{
		using type = ArgTypes::FormOrNumber;

		template <typename T>
		static consteval bool CanExtractAs()
		{
			if constexpr (is_variant_v<T>)
			{
				auto constexpr variant_size = 2;
				if constexpr (std::variant_size_v<T> != variant_size)
					return false;

				//Check if variant contains 1 form type, 1 numeric type (could be at either index for the variant).
				if constexpr (!VariantContains1OfType<T, kNVSEParamType_Number, variant_size>) {
					return false;
				}
				if constexpr (!VariantContains1OfType<T, kNVSEParamType_Form, variant_size>) {
					return false;
				}
				return true;
			}
			return false;
		}
	};
	template <> struct Get<kNVSEParamType_NoTypeCheck>	//????? not sure what this even does
	{
		using type = void*;

		template <typename T>
		static consteval bool CanExtractAs()	//can't extract this from plugins yet
		{
			return false;
		}
	};
	template <> struct Get<kNVSEParamType_Pair>
	{
		using type = ArgTypes::Pair;

		template <typename T>
		static consteval bool CanExtractAs()
		{
			return std::is_same_v<type, T>;
		}
	};
	template <> struct Get<kNVSEParamType_Slice>
	{
		using type = ArgTypes::Slice;

		template <typename T>
		static consteval bool CanExtractAs()
		{
			return std::is_same_v<type, T>;
		}
	};
	//todo: missing Var types (not needed atm)
}



//Return type depends on param type
template <typename T>
[[nodiscard]] T GetNthArg(PluginExpressionEvaluator& eval, const size_t nthArg)
{
	using ParamTypeToReturnType::CanExtractParamAs;
	if constexpr (auto const arg = eval.GetNthArg(nthArg); 
		CanExtractParamAs<kNVSEParamType_Boolean, T>())	//handle bool case before Number case, since the latter would convert to bool but handles less cases.
	{
		return arg->GetBool();
	}
	else if constexpr (CanExtractParamAs<kNVSEParamType_Number, T>())
	{
		return arg->GetFloat();
	}
	else if constexpr (CanExtractParamAs<kNVSEParamType_Form, T>())
	{
		return arg->GetTESForm();
	}
	else if constexpr (CanExtractParamAs<kNVSEParamType_String, T>())
	{
		return arg->GetString();
	}
	else if constexpr (CanExtractParamAs<kNVSEParamType_Array, T>())
	{
		return arg->GetArrayVar();
	}
	else if constexpr (CanExtractParamAs<kNVSEParamType_StringOrNumber, T>())
	{
		if (arg->CanConvertTo(kTokenType_String))
		{
			return arg->GetString();
		}
		return arg->GetFloat();	//assume bumber
	}
	else if constexpr (CanExtractParamAs<kNVSEParamType_FormOrNumber, T>())
	{
		ArrayElementR elem;
		arg->GetElement(elem);
		if (auto const form = elem.Form())
			return form;
		return elem.num;
	}
	else if constexpr (CanExtractParamAs<kNVSEParamType_BasicType, T>())
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
	else if constexpr (CanExtractParamAs<kNVSEParamType_Slice, T>())
	{
		return arg->GetSlice();
	}
	else if constexpr (CanExtractParamAs<kNVSEParamType_Pair, T>())
	{
		return arg->GetPair();
	}

	else if constexpr (CanExtractParamAs<kNVSEParamType_NoTypeCheck, T>())
	{
		//return ;
		static_assert(false, "No plugin code to extract NoTypeCheck arg");
	}
	else
	{
		static_assert(false, "Failure to match a return type for GetNthArg");
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
	using ParamTypeToReturnType::CanExtractParamAs;
	return (CanExtractParamAs<params[Is + numMandatoryArgs].typeID, std::tuple_element_t<Is, ArgsTuple>>() && ...);
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
			auto constexpr nthTupleIndex = nthArg - numMandatoryArgs;
			
			//Trust tuple arg types; assume we validated those earlier.
			using nthTupElem = std::remove_reference_t< std::tuple_element_t<nthTupleIndex, ArgsTuple> >;
			std::get<nthTupleIndex>(args) = GetNthArg<nthTupElem>(eval, nthArg);
			
			ExtractOptionalArgs_Recursive<size, params, nthArg + 1, argEnd, numMandatoryArgs>(eval, std::forward<ArgsTuple>(args), numOptArgsToExtract);
		}
	}
}

template<size_t size, const NVSEParamInfo(&params)[size], class ArgsTuple>
void ExtractOptionalArgs_Helper(PluginExpressionEvaluator& eval, ArgsTuple &&args)
{
	auto constexpr numMandatoryArgs = GetNumMandatoryArgs(params);
	UInt8 numOptArgsToExtract = eval.NumArgs() - numMandatoryArgs;
	ExtractOptionalArgs_Recursive<size, params, numMandatoryArgs, size, numMandatoryArgs>(eval, std::forward<ArgsTuple>(args), numOptArgsToExtract);
}

template <size_t size, const NVSEParamInfo(&params)[size], typename... ArgTypes>
void ExtractOptionalArgsFromPack(PluginExpressionEvaluator& eval, std::tuple<ArgTypes&...> &&args)
{
	using ArgsTupleBasic = std::tuple<ArgTypes...>;

	auto constexpr numOptArgs = GetNumOptionalArgs(params);
	if constexpr (numOptArgs <= 0)
	{
		static_assert(false, "Cannot extract any optional args; all are mandatory.");
	}
	else if constexpr (numOptArgs != std::tuple_size_v<ArgsTupleBasic>)
	{
		static_assert(false, "Provided number of optional args to extract does not match established count");
	}
	
	if constexpr (!ValidateOptionalArgs<size, params, numOptArgs, ArgsTupleBasic>())
	{
		static_assert(false, "ExtractOptionalArgsFromPack >> Invalid types for optional args provided.");
	}

	using ArgsTupleWithRefs = std::tuple<ArgTypes&...>;
	ExtractOptionalArgs_Helper<size, params>(eval, std::forward<ArgsTupleWithRefs>(args));
}


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



#if EnableSafeExtractArgsTests
//=====Testing zone


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
static constexpr NVSEParamInfo kNVSETestParams_OneOptionalString[1] =
{
	{	"string",	kNVSEParamType_String,	1	},
};

void TestSafeExtract_CompileTime(COMMAND_ARGS)
{
	PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
	
	auto [arg1, arg2] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneArray_OneForm, eval);
	static_assert(std::is_same_v<decltype(arg1), ArgTypes::ArrayType>);
	static_assert(std::is_same_v<decltype(arg2), ArgTypes::FormType>);

	
	//auto [arg1, arg2] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneArray, eval);
	// -> throws compiler error, trying to extract more args than the params allow

	auto [arg1_1] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneString, eval);
	static_assert(std::is_same_v<decltype(arg1_1), ArgTypes::StringType>);

	auto [arg1_2, arg2_2] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneString_OneStringOrNumber, eval);
	static_assert(std::is_same_v<decltype(arg1_2), ArgTypes::StringType>);
	static_assert(std::is_same_v<decltype(arg2_2), ArgTypes::StringOrNumber>);

	auto [arg1_3, arg2_3] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneBoolean, eval);
	static_assert(std::is_same_v<decltype(arg1_3), ArgTypes::BasicType>);
	static_assert(std::is_same_v<decltype(arg2_3), bool>);

	//auto [arg1_4, arg2_4] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval);
	// -> throws compiler error, trying to extract more non-optional args than there are.
	auto [arg1_5] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval);
	static_assert(std::is_same_v<decltype(arg1_5), ArgTypes::BasicType>);
	bool optArg1_0;
	EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval, std::tie(optArg1_0));
	//int optArg1_1;	//wrong type, will fail to compile call below
	//EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval, std::tie(optArg1_1));

	// Below fails to compile; there are no mandatory args to extract.
	//auto fail = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneOptionalString_TwoOptionalNumbers, eval);
	ArgTypes::StringType optString1_0 = "test";
	// Below fails to compile; Provided number of optional args to extract does not match established count
	//EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneOptionalString_TwoOptionalNumbers, eval, std::tie(optString1_0));

	//Below works because all arethmatic values are accepted (besides bool); data will be extracted as double, then cast down.
	float optNum1_0 = 0, optNum2_0 = 0;
	EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneOptionalString_TwoOptionalNumbers, eval, std::tie(optString1_0, optNum1_0, optNum2_0));
	
	double optNum1_1 = 0, optNum2_1 = 0;
	EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneOptionalString_TwoOptionalNumbers, eval, std::tie(optString1_0, optNum1_1, optNum2_1));

	const char* optStr_1_0;
	EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneOptionalString, eval, std::tie(optStr_1_0));
	std::string optStr_1_1;
	EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneOptionalString, eval, std::tie(optStr_1_1));

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