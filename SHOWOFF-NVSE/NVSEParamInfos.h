#pragma once

// Declare Params with "static", so they are only built once (so this file can be included multiple times).

#include "CommandTable.h"
#include "PluginAPI.h"

static ParamInfo kNVSEParams_OneArray[1] =
{
	{	"array",	kNVSEParamType_Array,	0	},
};

static ParamInfo kNVSEParams_OneForm_OneArray_OneOptionalIndex[3] =
{
	{	"form",	kNVSEParamType_Form,	0	},
	{	"array",	kNVSEParamType_Array,	0	},
	{	"index",	kNVSEParamType_Number,1	},
};

static ParamInfo kNVSEParams_OneArray_OneString_OneArray[3] =
{
	{	"array",	kNVSEParamType_Array,	0	},
	{	"string",	kNVSEParamType_String,0	},
	{	"array",	kNVSEParamType_Array,	0	},
};
static ParamInfo kNVSEParams_OneArray_OneString_OneDouble[3] =
{
	{	"array",	kNVSEParamType_Array,	0	},
	{	"string",	kNVSEParamType_String,0	},
	{	"float",	kNVSEParamType_Number,0	},
};

static ParamInfo kNVSEParams_OneForm_OneArray[2] =
{
	{	"form",	kNVSEParamType_Form,	0	},
	{	"array",	kNVSEParamType_Array,	0	},
};


static ParamInfo kNVSEParams_OneString_OneOptionalString_OneOptionalInt[3] =
{
	{	"string",	kNVSEParamType_String,0	},
	{	"string",	kNVSEParamType_String,1	},
	{	"int",	kNVSEParamType_Number,1	},
};
static ParamInfo kNVSEParams_OneString_OneOptionalString_TwoOptionalInts[4] =
{
	{	"string",	kNVSEParamType_String,0	},
	{	"string",	kNVSEParamType_String,1	},
	{	"int",	kNVSEParamType_Number,1	},
	{	"int",	kNVSEParamType_Number,1	},
};
static ParamInfo kNVSEParams_OneString_OneOptionalString_OneOptionalInt_OneOptionalBool[4] =
{
	{	"string",	kNVSEParamType_String,0	},
	{	"string",	kNVSEParamType_String,1	},
	{	"int",	kNVSEParamType_Number,1	},
	{	"bool",	kNVSEParamType_Boolean,1	},
};

static ParamInfo kNVSEParams_OneElem_OneString_OneOptionalString_OneOptionalInt[4] =
{
	{	"elem",	kNVSEParamType_BasicType,0	},
	{	"string",	kNVSEParamType_String,0	},
	{	"string",	kNVSEParamType_String,1	},
	{	"int",	kNVSEParamType_Number,1	},
};
static ParamInfo kNVSEParams_OneElem_OneString_OneOptionalString_OneOptionalInt_TwoOptionalBools[6] =
{
	{	"elem",	kNVSEParamType_BasicType,0	},
	{	"string",	kNVSEParamType_String,0	},
	{	"string",	kNVSEParamType_String,1	},
	{	"int",	kNVSEParamType_Number,1	},
	{	"bool",	kNVSEParamType_Boolean,1	},
	{	"bool",	kNVSEParamType_Boolean,1	},
};

static ParamInfo kNVSEParams_OneString_OneElem_TwoOptionalStrings[4] =
{
	{	"string",	kNVSEParamType_String,0	},
	{	"elem",	kNVSEParamType_BasicType,0	},
	{	"string",	kNVSEParamType_String,1	},
	{	"string",	kNVSEParamType_String,1	},
};
static ParamInfo kNVSEParams_OneString_OneFloatOrString_TwoOptionalStrings[4] =
{
	{	"string",	kNVSEParamType_String,0	},
	{	"float or string",	kNVSEParamType_StringOrNumber,0	},
	{	"string",	kNVSEParamType_String,1	},
	{	"string",	kNVSEParamType_String,1	},
};
static ParamInfo kNVSEParams_OneString_OneFloat_TwoOptionalStrings[4] =
{
	{	"string",	kNVSEParamType_String,0	},
	{	"float",	kNVSEParamType_Number,0	},
	{	"string",	kNVSEParamType_String,1	},
	{	"string",	kNVSEParamType_String,1	},
};
static ParamInfo kNVSEParams_OneString_TwoOptionalStrings[3] =
{
	{	"string",	kNVSEParamType_String,0	},
	{	"string",	kNVSEParamType_String,1	},
	{	"string",	kNVSEParamType_String,1	},
};
static ParamInfo kNVSEParams_OneString_OneOptionalString_OneOptionalFloat[3] =
{
	{	"string",	kNVSEParamType_String,0	},
	{	"string",	kNVSEParamType_String,1	},
	{	"float",	kNVSEParamType_Number,1	},
};
static ParamInfo kNVSEParams_OneString_OneOptionalString_OneOptionalFloat_OneOptionalString[4] =
{
	{	"string",	kNVSEParamType_String,0	},
	{	"string",	kNVSEParamType_String,1	},
	{	"float",	kNVSEParamType_Number,1	},
	{	"string",	kNVSEParamType_String,1	},
};
static ParamInfo kNVSEParams_OneString_ThreeOptionalStrings[4] =
{
	{	"string",	kNVSEParamType_String,0	},
	{	"string",	kNVSEParamType_String,1	},
	{	"string",	kNVSEParamType_String,1	},
	{	"string",	kNVSEParamType_String,1	},
};
static ParamInfo kNVSEParams_TwoStrings_TwoOptionalStrings[4] =
{
	{	"string",	kNVSEParamType_String,0	},
	{	"string",	kNVSEParamType_String,0	},
	{	"string",	kNVSEParamType_String,1	},
	{	"string",	kNVSEParamType_String,1	},
};