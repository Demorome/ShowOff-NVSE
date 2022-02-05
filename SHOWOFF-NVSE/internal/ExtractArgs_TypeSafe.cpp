
#include "ExtractArgs_TypeSafe.h"
#include "GameAPI.h"

#if EnableSafeExtractArgsTests




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

	EXTRACT_ALL_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneBoolean, eval, std::tie(arg1_3, arg2_3), g_NoArgs);


	std::tie(arg1_3, arg2_3) = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneBoolean, eval);
	int intArg1_0;
	std::tie(arg1_3, intArg1_0) = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneBoolean, eval);


	//auto [arg1_4, arg2_4] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval);
	// -> throws compiler error, trying to extract more non-optional args than there are.
	auto [arg1_5] = EXTRACT_MAND_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval);
	static_assert(std::is_same_v<decltype(arg1_5), ArgTypes::BasicType>);
	bool optArg1_0;
	EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval, std::tie(optArg1_0));

	//int optArg1_1;	//wrong type, will fail to compile call below
	//EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval, std::tie(optArg1_1));

	// Below fails to compile; there are mandatory args to extract, yet we passed g_NoArgs for them.
	//EXTRACT_ALL_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval, g_NoArgs, std::tie(optArg1_0));

	EXTRACT_ALL_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval, std::tie(arg1_5), std::tie(optArg1_0));

	// Below fails to compile; there are optional args to extract, yet we passed g_NoArgs for them.
	//EXTRACT_ALL_ARGS_EXP_MANUAL(kNVSETestParams_OneBasicType_OneOptionalBoolean, eval, std::tie(arg1_5), g_NoArgs);



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

	EXTRACT_ALL_ARGS_EXP_MANUAL(kNVSETestParams_OneOptionalString_TwoOptionalNumbers, eval, g_NoArgs, std::tie(optString1_0, optNum1_1, optNum2_1));


	const char* optStr_1_0;
	EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneOptionalString, eval, std::tie(optStr_1_0));
	std::string optStr_1_1;
	EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneOptionalString, eval, std::tie(optStr_1_1));

	//char* optStr_1_2;
	// Below fails to compile, since char* is not supported for optional string extraction. To make a copied string, use std::string instead.
	//EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneOptionalString, eval, std::tie(optStr_1_2));

	bool optBool1_0;
	EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneOptionalBoolean, eval, std::tie(optBool1_0));

	// Below fails to compile, since bool-to-number conversions are disallowed. Just use Number param type instead :P
	//int optNum1_2;
	//EXTRACT_OPT_ARGS_EXP_MANUAL(kNVSETestParams_OneOptionalBoolean, eval, std::tie(optNum1_2));
}


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

bool Cmd_TestSafeExtract_OneNumber_OneOptionalString_Alt_Execute(COMMAND_ARGS)
{
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		int num;
		ArgTypes::StringType opt_string = "[My Optional String]";
		EXTRACT_ALL_ARGS_EXP(TestSafeExtract_OneNumber_OneOptionalString_Alt, eval, std::tie(num), std::tie(opt_string));

		Console_Print("TestSafeExtract_OneNumber_OneOptionalString >> Number: %i, optional string: %s", num, opt_string.data());
	}
	return true;
}

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