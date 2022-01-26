#pragma once

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
	auto const arg = eval.GetNthArg(nthArg);
	if constexpr (std::is_same_v<T, ArgTypes::NumberType>)
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
		//TODO
		//return;
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
		//todo: needs missing GetBasicArgType() to handle string case
		/*
		ArrayElementR elem;
		arg->GetElement(elem);
		switch (elem.GetType())
		{
			case NVSEArrayVarInterface::kType_String
		}
		if (auto const form = elem.Form())
			return form;
		return elem.num;*/
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

//Extracts the non-optional args of a function as a tuple, for compile-time type safety.
//Assumes .ExtractArgs returned true before being called, and that NumArgs is >= number of non-optional args in params.
template <size_t size>
constexpr auto ExtractArgsTuple(
	PluginExpressionEvaluator& eval,
	const ParamInfo(&params)[size])
{
	size_t nthArg = 0;
	using t = ParamTypeToReturnType::Get_t<kNVSEParamType_Number/*static_cast<kNVSEParamType>(params[nthArg].typeID)*/>;
	return GetNthArg<t>(eval, nthArg++);
	/*
	return tuple_generator<GetNumMandatoryArgs(params)>(
		[&](auto) { return GetNthArg(params[nthArg], eval, nthArg++); }	//bug: func retn type is not constant.
	);*/
}



#endif