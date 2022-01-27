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

template <typename F, typename... Args, std::size_t ... Is>
constexpr auto tuple_generator_seq(F&& f, std::index_sequence<Is...>)
{
	return std::make_tuple<Args...>(f(Is)...);
}

template <std::size_t N, typename F>
constexpr auto tuple_generator(F&& f)
{
	return tuple_generator_seq(f, std::make_index_sequence<N>());
}

template <size_t size>
constexpr size_t GetNumMandatoryArgs(const ParamInfo(&params)[size])
{
	size_t i = 0;
	for (; i < size; i++)
	{
		if (params[i].isOptional)
			return i;
	}
	return i;
}

//todo: struct to extract arg type from param type enum

struct NVSEParamInfo
{
	const char* typeStr;
	const kNVSEParamType	typeID;		// ParamType
	const UInt32	isOptional;	// do other bits do things?
};

static constexpr NVSEParamInfo kNVSETestParams_OneArray_Test3[1] =
{
	{	"array",	kNVSEParamType_Array,	0	},
};

template <typename T, size_t n>
constexpr size_t array_size(const T(&)[n]) { return n; }


//Extracts the non-optional args of a function as a tuple, for compile-time type safety.
//Assumes .ExtractArgs returned true before being called, and that NumArgs is >= number of non-optional args in params.
template <size_t size, const NVSEParamInfo (&params)[size]>
constexpr auto ExtractArgsTuple(PluginExpressionEvaluator& eval)
{
	constexpr kNVSEParamType type = params[0].typeID;
	using t = ParamTypeToReturnType::Get_t<type>;
	
	//using t = ParamTypeToReturnType::Get_t<kNVSEParamType_Array>;

	return GetNthArg<t>(eval, 0);
	/*
	return tuple_generator<GetNumMandatoryArgs(params)>(
		[&](auto) { return GetNthArg(params[nthArg], eval, nthArg++); }	//bug: func retn type is not constant.
	);*/
}




#if _DEBUG
//=====Testing zone


#include "CommandTable.h"

struct NVSECommandInfo
{
	const char* longName;		// 00
	const char* shortName;		// 04
	UInt32		opcode;			// 08
	const char* helpText;		// 0C
	UInt16		needsParent;	// 10
	UInt16		numParams;		// 12
	const NVSEParamInfo* params;	// 14

	// handlers
	Cmd_Execute	execute;		// 18
	Cmd_Parse	parse;			// 1C
	Cmd_Eval	eval;			// 20

	UInt32		flags;			// 24		might be more than one field (reference to 25 as a byte)
};


#define DEFINE_COMMAND_PLUGIN_EXP_TEST(name, altName, description, refRequired, paramInfo) \
	extern bool Cmd_ ## name ## _Execute(COMMAND_ARGS); \
	static NVSECommandInfo (kCommandInfo_ ## name) = { \
		#name, \
		#altName, \
		0, \
		#description, \
		refRequired, \
		(sizeof(paramInfo) / sizeof(ParamInfo)), \
		paramInfo, \
		HANDLER(Cmd_ ## name ## _Execute), \
		Cmd_Expression_Plugin_Parse, \
		NULL, \
		0 \
	};

#define ExtractArgsSafe(params, eval) \
	ExtractArgsTuple<array_size(params), params>(eval)

static constexpr NVSEParamInfo kNVSETestParams_OneArray[1] =
{
	{	"array",	kNVSEParamType_Array,	0	},
};

static constexpr NVSEParamInfo kNVSETestParams_OneArray_OneForm[2] =
{
	{	"array",	kNVSEParamType_Array,	0	},
	{	"form",	kNVSEParamType_Form,	0	},
};

DEFINE_COMMAND_PLUGIN_EXP_TEST(TestSafeExtract, , "", false, kNVSETestParams_OneArray);
bool Cmd_TestSafeExtract_Execute(COMMAND_ARGS)
{
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		//auto args = ExtractArgsTuple<array_size(kNVSETestParams_OneArray), kNVSETestParams_OneArray>(eval);
		auto args = ExtractArgsSafe(kNVSETestParams_OneArray_OneForm, eval);
		
		//static_assert(std::is_same_v<decltype(args), NVSEArrayVar*>, "lolol");

		
	}
	return true;
}




#endif

#endif