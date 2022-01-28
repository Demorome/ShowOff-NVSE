#pragma once

#include <array>

#include "PluginAPI.h"

#define Test_TypeSafeExtract true && _DEBUG

#if Test_TypeSafeExtract

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
[[nodiscard]] constexpr T GetNthArg(
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
		if (arg->CanConvertTo(kTokenType_Array))
		{
			return arg->GetArrayVar();
		}
		return arg->GetString();	//assume string
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



//todo: struct to extract arg type from param type enum

//want this for additional typesafety
struct NVSEParamInfo
{
	const char* typeStr;
	const kNVSEParamType	typeID;		// ParamType
	const UInt32	isOptional;	// do other bits do things?
};

template <size_t size>
constexpr size_t GetNumMandatoryArgs(const NVSEParamInfo(&params)[size])
{
	size_t i = 0;
	for (; i < size; i++)
	{
		if (params[i].isOptional)
			return i;
	}
	return i;
}

//found at https://stackoverflow.com/questions/3368883/how-does-this-size-of-array-template-function-work#comment12975718_3368894
template <typename T, size_t n>
constexpr size_t array_size(const T(&)[n]) { return n; }


/*
template <auto Start, auto End, auto Inc, class F>
constexpr void constexpr_for(F&& f)
{
	if constexpr (Start < End)
	{
		f(std::integral_constant<decltype(Start), Start>());
		constexpr_for<Start + Inc, End, Inc>(f);
	}
}*/

/*
template <typename F, std::size_t ... Is>
constexpr auto tuple_generator_seq(F&& f, std::index_sequence<Is...>)
{
	return std::make_tuple(f(Is)...);
}

template <std::size_t N, typename F>
constexpr auto tuple_generator(F&& f)
{
	return tuple_generator_seq(f, std::make_index_sequence<N>());
}*/

template <size_t size, const NVSEParamInfo(&params)[size], size_t ... Is>
constexpr auto args_extract_tuple_seq(PluginExpressionEvaluator &eval, std::index_sequence<Is...>)
{
	return std::make_tuple(GetNthArg<ParamTypeToReturnType::Get_t<params[Is].typeID>>(eval, Is)...);
}

template <size_t numMandatoryArgs, size_t size, const NVSEParamInfo(&params)[size]>
constexpr auto args_extract_tuple(PluginExpressionEvaluator& eval)
{
	return args_extract_tuple_seq<size, params>(eval, std::make_index_sequence<numMandatoryArgs>());
}


//Extracts the non-optional args of a function as a tuple, for compile-time type safety.
//Assumes .ExtractArgs returned true before being called, and that NumArgs is >= number of non-optional args in params.
template <size_t size, const NVSEParamInfo (&params)[size]>
constexpr auto ExtractArgsTuple(PluginExpressionEvaluator& eval)
{
	auto constexpr numArgs = GetNumMandatoryArgs(params);
	return args_extract_tuple<numArgs, size, params>(eval);
}




#if _DEBUG
//=====Testing zone


#include "CommandTable.h"

template<size_t size>
struct NVSECommandInfo
{
	const char* longName;		// 00
	const char* shortName;		// 04
	UInt32		opcode;			// 08
	const char* helpText;		// 0C
	UInt16		needsParent;	// 10
	UInt16		numParams;		// 12
	//const NVSEParamInfo* params;	// 14
	const NVSEParamInfo (&params)[size];	// 14
	
	// handlers
	Cmd_Execute	execute;		// 18
	Cmd_Parse	parse;			// 1C
	Cmd_Eval	eval;			// 20

	UInt32		flags;			// 24		might be more than one field (reference to 25 as a byte)
};


#define DEFINE_COMMAND_PLUGIN_EXP_TEST(name, altName, description, refRequired, paramInfo) \
	extern bool Cmd_ ## name ## _Execute(COMMAND_ARGS); \
	static constexpr const NVSEParamInfo(&(kCommandParams_ ## name))[array_size(paramInfo)] = paramInfo; \
	static NVSECommandInfo (kCommandInfo_ ## name) = { \
		#name, \
		#altName, \
		0, \
		#description, \
		refRequired, \
		array_size(paramInfo), \
		paramInfo, \
		HANDLER(Cmd_ ## name ## _Execute), \
		Cmd_Expression_Plugin_Parse, \
		NULL, \
		0 \
	};

#define EXTRACT_ARGS_SAFE(params, eval) \
	ExtractArgsTuple<array_size(params), params>(eval)

#define EXTRACT_ARGS_SAFER(functionName, eval) \
	ExtractArgsTuple<array_size(kCommandParams_ ## functionName), (kCommandParams_ ## functionName)>(eval)

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

DEFINE_COMMAND_PLUGIN_EXP_TEST(TestSafeExtract, , "", false, kNVSETestParams_OneArray);
bool Cmd_TestSafeExtract_Execute(COMMAND_ARGS)
{
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		//auto args = ExtractArgsTuple<array_size(kNVSETestParams_OneArray), kNVSETestParams_OneArray>(eval);
		//auto args = ExtractArgsSafe(kNVSETestParams_OneArray_OneForm, eval);
		auto [arg1, arg2] = EXTRACT_ARGS_SAFE(kNVSETestParams_OneArray_OneForm, eval);
		static_assert(std::is_same_v<decltype(arg1), NVSEArrayVar*>);
		static_assert(std::is_same_v<decltype(arg2), TESForm*>);
		
		auto arg_tuple = EXTRACT_ARGS_SAFER(TestSafeExtract, eval);
		auto &[arg1_0] = arg_tuple;
		static_assert(std::is_same_v<decltype(arg1_0), NVSEArrayVar*>);

		//auto [arg1, arg2] = EXTRACT_ARGS_SAFE(kNVSETestParams_OneArray, eval);	//throws compiler error, trying to extract more args than the params allow
		
		auto[arg1_1] = EXTRACT_ARGS_SAFE(kNVSETestParams_OneString, eval);
		static_assert(std::is_same_v<decltype(arg1_1), std::string_view>);

		
	}
	return true;
}




#endif

#endif