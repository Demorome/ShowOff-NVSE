#pragma once

#include "PluginAPI.h"

#define Test_TypeSafeExtract false

#if Test_TypeSafeExtract 

template <typename T>
[[nodiscard]] constexpr std::tuple<T> GetNthArg(const kNVSEParamType param,
	PluginExpressionEvaluator& eval, const size_t nthArg)
{

	auto const arg = eval.GetNthArg(nthArg);
	switch (param)
	{
	case kNVSEParamType_NumericVar:
	case kNVSEParamType_Number: return arg->GetFloat();

	case kNVSEParamType_Boolean: return arg->GetBool();
	case kNVSEParamType_String: return arg->GetString();

	case kNVSEParamType_Form:
	case kNVSEParamType_RefVar: return arg->GetTESForm();

	case kNVSEParamType_Array: return arg->GetArrayVar();
	case kNVSEParamType_ArrayElement: throw;
	case kNVSEParamType_Slice: return arg->GetSlice();
	case kNVSEParamType_Command: throw;	//todo: export this
	case kNVSEParamType_Variable: throw;	//todo: idk

	case kNVSEParamType_StringVar: throw;	//todo: idk
	case kNVSEParamType_ArrayVar: throw;	//todo: idk
	case kNVSEParamType_ForEachContext: throw;
	case kNVSEParamType_Collection: throw;
	case kNVSEParamType_ArrayVarOrElement: throw;
	case kNVSEParamType_BasicType: throw;
	case kNVSEParamType_NoTypeCheck: throw;
	case kNVSEParamType_FormOrNumber: throw;
	case kNVSEParamType_StringOrNumber: throw;
	case kNVSEParamType_Pair: return arg->GetPair();
	default:
		throw;
	}
	return "test";
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

//Extracts the non-optional args of a function as a tuple, for compile-time type safety.
//Assumes .ExtractArgs returned true before being called, and that NumArgs is >= number of non-optional args in params.
template <size_t size>
constexpr auto ExtractArgsTuple(
	PluginExpressionEvaluator& eval,
	const ParamInfo(&params)[size])
{
	size_t nthArg = 0;
	return GetNthArg(static_cast<kNVSEParamType>(params[nthArg].typeID), eval, nthArg++);
	/*
	return tuple_generator<GetNumMandatoryArgs(params)>(
		[&](auto) { return GetNthArg(params[nthArg], eval, nthArg++); }	//bug: func retn type is not constant.
	);*/
}



#endif